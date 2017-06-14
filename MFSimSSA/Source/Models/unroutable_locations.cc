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
 * Source: unroutable_locations.cc															*
 * Original Code Author(s): Jordan Ishii											*
 * Original Completion/Release Date: 							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/

#include "../../Headers/Models/unroutable_locations.h"

UnroutableLocations::UnroutableLocations()
{
	hBounds = new vector<HorizBound>();
	vBounds = new vector<VertBound>();
}

UnroutableLocations::~UnroutableLocations()
{
	delete hBounds;
	delete vBounds;

}

bool UnroutableLocations::isClaimed(int xCoord, int yCoord, int layerNum)
{
	bool hasFound = false;

	for(unsigned i = 0; i < hBounds->size(); i++)
	{
		if(hBounds->at(i).check(xCoord, yCoord, layerNum))
		{
			i = hBounds->size();
			hasFound = true;
		}
	}

	if(hasFound != true)
	{
	for(unsigned i = 0; i < vBounds->size(); i++)
	{
		if(vBounds->at(i).check(xCoord, yCoord, layerNum))
		{
			i = vBounds->size();
			hasFound = true;
		}
	}
	}

	return hasFound;
}

void UnroutableLocations::setClaimedHoriz(int x1, int x2, int y, int layerNum)
{
	hBounds->push_back(HorizBound(x1, x2, y, layerNum));
}

void UnroutableLocations::setClaimedVert(int y1, int y2, int x, int layerNum)
{
	vBounds->push_back(VertBound(y1, y2, x, layerNum));
}

