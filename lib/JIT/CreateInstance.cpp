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
@brief Implementation of instance creation in class JIT
@author Jerome Gorin
@file CreateInstance.cpp
@version 0.1
@date 2010/04/12
*/

//------------------------------
#include <iostream>

#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ValueSymbolTable.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/TypeSymbolTable.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/Cloning.h"


#include "Jade/JIT.h"
#include "Jade/Actor/Action.h"
#include "Jade/Actor/ActionScheduler.h"
#include "Jade/Actor/Actor.h"
#include "Jade/Actor/FSM.h"
#include "Jade/Actor/Port.h"
#include "Jade/Actor/Variable.h"
#include "Jade/Decoder/Decoder.h"
#include "Jade/Decoder/InstancedActor.h"
#include "Jade/Decoder/Procedure.h"
#include "Jade/Fifo/AbstractFifo.h"
//------------------------------

using namespace llvm;
using namespace std;


void JIT::setDecoder(Decoder* decoder){
	this->decoder = decoder;
	this->module = decoder->getModule(); 
	
};

void JIT::setNewInstance(){
	ValueMap.clear();
	
};

bool JIT::LinkProcedureBody(Function* function){
	Function *F = cast<Function>(ValueMap[function]);
	if (!function->isDeclaration()) {
		Function::arg_iterator DestI = F->arg_begin();
		for (Function::const_arg_iterator J = function->arg_begin(); J != function->arg_end();
			++J) {
			DestI->setName(J->getName());
			ValueMap[J] = DestI++;
		}
	
		SmallVector<ReturnInst*, 8> Returns;  // Ignore returns cloned.

		this->LinkFunctionBody(F, function, ValueMap, Returns, decoder->getFifo());
		return true;
	}

	return false;
		
}

void JIT::LinkFunctionBody(Function *NewFunc, Function *OldFunc,
                             DenseMap<const Value*, Value*> &ValueMap,
                             SmallVectorImpl<ReturnInst*> &Returns, AbstractFifo* fifo,
                             const char *NameSuffix, ClonedCodeInfo *CodeInfo) {

  // Clone any attributes. 
  if (NewFunc->arg_size() == OldFunc->arg_size())
    NewFunc->copyAttributesFrom(OldFunc);
  else {
    // Some arguments were deleted with the ValueMap. Copy arguments one by one 
    for (Function::arg_iterator I = OldFunc->arg_begin(), 
           E = OldFunc->arg_end(); I != E; ++I)
      if (Argument* Anew = dyn_cast<Argument>(ValueMap[I]))
        Anew->addAttr( OldFunc->getAttributes()
                       .getParamAttributes(I->getArgNo() + 1));
    NewFunc->setAttributes(NewFunc->getAttributes()
                           .addAttr(0, OldFunc->getAttributes()
                                     .getRetAttributes()));
    NewFunc->setAttributes(NewFunc->getAttributes()
                           .addAttr(~0, OldFunc->getAttributes()
                                     .getFnAttributes()));

  }

  // Loop over all of the basic blocks in the function, cloning them as
  // appropriate.  Note that we save BE this way in order to handle cloning of
  // recursive functions into themselves.
  //
  for (Function::iterator BI = OldFunc->begin(), BE = OldFunc->end();
       BI != BE; ++BI) {
    const BasicBlock &BB = *BI;

    // Create a new basic block and copy instructions into it!
    BasicBlock *CBB = CloneBasicBlock(&BB, ValueMap, NameSuffix, NewFunc,
                                      CodeInfo);
    ValueMap[&BB] = CBB;                       // Add basic block mapping.

    if (ReturnInst *RI = dyn_cast<ReturnInst>(CBB->getTerminator()))
      Returns.push_back(RI);
  }
  
  // Loop over all of the instructions in the function, fixing up operand
  // references as we go.  This uses ValueMap to do all the hard work.
  //
  for (Function::iterator BB = cast<BasicBlock>(ValueMap[OldFunc->begin()]),
         BE = NewFunc->end(); BB != BE; ++BB)
    // Loop over all instructions, fixing each one as we find it...
	for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II){
	     for (Instruction::op_iterator op = II->op_begin(), E = II->op_end(); op != E; ++op) {
			 Value *V;
			 if (fifo->isFifoFunction((*op)->getName())){
				V = fifo->getFifoFunction((*op)->getName());
			 } else {
				V = MapValue(*op, ValueMap);
				if (V == NULL){
					int i = 0;
				}
			}
			 
			*op = V;
		}
	}
}

bool JIT::LinkGlobalInits(llvm::GlobalVariable* variable){
	string Error;
	const GlobalVariable *SGV = variable;
	Module* Dest = module;
    
	GlobalVariable *GV = cast<GlobalVariable>(ValueMap[variable]);
    
	if (variable->hasInitializer())
      GV->setInitializer(cast<Constant>(MapValue(variable->getInitializer(),
                                                 ValueMap)));
    GV->setLinkage(variable->getLinkage());
    GV->setThreadLocal(variable->isThreadLocal());
    GV->setConstant(variable->isConstant());
      
	return true;

  }

GlobalVariable* JIT::addVariable(string prefix, llvm::GlobalVariable* variable){
	string Err;
	const GlobalVariable *SGV = variable;
    GlobalValue *DGV = 0;
	Module *Dest = module;
	
      // No linking to be performed, simply create an identical version of the
      // symbol over in the dest module... the initializer will be filled in
      // later by LinkGlobalInits.
      GlobalVariable *NewDGV =
        new GlobalVariable(*Dest, SGV->getType()->getElementType(),
                           SGV->isConstant(), SGV->getLinkage(), /*init*/0,
                           prefix + SGV->getName(), 0, false,
                           SGV->getType()->getAddressSpace());
      // Propagate alignment, visibility and section info.
      CopyGVAttributes(NewDGV, SGV);

      // Make sure to remember this mapping.
      ValueMap[SGV] = NewDGV;

      return NewDGV;
}

GlobalValue* JIT::addFunctionProtos(string prefix, const Function* function){
    const Function *SF = function;   // SrcFunction
	Module* Dest = module;
	std::string Err;

	GlobalValue *DGV = 0;

	Function *NF =
      Function::Create(cast<FunctionType>(SF->getType()->getElementType()),
                       GlobalValue::ExternalLinkage,  prefix + SF->getName(), Dest);
    NF->copyAttributesFrom(SF);
	ValueMap[SF] = NF;

      return NF;
  }



/// CopyGVAttributes - copy additional attributes (those not needed to construct
/// a GlobalValue) from the SrcGV to the DestGV.
void JIT::CopyGVAttributes(GlobalValue *DestGV, const GlobalValue *SrcGV) {
  // Use the maximum alignment, rather than just copying the alignment of SrcGV.
  unsigned Alignment = std::max(DestGV->getAlignment(), SrcGV->getAlignment());
  DestGV->copyAttributesFrom(SrcGV);
  DestGV->setAlignment(Alignment);
}

std::map<Port*, llvm::GlobalVariable*>* JIT::createPorts(Instance* instance, map<string, Port*>* ports){
	map<Port*, llvm::GlobalVariable*>* newPorts = new map<Port*, llvm::GlobalVariable*>();
	map<string, Port*>::iterator it;

	for (it = ports->begin(); it != ports->end(); ++it){
		newPorts->insert(createPort(instance, (*it).second));
	}

	return newPorts;
}

//Creation of procedure must be done in two times because function can call other functions
map<Procedure*, Function*>* JIT::createProcedures(Instance* instance, map<string, Procedure*>* procs){
	map<Procedure*, Function*>* newProcs = new map<Procedure*, Function*>();
	map<string, Procedure*>::iterator it;

	
	//Creates function declaration
	for (it = procs->begin(); it != procs->end(); ++it){
		Procedure* proc = (*it).second;
		Function* newFunction = (Function*)addFunctionProtos(instance->getId()+"_", proc->getFunction());
		newProcs->insert(pair<Procedure*, Function*>(proc, newFunction));
	}

	//Link body
	for (it = procs->begin(); it != procs->end(); ++it){
		Procedure* proc = (*it).second;
		LinkProcedureBody(proc->getFunction());
	}
	
	return newProcs;
}

pair<Port*, llvm::GlobalVariable*> JIT::createPort(Instance* instance, Port* port){
	GlobalVariable* portVar = port->getGlobalVariable();
	GlobalVariable* var = CreateVariable(instance, portVar);
	return pair<Port*, llvm::GlobalVariable*>(port, var);
}


map<Variable*, GlobalVariable*>* JIT::createVariables(Instance* instance, map<string, Variable*>* vars){
	map<string, Variable*>::iterator it;
	map<Variable*, GlobalVariable*>* newVars = new map<Variable*, GlobalVariable*>();

	for (it = vars->begin(); it != vars->end(); ++it){
		
		Variable* var = (*it).second;
		GlobalVariable* newVar = CreateVariable(instance, var->getGlobalVariable());

		newVars->insert(pair<Variable*, GlobalVariable*>(var, newVar));
	}

	return newVars;
}

GlobalVariable* JIT::CreateVariable(Instance* instance, GlobalVariable* variable){
	GlobalVariable* var = addVariable(instance->getId()+"_", variable);
	LinkGlobalInits(variable);

	return var;
}


list<Action*>* JIT::createActions(Instance* instance, list<Action*>* actions){
	list<Action*>* newActions = new list<Action*>();
	
	list<Action*>::iterator it;

	for (it = actions->begin(); it != actions->end(); ++it){
		newActions->push_back(createAction(instance, *it));
	}

	return newActions;
}


Action* JIT::createAction(Instance* instance, Action* action){
		
		Procedure* scheduler = action->getScheduler();
		Procedure* body = action->getBody();
	
		Procedure* newScheduler = CreateProcedure(instance, scheduler);
		Procedure* newBody = CreateProcedure(instance, body);

		return new Action(NULL, newScheduler, newBody);
}

Procedure* JIT::CreateProcedure(Instance* instance, Procedure* procedure){
	Function* function = CreateFunction(instance, (Function*)procedure->getFunction());
	
	return new Procedure(procedure->getName(), procedure->getExternal(), function);
}

Function* JIT::CreateFunction(Instance* instance, Function* function){
	Function* newFunction = (Function*)addFunctionProtos(instance->getId()+"_", function);
	LinkProcedureBody(function);

	return newFunction;
}

FSM* JIT::createFSM(Instance* instance, FSM* fsm){
	list<llvm::Function*>::iterator it;
	
	FSM* newFSM = new FSM();
	
	//Create a new fsm_state
	GlobalVariable* fsmState = CreateVariable(instance, fsm->getFsmState());
	newFSM->setFsmState(fsmState);
	
	//Create a outside fsm if present
	if(fsm->getOutFsm() != NULL){
		Function* function = CreateFunction(instance, fsm->getOutFsm());
		newFSM->setOutFsm(function);
	}

	//Create state scheduler functions
	list<llvm::Function*>* functions = fsm->getFunctions();
	list<llvm::Function*>* newFunctions = new list<llvm::Function*>();
	
	for (it = functions->begin(); it != functions->end(); ++it){
		Function* function = CreateFunction(instance, *it);
		newFunctions->push_back(function);
	}

	newFSM->setFunctions(newFunctions);

	return newFSM;
}

ActionScheduler* JIT::createActionScheduler(Instance* instance, ActionScheduler* actionScheduler){
	FSM* fsm = NULL;
	Function* initializeFunction = NULL;
	
	//Create FSM if present
	if (actionScheduler->hasFsm()){
		fsm = createFSM(instance, actionScheduler->getFsm());
	}

	//Create intialize scheduler if present
	if (actionScheduler->hasInitializeScheduler()){
		initializeFunction = CreateFunction(instance, actionScheduler->getInitializeFunction());
	}
	
	//Create action scheduler
	Function* schedulerFunction = CreateFunction(instance, actionScheduler->getSchedulerFunction());

	return new ActionScheduler(schedulerFunction, initializeFunction, fsm);
}