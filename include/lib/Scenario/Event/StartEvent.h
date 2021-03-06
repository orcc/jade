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
@brief Description of the StartEvent class interface
@author Jerome Gorin
@file Event.h
@version 1.0
@date 26/01/2011
*/

//------------------------------
#ifndef STARTEVENT_H
#define STARTEVENT_H
#include "lib/Scenario/Event.h"
//------------------------------

/**
 * @brief  This class defines a start event.
 * 
 * @author Jerome Gorin
 * 
 */
class StartEvent : public Event {
public:
    /*!
     * @brief Create a new Start event
     *
     * @param id : the id of the generated decoder.
     *
     * @param threaded : start in threaded mode or not.
     */
    StartEvent(int id, std::string input, std::string mappingFile) : Event(id) {
        this->input = input;
        this->mapping = mappingFile;
    }

    /*!
     *  @brief Destructor
     *
     * Delete an event.
     */
    ~StartEvent(){}

    /*!
     * @brief Return true if the Event is a StartEvent
     *
     * @return true if Event is a StartEvent otherwise false
     */
    bool isStartEvent(){return true;}

    /*!
     * @brief Return the input file of the decoder
     *
     * @return the input file
     */
    std::string getInput(){return input;}

    /*!
     * @brief Return true if event is threaded
     *
     * @return true if event is threaded
     */
    std::string mappingFile(){
        return mapping;
    }
private:
    /** Start in a threaded mode */
    std::string mapping;

    /** Input file */
    std::string input;
};

#endif
