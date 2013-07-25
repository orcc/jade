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
@brief Description of Jade console
@author Jerome Gorin
@file Console.cpp
@version 1.0
@date 18/07/2011
*/

//------------------------------
#include <iostream>

#include "llvm/Support/CommandLine.h"

#include "lib/RVCEngine/RVCEngine.h"
#include "lib/XDFSerialize/XDFParser.h"
#include "lib/Scenario/Manager.h"
#include "lib/Scenario/Event/LoadEvent.h"
#include "lib/Scenario/Event/StartEvent.h"
#include "lib/Scenario/Event/StopEvent.h"
#include "lib/Scenario/Event/SetEvent.h"
#include "lib/Scenario/Event/WaitEvent.h"
#include "lib/Scenario/Event/PauseEvent.h"
#include "lib/Scenario/Event/PrintEvent.h"
#include "lib/Scenario/Event/VerifyEvent.h"

#include "Console.h"
//------------------------------

using namespace std;
using namespace llvm;
using namespace llvm::cl;

extern opt<std::string> OutputDir;
extern cl::opt<std::string> VTLDir;
extern cl::opt<std::string> InputDir;
extern RVCEngine* engine;

//Console control
Manager* manager;

void parseConsole(string cmd) {

    StringRef cmd_ref(cmd);

    if (0 == cmd_ref.compare_lower("verify")) {
        int id;
        string file;

        //Select network
        cout << "Select the id of the network to verify : ";
        cin >> id;

        //Select network
        cout << "Select an output file in case of error : ";
        cin >> file;

        manager->startEvent(new VerifyEvent(id, OutputDir + file));

    } else if (0 == cmd_ref.compare_lower("set")) {
        string file;
        int id;

        //Select network
        cout << "Select the id of the network to change : ";
        cin >> id;

        //Select network
        cout << "Select a new network : ";
        cin >> file;

        manager->startEvent(new SetEvent(id, VTLDir + file));
    } else if (0 == cmd_ref.compare_lower("load")) {
        string file;
        int id;


        //Select the network file
        cout << "Select a network to load : ";
        cin >> file;

        //Store the network in an id
        cout << "Select an id for this network : ";
        cin >> id;

        manager->startEvent(new LoadEvent(VTLDir + file, id));
    } else if (0 == cmd_ref.compare_lower("print")) {
        string output;
        int id;

        //Select network
        cout << "Select the id of the network to print : ";
        cin >> id;

        //Select network
        cout << "Select an ouput file : ";
        cin >> output;

        manager->startEvent(new PrintEvent(id, OutputDir + output));

    } else if (0 == cmd_ref.compare_lower("start")) {
        string input;
        int id;

        //Select network
        cout << "Select the id of the network to start : ";
        cin >> id;

        //Select network
        cout << "Select an input stimulus : ";
        cin >> input;

        manager->startEvent(new StartEvent(id, InputDir + input, NULL));

    } else if (0 == cmd_ref.compare_lower("stop")) {
        int id;

        //Select network
        cout << "Select the id of the network to stop : ";
        cin >> id;

        manager->startEvent(new StopEvent(id));
    } else if (0 == cmd_ref.compare_lower("remove")) {
        int id;

        //Select network
        cout << "Select the id of the network to remove : ";
        cin >> id;

        manager->startEvent(new RemoveEvent(id));

    } else if (0 == cmd_ref.compare_lower("list")) {
        manager->startEvent(new ListEvent());

    } else if (0 == cmd_ref.compare_lower("help")) {
        cout << "Command line options :\n";
        cout << "List : view a list of the networks loads \n";
        cout << "Load : load a network \n";
        cout << "Print : print a network \n";
        cout << "Remove : remove a network \n";
        cout << "Set : change a network to another\n";
        cout << "Start : start a network \n";
        cout << "Stop : stop a network \n";
        cout << "Verify : verify a network\n";
        cout << "X : exit console \n";
    }
}

void startConsole() {
    string cmdLine;
    manager = new Manager(engine);
    while (cmdLine != "X" && cmdLine != "x") {
        cout << "Enter a command (help for documentation) : ";
        cin >> cmdLine;
        parseConsole(cmdLine);
    }
}
