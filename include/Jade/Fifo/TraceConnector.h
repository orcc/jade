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
@brief Description of the TraceConnector class interface
@author Jerome Gorin
@file TraceConnector.h
@version 1.0
@date 15/11/2010
*/

//------------------------------
#ifndef TraceConnector_H
#define TraceConnector_H

#include "Jade/Fifo/FifoAbstract.h"
#include "Jade/Fifo/AbstractConnector.h"
//------------------------------

/**
 * @brief  This class defines TraceConnector.
 * 
 * @author Jerome Gorin
 * 
 */
class TraceConnector: public AbstractConnector {
private:
	/** Fifo structure */
	class FifoTrace : public FifoAbstract {
	public:
		FifoTrace(llvm::GlobalVariable *fifo,
					 llvm::Constant* size,
					 llvm::Constant* read_ind,
					 llvm::Constant* write_ind,
					 llvm::Constant* fill_count,
					 llvm::GlobalVariable* contents,
					 llvm::GlobalVariable* fifo_buffer,
					 llvm::GlobalVariable* file){
		 this->fifo = fifo;
		 this->size = size;
		 this->read_ind = read_ind;
		 this->write_ind = write_ind;
		 this->fill_count = fill_count;
		 this->contents = contents;
		 this->fifo_buffer = fifo_buffer;
		 this->file = file;
		};

		~FifoTrace();
	
	private:
		llvm::GlobalVariable *fifo;
		llvm::Constant* size;
		llvm::Constant* read_ind;
		llvm::Constant* write_ind;
		llvm::Constant* fill_count;
		llvm::GlobalVariable* contents;
		llvm::GlobalVariable* fifo_buffer;
		llvm::GlobalVariable* file;
	};

	/** Fifo function name */
	std::map<std::string,std::string> fifoMap()
	{
		std::map<std::string,std::string> fifo;
	
		fifo["i8_peek"] = "fifo_i8_peek";
		fifo["i8_write"] = "fifo_i8_write";
		fifo["i8_read"] = "fifo_i8_read";
		fifo["i8_hasToken"] = "fifo_i8_has_tokens";
		fifo["i8_hasRoom"] = "fifo_i8_has_room";
		fifo["i8_writeEnd"] = "fifo_i8_write_end";
		fifo["i8_readEnd"] = "fifo_i8_read_end";

		fifo["i16_peek"] = "fifo_i16_peek";
		fifo["i16_write"] = "fifo_i16_write";
		fifo["i16_read"] = "fifo_i16_read";
		fifo["i16_hasToken"] = "fifo_i16_has_tokens";
		fifo["i16_hasRoom"] = "fifo_i16_has_room";
		fifo["i16_writeEnd"] = "fifo_i16_write_end";
		fifo["i16_readEnd"] = "fifo_i16_read_end";

		fifo["i32_peek"] = "fifo_i32_peek";
		fifo["i32_write"] = "fifo_i32_write";
		fifo["i32_read"] = "fifo_i32_read";
		fifo["i32_hasToken"] = "fifo_i32_has_tokens";
		fifo["i32_hasRoom"] = "fifo_i32_has_room";
		fifo["i32_writeEnd"] = "fifo_i32_write_end";
		fifo["i32_readEnd"] = "fifo_i32_read_end";

		fifo["i64_peek"] = "fifo_i64_peek";
		fifo["i64_write"] = "fifo_i64_write";
		fifo["i64_read"] = "fifo_i64_read";
		fifo["i64_hasToken"] = "fifo_i64_has_tokens";
		fifo["i64_hasRoom"] = "fifo_i64_has_room";
		fifo["i64_writeEnd"] = "fifo_i64_write_end";
		fifo["i64_readEnd"] = "fifo_i64_read_end";

		fifo["u8_peek"] = "fifo_u8_peek";
		fifo["u8_write"] = "fifo_u8_write";
		fifo["u8_read"] = "fifo_u8_read";
		fifo["u8_hasToken"] = "fifo_u8_has_tokens";
		fifo["u8_hasRoom"] = "fifo_u8_has_room";
		fifo["u8_writeEnd"] = "fifo_u8_write_end";
		fifo["u8_readEnd"] = "fifo_u8_read_end";

		fifo["u16_peek"] = "fifo_u16_peek";
		fifo["u16_write"] = "fifo_u16_write";
		fifo["u16_read"] = "fifo_u16_read";
		fifo["u16_hasToken"] = "fifo_u16_has_tokens";
		fifo["u16_hasRoom"] = "fifo_u16_has_room";
		fifo["u16_writeEnd"] = "fifo_u16_write_end";
		fifo["u16_readEnd"] = "fifo_u16_read_end";

		fifo["u32_peek"] = "fifo_u32_peek";
		fifo["u32_write"] = "fifo_u32_write";
		fifo["u32_read"] = "fifo_u32_read";
		fifo["u32_hasToken"] = "fifo_u32_has_tokens";
		fifo["u32_hasRoom"] = "fifo_u32_has_room";
		fifo["u32_writeEnd"] = "fifo_u32_write_end";
		fifo["u32_readEnd"] = "fifo_u32_read_end";

		fifo["u64_peek"] = "fifo_u64_peek";
		fifo["u64_write"] = "fifo_u64_write";
		fifo["u64_read"] = "fifo_u64_read";
		fifo["u64_hasToken"] = "fifo_u64_has_tokens";
		fifo["u64_hasRoom"] = "fifo_u64_has_room";
		fifo["u64_writeEnd"] = "fifo_u64_write_end";
		fifo["u64_readEnd"] = "fifo_u64_read_end";

		return fifo;
	}

	/** Fifo function name */
	std::map<std::string,std::string> structMap()
	{
		std::map<std::string,std::string> fifoStruct;	
		fifoStruct["char_s"] = "struct.fifo_i8_s";
		fifoStruct["short_s"] = "struct.fifo_i16_s";
		fifoStruct["int_s"] = "struct.fifo_i32_s";
		fifoStruct["long_s"] = "struct.fifo_i64_s";
		fifoStruct["uchar_s"] = "struct.fifo_u8_s";
		fifoStruct["ushort_s"] = "struct.fifo_u16_s";
		fifoStruct["uint_s"] = "struct.fifo_u32_s";
		fifoStruct["ulong_s"] = "struct.fifo_u64_s";
		return fifoStruct;
	}

public:
	/**
     *  @brief Constructor
     *
	 *	Load and add fifo declaration inside the given decoder
	 *
	 *  @param jit : JIT use to load bitcoder
	 *
     */
	TraceConnector(llvm::LLVMContext& C, std::string system);
	
	~TraceConnector();

	void setConnection(Connection* connection, Decoder* decoder);
	
	void setConnections(Decoder* decoder);

	void unsetConnection(Connection* connection, Decoder* decoder){};

private:

	/** module for extern functions */
	llvm::Module* externMod;

	/** Counter of fifo */
	int fifoCnt;

	/** LLVM Context */
	llvm::LLVMContext &Context;
	
	/**
    *  @brief Parse fifo module
    */
	void parseHeader();

	/**
    * @brief Parse fifo functions into the header module
    */
	void parseFifoFunctions();

	/**
    * @brief Parse extern functions in the extern module
	*
    */
	void parseExternFunctions();

	/**
    * @brief Parse fifo structure
	*
    */
	void parseFifoStructs();

	/**
    * @brief add fifo function corresponding to the given name into the given decoder
	*
	* @param name : string of the function name into the header
	*
	* @param decoder : Decoder to had fifo function
	*
	* @return llvm::Function of the fifo function into the final decoder
    */
	void addFunctions(Decoder* decoder);

	/**
    * @brief declareFifoHeader inside decoder
    */
	void declareFifoHeader();

	llvm::StructType* getFifoType(llvm::IntegerType* type);

	/** System package location */
	std::string system;
};

#endif