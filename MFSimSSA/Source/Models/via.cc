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
 * Source: via.cc															*
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

#include "../../Headers/Models/pcb.h"
#include "../../Headers/Models/component_dimensions.h"
#include "../../Headers/WireRouter/pcb_layout.h"

Via::Via(DmfbArch *arch, int xGridCoord, int yGridCoord, int beginLayerNum, int endLayerNum)
{
	electrodePitch = arch->electrodePitchMicrons;
	cellDimMM = electrodePitch / 1000.0;
	conversionFactor = cellDimPixels / cellDimMM;

	// Calculate the number of via cells in radius around the via
	int oCap = arch->getWireRouter()->getNumHorizTracks();
	int tileGridSize = ((oCap+2)*2 - 1);
	int viaGridRadius = ((ComponentDimensions::VIA_CONTACT_WIDTH * conversionFactor  / 2) / cellDimPixels) * (tileGridSize - 1) + 0;

	x = xGridCoord;
	y = yGridCoord;
	leftX = x - viaGridRadius;
	rightX = x + viaGridRadius;
	topY = y - viaGridRadius;
	botY = y + viaGridRadius;
	beginLayer = beginLayerNum;
	endLayer = endLayerNum;
}

Via::~Via()
{

}

void Via::createBounds(DmfbArch *arch)
{
	// Set the boundaries
	PCBLayout::createUnrouteableZoneHoriz(arch, leftX, rightX, botY, -1);
	PCBLayout::createUnrouteableZoneHoriz(arch, leftX, rightX, topY, -1);
	PCBLayout::createUnrouteableZoneVert(arch, botY, topY, leftX, -1);
	PCBLayout::createUnrouteableZoneVert(arch, botY, topY, rightX, -1);
}

int Via::getElectrodeMicrons()
{
	return 500;
}



