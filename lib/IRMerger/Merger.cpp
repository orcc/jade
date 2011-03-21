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
@brief Implementation of class Merger
@author Jerome Gorin
@file Merger.cpp
@version 1.0
@date 24/12/2010
*/

//------------------------------
#include <map>
#include <list>

#include "Jade/Core/Network.h"
#include "Jade/Configuration/Configuration.h"
#include "Jade/Merger/Merger.h"
#include "Jade/Merger/SuperInstance.h"
//------------------------------

using namespace std;

Merger::Merger(Configuration* configuration){
	network = configuration->getNetwork();
}

void Merger::transform(){
	bool hasCondidate = true;
		
	// Iterate though all vertices until no candidate left
	while(hasCondidate){
		list<Vertex*>::iterator it;
		list<Vertex*>* vertices = network->getVertices();
		
		// First compute all successors in the network
		network->computeSuccessorsMaps();
		bool recompute = false;

		for (it = vertices->begin(); it != vertices->end(); it++){
			Vertex* src = *it;
			Instance* srcInst = src->getInstance();
			Actor* srcActor = srcInst->getActor();

			if (srcActor->getMoC()->isCSDF()){
				//Iterate though successors, try to find a static actor
				list<Vertex*>::iterator itDst;
				list<Vertex*>* dsts = network->getSuccessorsOf(src);

				for (itDst = dsts->begin(); itDst !=  dsts->end(); itDst++){
					Vertex* dst = *itDst;
					Instance* dstInst = dst->getInstance();
					Actor* dstActor = dstInst->getActor();

					if (dstActor->getMoC()->isCSDF()){
						// These two actors can be merged
						mergeVertex(src, dst);

						// Recompute graph
						recompute = true;
						break;
					}
				}
			}
			
			if (recompute){
				break;
			}
		}
		
		if (!recompute){
			// No merging found, end the analysis
			hasCondidate = false;
		}
	}
}

void Merger::mergeVertex(Vertex* src, Vertex* dst){
	SuperInstance* superInstance = new SuperInstance("merged", src->getInstance(), 1, dst->getInstance(), 1);

	network->removeVertex(src);
	network->removeVertex(dst);
	network->addVertex(new Vertex(superInstance));
}