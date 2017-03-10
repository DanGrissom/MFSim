/*------------------------------------------------------------------------------*
 *                       (c)2016, All Rights Reserved.     						*
 *       ___           ___           ___     									*
 *      /__/\         /  /\         /  /\    									*
 *      \  \:\       /  /:/        /  /::\   									*
 *       \  \:\     /  /:/        /  /:/\:\  									*
 *   ___  \  \:\   /  /:/  ___   /  /:/~/:/        								*
 *  /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/___     UCR DMFB Synthesis Framework  *
 *  \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:::::/     www.microfluidics.cs.ucr.edu	*
 *   \  \:\  /:/   \  \:\  /:/   \  \::/~~~~ 									*
 *    \  \:\/:/     \  \:\/:/     \  \:\     									*
 *     \  \::/       \  \::/       \  \:\    									*
 *      \__\/         \__\/         \__\/    									*
 *-----------------------------------------------------------------------------*/
/*---------------------------Implementation Details-----------------------------*
 * Source: dmfb_arch.cc															*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/Models/dmfb_arch.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
DmfbArch::DmfbArch()
{
	externalResources = new vector<FixedModule *>();
	ioPorts = new vector<IoPort*>();
	numCellsX = -1;
	numCellsY = -1;
	wireRouter = NULL;

	// Instantiate PCB wire calculation variables
	areaRouteWires = new vector<WireSegment *>();
	vias = new vector<Via* >();
	nextStepPositions = new vector<PinDestLoc *>();
	mcSide = 1;
	totalManhatDistToWRDest = 0;
	totalWireLengthGrid = 0;
	numCellsX = -1;
	numCellsY = -1;
	wireRouter = NULL;
	shiftRegisters = NULL;
	microcontroller = NULL;
	pcb = NULL;
	hasInitToAimFor = false;

	// Set component variables for PCB Layout
	electrodePitchMicrons = 1000;
	srType = 2;
	mcType = 0;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
DmfbArch::~DmfbArch()
{
	for (unsigned i = 0; i < externalResources->size(); i++)
	{
		FixedModule *fm = externalResources->back();
		externalResources->pop_back();
		delete fm;
	}
	delete externalResources;

	for (unsigned i = 0; i < ioPorts->size(); i++)
	{
		IoPort *p = ioPorts->back();
		ioPorts->pop_back();
		delete p;
	}
	delete ioPorts;

	if (pinMapper != NULL)
		delete pinMapper;

	if (wireRouter != NULL)
		delete wireRouter;

}



///////////////////////////////////////////////////////////////
// Determine if input/output exists
///////////////////////////////////////////////////////////////
IoPort * DmfbArch::getIoPort(string fluidName)
{
	for (unsigned i=0; i < ioPorts->size(); i++)
	{
		IoPort *p = ioPorts->at(i);
		if (Util::StringToUpper(p->getPortName()) == Util::StringToUpper(fluidName))
			return p;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////
// Determine if arch has heaters or detectors
///////////////////////////////////////////////////////////////
bool DmfbArch::hasHeaters()
{
	for (int i = 0; i < externalResources->size(); i++)
		if (externalResources->at(i)->getResourceType() == H_RES || externalResources->at(i)->getResourceType() == DH_RES)
			return true;
	return false;
}
bool DmfbArch::hasDetectors()
{
	for (int i = 0; i < externalResources->size(); i++)
		if (externalResources->at(i)->getResourceType() == D_RES || externalResources->at(i)->getResourceType() == DH_RES)
			return true;
	return false;
}
