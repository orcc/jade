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
@brief Description of the VerifyEvent class interface
@author Jerome Gorin
@file WaitEvent.h
@version 1.0
@date 26/01/2011
*/

//------------------------------
#ifndef REMOVEEVENT_H
#define REMOVEEVENT_H
#include "lib/Scenario/Event.h"
//------------------------------

/**
 * @brief  This class defines a remove event.
 * 
 * @author Jerome Gorin
 * 
 */
class RemoveEvent : public Event {
public:
    /*!
     * @brief Create a new remove event
     *
     * @param id : the id of the decoder to remove.
     */
    RemoveEvent(int id) : Event(id) {
    }

    /*!
     *  @brief Destructor
     *
     * Delete an event.
     */
    ~RemoveEvent(){}

    /*!
     * @brief Return true if the Event is a RemoveEvent
     *
     * @return true if Event is a RemoveEvent otherwise false
     */
    bool isRemoveEvent(){return true;}

};

#endif
