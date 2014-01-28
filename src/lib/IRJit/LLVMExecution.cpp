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
@brief Implementation of LLVMExecution
@author Jerome Gorin
@file LLVMExecution.cpp
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <iostream>
#include <errno.h>
#include <time.h>

#include "NativeDecl.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/JITEventListener.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Process.h"

#include "lib/RVCEngine/Decoder.h"
#include "lib/IRCore/Port.h"
#include "lib/IRCore/Actor/Procedure.h"
#include "lib/RoundRobinScheduler/Fifo.h"
#include "lib/IRJit//LLVMExecution.h"
//------------------------------

using namespace llvm;
using namespace std;

//Execution engine options
extern cl::opt<bool> ForceInterpreter;
extern cl::opt<std::string> MArch;
extern cl::opt<bool> DisableCoreFiles;
extern cl::opt<bool> NoLazyCompilation;
extern cl::list<std::string> MAttrs;
extern cl::opt<std::string> MCPU;
extern cl::opt<std::string> TargetTriple;
cl::opt<bool> UseMCJIT(
        "use-mcjit", cl::desc("Enable use of the MC-based JIT (if available)"),
        cl::init(false));
extern cl::opt<llvm::FloatABI::ABIType> UserDefinedFloatABI;

//===----------------------------------------------------------------------===//
// main Driver function
//
LLVMExecution::LLVMExecution(LLVMContext& C, Decoder* decoder, bool verbose): Context(C)  {
    std::string ErrorMsg;

    this->decoder = decoder;
    this->verbose = verbose;
    this->stopVal = 0;

    Module* module = decoder->getModule();

    // If the user doesn't want core files, disable them.
    if (DisableCoreFiles)
        sys::Process::PreventCoreFiles();

    // If not jitting lazily, load the whole bitcode file eagerly too.
    if (NoLazyCompilation) {
        if (module->MaterializeAllPermanently(&ErrorMsg)) {
            cout << "bitcode didn't read correctly." << endl;
            cerr << "Reason: " << ErrorMsg << endl;
            exit(1);
        }
    }

    EngineBuilder builder(module);
    builder.setMArch(StringRef(MArch));
    builder.setMCPU(StringRef(MCPU));
    builder.setMAttrs(MAttrs);
    builder.setErrorStr(&ErrorMsg);
    builder.setEngineKind(ForceInterpreter
                          ? EngineKind::Interpreter
                          : EngineKind::JIT);

    // Configure float ABI for target
    TargetOptions TrgtOptions;
    TrgtOptions.FloatABIType = UserDefinedFloatABI;
    builder.setTargetOptions(TrgtOptions);

    // If we are supposed to override the target triple, do so now.
    if (!TargetTriple.empty())
        module->setTargetTriple(Triple::normalize(TargetTriple));

    // Enable MCJIT, if desired.
    if (UseMCJIT)
        builder.setUseMCJIT(true);

    //TODO : select optimization level
    char OptLevel = '2';
    CodeGenOpt::Level OLvl = CodeGenOpt::Default;
    switch (OptLevel) {
    default:
        exit (1);
    case ' ': break;
    case '0': OLvl = CodeGenOpt::None; break;
    case '1': OLvl = CodeGenOpt::Less; break;
    case '2': OLvl = CodeGenOpt::Default; break;
    case '3': OLvl = CodeGenOpt::Aggressive; break;
    }
    builder.setOptLevel(OLvl);

    EE = builder.create();
    if (!EE) {
        if (!ErrorMsg.empty())
            cout << ": error creating EE: " << ErrorMsg << endl;
        else
            cerr << ": unknown error creating EE!" << endl;
        exit(1);
    }

    //Set properties of the EE
    EE->RegisterJITEventListener(JITEventListener::createOProfileJITEventListener());

    EE->DisableLazyCompilation(NoLazyCompilation);

    // If the program doesn't explicitly call exit, we will need the Exit
    // function later on to make an explicit call, so get the function now.
    Exit = (Function*) module->getOrInsertFunction("exit", Type::getVoidTy(Context),
                                                   Type::getInt32Ty(Context),
                                                   NULL);
}

void* LLVMExecution::getExit() {
    return EE->getPointerToFunction(Exit);
}

void LLVMExecution::mapProcedure(Procedure* procedure, void *Addr) {
    Function* function = procedure->getFunction();

    if (EE->getPointerToGlobalIfAvailable(function) == NULL){
        EE->addGlobalMapping(function, Addr);
    }
}


bool LLVMExecution::mapFifo(Port* port, Fifo* fifo) {
    void **portGV = (void**)EE->getPointerToGlobalIfAvailable(port->getFifoVar());

    //Port has already been compiled
    if (portGV != NULL){
        //Initialize fifo
        void* fifoGV = EE->getOrEmitGlobalVariable(fifo->getGV());

        //Connect to compiled port
        *portGV = fifoGV;

        return true;
    }

    return false;
}


void LLVMExecution::linkExternalProc(list<Procedure*> externs){
    list<Procedure*>::iterator it;

    // Loop over all native procedure to find their equivalences
    for (it = externs.begin(); it != externs.end(); it++){
        map<std::string,void*>::iterator itNative;

        // Find native function corresponding to native procedure name
        itNative = nativeMap.find((*it)->getName());

        if (itNative == nativeMap.end()){
            cerr << "Unknown native function :"<< (*it)->getName() << endl;
            exit(1);
        }

        // FIXME: Should be better to ensure globalMapping is done only once for
        // all, and not from LLVMExecution::initialize function, which is used each
        // time the decoder is reconfigured
        if( !EE->getPointerToGlobalIfAvailable((*it)->getFunction())) {
            // Link native procedures
            EE->addGlobalMapping((*it)->getFunction(), itNative->second);
        }
    }

}

void LLVMExecution::launchPartitions(map<Partition*, Scheduler*>* parts) {
    // Get scheduler's partition
    map<Partition*, Scheduler*>::iterator it;

    for (it = parts->begin(); it != parts->end(); it++){
        Scheduler* sched = it->second;

        pthread_t* thread = new pthread_t();
        threads.push_back(thread);

        procThread* th = new procThread;
        th->EE = EE;
        th->func = sched->getMainFunction();

        pthread_create( thread, NULL, &LLVMExecution::threadProc, th);
    }
}

void LLVMExecution::run() {
    stopVal = 0;

    if (decoder->hasPartitions()){
        // Start partitions
        launchPartitions(decoder->getSchedParts());
    }

    // Get main scheduler functions
    Scheduler* scheduler = decoder->getScheduler();
    Function* func = dyn_cast<Function>(scheduler->getMainFunction());

    // Run main scheduler
    EE->runFunction(func, vector<GenericValue>());
}

void* LLVMExecution::threadProc( void* args ){
    procThread* th = static_cast<procThread*>(args);
    Function* f = th->func;
    ExecutionEngine* E = th->EE;

    std::vector<GenericValue> noargs;
    E->runFunction(f, noargs);

    return NULL;
}

int* LLVMExecution::initialize(){
    std::string ErrorMsg;
    Module* module = decoder->getModule();
    clock_t timer = clock ();

    // Link external procedure of the decoder
    linkExternalProc(decoder->getExternalProcs());

    // Set stop condition of the scheduler
    Scheduler* scheduler = decoder->getScheduler();

    GlobalVariable* stopGV = scheduler->getStopGV();
    if(!EE->getPointerToGlobalIfAvailable(stopGV))
        EE->addGlobalMapping(stopGV, &stopVal);

    if (decoder->hasPartitions()){
        // Get scheduler's partition
        map<Partition*, Scheduler*>::iterator it;
        map<Partition*, Scheduler*>* parts = decoder->getSchedParts();

        for (it = parts->begin(); it != parts->end(); it++){
            int* test = new int();
            Scheduler* sched = it->second;

            GlobalVariable* stopGVpart = sched->getStopGV();
            if(!EE->getPointerToGlobalIfAvailable(stopGVpart))
                EE->addGlobalMapping(stopGVpart, &test);
        }
    }

    // Run static constructors.
    EE->runStaticConstructorsDestructors(false);

    // In case of no lazy compilation, compile all
    if (NoLazyCompilation) {
        for (Module::iterator I = module->begin(), E = module->end(); I != E; ++I) {
            Function *Fn = &*I;
            if (!Fn->isDeclaration())
                EE->getPointerToFunction(Fn);
        }
        cout << "--> No lazy compilation enable, the decoder has been compiled in : "<< (clock () - timer) * 1000 / CLOCKS_PER_SEC << " ms" << endl;
    }

    // Initialize the network
    Function* init = dyn_cast<Function>(scheduler->getInitFunction());
    std::vector<GenericValue> noargs;
    EE->runFunction(init, noargs);

    // Return and set stop variable
    stopVar = &stopVal;
    return &stopVal;
}

void LLVMExecution::runFunction(Function* function) {
    std::vector<GenericValue> noargs;
    GenericValue Result = EE->runFunction(function, noargs);
}

void LLVMExecution::stop() {
    Scheduler* scheduler = decoder->getScheduler();
    int* stop = (int*)EE->getPointerToGlobalIfAvailable(scheduler->getStopGV());
    if(stop) {
        *stop = 1;
    }
}

void LLVMExecution::recompile(Function* function) {
    EE->recompileAndRelinkFunction(function);
}

bool LLVMExecution::isCompiledGV(llvm::GlobalVariable* gv){
    return EE->getPointerToGlobalIfAvailable(gv) != NULL;
}

void* LLVMExecution::getGVPtr(llvm::GlobalVariable* gv){
    return EE->getPointerToGlobal(gv);
}

void LLVMExecution::clear() {
    EE->runStaticConstructorsDestructors(true);
    EE->clearAllGlobalMappings();
}


LLVMExecution::~LLVMExecution(){
    // Run static destructors.
    EE->runStaticConstructorsDestructors(true);

    delete EE;
    llvm_shutdown();
}
