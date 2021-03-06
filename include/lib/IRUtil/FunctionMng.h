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
@brief Description of the FifoMng class interface
@author Jerome Gorin
@file FifoMng.h
@version 1.0
@date 15/11/2010
*/

//------------------------------
#ifndef FUNCTIONMNG_H
#define FUNCTIONMNG_H
#include <string>
#include <vector>

class Decoder;

namespace llvm{
class Instruction;
}

//------------------------------

/**
 * @class FunctionMng
 *
 * @brief This class contains methods for managing llvm functions.
 *
 * @author Jerome Gorin
 *
 */
class FunctionMng {

public:

    /**
     *  @brief Create a puts to display the given message
     *
     */
    static llvm::Function* createPrintf(llvm::Module* module, std::string message = "", llvm::Instruction* instr = NULL, llvm::Value* value = NULL);

    static llvm::Function* createPrintf(llvm::Module* module, std::string message, llvm::Instruction* instr, std::vector<llvm::Value*> values);

    static void createPuts(llvm::Module* module, std::string message, llvm::Instruction* instr);

    static llvm::Constant* createStdMessage(llvm::Module* module, std::string message);
};

#endif
