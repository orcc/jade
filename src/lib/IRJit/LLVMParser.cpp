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
@brief Implementation of LLVMParser
@author Jerome Gorin
@file LLVMParser.cpp
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <iostream>

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"

#include "lib/IRJit//LLVMParser.h"
#include "lib/IRUtil/PackageMng.h"
//------------------------------

using namespace llvm;
using namespace std;

LLVMParser::LLVMParser(LLVMContext& C, string directory, bool verbose): Context(C){
    this->directory = directory;
    this->verbose = verbose;
}


Module* LLVMParser::loadModule(Package* package, string file) {
    SMDiagnostic Err;
    Module *Mod;

    //Get filename of the actor
    string Filename(directory + package->getDirectory() + "/" + file);

    Mod = ParseIRFile(Filename, Err, Context);

    if (verbose) cout << "Loading '" << Filename << "'" << endl;

    if (!Mod) {
        //Error when parsing module
        cerr << "Error parsing bitcode file '" << file << "' at line " << Err.getLineNo() << endl;
        cerr << Err.getMessage().str() << endl;
        cerr << Err.getFilename().str() << endl;
        exit(1);
    }

    return Mod;
}
