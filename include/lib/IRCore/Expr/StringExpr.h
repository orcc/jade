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
@brief Description of the StringExpr class interface
@author Jerome Gorin
@file StringExpr.h
@version 1.0
@date 15/11/2010
*/

//------------------------------
#ifndef STRINGEXPR_H
#define STRINGEXPR_H

#include <string>

#include "lib/IRCore/Expression.h"
#include "lib/IRCore/Type/StringType.h"
//------------------------------

/**
 * @class IntExpr
 *
 * @brief  This class defines an integer expression.
 *
 * This class represents an integer Expression in a network.
 *
 * @author Jerome Gorin
 *
 */
class StringExpr : public Expr {
public:
    /*!
     *  @brief Constructor
     *
     * Creates a new string expression.
     *
     *  @param C : llvm::LLVMContext.
     *  @param value : string value of the StringExpr.
     *
     */
    StringExpr(llvm::LLVMContext &C, std::string value): Expr(C){
        this->value = value;
    }

    ~StringExpr();

    /**
     * @brief Returns true if the expression is an instance of StringExpr
     *
     * @return True if the expression is an instance of StringExpr
     */
    bool isStringExpr(){return true;}

    /*!
     *  @brief Return ir::Type of the string expression
     *
     *  @return ir::Type of the string expression.
     *
     */
    IRType* getIRType(){return new StringType();}

    /**
     * @brief Returns llvm::Constant corresponding to the llvm value of this expression.
     *
     * @return llvm::Constant of this expression
     */
    llvm::Constant* getConstant();

    /*!
     *  @brief Get Value
     *
     *  @return Value
     *
     */
    std::string getValue(){return value;}

private:
    std::string value;

    llvm::Constant* constantVal;
};

#endif
