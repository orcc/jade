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
@brief Implementation of class IRParser
@author Jerome Gorin
@file IRParser.cpp
@version 0.1
@date 2010/04/12
*/

//------------------------------
#include <map>
#include <iostream>

#include "llvm/Constants.h"
#include "llvm/ValueSymbolTable.h"
#include "llvm/Metadata.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Module.h"

#include "Jade/Core/Actor/ActionScheduler.h"
#include "Jade/Core/Actor/Action.h"
#include "Jade/Core/Actor/ActionTag.h"
#include "Jade/Core/Actor.h"
#include "Jade/Core/Actor/FSM.h"
#include "Jade/Serialize/IRParser.h"
#include "Jade/Core/Port.h"
#include "Jade/Core/StateVariable.h"
#include "Jade/Core/Actor/Procedure.h"
#include "Jade/Fifo/AbstractFifo.h"
#include "Jade/Jit/LLVMParser.h"


#include "IRConstant.h"
//------------------------------

using namespace std;
using namespace llvm;

extern cl::opt<std::string> VTLDir;
extern cl::opt<bool> nodisplay;

// Define MDNode keys
const std::string IRConstant::KEY_ACTION_SCHED = "action_scheduler";
const std::string IRConstant::KEY_ACTIONS= "actions";
const std::string IRConstant::KEY_INITIALIZES= "initializes";
const std::string IRConstant::KEY_INPUTS= "inputs";
const std::string IRConstant::KEY_NAME= "name";
const std::string IRConstant::KEY_OUTPUTS= "outputs";
const std::string IRConstant::KEY_PARAMETERS= "parameters";
const std::string IRConstant::KEY_PROCEDURES= "procedures";
const std::string IRConstant::KEY_SOURCE_FILE= "source_file";
const std::string IRConstant::KEY_STATE_VARS= "state_variables";


IRParser::IRParser(llvm::LLVMContext& C, AbstractFifo* fifo) : Context(C){
	this->fifo = fifo;
	this->inputs = NULL;
	this->outputs = NULL;
}


Actor* IRParser::parseActor(string classz){
    size_t found=classz.find_last_of("/");;
	string file;

	if (found!=string::npos){
		file = classz.substr(found + 1);
	}else {
		file = classz;
	}

	//Don't load actor display if option nodisplay is selected
	if(nodisplay){
		if (classz.compare("Display")==0){
			file = "Nodisplay";
		}
	}
	
	//Parse the bitcode
	LLVMParser parser(Context, VTLDir);
	Module* module = parser.loadBitcode(file);

	if (module == 0){
		cerr << "Error when parsing bytecode";
		exit(0);
	}

	//Empty action list
	actions.clear();
	untaggedActions.clear();
	
	// Parse name
	NamedMDNode* nameNMD =  module->getNamedMetadata(IRConstant::KEY_NAME);
	MDNode* nameMD =cast<MDNode>(nameNMD->getOperand(0));
	MDString* name = cast<MDString>(nameMD->getOperand(0));

	// Parse actor elements
	map<string, Type*>* fifos = parseFifos(module);
	inputs = parsePorts(IRConstant::KEY_INPUTS, module);
	outputs = parsePorts(IRConstant::KEY_OUTPUTS, module);
	map<string, Variable*>* parameters =  parseParameters(module);
	map<string, Variable*>* stateVars = parseStateVars(module);
	map<string, Procedure*>* procs = parseProcs(module);
	list<Action*>* initializes = parseActions(IRConstant::KEY_INITIALIZES, module);
	list<Action*>* actions = parseActions(IRConstant::KEY_ACTIONS, module);
	ActionScheduler* actionScheduler = parseActionScheduler(module);

	return new Actor(name->getString(), classz, fifos, inputs, outputs, stateVars, 
						parameters, procs, initializes, actions, actionScheduler);
}

map<Port*, ConstantInt*>* IRParser::parsePattern(map<std::string, Port*>* ports, Value* value){
	map<Port*, ConstantInt*>* pattern = new map<Port*, ConstantInt*>();
	
	if (value != NULL){
		MDNode* patternNode = cast<MDNode>(value);
		map<std::string, Port*>::iterator it;

		for (unsigned i = 0, e = patternNode->getNumOperands(); i != e;) {
			//Find port of the pattern
			MDNode* portNode = cast<MDNode>(patternNode->getOperand(i));
			MDString* name = cast<MDString>(portNode->getOperand(1));
			it = ports->find(name->getString());
			
			//Get number of token consummed by this port
			ConstantInt* numTokens = cast<ConstantInt>(patternNode->getOperand(++i));
			pattern->insert(pair<Port*, ConstantInt*>(it->second, numTokens));
			i++;
		}
	}

	return pattern;
}

map<string, Port*>* IRParser::parsePorts(string key, Module* module){
	map<string, Port*>* ports = new map<string, Port*>();
	
	NamedMDNode* inputsMD =  module->getNamedMetadata(key);

	if (inputsMD == NULL) {
		return ports;
	}
	
	 for (unsigned i = 0, e = inputsMD->getNumOperands(); i != e; ++i) {
		 Port* port = parsePort(inputsMD->getOperand(i));
		 ports->insert(pair<string,Port*>(port->getName(), port));
	 }

	return ports;
}


map<string, Type*>* IRParser::parseFifos(Module* module){
	map<string,Type*>* fifos = new map<string,Type*>();
	std::map<std::string, llvm::Type*>::iterator it;

	std::map<std::string, llvm::Type*>* abstractFifoTypes = fifo->getFifoTypes();
	
	for (it = abstractFifoTypes->begin(); it != abstractFifoTypes->end(); it++){
		string structName = it->first;
		fifos->insert(pair<string, Type*>(structName, parseFifo(structName, module)));
	}	

	return fifos;
}

Type* IRParser::parseFifo(std::string name, Module* module){
	Type* type = (Type*)module->getTypeByName(name);

/*	if (type == NULL){
		fprintf(stderr,"Structure %s hasn't been found in a parsed actor.", name.c_str());
		exit(0);
	}
*/
	return type;
}


map<string, Procedure*>* IRParser::parseProcs(Module* module){
	map<string, Procedure*>* procedures = new map<string, Procedure*>();

	NamedMDNode* inputsMD =  module->getNamedMetadata(IRConstant::KEY_PROCEDURES);

	if (inputsMD != NULL){
		for (unsigned i = 0, e = inputsMD->getNumOperands(); i != e; ++i) {
			
			//Parse a procedure
			Procedure* proc= parseProc(inputsMD->getOperand(i));
			procedures->insert(pair<string, Procedure*>(proc->getName(), proc));
		}
		
	}

	return procedures;
}

ActionScheduler* IRParser::parseActionScheduler(Module* module){
	NamedMDNode* inputsMD =  module->getNamedMetadata(IRConstant::KEY_ACTION_SCHED);
	
	MDNode* actionSchedulerMD = cast<MDNode>(inputsMD->getOperand(0));
	list<Action*>* actions = new list<Action*>();
	FSM* fsm = NULL;

	//Get actions outside fsm if present
	Value* actionsValue = actionSchedulerMD->getOperand(0);

	if (actionsValue != NULL){
		MDNode* actionsNode =  cast<MDNode>(actionsValue);
		for (unsigned i = 0, e = actionsNode->getNumOperands(); i != e; ++i) {
			actions->push_back(getAction(cast<MDNode>(actionsNode->getOperand(i))));
		}
	}

	//Get fsm if present
	Value* fsmValue = actionSchedulerMD->getOperand(1);

	if (fsmValue != NULL){
		fsm = parseFSM(cast<MDNode>(fsmValue));
	}

	return new ActionScheduler(actions, fsm);
}

map<string, Variable*>* IRParser::parseParameters(Module* module){
	map<string, Variable*>* parameters = new map<string, Variable*>();
	
	NamedMDNode* inputsMD =  module->getNamedMetadata(IRConstant::KEY_PARAMETERS);
	if (inputsMD != NULL){
		for (unsigned i = 0, e = inputsMD->getNumOperands(); i != e; ++i) {
			
			//Parse a parameter
			MDNode* parameterNode = cast<MDNode>(inputsMD->getOperand(i));
			MDNode* details = cast<MDNode>(parameterNode->getOperand(0));
			MDString* nameMD = cast<MDString>(details->getOperand(0));

			Type* type = parseType(cast<MDNode>(parameterNode->getOperand(1)));

			GlobalVariable* variable = cast<GlobalVariable>(parameterNode->getOperand(2));

			//Parse create parameter
			StateVar* parameter = new StateVar(type, nameMD->getString(), false, variable);

			parameters->insert(pair<string, Variable*>(nameMD->getString(), parameter));
		}
		
	}
	return parameters;
}


map<string, Variable*>*  IRParser::parseStateVars(Module* module){
	map<string, Variable*>* stateVars = new map<string, Variable*>();

	NamedMDNode* stateVarsMD =  module->getNamedMetadata(IRConstant::KEY_STATE_VARS);
	
	if (stateVarsMD == NULL) {
		return stateVars;
	}
	
	for (unsigned i = 0, e = stateVarsMD->getNumOperands(); i != e; ++i) {
		 Variable* var = parseStateVar(stateVarsMD->getOperand(i));
		 stateVars->insert(pair<string,Variable*>(var->getName(), var));
	}

	return stateVars;
}


Variable* IRParser::parseStateVar(MDNode* node){
	// Parsing VarDef
	MDNode* varDefMD = cast<MDNode>(node->getOperand(0));
	
	MDString* name = cast<MDString>(varDefMD->getOperand(0));
	ConstantInt* assignable = cast<ConstantInt>(varDefMD->getOperand(1));

	IntegerType* type = (IntegerType*)parseType(cast<MDNode>(node->getOperand(1)));
	GlobalVariable* variable = cast<GlobalVariable>(node->getOperand(2));


	return new StateVar(type, name->getString(), assignable->getValue().getBoolValue(), variable);
}

list<Action*>* IRParser::parseActions(string key, Module* module){
	list<Action*>* actions = new list<Action*>();
	
	NamedMDNode* inputsMD =  module->getNamedMetadata(key);

	if (inputsMD == NULL) {
		return actions;
	}

	for (unsigned i = 0, e = inputsMD->getNumOperands(); i != e; ++i) {
		Action* action = parseAction(inputsMD->getOperand(i));
		actions->push_back(action);
	}

	return actions;
}


Action* IRParser::parseAction(MDNode* node){
	Value* tagValue = node->getOperand(0);
	ActionTag* tag = new ActionTag();
	
	if (tagValue != NULL){
		MDNode* tagArray = cast<MDNode>(tagValue);
		for (unsigned i = 0, e = tagArray->getNumOperands(); i != e; ++i) {
			MDString* tagMD = cast<MDString>(tagArray->getOperand(i));
			tag->add(tagMD->getString());
		}
	}

	map<Port*, ConstantInt*>* ip = parsePattern(inputs, node->getOperand(1));
	map<Port*, ConstantInt*>* op = parsePattern(outputs, node->getOperand(2));

	Procedure* scheduler = parseProc(cast<MDNode>(node->getOperand(3)));
	Procedure* body = parseProc(cast<MDNode>(node->getOperand(4)));
	Action* action = new Action(tag, ip, op, scheduler, body);

	putAction(tag, action);

	return action;
}


Procedure* IRParser::parseProc(MDNode* node){
	MDString* name = cast<MDString>(node->getOperand(0));
	ConstantInt* isExtern = cast<ConstantInt>(node->getOperand(1));
	Function* function = cast<Function>(node->getOperand(2));

	return new Procedure(name->getString(), isExtern, function);
}



Port* IRParser::parsePort(MDNode* node){
	Type* type = (Type*)parseType(cast<MDNode>(node->getOperand(0)));
	MDString* name = cast<MDString>(node->getOperand(1));
	GlobalVariable* variable = cast<GlobalVariable>(node->getOperand(2));
	
	return new Port(name->getString(), (IntegerType*)type, variable);
}

Type* IRParser::parseType(MDNode* node){
	
	//Get size of the element
	ConstantInt* typeSize = cast<ConstantInt>(node->getOperand(0));
	Type* type = (Type*)IntegerType::get(Context, typeSize->getLimitedValue());
	
	//if operand 2 is not null, the element is a list
	Value* listSize = cast<Value>(node->getOperand(1));
	
	if (listSize != NULL){
		for (unsigned i = 1, e = node->getNumOperands(); i != e; ++i){
			ConstantInt* size = cast<ConstantInt>(node->getOperand(i));
			type = ArrayType::get(type, size->getLimitedValue());
		}
		
	}

	return type;
}


FSM* IRParser::parseFSM(llvm::MDNode* node){
	FSM* fsm = new FSM();

	//Parse initial state
	MDString* initialState = cast<MDString>(node->getOperand(0));
	
	//Parse all fsm state
	MDNode* stateArray = cast<MDNode>(node->getOperand(1));

	for (unsigned i = 0, e = stateArray->getNumOperands(); i != e; ++i){
		MDString* stateMD = cast<MDString>(stateArray->getOperand(i));
		fsm->addState(stateMD->getString());
	}

	// set the initial state after initializing the states
	fsm->setInitialState(initialState->getString());

	
	//Parse transition
	MDNode* transitionsArray = cast<MDNode>(node->getOperand(2));

	for (unsigned i = 0, e = transitionsArray->getNumOperands(); i != e; ++i){
		MDNode* transitionArray = cast<MDNode>(transitionsArray->getOperand(i));
		MDString* source = cast<MDString>(transitionArray->getOperand(0));


		Value* targetValue = transitionArray->getOperand(1);
		
		//In case of "undefined" state, no target are given
		if (targetValue != NULL){
			MDNode* targetsArray = cast<MDNode>(targetValue);		
			for (unsigned j = 0, f = targetsArray->getNumOperands() ; j != f; ++j){
				MDNode* targetArray = cast<MDNode>(targetsArray->getOperand(j));

				Action* action = getAction(cast<MDNode>(targetArray->getOperand(0)));
				MDString* target = cast<MDString>(targetArray->getOperand(1));
			
				
				fsm->addTransition(source->getString(), target->getString(), action);
			}
		}
	}

	return fsm;
}

Action* IRParser::getAction(llvm::MDNode* node) {
	Value* idValue = node->getOperand(0);
	
	if (idValue == NULL){
		// removes the first untagged action found
		Action* action = untaggedActions.front();
		untaggedActions.remove(action);
		
		return action;
	}
	
	//Get identifiers of action tag
	map<std::string, Action*>::iterator it;
	ActionTag tag;
	MDNode* idNode = cast<MDNode>(idValue);
	
	for (unsigned i = 0, e = idNode->getNumOperands(); i != e; ++i){
		MDString* tagMD = cast<MDString>(idNode->getOperand(i));
		tag.add(tagMD->getString());
	}
	
	it = actions.find(tag.getIdentifier());
	
	return it->second;
}

void IRParser::putAction(ActionTag* tag, Action* action){
	if (tag->isEmpty()){
		untaggedActions.push_back(action);
	} else {
		actions.insert(pair<std::string, Action*>(tag->getIdentifier(), action));
	}

}