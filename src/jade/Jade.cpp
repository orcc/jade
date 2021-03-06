/*
 * Copyright (c) 2009, IETR/INSA of Rennes
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of the IETR/INSA of Rennes nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
@brief Main function of Jade
@author Jerome Gorin
@file Jade.cpp
@version 1.0
@date 2010/04/13
*/

//------------------------------
#include <time.h>
#include <signal.h>
#include <iostream>
#include <map>

#include "Console.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassNameParser.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Threading.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

#include "lib/XDFSerialize/XDFParser.h"
#include "lib/XCFSerialize/XCFParser.h"
#include "lib/RVCEngine/RVCEngine.h"
#include "lib/Scenario/Manager.h"
#include "lib/IRUtil/OptionMng.h"
//------------------------------

#define DISPLAY_DISABLE 0
#define DISPLAY_ENABLE 1

#ifdef __APPLE__
#include "SDL.h"
#endif

using namespace std;
using namespace llvm;
using namespace llvm::cl;
using namespace llvm::sys;

// Jade options
cl::opt<string>
XDFFile("xdf", desc("XDF network file"), value_desc("XDF file"));

cl::opt<string>
XCFFile("xcf", desc("XCF mapping file"), value_desc("XCF file"));

cl::opt<string>
VidFile("i", desc("Encoded video file"), value_desc("Video file"));

cl::opt<string>
VTLDir("L", desc("Video Tools Library directory"),
       value_desc("VTL Folder"),
       Required,
       init(""));

cl::opt<string>
InputDir("I", desc("Stimulus directory"),
         value_desc("Folder of input stimulus"),
         Hidden,
         init(""));

cl::opt<string>
YuvFile("o", desc("Compare output with a decoded YUV video file"),
        value_desc("YUV filename"),
        init(""));

cl::opt<string>
ScFile("scenario", desc("Use a decoding scenario"),
       value_desc("decoding scenario"),
       init(""));

cl::opt<string>
OutputDir("w", desc("Output folder for writing trace/Module/Error files"),
          value_desc("Trace folder"),
          init(""));

cl::opt<bool>
ForceInterpreter("force-interpreter", desc("Force interpretation: disable JIT"),
                 init(false));

cl::opt<bool>
nodisplay("nodisplay", desc("Deactivate display"),
          init(false));

cl::list<string>
debexec("debexec", desc("Display debugging information for the given instances"),
        cl::value_desc("A list of instance id"));

cl::opt<string>
MArch("march", desc("Architecture to generate assembly for (see --version)"));

cl::opt<llvm::FloatABI::ABIType>
UserDefinedFloatABI("float-abi",
  cl::desc("Choose float ABI type"),
  cl::init(FloatABI::Default),
  cl::values(
    clEnumValN(FloatABI::Default, "default",
               "Target default float ABI type"),
    clEnumValN(FloatABI::Soft, "soft",
               "Soft float ABI"),
    clEnumValN(FloatABI::Hard, "hard",
               "Hard float ABI (uses FP registers)"),
    clEnumValEnd));

cl::opt<bool>
DisableCoreFiles("disable-core-files", Hidden,
                 desc("Disable emission of core files if possible"));

cl::opt<bool>
NoLazyCompilation("disable-lazy-compilation",
                  desc("Disable JIT lazy compilation"),
                  init(false));

cl::list<string>
MAttrs("mattr", CommaSeparated,
       desc("Target specific IRAttributes (-mattr=help for details)"),
       value_desc("a1,+a2,-a3,..."));


cl::opt<string>
MCPU("mcpu", desc("Target a specific cpu type (-mcpu=help for details)"),
     value_desc("cpu-name"),
     init(""));

cl::opt<bool> OptLevelO1("O1", cl::desc("Optimization level 1. Similar to llvm-gcc -O1"));

cl::opt<bool> OptLevelO2("O2",
                         cl::desc("Optimization level 2. Similar to llvm-gcc -O2"));

cl::opt<bool> OptLevelO3("O3",
                         cl::desc("Optimization level 3. Similar to llvm-gcc -O3"));

cl::opt<int> FifoSize("fifo-size-default",
                      cl::desc("Defaut size of fifos"),
                      cl::init(512));

static cl::opt<bool> Verbose("verbose", cl::desc("Print information about actions taken"), cl::init(false));

static cl::opt<bool> Verify("v", cl::desc("Verify every generated decoders"), cl::init(false));

static cl::opt<bool> Console("console", cl::desc("Enter in console mode"), cl::init(false));

static cl::opt<bool> noMerging("nomerging", cl::desc("Deactivate merging of static actors"), cl::init(false));

cl::list<const PassInfo*, bool, PassNameParser> PassList(cl::desc("Optimizations available:"));

cl::opt<bool> ArmFix("arm-fix", cl::desc("Fix execution for ARM platform (Linux only)"));

bool enableTrace = false;
char **environnement;

void clean_exit(int sig){
    exit(0);
}

// Variable from native functions
extern "C" {
extern char* input_file;
extern char* yuv_file;
extern char* write_file;
extern char display_flags;
}

//Verify if directory is well formed
void setDirectory(string* dir){
    if (dir->compare("") != 0){
        size_t found = dir->find_last_of("/\\");
        if(found != dir->length()-1){
            dir->insert(dir->length(),"/");
        }
    }
}

int optLevel;
RVCEngine* engine;

//Check options of the decoder engine
void setOptions(){
    //Verify if directory is well formed
    OptionMng::setDirectory(&VTLDir);
    OptionMng::setDirectory(&OutputDir);
    OptionMng::setDirectory(&InputDir);

    //Set an optimization level
    if (OptLevelO1){
        optLevel = 1;
    }else if (OptLevelO2){
        optLevel = 2;
    }else if (OptLevelO3){
        optLevel = 3;
    }else{
        optLevel = 0;
    }

    //Set native variables
    string writer_file = OutputDir + "writer.txt";
    if (YuvFile != ""){
        yuv_file = (char*)YuvFile.c_str();
    }
    write_file = (char*)writer_file.c_str();

    if (nodisplay){
        display_flags = DISPLAY_DISABLE;
    } else {
        display_flags = DISPLAY_ENABLE;
    }

    if (!debexec.empty()){
        enableTrace = true;
    }

}

void setTraces(Network* network){
    std::list<Instance*>::iterator it;
    std::list<Instance*>* instances = network->getInstances();

    bool traceAll = false;

    // Check if "all" option is activate
    if (debexec.begin()->compare("all")==0){
        traceAll = true;
    }

    for (it = instances->begin(); it != instances->end(); it++){
        Instance* instance = *it;

        if (traceAll || (debexec.end() != find(debexec.begin(), debexec.end(), instance->getId()))){
            instance->setTrace(true);
        }
    }
}

//Command line decoder control
void startCmdLine(){
    LLVMContext &Context = getGlobalContext();

    for (unsigned int i =0 ; i < PassList.size(); i++ ){
        cout << "Pass added: "<< PassList[i]->getPassName() << endl;
        cout << "Argument name :" << PassList[i]->getPassArgument() << endl;
    }

    clock_t timer = clock();

    //Parsing XDF file
    std::cout << "Parsing file " << XDFFile.getValue() << "." << endl;

    XDFParser xdfParser(Verbose);
    Network* network = xdfParser.parseFile(XDFFile, Context);

    cout << "Network parsed in : "<< (clock() - timer) * 1000 / CLOCKS_PER_SEC << " ms, start engine" << endl;

    //Parsing XCF file if needed
    if(XCFFile != "") {
        std::cout << "Parsing file " << XCFFile.getValue() << "." << endl;

        XCFParser xcfParser(Verbose);
        map<string, string>* mapping = xcfParser.parseFile(XCFFile);
        network->setMapping(mapping);
    }

    if (enableTrace){
        setTraces(network);
    }

    //Load network
    engine->load(network);

    // Optimizing decoder
    if (optLevel > 0){
        engine->optimize(network, optLevel);
    }

    // Verify the given decoder if needed
    if (Verify){
        engine->verify(network, "error.txt");
    }

    // Set input file
    input_file = (char*)VidFile.c_str();

    // Print the given decoder if needed
    if (OutputDir != ""){
        engine->print(network);
    }

    //Run network
    engine->run(network);

    cout << "End of Jade" << endl;
    cout << "Total time: " << (clock() - timer) * 1000 / CLOCKS_PER_SEC << " ms" << endl;
    if(XCFFile != "") {
        cout << "Note: This execution time is calculated from CPU clock. When more than 1 thread were run, "
                "the value displayed is higher than the real execution time." << endl;
    }
}

int main(int argc, char **argv, char **envp) {
    clock_t start = clock ();
    LLVMContext &Context = getGlobalContext();
    environnement = envp;

    // Print a stack trace if we signal out.
    PrintStackTraceOnErrorSignal();
    ParseCommandLineOptions(argc, argv, "Just-In-Time Adaptive Decoder Engine (Jade) \n");
    (void) signal(SIGINT, clean_exit);

    if (Verbose){
        cout << "> Preparing core of Jade :" << endl;
    }

    //Initialize context
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    setOptions();

    //Loading decoderEngine
    engine = new RVCEngine(Context, VTLDir, OutputDir, noMerging, Verbose, ArmFix);

    if (Verbose){
        cout << "> Core preparation finished in " << (clock () - start) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
    }

    if (Console){

        if (Verbose){
            cout << "> Starting console mode :" << endl;
        }
        //Enter in console mode
        startConsole();

        if (Verbose){
            cout << "> Exiting console mode." << endl;
        }
    } else if (ScFile != ""){
        //Scenario mode
        if (Verbose){
            cout << "> Starting scenario mode :" << endl;
        }
        Manager manager(engine, optLevel, Verify, Verbose);
        manager.start(ScFile);

        if (Verbose){
            cout << "> Exiting scenario mode." << endl;
        }
    } else {
        if ((VidFile == "") || (VTLDir == "") || (XDFFile == "")){
            cerr << "An input stimulus (-i), a VTL directory (-L) and an XDF Network (-xdf) is mandatory to start Jade in command line mode." << endl;
            exit(1);
        }

        //Enter in command line mode
        startCmdLine();
    }

    return 0;
}
