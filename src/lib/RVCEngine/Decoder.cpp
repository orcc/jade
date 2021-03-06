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
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <list>
#include <iostream>
#include <fstream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Host.h"

#include "lib/RVCEngine/Decoder.h"
#include "lib/IRCore/Network.h"
#include "lib/ConfigurationEngine/ConfigurationEngine.h"
#include "lib/IRJit/LLVMExecution.h"
#include "lib/IRJit/LLVMArmFix.h"
#include "lib/RoundRobinScheduler/RoundRobinScheduler.h"
//------------------------------

using namespace llvm;
using namespace std;

Decoder::Decoder(LLVMContext& C, Configuration* configuration, bool verbose, bool armFix): Context(C){

    //Set property of the decoder
    this->configuration = configuration;
    this->verbose = verbose;
    this->executionEngine = NULL;
    this->fifoFn = NULL;
    this->running = false;
    this->scheduler = NULL;
    this->armFix = armFix;

    //Create a new module that contains the current decoder
    module = new Module("decoder", C);

    //Configure the decoder
    ConfigurationEngine engine(Context, verbose);
    engine.configure(this);

    //Set schedulers of the decoder
    map<string, Partition*>::iterator itPartition;
    map<string, Partition*>* partitions = configuration->getPartitions();

    // Unpartitionned instance scheduler
    scheduler = new RoundRobinScheduler(Context, this, configuration->getUnpartitioned(), configuration->mergeActors(), verbose);

    // Partitionned instance scheduler
    for(itPartition = partitions->begin(); itPartition != partitions->end(); itPartition++){
        Partition* partition = itPartition->second;
        Scheduler* procSchedul = new RoundRobinScheduler(Context, this, partition->getInstances(), configuration->mergeActors(), verbose);
        procSchedulers.insert(pair<Partition*, Scheduler*>(partition, procSchedul));
    }

    //Create execution engine
    if (armFix) {
        executionEngine = new LLVMArmFix(Context, this, verbose);
    } else {
        executionEngine = new LLVMExecution(Context, this, verbose);
    }
}

Decoder::~Decoder (){
    delete scheduler;
    delete module;
}

list<Procedure*> Decoder::getExternalProcs(){
    list<Procedure*> externs;

    // Look across all instances for external procedures
    map<std::string, Instance*>* instances = configuration->getInstances();
    for (map<std::string, Instance*>::iterator it = instances->begin(); it != instances->end(); it++){
        Instance* instance = it->second;

        // Look accross all procs for external property
        map<string, Procedure*>::iterator itProc;
        map<string, Procedure*>* procs = instance->getProcs();

        for (itProc = procs->begin(); itProc != procs->end(); itProc++){
            Procedure* proc = itProc->second;

            if (proc->isExternal()){
                externs.push_back(proc);
            }

        }
    }

    return externs;
}

void Decoder::setConfiguration(Configuration* newConfiguration){
    clock_t start = clock ();
    /*if (running){
        //Decoder is currently running
        cerr << "Can't set a configuration, the decoder is currently running.";
        exit(1);
    }*/

    //Reconfigure the decoder
    ConfigurationEngine engine(Context, verbose);
    engine.reconfigure(this, newConfiguration);
    if (verbose){
        cout << "---> Reconfiguring times of engines takes " << (clock () - start) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
        start = clock();
    }

    //Delete old configuration and set the new one
    delete configuration;
    configuration = newConfiguration;

    executionEngine->recompile(scheduler->getMainFunction());

    if (verbose){
        cout << "---> Scheduling recompilation takes " << (clock () - start) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
    }
}

void Decoder::run(){
    running = true;
    executionEngine->initialize();
    executionEngine->run();
}

void Decoder::stop(){
    executionEngine->stop();

    running = false;

    ConfigurationEngine engine(Context);
    engine.reinit(this);
}
