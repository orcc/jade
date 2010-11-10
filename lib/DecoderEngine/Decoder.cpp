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
@brief Implementation of class Decoder
@author Jerome Gorin
@file Decoder.cpp
@version 0.1
@date 2010/04/12
*/

//------------------------------
#include <list>
#include <iostream>
#include <fstream>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"

#include "Instantiator.h"

#include "Jade/Decoder.h"
#include "Jade/Fifo/AbstractFifo.h"
#include "Jade/Core/Network.h"
#include "Jade/Fifo/AbstractFifo.h"
#include "Jade/Serialize/IRWriter.h"
#include "Jade/Scheduler/RoundRobinScheduler.h"
#include "Jade/Transform/ActionSchedulerAdder.h"
#include "Jade/Transform/BroadcastAdder.h"
//------------------------------

using namespace llvm;
using namespace std;

Decoder::Decoder(llvm::LLVMContext& C, Network* network, AbstractFifo* fifo): Context(C){
	
	//Set property of the decoder
	this->network = network;
	this->fifo = fifo;
	this->instances = network->getInstances();

	//Create a new module that contains the current decoder
	module = new Module("decoder", C);
}

Decoder::~Decoder (){
	delete module;
}

void Decoder::addInstance(Instance* instance){
	instances->insert(std::pair<std::string, Instance*>(instance->getId(), instance));
};

Actor* Decoder::getActor(std::string name){
	map<string, Actor*>::iterator it;

	it = actors->find(name);

	if(it != actors->end()){
		return it->second;
	}

	return NULL;
}


Instance* Decoder::getInstance(std::string name){
	map<string, Instance*>::iterator it;

	it = instances->find(name);

	if (it == instances->end()){
		return NULL;
	}

	return it->second;
}

bool Decoder::compile(map<string, Actor*>* actors){
	map<string, Instance*>::iterator it;
	this->actors = actors;

	// Add Fifo function and fifo type into the decoder
	fifo->addFifoHeader(this);
	
	// Instanciate the network
	Instantiator Instantiator(network, actors);

	// Adding broadcast 
	BroadcastAdder broadAdder(Context, this);
	broadAdder.transform();

	//Write instance
	for (it = instances->begin(); it != instances->end(); it++){
		IRWriter writer(it->second);
		writer.write(this);
	}

	//Adding action scheduler
	ActionSchedulerAdder actionSchedulerAdder(Context, this);
	actionSchedulerAdder.transform();

	// Setting connections of the decoder
	fifo->setConnections(this);

	return true;
}

void Decoder::setScheduler(RoundRobinScheduler* scheduler){
	this->scheduler = scheduler;
	scheduler->createScheduler(this);
}

void Decoder::start(){
	scheduler->execute();
}