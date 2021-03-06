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
@brief Implementation of LLVMWriter
@author Jerome Gorin
@file LLVMWriter.cpp
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include "lib/RVCEngine/Decoder.h"
#include "lib/IRJit/LLVMWriter.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Attributes.h"
#include "llvm/Transforms/Utils/Cloning.h"
//------------------------------

using namespace std;
using namespace llvm;

LLVMWriter::LLVMWriter(string prefix, Decoder* decoder){
    this->decoder = decoder;
    this->module = decoder->getModule();
    this->prefix = prefix;
    this->fifoFns = decoder->getFifoFn();
    void createInstrisics();

}

GlobalVariable* LLVMWriter::createVariable(GlobalVariable* variable){
    GlobalVariable* var = addVariable(variable);
    LinkGlobalInits(variable);

    return var;
}

GlobalVariable* LLVMWriter::createPortVariable(Port* port){
    GlobalVariable* portVar = port->getFifoVar();

    GlobalVariable *newPortVar =  new GlobalVariable(*module, portVar->getType(),
                                                     true, portVar->getLinkage(), ConstantPointerNull::get(portVar->getType()),
                                                     prefix + portVar->getName());

    return newPortVar;
}

GlobalVariable* LLVMWriter::addVariable(llvm::GlobalVariable* variable){
    string Err;
    const GlobalVariable *SGV = variable;
    Module *Dest = module;

    // No linking to be performed, simply create an identical version of the
    // symbol over in the dest module... the initializer will be filled in
    // later by LinkGlobalInits.
    GlobalVariable *NewDGV =
            new GlobalVariable(*Dest, SGV->getType()->getElementType(),
                               SGV->isConstant(), SGV->getLinkage(), /*init*/0,
                               prefix + SGV->getName(), 0, GlobalVariable::NotThreadLocal,
                               SGV->getType()->getAddressSpace());
    // Propagate alignment, visibility and section info.
    CopyGVAttributes(NewDGV, SGV);

    // Make sure to remember this mapping.
    ValueMap[SGV] = NewDGV;

    return NewDGV;
}

bool LLVMWriter::LinkGlobalInits(llvm::GlobalVariable* variable){
    string Error;

    GlobalVariable *GV = cast<GlobalVariable>(ValueMap[variable]);

    if (variable->hasInitializer())
        GV->setInitializer(cast<Constant>(MapValue(variable->getInitializer(),
                                                   ValueMap)));
    GV->setLinkage(variable->getLinkage());
    GV->setThreadLocal(variable->isThreadLocal());
    GV->setConstant(variable->isConstant());

    return true;
}

/// CopyGVAttributes - copy additional attributes (those not needed to construct
/// a GlobalValue) from the SrcGV to the DestGV.
void LLVMWriter::CopyGVAttributes(GlobalValue *DestGV, const GlobalValue *SrcGV) {
    DestGV->copyAttributesFrom(SrcGV);
}

Function* LLVMWriter::createFunction(Function* function){
    Function* newFunction = (Function*)addFunctionProtosInternal(function);
    linkProcedureBody(function);

    return newFunction;
}

Function* LLVMWriter::addFunctionProtosInternal(const Function* function){
    const Function *SF = function;   // SrcFunction
    Module* Dest = module;
    std::string Err;

    Function *NF =
            Function::Create(cast<FunctionType>(SF->getType()->getElementType()),
                             GlobalValue::InternalLinkage,  prefix + SF->getName(), Dest);
    NF->copyAttributesFrom(SF);
    ValueMap[SF] = NF;

    return NF;
}

Function* LLVMWriter::addFunctionProtosExternal(const Function* function){
    const Function *SF = function;   // SrcFunction
    Module* Dest = module;
    std::string Err;

    Function *NF =
            Function::Create(cast<FunctionType>(SF->getType()->getElementType()),
                             GlobalValue::ExternalLinkage, SF->getName(), Dest);
    NF->copyAttributesFrom(SF);
    ValueMap[SF] = NF;

    return NF;
}

void LLVMWriter::linkExternalFunction(Function* srcFunction, Function* dstFunction){
    ValueMap[srcFunction] = dstFunction;

}


bool LLVMWriter::linkProcedureBody(Function* function){
    Function *F = cast<Function>(ValueMap[function]);
    if (!function->isDeclaration()) {
        Function::arg_iterator DestI = F->arg_begin();
        for (Function::const_arg_iterator J = function->arg_begin(); J != function->arg_end();
             ++J) {
            DestI->setName(J->getName());
            ValueMap[J] = DestI++;
        }

        SmallVector<ReturnInst*, 8> Returns;  // Ignore returns cloned.

        linkFunctionBody(F, function, ValueMap, /*ModuleLevelChanges=*/true, Returns/*,  decoder->getFifo()*/);

    }

    F->setLinkage(function->getLinkage());

    return true;
}

void LLVMWriter::linkFunctionBody(Function *NewFunc, const Function *OldFunc,
                                  ValueToValueMapTy &VMap,
                                  bool ModuleLevelChanges,
                                  SmallVectorImpl<ReturnInst*> &Returns,
                                  const char *NameSuffix, ClonedCodeInfo *CodeInfo) {
    // Clone any attributes.
    if (NewFunc->arg_size() == OldFunc->arg_size())
        NewFunc->copyAttributesFrom(OldFunc);
    else {
        //Some arguments were deleted with the VMap. Copy arguments one by one
        for (Function::const_arg_iterator I = OldFunc->arg_begin() ; I != OldFunc->arg_end() ; ++I){
            if (Argument* Anew = dyn_cast<Argument>(VMap[I])) {
                Anew->addAttr( OldFunc->getAttributes()
                               .getParamAttributes(I->getArgNo() + 1));
            }
        }
        NewFunc->addAttributes(0, OldFunc->getAttributes().getRetAttributes());
        NewFunc->addAttributes(~0, OldFunc->getAttributes().getFnAttributes());
    }

    // Loop over all of the basic blocks in the function, cloning them as
    // appropriate.  Note that we save BE this way in order to handle cloning of
    // recursive functions into themselves.
    //
    for (Function::const_iterator BI = OldFunc->begin(), BE = OldFunc->end();
         BI != BE; ++BI) {
        const BasicBlock &BB = *BI;

        // Create a new basic block and copy instructions into it!
        BasicBlock *CBB = CloneBasicBlock(&BB, VMap, NameSuffix, NewFunc, CodeInfo);
        VMap[&BB] = CBB;                       // Add basic block mapping.

        if (ReturnInst *RI = dyn_cast<ReturnInst>(CBB->getTerminator()))
            Returns.push_back(RI);
    }

    // Loop over all of the instructions in the function, fixing up operand
    // references as we go.  This uses VMap to do all the hard work.
    for (Function::iterator BB = cast<BasicBlock>(VMap[OldFunc->begin()]),
         BE = NewFunc->end(); BB != BE; ++BB)
        // Loop over all instructions, fixing each one as we find it...
        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II)
            RemapInstruction(II, VMap,
                             ModuleLevelChanges ? RF_None : RF_NoModuleLevelChanges);
}

bool LLVMWriter::addType(string name, StructType* type){
    return StructType::create(name, type, NULL);
}
