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
@brief Description of the Instantiator class interface
@author Jerome Gorin
@file Instantiator.h
@version 1.0
@date 15/11/2010
*/

//------------------------------
#ifndef INSTANTIATOR_H
#define INSTANTIATOR_H

#include <map>

#include "lib/IRCore/Actor.h"

class Connection;
class Configuration;
class HDAGGraph;
//------------------------------

/**
 * @class Instantiator
 *
 * @brief This class is used by network transformation to process the instanciation.
 *
 * @author Jerome Gorin
 *
 */
class Instantiator {
public:

    /**
     * @brief instanciate a Configuration.
     *
     * Instantiate actors and checks that connections actually point to ports defined in actors. Instantiating an
     * actor implies first loading it and then giving it the right parameters.
     *
     * @param configuration : Configuration to instanciate
     */
    Instantiator(Configuration* configuration);

    ~Instantiator(){}

private:

    /**
     * @brief Update instance and connections of the network.
     *
     * Updates the connections of this network. MUST be called before actors are
     * instantiated.
     *
     */
    void updateInstances();

    /**
     * @brief Update instance of the network.
     *
     * Updates the instance of this network using its corresponding actor
     *
     */
    void updateInstance(Instance* instance);

    /**
     * @brief Update a connection in the network.
     *
     * Updates instance using the given connection's source and target port by getting the
     * ports from the source and target instances, after checking the ports
     * exist and have compatible types.
     *
     * @param connection : Connection to update
     */
    void updateConnection(Connection* connection);

    /* Graph to transform */
    HDAGGraph* graph;

    /** Configuration to instanciate */
    Configuration* configuration;

    /** List of actor from the network */
    std::map<std::string, Actor*>* actors;

};

#endif
