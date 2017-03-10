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
 * Source: pcb.cc															*
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

PCB::PCB(DmfbArch *arch)
{
	ShiftRegister *tempSR = new ShiftRegister(arch, 0, 0, true, false);
	Microcontroller *tempMC = new Microcontroller(arch, 0);

	// Parse architecture
	int numCellsX = arch->numCellsX;
	int numCellsY = arch->numCellsY;
	electrodePitch = arch->electrodePitchMicrons;
	cellDimMM = electrodePitch / 1000.0;

	// Calculate conversion factor between milimeters and pixels
	conversionFactor = cellDimPixels / cellDimMM;

	int oCap = arch->getWireRouter()->getNumHorizTracks();
	int tileGridSize = ((oCap+2)*2 - 1);
	tileGridSize = arch->getWireRouter()->getModel()->getTileGridSize();
	int numXCells = arch->getNumCellsX();
	int numYCells = arch->getNumCellsY();

	int maxX = tileGridSize-1 + (numXCells*(tileGridSize-1));
	int maxY = tileGridSize-1 + (numYCells*(tileGridSize-1));
	topPins = new vector<WireSegment *>();
	leftPins = new vector<WireSegment *>();
	rightPins = new vector<WireSegment *>();
	botPins = new vector<WireSegment *>();
	// Calculate the number of pins on each side of the array after escape routing
	vector< vector<WireSegment *> *> *wires = arch->getWireRouter()->getWireRoutesPerPin();
	int maxLayer = 0;
	for (unsigned i = 0; i < wires->size(); i++)
	{
		vector<WireSegment *> *wire = wires->at(i);
		for (unsigned j = 0; j < wire->size(); j++)
		{
			WireSegment *ws = wire->at(j);
			if(ws->getSourceWireCellX(0) == 0)
			{
				leftPins->push_back(wire->at(j));
				if(wire->at(j)->getLayerNum() > maxLayer)
					maxLayer = wire->at(j)->getLayerNum();
			}
			if(ws->getSourceWireCellY(0) == 0)
			{
				topPins->push_back(wire->at(j));
				if(wire->at(j)->getLayerNum() > maxLayer)
					maxLayer = wire->at(j)->getLayerNum();
			}
			if(ws->getSourceWireCellX(0) == maxX)
			{
				rightPins->push_back(wire->at(j));
				if(wire->at(j)->getLayerNum() > maxLayer)
					maxLayer = wire->at(j)->getLayerNum();
			}
			if(ws->getSourceWireCellY(0) == maxY)
			{
				botPins->push_back(wire->at(j));
				if(wire->at(j)->getLayerNum() > maxLayer)
					maxLayer = wire->at(j)->getLayerNum();
			}
		}
	}
	arch->maxLayer = maxLayer + 1;

	int testa = leftPins->size();
	int testb = rightPins->size();
	int testc = topPins->size();
	int testd = botPins->size();

	ignoreOuterOutputs = true;	// Determines whether or not to route to outputs on far side of shift registers (can block other pins)

	// Turn the number of pins on each into amount of shift registers (out of 100)
	int totalMCPins = tempMC->pinOutQueue->size();
	int totalArrayPins = rightPins->size() + leftPins->size() + topPins->size() + botPins->size();

	int numPinsForSR = 0;
	if(totalArrayPins <= totalMCPins)
		numPinsForSR = 0;
	else
		numPinsForSR = ((totalArrayPins) - totalMCPins + 4);
	if(numPinsForSR < 0)
		numPinsForSR = 0;
	int numSRToDisperse = ceil(numPinsForSR / (double) tempSR->pinOutQueue->size());

	// Disperse Shift Registers (starting furthest)
	numSRTop = 0;
	numSRBot = 0;
	numSRLeft = 0;
	numSRRight = 0;
	int numPerSR = tempSR->pinOutQueue->size();
	int numRight = rightPins->size();
	int numLeft = leftPins->size();
	int numTop = topPins->size();
	int numBot = botPins->size();

	while(numSRToDisperse > 0)
	{
		if(numBot / numPerSR > 0)
		{
			numBot -= numPerSR;
			numSRToDisperse--;
			numSRBot++;
		}
		else if(numLeft / numPerSR > 0)
		{
			if(numBot > 0)
			{
				numLeft += numBot;
				numBot = 0;
			}

			numLeft -= numPerSR;
			numSRToDisperse--;
			numSRLeft++;
		}
		else if(numTop / numPerSR > 0)
		{
			if(numLeft > 0)
			{
				numTop += numLeft;
				numLeft = 0;
			}

			numTop -= numPerSR;
			numSRToDisperse--;
			numSRTop++;
		}
		else if(numRight / numPerSR > 0)
		{
			if(numTop > 0)
			{
				numRight += numTop;
				numTop = 0;
			}

			numRight -= numPerSR;
			numSRToDisperse--;
			numSRRight++;	// Disperse them on top to keep from blocking off spot in between components
		}
	}

	numShiftRegs = numSRRight + numSRLeft + numSRTop + numSRBot;

	/////////////////////////////////////////////////////////////////////
	// Calculate relevant bufferings
	mcSide = 1;	// 0 for top, 1 for right, 2 for bottom, 3 for left

	// Calculate number of foreign pins on the excess side
	int currentSide = -1;
	int finalSide = 0;
	int lastExcess = 0;	// The number of foreign pins on a given side (added to excess)
	int numExcessOnMCSide = 0;
	if(numSRRight + numSRLeft + numSRTop + numSRBot != 0)
	{
		while(currentSide != mcSide)
		{
			// Find the side below the Microcontroller
			if(currentSide == -1)
			{
				if(mcSide == 3)
					currentSide = 0;
				else
					currentSide = mcSide + 1;

				if(mcSide == 0)
					finalSide = 3;
				else
					finalSide = mcSide - 1;
			}

			// Route the excess pins to the other side
			if(currentSide == 0)	// Top
			{
				int numExcess = topPins->size() - (numSRTop * tempSR->numOutputs) + lastExcess;
				lastExcess = numExcess;
				if(numExcess < 0)
					numExcess = 0;

				if(currentSide == finalSide)
					numExcessOnMCSide = numExcess;

				currentSide = 1;
			}
			else if(currentSide == 1)	// Right
			{
				int numExcess = rightPins->size() - (numSRRight * tempSR->numOutputs) + lastExcess;
				lastExcess = numExcess;
				if(numExcess < 0)
					numExcess = 0;

				if(currentSide == finalSide)
					numExcessOnMCSide = numExcess;

				currentSide = 2;
			}
			else if(currentSide == 2)	// Bot
			{
				int numExcess = botPins->size() - (numSRBot * tempSR->numOutputs) + lastExcess;
				if(numExcess < 0)
					numExcess = 0;

				lastExcess = numExcess;

				if(currentSide == finalSide)
					numExcessOnMCSide = numExcess;

				currentSide = 3;
			}
			else if(currentSide == 3)	// Left
			{
				int numExcess = leftPins->size() - (numSRLeft * tempSR->numOutputs) + lastExcess;
				lastExcess = numExcess;
				if(numExcess < 0)
					numExcess = 0;

				if(currentSide == finalSide)
					numExcessOnMCSide = numExcess;
				currentSide = 0;
			}
		}
	}

	// Calculate the number of pins that will go through the shift registers to the microcontroller
	numOutputsSRSide = 0;
	numOutputsSRSide = tempSR->numOutputs * numSRRight;
	difference = 0;
	if(numSRRight > 1)
		difference = (rightPins->size() + numExcessOnMCSide - numOutputsSRSide) % ((int) numSRRight - 1);
	if(difference < 0)
		difference = 0;
	numToMCPerSection = 0;
	if(numSRRight > 1)
		numToMCPerSection = (rightPins->size() + numExcessOnMCSide - numOutputsSRSide) / (numSRRight - 1);
	else
		numToMCPerSection = 0;
	numExcess = tempSR->numOutputs - numExcessOnMCSide;

	// Calculate the SR buffers
	int bufferPixels = 0;
	bufferPixels = (tempSR->pinHeight * 2);
	double numPixelsPerGrid = 1.0 / (tileGridSize - 1) * (double) cellDimPixels;
	int decimalPlace = (int) (numPixelsPerGrid * 10) % 10;
	int numLeftover = 10 / decimalPlace;	// Because numPixelsPerGrid is used as an int below, when there are a lot of numToMCPerSection, the integer error can add up
	pcbBufferToMC = pcbBufferSR * conversionFactor + bufferPixels;

	//if(pcbBufferSR < .1 || numToMCPerSection == 0)
	pcbBufferSR = 1.25;

	// Calculate area of PCB already placed
	int sectionX = 0;
	if(arch->shiftRegisters != NULL)
	{
		// This part has to be done after pcb creation
		ShiftRegister *temporarySR = new ShiftRegister(arch, 0, 0, true, false);

		sectionX += temporarySR->sectionWidth;
		srSideWidth = temporarySR->sectionWidth;
	}
	if(arch->microcontroller != NULL)
	{
		sectionX += arch->microcontroller->sectionWidth;
	}

	// Calculate dimensions of array
	arrayHeight = cellDimPixels * numCellsY;
	arrayWidth =  cellDimPixels * numCellsX;

	// Set PCB x and y to 0
	xPos = 0;

	yPos = 0;

	// Calculate full PCB height
	height = 0;

	// Dmfb Positioning is calculated post-initialization
	dmfbX = sectionX + ((ComponentDimensions::PCB_BUFFER_DMFB) * conversionFactor);
	dmfbY = (ComponentDimensions::PCB_BUFFER_DMFB) * conversionFactor;

	// Calculated after initialization of object
	mcSideWidth = 0;
	srSideWidth = 0;
}

PCB::~PCB()
{
//	if(topPins != NULL)
//	{
//		topPins->pop_back();
//		while (topPins->size() > 0)
//		{
//			WireSegment *ws = topPins->back();
//			topPins->pop_back();
//			delete ws;
//		}
//		delete topPins;
//	}
//
//	if(botPins != NULL)
//	{
//		botPins->pop_back();
//		while (botPins->size() > 0)
//		{
//			WireSegment *ws = botPins->back();
//			botPins->pop_back();
//			delete ws;
//		}
//		delete botPins;
//	}
//
//	if(rightPins != NULL)
//	{
//		rightPins->pop_back();
//		while (rightPins->size() > 0)
//		{
//			WireSegment *ws = rightPins->back();
//			rightPins->pop_back();
//			delete ws;
//		}
//		delete rightPins;
//	}
//
//	if(leftPins != NULL)
//	{
//		leftPins->pop_back();
//		while (leftPins->size() > 0)
//		{
//			WireSegment *ws = leftPins->back();
//			leftPins->pop_back();
//			delete ws;
//		}
//		delete leftPins;
//	}
}

int PCB::getElectrodeMicrons()
{
	return electrodePitch;
}
