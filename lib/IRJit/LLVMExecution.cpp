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
@version 0.1
@date 2010/04/12
*/

//------------------------------
#include <iostream>
#include <errno.h>

#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/JITEventListener.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/System/Signals.h"
#include "llvm/System/Process.h"
#include "llvm/Target/TargetSelect.h"

#include "Jade/Decoder.h"
#include "Jade/Core/Actor/Procedure.h"
#include "Jade/Jit/LLVMExecution.h"
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

//===----------------------------------------------------------------------===//
// main Driver function
//
LLVMExecution::LLVMExecution(LLVMContext& C, Decoder* decoder): Context(C)  {
  std::string ErrorMsg;
  module = decoder->getModule();

  // If the user doesn't want core files, disable them.
  if (DisableCoreFiles)
    sys::Process::PreventCoreFiles();
  
   // If not jitting lazily, load the whole bitcode file eagerly too.
  if (NoLazyCompilation) {
    if (module->MaterializeAllPermanently(&ErrorMsg)) {
      cout << "bitcode didn't read correctly.\n";
      cout << "Reason: " << ErrorMsg << "\n";
      exit(1);
    }
  }

  EngineBuilder builder(module);
  builder.setMArch(MArch);
  builder.setMCPU(MCPU);
  builder.setMAttrs(MAttrs);
  builder.setErrorStr(&ErrorMsg);
  builder.setEngineKind(ForceInterpreter
                        ? EngineKind::Interpreter
                        : EngineKind::JIT);

  // If we are supposed to override the target triple, do so now.
  if (!TargetTriple.empty())
    module->setTargetTriple(Triple::normalize(TargetTriple));

  EE = builder.create();
  if (!EE) {
    if (!ErrorMsg.empty())
      cout << ": error creating EE: " << ErrorMsg << "\n";
    else
      cout << ": unknown error creating EE!\n";
    exit(1);
  }

  //Set properties of the EE
  EE->RegisterJITEventListener(createOProfileJITEventListener());

  EE->DisableLazyCompilation(NoLazyCompilation);
  
  // If the program doesn't explicitly call exit, we will need the Exit 
  // function later on to make an explicit call, so get the function now. 
  Exit = (Function*) module->getOrInsertFunction("exit", Type::getVoidTy(Context),
                                                    Type::getInt32Ty(Context),
                                                    NULL);
  
  // Reset errno to zero on entry to main.
  errno = 0;

  // Run static constructors.
  EE->runStaticConstructorsDestructors(false);

   if (NoLazyCompilation) {
    for (Module::iterator I = module->begin(), E = module->end(); I != E; ++I) {
      Function *Fn = &*I;
      if (!Fn->isDeclaration())
        EE->getPointerToFunction(Fn);
    }
  }
}

void* LLVMExecution::getExit() {
	return EE->getPointerToFunction(Exit);
}

void LLVMExecution::mapProcedure(Procedure* procedure, void *Addr) {
	EE->addGlobalMapping(procedure->getFunction(), Addr);
}

void LLVMExecution::run(std::string function) {
	std::string ErrorMsg;

	Function* func = module->getFunction(function);

	std::vector<GenericValue> noargs;
	GenericValue Result = EE->runFunction(func, noargs);

	// Run static destructors.
	EE->runStaticConstructorsDestructors(true);
}

LLVMExecution::~LLVMExecution(){
	delete EE;
	llvm_shutdown();
}