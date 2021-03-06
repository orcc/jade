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
@brief Implementation of class PackageMng
@author Jerome Gorin
@file FunctionMng.cpp
@version 1.0
@date 22/10/2011
*/

//------------------------------
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Attributes.h"

#include "lib/RVCEngine/Decoder.h"
#include "lib/IRUtil/FunctionMng.h"
//------------------------------

using namespace std;
using namespace llvm;

Function* FunctionMng::createPrintf(Module* module, string message, Instruction* instr, Value* value){
    vector<Value*> values;

    if (value != NULL){
        values.push_back(value);
    }

    return createPrintf(module, message, instr, values);
}

Function* FunctionMng::createPrintf(Module* module, string message, Instruction* instr, vector<Value*> values){
    Function* func_printf = module->getFunction("printf");
    if (!func_printf) {
        // Printf does'nt exist, create it
        PointerType* PointerTy_0 = PointerType::get(IntegerType::get(module->getContext(), 8), 0);

        std::vector<Type*>FuncTy_8_args;
        FuncTy_8_args.push_back(PointerTy_0);
        FunctionType* FuncTy_8 = FunctionType::get(IntegerType::get(module->getContext(), 32), FuncTy_8_args, true);

        func_printf = Function::Create(FuncTy_8, GlobalValue::ExternalLinkage, "printf", module); // (external, no body)
        func_printf->setCallingConv(CallingConv::C);

        AttributeSet func_printf_PAL;
        func_printf->setAttributes(func_printf_PAL);
    }

    if (instr== NULL){
        return func_printf;
    }

    // Create the message
    Value* messageExpr = createStdMessage(module, message);


    // Create arguments
    std::vector<Value*> params;
    params.push_back(messageExpr);

    if (!values.empty()){
        vector<Value*>::iterator it;

        for (it = values.begin(); it != values.end(); ++it){
            Value* value = *it;
            Type* type = value->getType();
            if (type->isIntegerTy()){

                IntegerType* intTy = cast<IntegerType>(type);
                if (intTy->getBitWidth() < 32){
                    value = new ZExtInst(value, Type::getInt32Ty(module->getContext()), "", instr);
                }else if (intTy->getBitWidth() > 32){
                    value = new TruncInst (value, Type::getInt32Ty(module->getContext()), "", instr);
                }

                params.push_back(value);
            }
        }

    }

    CallInst* int32_25 = CallInst::Create(func_printf, params, "", instr);
    int32_25->setCallingConv(CallingConv::C);
    int32_25->setTailCall(false);
    AttributeSet int32_25_PAL;
    int32_25->setAttributes(int32_25_PAL);

    return func_printf;
}

void FunctionMng::createPuts(Module* module, string message, Instruction* instr){
    Function* func_puts = module->getFunction("puts");

    if (func_puts == NULL){
        // Puts does'nt exist, create it
        PointerType* PointerTy_4 = PointerType::get(IntegerType::get(module->getContext(), 8), 0);

        vector<Type*>FuncTy_6_args;
        FuncTy_6_args.push_back(PointerTy_4);
        FunctionType* FuncTy_6 = FunctionType::get(IntegerType::get(module->getContext(), 32), FuncTy_6_args, false);

        func_puts = Function::Create(FuncTy_6, GlobalValue::ExternalLinkage,"puts", module);
        func_puts->setCallingConv(CallingConv::C);

        AttributeSet func_puts_PAL;
        func_puts_PAL.addAttribute(module->getContext(), 1U, Attribute::NoCapture);
        func_puts_PAL.addAttribute(module->getContext(), 4294967295U, Attribute::NoUnwind);
        func_puts->setAttributes(func_puts_PAL);
    }

    // Create the message
    Value* messageExpr = createStdMessage(module, message);


    // Call puts
    CallInst* int32_puts = CallInst::Create(func_puts, messageExpr, "puts", instr);
    int32_puts->setCallingConv(CallingConv::C);
    int32_puts->setTailCall(true);
    AttributeSet int32_puts_PAL;
    int32_puts->setAttributes(int32_puts_PAL);
}


Constant* FunctionMng::createStdMessage(Module* module, string message){
    std::vector<Constant*> msgArray;
    msgArray.reserve(message.size() + 1);
    for (unsigned i = 0; i < message.size(); ++i){
        msgArray.push_back(ConstantInt::get(Type::getInt8Ty(module->getContext()), message[i]));
    }
    // Add a null terminator to the string...
    msgArray.push_back(ConstantInt::get(Type::getInt8Ty(module->getContext()), 0));

    ArrayType *ATy = ArrayType::get(Type::getInt8Ty(module->getContext()), msgArray.size());
    GlobalVariable* gvar_array_str = new GlobalVariable(*module, ATy, true, GlobalValue::InternalLinkage, ConstantArray::get(ATy, msgArray), "str");

    std::vector<Constant*> const_ptr_8_indices;
    ConstantInt* const_int64_9 = ConstantInt::get(module->getContext(), APInt(64, StringRef("0"), 10));
    const_ptr_8_indices.push_back(const_int64_9);
    const_ptr_8_indices.push_back(const_int64_9);

    return ConstantExpr::getGetElementPtr(gvar_array_str, const_ptr_8_indices);
}
