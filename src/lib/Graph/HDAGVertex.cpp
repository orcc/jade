/*********************************************************
Copyright or © or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#include "lib/Graph/HDAGVertex.h"
#include <cstdio>
#include <cstring>

/**
 Constructor
*/
HDAGVertex::HDAGVertex()
{
	nbSuccessors = 0;
}

/**
 Destructor
*/
HDAGVertex::~HDAGVertex()
{
}
