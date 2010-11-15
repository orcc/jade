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

#include "llvm/LLVMContext.h"
#include "llvm/Target/TargetSelect.h"


#include "Jade/XDFParser.h"
#include "Jade/DecoderEngine.h"
#include "Jade/Fifo/FifoCircular.h"
#include "Jade/Fifo/FifoTrace.h"
#include "Jade/Fifo/UnprotectedFifo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/System/Signals.h"
#include "llvm/Support/PrettyStackTrace.h"
//------------------------------

#ifdef __APPLE__
#include "SDL.h"
#endif

using namespace std;
using namespace llvm;
using namespace llvm::cl;
using namespace llvm::sys;

//Fifos
enum FifoTy { circular, trace, unprotected };

cl::opt<FifoTy> 
Fifo("fifo", CommaSeparated,
			 desc("Specify fifo to be used in the decoder"),
			 value_desc("trace, circular, fast"),
			 values(clEnumVal(trace,   "trace"),
                    clEnumVal(circular,  "circular"),
                    clEnumVal(unprotected, "fast"),
                    clEnumValEnd),
			 init(circular),
			 cl::Optional);

// Jade options
cl::opt<std::string>
XDFFile("xdf", Required, ValueRequired, desc("XDF network file"), value_desc("XDF filename"));

cl::opt<std::string> 
BSDLFile("bsdl", desc("Bitstream description file"), value_desc("BSDL filename"));

cl::opt<std::string> 
VidFile("i", Required, ValueRequired, desc("Encoded video file"), value_desc("Video filename"));

cl::opt<std::string>
VTLDir("L", desc("Video Tools Library directory"),
	   Required,
	   ValueRequired,
	   value_desc("VTL Folder"), 
	   init(""));

cl::opt<std::string> 
ToolsDir("T", desc("Jade tools directory"),
			  Required,
			  ValueRequired,
			  value_desc("Tools Folder"), 
			  init(""));

cl::opt<std::string> 
YuvFile("o", desc("Decoded YUV video file for compare mode"), 
			  value_desc("YUV filename"), 
			  init(""));

cl::opt<std::string> 
OutputDir("w", desc("Output folder for writing trace/Module/Error files"), 
			  value_desc("Trace folder"), 
			  init(""));

cl::opt<bool> 
ForceInterpreter("force-interpreter", desc("Force interpretation: disable JIT"),
									  init(false));

cl::opt<bool> 
nodisplay("nodisplay", desc("Deactivate display"),
					   init(false));

cl::opt<std::string> 
MArch("march", desc("Architecture to generate assembly for (see --version)"));

cl::opt<bool> 
DisableCoreFiles("disable-core-files", Hidden,
                   desc("Disable emission of core files if possible"));

cl::opt<bool> 
NoLazyCompilation("disable-lazy-compilation",
                  desc("Disable JIT lazy compilation"),
                  init(false));

cl::list<std::string> 
MAttrs("mattr", CommaSeparated,
         desc("Target specific IRAttributes (-mattr=help for details)"),
         value_desc("a1,+a2,-a3,..."));

cl::opt<std::string>
  TargetTriple("mtriple", cl::desc("Override target triple for module"));

cl::opt<std::string> 
MCPU("mcpu", desc("Target a specific cpu type (-mcpu=help for details)"),
       value_desc("cpu-name"),
       init(""));

void clean_exit(int sig){
	exit(0);
}

AbstractFifo* getFifo(LLVMContext &Context){
	//Select fifo according to options
	switch (Fifo) {
		case circular :
			return new FifoCircular(Context);
			break;

		case trace :
			return new FifoTrace(Context);
			break;

		case unprotected :
			return new UnprotectedFifo(Context);
			break;

		default: 
			cout <<"Fifo selection error: type undefined.\n";
			exit(0);
			break;
	}
}

//Verify if directory is well formed
void setDirectory(std::string* dir){
	size_t found = dir->find_last_of("/\\");
	if(found != dir->length()-1){
		dir->insert(dir->length(),"/");
	}
}


//Check options of the decoder engine
void setOptions(){
	setDirectory(&VTLDir);
	setDirectory(&ToolsDir);
	setDirectory(&OutputDir);
}	

int main(int argc, char **argv) {

	// Print a stack trace if we signal out.
	PrintStackTraceOnErrorSignal();
	PrettyStackTraceProgram X(argc, argv);
	ParseCommandLineOptions(argc, argv, "Just-In-Time Adaptive Decoder Engine (Jade) \n");
	(void) signal(SIGINT, clean_exit);
    
	//Initialize context
	InitializeNativeTarget();
	LLVMContext &Context = getGlobalContext();
	clock_t timer = clock ();

	setOptions();

	//Parsing XDF file
	std::cout << "Jade started, parsing file " << XDFFile.getValue() << ". \n";
	XDFParser xdfParser(XDFFile);
	Network* network = xdfParser.ParseXDF(Context);
	cout << "Network parsed in : "<< (clock () - timer) * 1000 / CLOCKS_PER_SEC << " ms, start engine :\n";

	//Load and execute the parsed network
	DecoderEngine engine(Context, getFifo(Context));
	engine.load(network);

	cout << "End of Jade:" << (clock () - timer) * 1000 / CLOCKS_PER_SEC;
}