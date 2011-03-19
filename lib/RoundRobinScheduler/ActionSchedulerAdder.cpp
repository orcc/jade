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
@brief Implementation of class ActionSchedulerAdder
@author Jerome Gorin
@file ActionSchedulerAdder.cpp
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <iostream>
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"

#include "ActionSchedulerAdder.h"

#include "Jade/Decoder.h"
#include "Jade/Configuration/Configuration.h"
#include "Jade/Core/Actor/Action.h"
#include "Jade/Core/Actor/ActionScheduler.h"
#include "Jade/Core/Actor/ActionTag.h"
#include "Jade/Core/Actor.h"
#include "Jade/Core/Port.h"
#include "Jade/Core/Actor/Procedure.h"
#include "Jade/Core/Network/Instance.h"
#include "Jade/Util/FifoMng.h"
//------------------------------

using namespace llvm;
using namespace std;

ActionSchedulerAdder::ActionSchedulerAdder(llvm::LLVMContext& C, Decoder* decoder) : Context(C) {
	this->decoder = decoder;
}

void ActionSchedulerAdder::transform(Instance* instance) {
	//Create action scheduler
	createActionScheduler(instance);

	//Create an action scheduler initializer
	if (instance->hasInitializes()){
		createInitialize(instance);
	}
}

void ActionSchedulerAdder::createActionScheduler(Instance* instance){
	//Get properties of the instance
	Module* module = decoder->getModule();
	actionScheduler = instance->getActionScheduler();
	moc = instance->getMoC();

	string name = instance->getId();
	name.append("_scheduler");

	Function* scheduler = cast<Function>(module->getOrInsertFunction(name, Type::getInt32Ty(Context),
										  (Type *)0));
	actionScheduler->setSchedulerFunction(scheduler);

	//Create values
	Value *Zero = ConstantInt::get(Type::getInt32Ty(Context), 0);
	Value *One = ConstantInt::get(Type::getInt32Ty(Context), 1);

	
	// Add a basic block entry to the scheduler.
	BasicBlock* BBEntry = BasicBlock::Create(Context, "entry", scheduler);

	//Create alloca on i and store 0
	AllocaInst* iVar = new AllocaInst(Type::getInt32Ty(Context), "i", BBEntry);
	StoreInst* storeInst = new StoreInst(Zero, iVar, BBEntry);

	// Add a basic block to bb and branch entry to bb.
	BasicBlock* BB = BasicBlock::Create(Context, "bb", scheduler);
	BranchInst::Create(BB, BBEntry);
	
	// Add a basic block return that return %i
	BasicBlock* returnBB = BasicBlock::Create(Context, "return", scheduler);
	LoadInst* loadIRet = new LoadInst(iVar, "i_ret", returnBB);
	ReturnInst::Create(Context, loadIRet, returnBB);


	// Add a basic block inc that return %i and branch to bb
	BasicBlock* incBB = BasicBlock::Create(Context, "inc_i", scheduler);
	LoadInst* loadIInc = new LoadInst(iVar, "i_load", incBB);
	BinaryOperator* iAdd = BinaryOperator::CreateNSWAdd(loadIInc, One, "i_add", incBB);
	new StoreInst(iAdd, iVar, incBB);
	BranchInst::Create(BB, incBB);
	
	createScheduler(instance, BB, incBB, returnBB , scheduler);
}

void ActionSchedulerAdder::createInitialize(Instance* instance){
	
	//Get properties of the instance
	Module* module = decoder->getModule();
	ActionScheduler* actionScheduler = instance->getActionScheduler();
	list<Action*>* initializes = instance->getInitializes();
	string name = instance->getId();
	name.append("_initialize");

	//Create initialize function
	Function* initialize = cast<Function>(module->getOrInsertFunction(name, Type::getVoidTy(Context),
											  (Type *)0));
	//Set function to the ActionScheduler
	actionScheduler->setInitializeFunction(initialize);

	// Add a basic block entry to the scheduler.
	BasicBlock* BB = BasicBlock::Create(Context, "entry", initialize);

	// Add a basic block return to the scheduler.
	BasicBlock* returnBB = BasicBlock::Create(Context, "return", initialize);
	ReturnInst::Create(Context, returnBB);

	//Test initialize function
	list<Action*>::iterator it;

	for ( it=initializes->begin() ; it != initializes->end(); it++ ){
		//Launch action body
		Procedure* body = (*it)->getBody();
		CallInst* bodyInst = CallInst::Create(body->getFunction(), "",  BB);
	}

	//Create branch from skip to return
	BranchInst::Create(returnBB, BB);
}

BasicBlock* ActionSchedulerAdder::checkInputPattern(Pattern* pattern, Function* function, BasicBlock* skipBB, BasicBlock* BB){	
	//Pattern is empty, return current basic block
	if (pattern->isEmpty()){
		return BB;
	}

	//Check inputs
	list<Value*>::iterator itValue;
	list<Value*> values;
	map<Port*, ConstantInt*>::iterator it;
	map<Port*, ConstantInt*>* numTokens = pattern->getNumTokensMap();

	for ( it=numTokens->begin() ; it != numTokens->end(); it++ ){
		Value* hasTokenValue = createInputTest(it->first, it->second, BB);
		TruncInst* truncTokenInst = new TruncInst(hasTokenValue, Type::getInt1Ty(Context),"", BB);
		values.push_back(truncTokenInst);
	}
	
	itValue=values.begin();
	Value* value1 = *itValue;
	for ( itValue=++itValue ; itValue != values.end(); itValue++ ){
		Value* value2 = *itValue;
		value1 = BinaryOperator::Create(Instruction::And,value1, value2, "", BB);
	}

	// Add a basic block hasToken that test the isSchedulable of a function
	string hasTokenBrName = "hasToken";
	BasicBlock* tokenBB = BasicBlock::Create(Context, hasTokenBrName, function);

	//Finally branch fire to hasToken block if all inputs have tokens
	BranchInst* brInst = BranchInst::Create(tokenBB, skipBB, value1, BB);
	return tokenBB;
}

BasicBlock* ActionSchedulerAdder::checkOutputPattern(Pattern* pattern, llvm::Function* function, llvm::BasicBlock* skipBB, llvm::BasicBlock* BB){
	//No output pattern return basic block
	if (pattern->isEmpty()){
		return BB;
	}
	
	//Test if rooms are available on output
	list<Value*>::iterator itValue;
	list<Value*> values;
	map<Port*, ConstantInt*>::iterator it;
	map<Port*, ConstantInt*>* numTokens = pattern->getNumTokensMap();
	

	for ( it=numTokens->begin() ; it != numTokens->end(); it++ ){
		Value* hasRoomValue = createOutputTest(it->first, it->second, BB);
		TruncInst* truncRoomInst = new TruncInst(hasRoomValue, Type::getInt1Ty(Context),"", BB);
		values.push_back(truncRoomInst);
	}

	itValue=values.begin();
	Value* value1 = *itValue;
	for ( itValue=++itValue ; itValue != values.end(); itValue++ ){
		Value* value2 = *itValue;
		value1 = BinaryOperator::Create(Instruction::And,value1, value2, "", BB);
	}

	// Add a basic block hasRoom that fires the action
	string hasRoomBrName = "hasRoom";
	BasicBlock* roomBB = BasicBlock::Create(Context, hasRoomBrName, function);

	//Finally branch fire to hasRoom block if all outputs have free room
	BranchInst* brInst = BranchInst::Create(roomBB, skipBB, value1, BB);

	return roomBB;
}

void ActionSchedulerAdder::checkPeekPattern(Pattern* pattern, Function* function, BasicBlock* BB){
	//Test if rooms are available on output
	map<Port*, Variable*>::iterator it;
	map<Port*, Variable*>* peeked = pattern->getPeekedMap();
	map<Port*, ConstantInt*>* numTokensMap = pattern->getNumTokensMap();

	for ( it=peeked->begin() ; it != peeked->end(); it++ ){
		Port* port = it->first;

		// Get number of tokens to peek
		map<Port*, ConstantInt*>::iterator itToken;

		itToken = numTokensMap->find(port);

		createPeek(it->first, it->second, itToken->second, BB);
	}
}

void ActionSchedulerAdder::createPeek(Port* port, Variable* variable, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	GlobalVariable* fifoVar = port->getFifoVar();
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call peek function
	Function* peekFn = FifoMng::getPeekFunction(port->getType(), decoder);
	Value* peekArgs[] = { loadPort, numTokens};
	CallInst* callInst = CallInst::Create(peekFn, peekArgs, peekArgs+2,"",  BB);

	// Store fifo pointer in port pointer
	GlobalVariable* portPtr = variable->getGlobalVariable();
	new StoreInst(callInst, portPtr, BB);
}

void ActionSchedulerAdder::createWriteEnds(Pattern* pattern, llvm::BasicBlock* BB){

	//Get tokens and var
	map<Port*, ConstantInt*>::iterator it;
	map<Port*, ConstantInt*>* numTokensMap = pattern->getNumTokensMap();

	for (it = numTokensMap->begin(); it != numTokensMap->end(); it++){
		//Create write end
		createWriteEnd(it->first, it->second, BB);
	}
}

void ActionSchedulerAdder::createReadEnds(Pattern* pattern, llvm::BasicBlock* BB){
	//Get tokens and var
	map<Port*, ConstantInt*>::iterator it;
	map<Port*, ConstantInt*>* numTokensMap = pattern->getNumTokensMap();

	for (it = numTokensMap->begin(); it != numTokensMap->end(); it++){
		//Create read end
		createReadEnd(it->first, it->second, BB);
	}
}

void ActionSchedulerAdder::createReadEnd(Port* port, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	GlobalVariable* fifoVar = port->getFifoVar();
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call peek function
	Function* readEndFn = FifoMng::getReadEndFunction(port->getType(), decoder);
	Value* readEndArgs[] = { loadPort, numTokens};
	CallInst::Create(readEndFn, readEndArgs, readEndArgs+2,"",  BB);
}

void ActionSchedulerAdder::createWriteEnd(Port* port, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	GlobalVariable* fifoVar = port->getFifoVar();
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call peek function
	Function* writeEndFn = FifoMng::getWriteEndFunction(port->getType(), decoder);
	Value* writeEndArgs[] = { loadPort, numTokens};
	CallInst::Create(writeEndFn, writeEndArgs, writeEndArgs+2,"",  BB);
}


void ActionSchedulerAdder::createWrites(Pattern* pattern, llvm::BasicBlock* BB){
	//Get tokens and var
	map<Port*, ConstantInt*>::iterator it;
	map<Port*, Variable*>::iterator itVar;
	map<Port*, ConstantInt*>* numTokensMap = pattern->getNumTokensMap();

	for (it = numTokensMap->begin(); it != numTokensMap->end(); it++){
		//Create write
		Port* port = it->first;
		createWrite(port, port->getPtrVar(), it->second, BB);
	}
}

void ActionSchedulerAdder::createReads(Pattern* pattern, llvm::BasicBlock* BB){
	//Get tokens and var
	map<Port*, ConstantInt*>::iterator it;
	map<Port*, Variable*>::iterator itVar;
	map<Port*, ConstantInt*>* numTokensMap = pattern->getNumTokensMap();

	for (it = numTokensMap->begin(); it != numTokensMap->end(); it++){
		// Get associated port variable
		Port* port = it->first;
		createRead(port, port->getPtrVar(), it->second, BB);
	}
}

void ActionSchedulerAdder::createWrite(Port* port, Variable* variable, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	GlobalVariable* fifoVar = port->getFifoVar();
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call peek function
	Function* writeFn = FifoMng::getWriteFunction(port->getType(), decoder);
	Value* writeArgs[] = { loadPort, numTokens};
	CallInst* callInst = CallInst::Create(writeFn, writeArgs, writeArgs+2,"",  BB);

	// Store fifo pointer in port pointer
	GlobalVariable* portPtr = variable->getGlobalVariable();
	new StoreInst(callInst, portPtr, BB);
}


void ActionSchedulerAdder::createRead(Port* port, Variable* variable, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	GlobalVariable* fifoVar = port->getFifoVar();
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call peek function
	Function* readFn = FifoMng::getReadFunction(port->getType(), decoder);
	Value* readArgs[] = { loadPort, numTokens};
	CallInst* callInst = CallInst::Create(readFn, readArgs, readArgs+2,"",  BB);

	// Store fifo pointer in port pointer
	GlobalVariable* portPtr = variable->getGlobalVariable();
	new StoreInst(callInst, portPtr, BB);
}

CallInst* ActionSchedulerAdder::createOutputTest(Port* port, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call hasRoom function
	Function* hasRoomFn = FifoMng::getHasRoomFunction(port->getType(), decoder);
	Value* hasRoomArgs[] = { loadPort, numTokens};
	CallInst* callInst = CallInst::Create(hasRoomFn, hasRoomArgs, hasRoomArgs+2,"",  BB);

	return callInst;
}

CallInst* ActionSchedulerAdder::createInputTest(Port* port, ConstantInt* numTokens, BasicBlock* BB){
	//Load selected port
	LoadInst* loadPort = new LoadInst(port->getFifoVar(), "", BB);
	
	//Call hasToken function
	Function* hasTokenFn = FifoMng::getHasTokenFunction(port->getType(), decoder);
	Value* hasTokenArgs[] = { loadPort, numTokens};
	CallInst* callInst = CallInst::Create(hasTokenFn, hasTokenArgs, hasTokenArgs+2,"",  BB);

	return callInst;
}