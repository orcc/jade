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
@brief Description of the TypeEntry class interface
@author Jerome Gorin
@file ExprType.h
@version 1.0
@date 15/11/2010
*/

//------------------------------
#include <iostream>

#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/DerivedTypes.h"

#include "lib/IRCore/Entry/ExprEntry.h"
#include "lib/IRCore/Entry/TypeEntry.h"
#include "lib/IRCore/Expr/IntExpr.h"
#include "lib/IRCore/Type/BoolType.h"
#include "lib/IRCore/Type/IntType.h"
#include "lib/IRCore/Type/UIntType.h"

#include "TypeParser.h"
//------------------------------

using namespace std;
using namespace llvm;

TypeParser::TypeParser (llvm::LLVMContext& C) : Context(C){
    exprParser = new ExprParser(C);
}

TypeParser::~TypeParser (){

}

IRType* TypeParser::parseType(TiXmlNode* node){ 

    while(node != NULL){
        if (TiXmlString(node->Value()) == XDFNetwork::TYPE) {
            TiXmlElement* eltType = (TiXmlElement*)node;
            TiXmlString name(eltType->Attribute( XDFNetwork::NAME));

            if (name == XDFNetwork::TYPE_BOOL) {
                return new BoolType();
            }else if (name == XDFNetwork::TYPE_INT) {
                map<string, Entry*> *entries = parseTypeEntries(node->FirstChild());
                Expr* expr = parseTypeSize(entries);
                return new IntType(expr->evaluateAsInteger());
            }else if (name == XDFNetwork::TYPE_LIST) {
                cerr << "List elements are not supPorted yet";
                exit(1);
            }else if (name == XDFNetwork::TYPE_STRING) {
                cerr << "String elements are not supPorted yet";
                exit(1);
            }else if (name == XDFNetwork::TYPE_UINT) {
                map<string, Entry*> *entries = parseTypeEntries(node->FirstChild());
                Expr* expr = parseTypeSize(entries);
                return new UIntType(expr->evaluateAsInteger());
            }else {
                cerr << "Unknown Type name: " << name.c_str();
                exit(1);
            }
        }

        node = node->NextSibling();
    }
    return NULL;
}

map<string, Entry*>* TypeParser::parseTypeEntries(TiXmlNode* node){
    map<string, Entry*> *entries = new map<string, Entry*>;

    while(node != NULL){
        if (TiXmlString(node->Value()) == XDFNetwork::ENTRY) {
            Entry* entry = NULL;
            TiXmlElement* element = (TiXmlElement*)node;

            TiXmlString name(element->Attribute(XDFNetwork::NAME));
            TiXmlString kind(element->Attribute(XDFNetwork::KIND));

            if (kind == XDFNetwork::EXPR) {
                Expr* expression = exprParser->parseExpr(node->FirstChild());
                entry = new ExprEntry(expression);
            }else if (kind == XDFNetwork::TYPE) {
                IRType* type = parseType(node->FirstChild());
                entry = new TypeEntry(type);
            }else {
                fprintf(stderr, "UnsupPorted entry Type: \"%s\"", kind.c_str());
                exit(0);
            }

            entries->insert(pair<string, Entry*>(string(name.c_str()),entry));
        }
        node = node->NextSibling();
    }

    return entries;
}

Expr* TypeParser::parseTypeSize(map<string, Entry*>* entries){
    map<string, Entry*>::iterator it;

    it = entries->find("size");

    if (it == entries->end()){
        return new IntExpr(Context, INT_SIZE);
    }

    //Size Attribute found
    Entry* entry = (*it).second;

    if (entry->isTypeEntry()){
        fprintf(stderr,"Entry does not contain an Expression");
        exit(0);
    }

    return ((ExprEntry*) entry)->getExprEntry();

}
