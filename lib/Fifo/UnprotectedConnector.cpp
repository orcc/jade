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
@brief Implementation of class UnprotectedConnector
@author Jerome Gorin
@file UnprotectedConnector.cpp
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <sstream>
#include <string>
#include <iostream>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"

#include "Jade/Decoder.h"
#include "Jade/Core/Port.h"
#include "Jade/Fifo/UnprotectedConnector.h"
#include "Jade/Core/Network.h"
#include "Jade/Jit/LLVMParser.h"
#include "Jade/Jit/LLVMWriter.h"
//------------------------------

using namespace llvm;
using namespace std;


UnprotectedConnector::UnprotectedConnector(llvm::LLVMContext& C, string system): Context(C), AbstractConnector()
{
	//Initialize map
	createFifoMap();
	createStructMap();

	//Declare header
	declareFifoHeader();
	
	// Initialize fifo counter
	fifoCnt = 0;

	// Set location of system
	this->system = system;
}

UnprotectedConnector::~UnprotectedConnector (){

}

void UnprotectedConnector::declareFifoHeader (){
	parseHeader();
	parseFifoStructs();
	parseFifoFunctions();
}

void UnprotectedConnector::parseHeader (){
	//Create the parser
	LLVMParser parser(Context, system);

	header = parser.loadBitcode("System", "FifoUnprotected");

	if (header == NULL){
		cerr << "Unable to parse fifo header file";
		exit(0);
	}
}

void UnprotectedConnector::parseFifoFunctions(){
	
	// Iterate though functions of header 
	for (Module::iterator I = header->begin(), E = header->end(); I != E; ++I) {
		string name = I->getName();
		
		if (isFifoFunction(name)){
			setFifoFunction(name, I);
			continue;
		}

		otherFunctions.push_back(I);
	}
}

void UnprotectedConnector::parseFifoStructs(){
	map<string,string>::iterator it;
	
	// Iterate though structure
	for (it = structName.begin(); it != structName.end(); ++it) {
		string name = it->second;

		Type* type = (Type*)header->getTypeByName(name);

		if (type == NULL){
			cerr << "Error when parsing fifo, structure " << name << " has not beend found";
			exit(0);
		}

		setFifoStruct(name, type);
		
	}
}

void UnprotectedConnector::addFunctions(Decoder* decoder){
	std::list<llvm::Function*>::iterator itList;
	LLVMWriter writer("", decoder);

	for(itList = otherFunctions.begin(); itList != otherFunctions.end(); ++itList){
		Function* function = writer.addFunctionProtosExternal(*itList);
		writer.linkProcedureBody(*itList);
		*itList = function;
	}

	std::map<std::string,llvm::Function*>::iterator itMap;

	for(itMap = fifoAccess.begin(); itMap != fifoAccess.end(); ++itMap){
		Function* function = writer.addFunctionProtosExternal((*itMap).second);
		writer.linkProcedureBody((*itMap).second);
		(*itMap).second = function;
	}
}

void UnprotectedConnector::setConnection(Connection* connection, Decoder* decoder){
	Module* module = decoder->getModule();
	
	// fifo name 
	ostringstream arrayName;
	ostringstream fifoName;

	arrayName << "array_" << fifoCnt;
	fifoName << "fifo_" << fifoCnt;

	// Get vertex of the connection
	Port* src = connection->getSourcePort();
	Port* dst = connection->getDestinationPort();
	GlobalVariable* srcVar = src->getGlobalVariable();
	GlobalVariable* dstVar = dst->getGlobalVariable();
	IntegerType* connectionType = cast<IntegerType>(src->getType());

	//Get fifo structure
	StructType* structType = getFifoType(connectionType);

	// Initialize array 
	PATypeHolder EltTy(connectionType);
	const ArrayType* arrayType = ArrayType::get(EltTy, connection->getSize());
	Constant* arrayContent = ConstantArray::get(arrayType, NULL,0);
	GlobalVariable *NewArray =
        new GlobalVariable(*module, arrayType,
		true, GlobalVariable::InternalLinkage, arrayContent, arrayName.str());
	
	
	// Initialize fifo elements
	Constant* elt_size = ConstantInt::get(Type::getInt32Ty(Context), connectionType->getBitWidth());
	Constant* size = ConstantInt::get(Type::getInt32Ty(Context), connection->getSize());
	Constant* read_ptr = ConstantInt::get(Type::getInt32Ty(Context), 0);
	Constant* write_ptr = ConstantInt::get(Type::getInt32Ty(Context), 0);
	Constant* expr = ConstantExpr::getBitCast(NewArray,Type::getInt8PtrTy(Context));
	
	// Add initialization vector 
	vector<Constant*> Elts;
	Elts.push_back(elt_size);
	Elts.push_back(size);
	Elts.push_back(expr);
	Elts.push_back(read_ptr);
	Elts.push_back(write_ptr);
	Constant* fifoStruct =  ConstantStruct::get(structType, Elts);

	// Create fifo 
	GlobalVariable *NewFifo =
        new GlobalVariable(*module, structType,
		true, GlobalVariable::InternalLinkage, fifoStruct, fifoName.str());

	// Set initialize to instance port 
	srcVar->setInitializer(NewFifo);
	dstVar->setInitializer(NewFifo);

	// Increment fifo counter 
	fifoCnt++;
	
}

StructType* UnprotectedConnector::getFifoType(IntegerType* type){
	map<string,Type*>::iterator it;

	// Struct name 
	ostringstream structName;
	structName << "struct.fifo_i" << type->getBitWidth() << "_s";

	it = structAcces.find(structName.str());
		
	return cast<StructType>(it->second);
}
