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
@brief Implementation of class Connection
@author Jerome Gorin
@file Connection.cpp
@version 0.1
@date 2010/04/12
*/

//------------------------------
#include <iostream>

#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"

#include "Jade/Core/Connection.h"
#include "Jade/Core/Expression.h"
#include "Jade/Core/Type.h"
#include "Jade/Core/Attribute.h"
#include "Jade/Attribute/ValueAttribute.h"
#include "Jade/Attribute/TypeAttribute.h"
#include "Jade/Attribute/TypeAttribute.h"
//------------------------------

using namespace std;
using namespace llvm;


Connection::Connection(Port* source, Port* target, std::map<std::string, Attribute*>* attributes): HDAGEdge()
{	this->attributes = attributes; 
	this->source = source;	
	this->target = target;
	this->fifo = NULL;
}


int Connection::evaluateAsInteger(Constant* expr){
	if(isa<ConstantInt>(expr)){
		ConstantInt* value = cast<ConstantInt>(expr);
		return (int)value->getValue().getLimitedValue();

	} else if (isa<ConstantExpr>(expr)){
		ConstantExpr* value = cast<ConstantExpr>(expr);
	}
	return 32;
}

int Connection::getFifoSize(){
	std::map<std::string, Attribute*>::iterator it;	
	it = attributes->find("bufferSize");
		
	if(it != attributes->end()){
		Attribute* attr = (*it).second;
			
		if (!attr->isValue()){
			cerr<<"Error when parsing type of a connection";
//			fprintf(stderr,"Error when parsing type of a connection");
			exit(0);
		}
			
		llvm::Constant* expr = ((ValueAttribute*)attr)->getValue();

		return evaluateAsInteger(expr);
	}		
	
	return SIZE;
}

int Connection::getType(){
	return type->getBitWidth()/8;
}