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
@brief Description of the BinaryExpr class interface
@author Jerome Gorin
@file BinaryExpr.h
@version 1.0
@date 15/11/2010
*/

//------------------------------
#ifndef BINARYEXPR_H
#define BINARYEXPR_H
#include "lib/IRCore/IRType.h"
#include "lib/IRCore/Expression.h"
#include "BinaryOp.h"
//------------------------------

/**
 * @class BinaryExpr
 *
 * @brief  This class defines an binary expression.
 *
 * This class represents an binary Expression in a network.
 *
 * @author Jerome Gorin
 *
 */

class BinaryExpr : public Expr {
public:

    /**
     *  @brief Constructor of the class BinaryExpr
     *
     *  @param C : llvm::LLVMContext.
     *  @param  e1 : first Expr of binary expression
     *  @param  op : BinaryOp of the binary expression
     *  @param  e2 : second Expr of binary expression
     *
     */
    BinaryExpr(llvm::LLVMContext &C, Expr* e1, BinaryOp* op, Expr* e2) :  Expr(C){
        this->e1 = e1;
        this->op = op;
        this->e2 = e2;
    }
    ~BinaryExpr();

    /*!
     *  @brief Return IRType of the binary expression
     *
     *  @return IRType of the binary expression.
     *
     */
    IRType* getIRType(){return type;}

    /**
     * @brief Returns the first operand of this binary expression as an expression.
     *
     * @return the first operand of this binary expression
     */
    Expr* getE1() {return e1;}

    /**
     * @brief Returns the second operand of this binary expression as an expression.
     *
     * @return the second operand of this binary expression
     */
    Expr* getE2() {return e2;}

    /**
     * @brief Returns the operator of this binary expression.
     *
     * @return the operator of this binary expression
     */
    BinaryOp* getOp() {return op;}

    /**
     * @brief Returns llvm::Constant corresponding to the llvm value of this expression.
     *
     * @return llvm::Constant of this expression
     */
    llvm::Constant* getConstant();


    /**
     * @brief Returns true if the expression is an instance of BinaryExpr
     *
     * @return True if the expression is an instance of BinaryExpr
     */
    bool isBinaryExpr(){return true;}

private:
    Expr* e1;
    Expr* e2;
    BinaryOp* op;
    IRType* type;
};

#endif
