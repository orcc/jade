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
@brief Implementation of class Scenario
@author Jerome Gorin
@file Scenario.cpp
@version 1.0
@date 26/01/2011
*/

//------------------------------
#include <iostream>

#include "llvm/IR/LLVMContext.h"

#include "lib/RVCEngine/RVCEngine.h"
#include "lib/XDFSerialize/XDFParser.h"
#include "lib/XCFSerialize/XCFParser.h"
#include "lib/Scenario/Manager.h"

#include "ScenarioParser.h"

extern "C" {
extern char* input_file;
}

//------------------------------

using namespace std;
using namespace llvm;

Manager::Manager(RVCEngine* engine, int optLevel, bool verify, bool verbose){
    this->engine = engine;
    this->verbose = verbose;
    this->verify = verify;
    this->optLevel = optLevel;
}

bool Manager::start(std::string scFile){
    clock_t start = clock ();
    if (verbose){
        cout << "-> Parsing scenario file : " << scFile << endl;
    }

    // Parse the scenario file
    ScenarioParser parser(scFile);
    Scenario* scenario = parser.parse();

    if (scenario == NULL){
        cerr << "Error of scenario parsing" << endl;
        return false;
    }

    if (verbose){
        cout << "-> Scenario parsing finished in "<< (clock () - start) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
        cout << "-> Start scenario :" << endl;
    }

    while (!scenario->end()){
        //Get a first event
        Event* curEvent = scenario->getEvent();

        //Execute event
        if (!startEvent(curEvent)){
            cerr << "Getting an while running events. \n ";
            return false;
        }
    }

    //Manager executed all events properly
    return true;
}

bool Manager::startEvent(Event* newEvent){
    if (newEvent->isStartEvent()){
        return runStartEvent((StartEvent*)newEvent);
    }else if (newEvent->isLoadEvent()){
        return runLoadEvent((LoadEvent*)newEvent);
    }else if (newEvent->isSetEvent()){
        return runSetEvent((SetEvent*)newEvent);
    }else if (newEvent->isStopEvent()){
        return runStopEvent((StopEvent*)newEvent);
    }else if (newEvent->isWaitEvent()){
        return runWaitEvent((WaitEvent*)newEvent);
    }else if (newEvent->isPauseEvent()){
        return runPauseEvent((PauseEvent*)newEvent);
    }else if (newEvent->isVerifyEvent()){
        return runVerifyEvent((VerifyEvent*)newEvent);
    }else if (newEvent->isPrintEvent()){
        return runPrintEvent((PrintEvent*)newEvent);
    }else if (newEvent->isRemoveEvent()){
        return runRemoveEvent((RemoveEvent*)newEvent);
    }else if (newEvent->isListEvent()){
        return runListEvent((ListEvent*)newEvent);
    }else{
        cerr << "Unrecognize event. \n ";
        return false;
    }


    return true;
}

bool Manager::runLoadEvent(LoadEvent* loadEvent){
    clock_t timer1 = clock ();
    clock_t timer2 = clock ();

    if (verbose){
        cout << "-> Execute load event :" << endl;
        cout << "--> Parsing network :" << endl;
    }
    //Load network
    LLVMContext &Context = getGlobalContext();
    XDFParser xdfParser(verbose);
    Network* network = xdfParser.parseFile(loadEvent->getFile(), Context);

    if (network == NULL){
        cerr << "Event error ! No network load." << endl;
        return false;
    }

    if (verbose){
        cout << "--> Parsing network finished in : "<< (clock () - timer1) * 1000 / CLOCKS_PER_SEC << "ms." << endl;
        cout << "--> Loading decoder :" << endl;
        timer1 = clock ();
    }

    //Loading decoder
    engine->load(network);

    // Optimize decoder
    if (optLevel > 0){
        engine->optimize(network, optLevel);
    }

    int id = loadEvent->getId();
    //Store resulting network
    networks.insert(pair<int, Network*>(id, network));

    if (verify){
        engine->verify(network, "error.txt");
    }

    if (verbose){
        cout << "--> Load decoder finished in : "<< (clock () - timer1) * 1000 / CLOCKS_PER_SEC << "ms." << endl;
        cout << "-> Load event executed in :" << (clock () - timer2) * 1000 / CLOCKS_PER_SEC <<"" << endl;
    }
    return true;
}

bool Manager::runStartEvent(StartEvent* startEvent) {
    clock_t timer = clock ();
    if (verbose){
        cout << "-> Execute start event :" << endl;
    }

    //Get network
    netPtr = networks.find(startEvent->getId());

    if(netPtr == networks.end()){
        cerr << "Event error ! No network loads at id " << startEvent->getId();
        return false;
    }

    //Set input file
    string input = startEvent->getInput();
    input_file = (char*)input.c_str();

    string mappingFile = startEvent->mappingFile();
    if(!mappingFile.empty()) {

        std::cout << "Parsing file " << mappingFile << "." << endl;

        XCFParser xcfParser(verbose);
        map<string, string>* mapping = xcfParser.parseFile(mappingFile);
        netPtr->second->setMapping(mapping);
    }

    //Execute network
    engine->run(netPtr->second);

    if (verbose){
        cout << "-> Decoder started in :"<< (clock () - timer) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
    }

    return true;
}

bool Manager::runWaitEvent(WaitEvent* waitEvent){
    if (verbose){
        cout << "-> Execute wait event :" << endl;
    }
    //sys::Sleep(waitEvent->getTime());
    cout << "-> Wait is deprecated." << endl;

    if (verbose){
        cout << "-> Wait is event." << endl;
    }
    return true;
}

bool Manager::runVerifyEvent(VerifyEvent* verifyEvent){
    clock_t timer = clock ();
    if (verbose){
        cout << "-> Execute verify event :" << endl;
    }

    //Get network
    netPtr = networks.find(verifyEvent->getId());

    if(netPtr == networks.end()){
        cerr << "Event error ! No network loads at id " << verifyEvent->getId();
        return false;
    }

    engine->verify(netPtr->second, verifyEvent->getFile());

    if (verbose){
        cout << "-> Decoder verified in : "<< (clock () - timer) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
    }
    return true;
}

bool Manager::runPrintEvent(PrintEvent* printEvent){
    clock_t timer = clock ();
    if (verbose){
        cout << "-> Execute print event :" << endl;
    }
    //Get network
    netPtr = networks.find(printEvent->getId());

    if(netPtr == networks.end()){
        cerr << "Event error ! No network loads at id " << printEvent->getId();
        return false;
    }

    engine->print(netPtr->second, printEvent->getFile());

    if (verbose){
        cout << "-> Decoder printed in : "<< (clock () - timer) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
    }
    return true;
}

bool Manager::runPauseEvent(PauseEvent* waitEvent){
    if (verbose){
        cout << "-> Execute pause event :" << endl;
    }

    std::string crs;
    cout << "Scenario pause, press any key to continue." << endl;
    cin >> crs;

    if (verbose){
        cout << "-> End of pause event." << endl;
    }
    return true;
}

bool Manager::runStopEvent(StopEvent* stopEvent){
    clock_t timer = clock ();
    if (verbose){
        cout << "-> Execute stop event :" << endl;
    }

    //Get network
    netPtr = networks.find(stopEvent->getId());

    if(netPtr == networks.end()){
        cerr << "Event error ! No network loads at the given id." << endl;
        return false;
    }

    //Stop the given network
    engine->stop(netPtr->second);

    if (verbose){
        cout << "-> Decoder stopped in : "<< (clock () - timer) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
    }

    return true;
}

bool Manager::runSetEvent(SetEvent* setEvent){
    clock_t timer1 = clock ();
    clock_t timer2 = clock ();
    if (verbose){
        cout << "-> Execute set event :" << endl;
    }

    //Get the network
    int id = setEvent->getId();
    netPtr = networks.find(id);
    if(netPtr == networks.end()){
        cout << "Event error ! No network loads at the given id." << endl;
        return false;
    }

    if (verbose){
        cout << "--> Start parsing network :" << endl;
    }

    //Load network
    LLVMContext &Context = getGlobalContext();
    XDFParser xdfParser(verbose);
    Network* network = xdfParser.parseFile(setEvent->getFile(), Context);

    if (network == NULL){
        cout << "No network load." << endl;
        return false;
    }

    if (verbose){
        cout << "--> Network parsed in : "<< (clock () - timer1) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
        cout << "--> Reconfiguring decoder :" << endl;
        timer1 = clock();
    }

    //Reconfiguration decoder
    engine->reconfigure(netPtr->second, network);

    if (verbose){
        cout << "--> Decoder reconfigured in : "<< (clock () - timer1) * 1000 / CLOCKS_PER_SEC <<" ms." << endl;
        cout << "--> Executed set event in "<< (clock () - timer2) * 1000 / CLOCKS_PER_SEC  << " ms." << endl;
    }

    //Set the new network
    networks.erase(id);
    networks.insert(pair<int, Network*>(id, network));

    return true;
}

bool Manager::runRemoveEvent(RemoveEvent* removeEvent){
    clock_t timer = clock ();
    if (verbose){
        cout << "-> Execute remove event :" << endl;
    }

    //Get the network
    int id = removeEvent->getId();
    netPtr = networks.find(id);

    if(netPtr == networks.end()){
        cout << "Event error ! No network loads at the given id." << endl;
        return false;
    }

    //Remove network
    engine->unload(netPtr->second);
    networks.erase(netPtr);
    delete netPtr->second;

    if (verbose){
        cout << "-> Remove event executed in :"<< (clock () - timer) * 1000 / CLOCKS_PER_SEC  << " ms." << endl;
    }

    return true;
}

bool Manager::runListEvent(ListEvent* listEvent){
    if (verbose){
        cout << "-> Execute list event :" << endl;
    }

    map<int, Network*>::iterator it;
    string input;

    for (it = networks.begin(); it != networks.end(); it++){
        Network* network = it->second;
        cout << it->first << " : " << network->getName() << "" << endl;
    }

    return true;
}
