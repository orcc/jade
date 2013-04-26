/*
 * Copyright (c) 2013, IETR/INSA of Rennes
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

#include "lib/Graph/SchedulingError.h"

void exitWithCode(int exitCode){

    // ExitCodes:
    // 1000: Adding a vertex while the graph is already full
    // 1001: Adding an edge while the graph is already full
    // 1002: The edge source repetition number outsizes the maximal pattern size
    // 1003: Two input edges set different repetition numbers to their common sink
    // 1004: The edge sink repetition number outsizes the maximal pattern size
    // 1005: The CSDAG to HDAG transformation led to an unbalanced number of produced and consumed token for an edge
    // 1006: HDAG edge creation led to an unbalanced number of produced and consumed token for an edge
    // 1007: Removing an edge from an empty graph
    // 1008: Adding a slave when the slave table is full
    // 1009: Adding a slave type with a too big index
    // 1010: Adding a production pattern in a full table
    // 1011: Adding a consumption pattern in a full table
    // 1012: Adding a variable in an already full variable table
    // 1013: Too long expression
    // 1014: Early end of expression after delimiter
    // 1015: User entered a not existing operator like "//"
    // 1016: Garbage at the end of the expression
    // 1018: Unwanted character in expression
    // 1019: Unexpected end of expression
    // 1020: Expression syntax error
    // 1021: Operator is not recognized
    // 1022: Operator is not recognized
    // 1023: Unknown variable
    // 1024: Unknown timing
    // 1025: Trying to allocate a buffer on a full memory
    // 1026: Adding an expression element in a full stack
    // 1027: Searching a non existing variable while creating a stack
    // 1028: Unrecognized token
    // 1029: Adding an expression element in a full queue
    // 1030: Removing an expression element from an empty queue
    // 1031: Popping an expression element from an empty stack
    // 1032: Parser parenthesis mismatch
    // 1033: Parser parenthesis mismatch. Left parenthesis missing
    // 1035: Peeking an expression element from an empty stack
    // 1036: Failure in RPN parsing
    // 1037: Expression syntax error: wrong variable assignment
    // 1038: Problem with variable assignment
    // 1039: Adding a parameter pattern in a full table
    // 1040: Adding a parameter in a full table
    // 1041: Accessing a slave with an incorrect index
    // 1042: Setting an improper data rate
    // 1043: Removing a slave from an empty architecture
    // 1044: Adding a successor vertex in a full table

    exit(exitCode);
}
