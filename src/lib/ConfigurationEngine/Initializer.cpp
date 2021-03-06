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
@brief Implementation of class Instantiator
@author Jerome Gorin
@file Instantiator.cpp
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <iostream>

#include "Initializer.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"

#include "lib/RVCEngine/Decoder.h"
#include "lib/IRCore/Network/Instance.h"
#include "lib/IRJit/LLVMExecution.h"
//------------------------------


using namespace std;
using namespace llvm;

Initializer::Initializer( LLVMContext& C, Decoder* decoder) : Context(C){
    this->executionEngine = decoder->getEE();
    this->decoder = decoder;

    createInitializeFn(decoder->getModule());
}

Initializer::~Initializer(){
    if (initFn != NULL){
        initFn->eraseFromParent();
    }
}

void Initializer::initialize(){
    //Close function
    ReturnInst::Create(Context, 0, entryBB);

    executionEngine->runFunction(initFn);
}

void Initializer::createInitializeFn(Module* module){

    FunctionType* FTY = FunctionType::get(Type::getVoidTy(Context), (Type*)NULL);
    initFn = Function::Create(FTY, Function::ExternalLinkage, "init", module);

    // Add a basic block entry to init.
    entryBB = BasicBlock::Create(Context, "entry", initFn);

}

void Initializer::add(Instance* instance){
    ActionScheduler* actionScheduler = instance->getActionScheduler();

    if (actionScheduler->hasFsm()){
        initializeFSM(actionScheduler->getFsm());
    }

    initializeStateVariables(instance->getStateVars());
}

void Initializer::initializeFSM(FSM* fsm){

    FSM::State* state = fsm->getInitialState();
    int stateIndex = state->getIndex();

    GlobalVariable* fsmVar = fsm->getFsmState();

    if (executionEngine->isCompiledGV(fsmVar)){
        int* fsmPtr = (int*)executionEngine->getGVPtr(fsmVar);

        *fsmPtr = stateIndex;
    }
}

void Initializer::initializeStateVariables(map<string, StateVar*>* vars){
    map<string, StateVar*>::iterator it;

    for (it = vars->begin(); it != vars->end(); ++it){
        StateVar* var = it->second;

        if (var->isAssignable() && var->hasInitialValue()){
            if (executionEngine->isCompiledGV(var->getGlobalVariable())){
                //Variable has been previously compiled
                initializeVariable(var);
            }
        }
    }
}

void Initializer::initializeParameters(map<string, Variable*>* parameters){
    map<string, Variable*>::iterator it;

    for (it = parameters->begin(); it != parameters->end(); ++it){
        Variable* var = it->second;

        if (executionEngine->isCompiledGV(var->getGlobalVariable())){
            //Variable has been previously compiled
            initializeVariable(var);
        }
    }
}

void Initializer::initializeVariable(Variable* var){
    Expr* initVal = var->getInitialValue();

    if (initVal->isBooleanExpr()){
        initializeBoolExpr(var->getGlobalVariable(), (BoolExpr*)initVal);
    }else if (initVal->isIntExpr()){
        initializeIntExpr(var->getGlobalVariable(), (IntExpr*)initVal);
    }else if (initVal->isListExpr()){
        initializeListExpr(var->getGlobalVariable(), (ListExpr*)initVal);
    }else{
        cerr << "Initialize only support initialization of integer." << endl;
        exit(1);
    }
}

void Initializer::initializeIntExpr(GlobalVariable* var, IntExpr* expr){
    new StoreInst(expr->getConstant(), var, entryBB);
}

void Initializer::initializeBoolExpr(GlobalVariable* var, BoolExpr* expr){
    new StoreInst(expr->getConstant(), var, entryBB);
}

void Initializer::initializeListExpr(GlobalVariable* var, ListExpr* expr){
    Constant* cst = expr->getConstant();

    const PointerType* ptrType = cast<PointerType>(var->getType());
    const ArrayType* arraytype = cast<ArrayType>(ptrType->getElementType());

    uint64_t numElements = arraytype->getNumElements();

    for (uint64_t i = 0; i < numElements; ++i){
        Constant* Elt = cst->getAggregateElement(i);


        if (Elt == NULL){
            cerr << "Initialization error of array." << endl;
            exit(1);
        }

        Value *Idxs[2];
        Idxs[0] = ConstantInt::get(Context, APInt(32, 0));
        Idxs[1] = ConstantInt::get(Context, APInt(32, i));

        GetElementPtrInst* getInst = GetElementPtrInst::Create(var, Idxs, "", entryBB);

        new StoreInst( Elt, getInst, entryBB);
    }
}
