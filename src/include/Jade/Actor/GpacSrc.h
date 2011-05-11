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
@brief Description of the Source class interface
@author Olivier Labois
@file GpacSrc.h
@version 1.0
@date 02/05/2011
*/

//------------------------------
#ifndef GPACSRC_H
#define GPACSRC_H
#include "Jade/Actor/Source.h"

//------------------------------


/**
 * @class GpacSrc
 *
 * @brief  This class represents a source that read a gpac nal.
 * 
 * @author Olivier labois
 * 
 */
class GpacSrc : public Source {
public:
	/**
     *  @brief Create a new gpac nal reader for the decoder 
	 *   
	 *  @param id : the id of the decoder
     */
	GpacSrc(int id);

	~GpacSrc();

	void setNal(unsigned char* nal, int nal_length);

	/**
     *  @brief Injecteur in the decoder of data from input gpac nal 
	 *   
	 *  @param tokens : the adress where data must be injected
     */
	void source_get_src(unsigned char* tokens);

	/**
     *  @brief Set pointer of value which can stop the scheduler
	 *
	 *  This value is continiously tested by the scheduler, it MUST be an int.
	 *	The scheduler only stop when this value is set to 1, otherwise the scheduler
	 *	continuously test firing rules of actors
     */
	void setStopSchPtr(int* stopSchVal) {this->stopSchVal = stopSchVal;}

protected:

	/** input gpac nal */
	unsigned char* nal;

	/** nal length */
	int nal_length;

	/** byte read counter */
	int cnt;

	/** This is the value which can stop the scheduler */
	int* stopSchVal;
};

#endif