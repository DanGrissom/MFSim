/*------------------------------------------------------------------------------*
 *                       (c)2016, All Rights Reserved.     						*
 *       ___           ___           ___     									*
 *      /__/\         /  /\         /  /\    									*
 *      \  \:\       /  /:/        /  /::\   									*
 *       \  \:\     /  /:/        /  /:/\:\  									*
 *   ___  \  \:\   /  /:/  ___   /  /:/~/:/        								*
 *  /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/___     UCR DMFB PCBLayout Framework  *
 *  \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:::::/     www.microfluidics.cs.ucr.edu	*
 *   \  \:\  /:/   \  \:\  /:/   \  \::/~~~~ 									*
 *    \  \:\/:/     \  \:\/:/     \  \:\     									*
 *     \  \::/       \  \::/       \  \:\    									*
 *      \__\/         \__\/         \__\/    									*
 *-----------------------------------------------------------------------------*/
/*---------------------------Implementation Details-----------------------------*
 * Source: shift_register.cc															*
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

void PCBLayout::calculatePCBLayout(DmfbArch *arch)
{
	// Test variables to control whether or not certain wires are routed
	bool routeExcessRoutes = true;
	bool routeToMCSide = true;
	bool routeFullSides = true;
	bool routeChainWires = true;

	int oCap = arch->getWireRouter()->getNumHorizTracks();
	int tileGridSize = ((oCap+2)*2 - 1);
	tileGridSize = arch->getWireRouter()->getModel()->getTileGridSize();
	int numXCells = arch->getNumCellsX();
	int numYCells = arch->getNumCellsY();
	vector<WireSegment*> *botPinsToMC = new vector<WireSegment*>();	// For excess routing

	arch->pcb = new PCB(arch);

	arch->shiftRegisters = new vector<ShiftRegister *>();

	arch->microcontroller = new Microcontroller(arch, arch->pcb->numShiftRegs);


	///////////////////////////////////////////////////////////////////////
	//	LAYOUT ENCIRCLING ARRAY
	// Calculate the max wire routing position
	int cellDimPixels = arch->pcb->cellDimPixels;

	vector<ShiftRegister *> *topShiftRegs = new vector<ShiftRegister *>();
	vector<ShiftRegister *> *botShiftRegs = new vector<ShiftRegister *>();
	vector<ShiftRegister *> *rightShiftRegs = new vector<ShiftRegister *>();
	vector<ShiftRegister *> *leftShiftRegs = new vector<ShiftRegister *>();

	// Add all Shift Registers
	int numShiftRegistersAdded = 0;
	for(int i = 0; i < arch->pcb->numSRTop; i++)
	{
		ShiftRegister *sr = new ShiftRegister(arch, numShiftRegistersAdded, 270, arch->pcb->ignoreOuterOutputs, 0);
		numShiftRegistersAdded++;
		topShiftRegs->push_back(sr);
		arch->shiftRegisters->push_back(sr);
	}
	for(int i = 0; i < arch->pcb->numSRBot; i++)
	{
		ShiftRegister *sr = new ShiftRegister(arch, numShiftRegistersAdded, 90, arch->pcb->ignoreOuterOutputs, 2);
		numShiftRegistersAdded++;
		arch->shiftRegisters->push_back(sr);
	}
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i++)
	{
		ShiftRegister *sr = new ShiftRegister(arch, numShiftRegistersAdded, 180, arch->pcb->ignoreOuterOutputs, 3);
		numShiftRegistersAdded++;

		arch->shiftRegisters->push_back(sr);
	}
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft + arch->pcb->numSRRight; i++)
	{
		ShiftRegister *sr = new ShiftRegister(arch, numShiftRegistersAdded, 270, arch->pcb->ignoreOuterOutputs, 1);
		numShiftRegistersAdded++;

		topShiftRegs->push_back(sr);
		rightShiftRegs->push_back(sr);
		arch->shiftRegisters->push_back(sr);
	}

	ShiftRegister dummySR = ShiftRegister(arch, arch->pcb->numShiftRegs, 0, true, 0);

	// Calculate full PCB height
	int maxNumSRLR = 0;
	int maxNumSRTB = 0;
	if(arch->pcb->numSRRight > arch->pcb->numSRLeft)
		maxNumSRLR = arch->pcb->numSRRight;
	else
		maxNumSRLR = arch->pcb->numSRLeft;
	if(arch->pcb->numSRTop > arch->pcb->numSRBot)
		maxNumSRTB = arch->pcb->numSRTop;
	else
		maxNumSRTB = arch->pcb->numSRBot;

	// Calculate sr and array section width now that pcb and shift regs initialized
	int mostTop;
	if(arch->pcb->numSRTop > arch->pcb->numSRBot)
		mostTop = arch->pcb->numSRTop;
	else
		mostTop = arch->pcb->numSRBot;

	int containsSRLeft = 1;	// 0  if false
	int containsSRRight = 1;
	if(arch->pcb->numSRLeft == 0 && arch->pcb->numSRRight == 0)
	{
		containsSRLeft = 0;
		containsSRRight = 0;
	}

	// Potential widths for if the total sr positioning is larger horizontally than the default buffered positioning
	int potentialArraySpreadSideWidth = ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * dummySR.conversionFactor * 2 + dummySR.arrayWidth + (dummySR.fullBodyWidthMM * dummySR.conversionFactor + ComponentDimensions::PCB_BUFFER_SR * dummySR.conversionFactor) * (containsSRLeft + containsSRRight);
	int potentialSRSpreadSideWidth = mostTop * (dummySR.fullBodyHeightMM * dummySR.conversionFactor + arch->pcb->pcbBufferSR * dummySR.conversionFactor) + containsSRLeft * (dummySR.fullBodyWidthMM * dummySR.conversionFactor + ComponentDimensions::PCB_BUFFER_SR * dummySR.conversionFactor) + containsSRRight * (dummySR.fullBodyWidthMM * dummySR.conversionFactor + ComponentDimensions::PCB_BUFFER_SR * dummySR.conversionFactor);
	if(potentialArraySpreadSideWidth > potentialSRSpreadSideWidth)
		arch->pcb->srSideWidth = potentialArraySpreadSideWidth;
	else
		arch->pcb->srSideWidth = potentialSRSpreadSideWidth;
	int topAndBot = 2;	// Always 2 just to keep symmetry
	if(arch->pcb->numSRTop == 0 && arch->pcb->numSRBot == 0)
		topAndBot = 0;

	// The problem is the array being too big
	int minArraySpan = arch->pcb->arrayHeight + ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * arch->pcb->conversionFactor * 3 + dummySR.fullBodyWidthMM * dummySR.conversionFactor * topAndBot;
	int minSRSpan = arch->pcb->pcbBufferSR * dummySR.conversionFactor * (maxNumSRLR) + (dummySR.fullBodyHeightMM *  maxNumSRLR + dummySR.fullBodyWidthMM * topAndBot) * dummySR.conversionFactor + 1.25 * 2 * arch->pcb->conversionFactor;	// 2.5 for pcb to top and bot sr
	int minPCBSpan = arch->microcontroller->fullBodyHeightMM * arch->microcontroller->conversionFactor + ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * 1.5 * arch->microcontroller->conversionFactor;
	if(minArraySpan > minSRSpan && minArraySpan > minPCBSpan)
		arch->pcb->height = minArraySpan;
	else if(minSRSpan > minPCBSpan)
		arch->pcb->height = minSRSpan;
	else
		arch->pcb->height = minPCBSpan;

	// Open up extra space to add room for more pins on top
	if(arch->pcb->numShiftRegs == 0 && arch->pcb->leftPins->size() > 10)
		arch->pcb->height += ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * arch->pcb->conversionFactor;

	arch->pcb->dmfbY = (arch->pcb->height - arch->pcb->arrayHeight) / 2;

	arch->microcontroller->xPos = dummySR.sectionWidth + ComponentDimensions::PCB_BUFFER_DMFB * dummySR.conversionFactor;
	arch->microcontroller->yPos = (arch->pcb->height - arch->microcontroller->fullBodyHeightMM * arch->pcb->conversionFactor) / 2 + arch->microcontroller->pinHeight;
	if(arch->pcb->numSRRight >= 1)	// Put MC further away if has lots of SR's
	{
		arch->microcontroller->xPos += 40;
		arch->microcontroller->yPos += 32;
	}
	int buffer = ((dummySR.pinHeight * 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);	// numPinsPS = first pin on side 1... numPinsPS - 1 = last pin on side 0...
	int pixBuffer = (dummySR.pinHeight * 2);
	if(arch->pcb->numSRBot > 0 && arch->pcb->numSRTop == 0 && arch->pcb->numSRRight == 0)
	{
		arch->pcb->height -= pixBuffer * 2;
		arch->pcb->dmfbY -= pixBuffer * 2;
		arch->microcontroller->yPos -= pixBuffer * 2;
	}


	// Calculate pcb section variables post-component initialization
	arch->pcb->mcSideWidth = arch->microcontroller->sectionWidth;

	// If no sr on right side, can cut down pcb width and microcontroller xPos partially

	arch->microcontroller->xPos -= ComponentDimensions::PCB_BUFFER_DMFB / 2 * dummySR.conversionFactor;
	arch->pcb->mcSideWidth -= ComponentDimensions::PCB_BUFFER_DMFB / 2 * dummySR.conversionFactor;

	if(arch->pcb->numSRLeft != 0)
	{
		arch->microcontroller->xPos -= (dummySR.fullBodyWidthMM * dummySR.conversionFactor + ComponentDimensions::PCB_BUFFER_SR * dummySR.conversionFactor);
		arch->pcb->mcSideWidth -= (dummySR.fullBodyWidthMM * dummySR.conversionFactor + ComponentDimensions::PCB_BUFFER_SR * dummySR.conversionFactor);
	}

	// Calculate array x and y
	int arrayAndSrSideWidths = dummySR.sectionWidth;

	arch->pcb->height += cellDimPixels - ((arch->pcb->height) % cellDimPixels);
	arch->pcb->dmfbX = (arrayAndSrSideWidths / 2.0) - (arch->pcb->arrayWidth / 2.0);

	// Add to sides of array in order to make PCB width divisible by electrode pitch
	arch->pcb->srSideWidth += cellDimPixels - ((arch->pcb->mcSideWidth + arch->pcb->srSideWidth) % cellDimPixels);
	arch->pcb->dmfbX += cellDimPixels - ((arch->pcb->dmfbX) % cellDimPixels);

	// Add to height in order to make PCB width divisible by electrode pitch
	if(cellDimPixels - ((arch->pcb->dmfbY) % cellDimPixels) != 0)
		arch->pcb->dmfbY += cellDimPixels - ((arch->pcb->dmfbY) % cellDimPixels);
	if(cellDimPixels - ((arch->microcontroller->yPos) % cellDimPixels) != 0)
		arch->microcontroller->yPos += cellDimPixels - ((arch->microcontroller->yPos) % cellDimPixels);
	arch->microcontroller->updatePinLocations();	// HAS TO BE DONE EVERY TIME MICROCONTROLLER POSITION IS CHANGED (probably should combine this with change in x or y to make new method in class)

	// Calculate number of wire routing cells in PCB
	arch->numWRCellsHoriz = (arch->pcb->srSideWidth + arch->pcb->mcSideWidth) / cellDimPixels;
	arch->numWRCellsVert = arch->pcb->height / cellDimPixels;

	// Calculate the distance from the pcb to the array in wire routing coordinates
	arch->wrOffsetX = (arch->pcb->dmfbX / cellDimPixels) * (tileGridSize - 1) - (tileGridSize / 2);
	arch->wrOffsetY = (arch->pcb->dmfbY / cellDimPixels) * (tileGridSize - 1) - (tileGridSize / 2);



	////////////////////////////////////////////////////////////////////////////////////////
	// Create the wire grid
	int gridWidth = arch->numWRCellsHoriz * (tileGridSize - 1);	// 472
	int gridHeight = arch->numWRCellsVert * (tileGridSize - 1); // 424
	arch->wireGrid = new bool **[arch->maxLayer];
	for(int i = 0; i < arch->maxLayer; i++)
	{
		arch->wireGrid[i] = new bool *[gridWidth];
		for(int j = 0; j < gridWidth; j++)
		{
			arch->wireGrid[i][j] = new bool[gridHeight];
			for(int k = 0; k < gridHeight; k++)
				arch->wireGrid[i][j][k] = false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// Find X coordinate of first top Shift Registers (1 at a time, so rotation doesn't matter)
	int topPosX = 0;
	for(int i = 0; i < arch->pcb->numSRTop; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);
		topPosX += ((sr->fullBodyWidthMM + arch->pcb->pcbBufferSR) *sr->conversionFactor);

		if(i == arch->pcb->numSRTop - 1)
		{
			topPosX -= arch->pcb->pcbBufferSR * sr->conversionFactor;
			topPosX = (arch->pcb->dmfbX + arch->pcb->arrayWidth / 2) - ((topPosX)/2);
		}
	}

	// Set X Coordinate of top Shift Registers
	int rightPosX = topPosX;
	for(int i = 0; i < arch->pcb->numSRTop; i++)
	{
		ShiftRegister *shiftReg = arch->shiftRegisters->at(i);
		shiftReg->setXPos(rightPosX);
		rightPosX += (shiftReg->fullBodyWidthMM + arch->pcb->pcbBufferSR) * shiftReg->conversionFactor;
	}

	// Add bot Shift Regs to list
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot - 1; i >= arch->pcb->numSRTop; i--)
	{
		botShiftRegs->push_back(arch->shiftRegisters->at(i));
	}

	// Find X coordinate of the first bottom Shift Registers (1 at a time, so rotation doesn't matter)
	int botPosX = 0;
	for(int i = arch->pcb->numSRTop; i < arch->pcb->numSRTop + arch->pcb->numSRBot; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);
		botPosX += (sr->fullBodyWidthMM * sr->conversionFactor + arch->pcb->pcbBufferSR *sr->conversionFactor);

		if(i == arch->pcb->numSRTop + arch->pcb->numSRBot - 1)
		{
			botPosX -= arch->pcb->pcbBufferSR * sr->conversionFactor;
			botPosX = (arch->pcb->dmfbX + arch->pcb->arrayWidth / 2) - ((botPosX)/2);
		}
	}

	// Set X Coordinate of bottom Shift Registers
	int changedX = botPosX;
	for(int i = arch->pcb->numSRTop; i < arch->pcb->numSRTop + arch->pcb->numSRBot; i++)
	{
		ShiftRegister *shiftReg = arch->shiftRegisters->at(i);
		shiftReg->setXPos(changedX);

		changedX += (shiftReg->fullBodyWidthMM + arch->pcb->pcbBufferSR) * shiftReg->conversionFactor;
	}

	// Find the one that sticks out more to make sure left and right don't overlap
	int smallerX = 0;
	if(topPosX == 0 && botPosX == 0)
		smallerX = arch->pcb->pcbBufferSR * arch->pcb->conversionFactor / 2 + arch->pcb->dmfbX;
	else if(topPosX == 0)
		smallerX = botPosX;
	else if(botPosX == 0)
		smallerX = topPosX;
	else if(topPosX < botPosX)
		smallerX = topPosX;
	else
		smallerX = botPosX;

	// Find X Coordinate of left Shift Registers
	int leftPosX = 0;
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);
		numShiftRegistersAdded++;

		leftPosX = smallerX - (sr->fullBodyWidthMM * sr->conversionFactor) - arch->pcb->pcbBufferSR * sr->conversionFactor / 2;

		sr->setXPos(leftPosX);
	}

	// Add left Shift Regs to list backwards for ease of routing routing
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft - 1; i >= arch->pcb->numSRTop + arch->pcb->numSRBot; i--)
	{
		leftShiftRegs->push_back(arch->shiftRegisters->at(i));
	}

	// Find X Coordinate of right Shift Registers
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft + arch->pcb->numSRRight; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);
		numShiftRegistersAdded++;

		sr->setXPos(rightPosX);
		topShiftRegs->push_back(sr);

		rightPosX += (sr->fullBodyWidthMM + arch->pcb->pcbBufferSR) * sr->conversionFactor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Find Y coordinate of first left Shift Registers (1 at a time, so rotation doesn't matter)
	int leftPosY = 0;
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);
		leftPosY += (sr->fullBodyHeightMM * sr->conversionFactor + arch->pcb->pcbBufferSR *sr->conversionFactor);

		if(i == arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft - 1)
		{
			leftPosY = (arch->pcb->numSRLeft * (sr->fullBodyHeightMM * sr->conversionFactor + arch->pcb->pcbBufferSR *sr->conversionFactor)) - arch->pcb->pcbBufferSR * sr->conversionFactor;
			leftPosY = (arch->pcb->dmfbY + arch->pcb->arrayHeight / 2) - (leftPosY / 2);
		}
	}

	// Set Y Coordinate of left Shift Registers
	int changedY = leftPosY;
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i++)
	{
		ShiftRegister *shiftReg = arch->shiftRegisters->at(i);
		shiftReg->setYPos(changedY);

		changedY += (shiftReg->fullBodyHeightMM + arch->pcb->pcbBufferSR) * shiftReg->conversionFactor;
	}

	// Find the one that sticks out more to make sure top and bottom don't overlap
	int smallerY = 0;
	smallerY = leftPosY;

	if(leftPosY == 0)
		smallerY = arch->pcb->dmfbY + arch->pcb->arrayHeight / 3;	// This will make the position shift upwards to a safe distance

	// Find Y Coordinate of top Shift Registers
	int topPosY = 0;
	for(int i = 0; i < arch->pcb->numSRTop; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);

		topPosY = smallerY - (arch->pcb->pcbBufferSR * sr->conversionFactor / 2 + sr->fullBodyHeightMM * sr->conversionFactor);

		sr->setYPos(topPosY);
	}

	// Set Y Coordinate of right Shift Registers
	int rPosY = topPosY;
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft + arch->pcb->numSRRight; i++)
	{
		ShiftRegister *shiftReg = arch->shiftRegisters->at(i);
		shiftReg->setYPos(rPosY);
	}

	// Find Y Coordinate of bottom Shift Registers
	int botPosY = 0;
	for(int i = arch->pcb->numSRTop; i < arch->pcb->numSRTop + arch->pcb->numSRBot; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);

		ShiftRegister *furthestSR = arch->shiftRegisters->at(arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft - 1);
		botPosY = furthestSR->yPos + furthestSR->fullBodyHeightMM * furthestSR->conversionFactor + arch->pcb->pcbBufferSR * furthestSR->conversionFactor / 2;

		sr->setYPos(botPosY);
	}

	///////////////////////////////////////////////////////////////////////
	//	LAYOUT ENCIRCLING ARRAY
	// Make sure that top Shift Register y doesn't overlap array
	double conversionFactor = arch->pcb->conversionFactor;
	for(int i = 0; i < arch->pcb->numSRTop; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);

		if(sr->yPos + sr->fullBodyHeightMM * conversionFactor > arch->pcb->dmfbY - ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * conversionFactor)
		{
			// This is because since sometimes the pins going to MC are too many and can overflow the top srs
			if(arch->pcb->topPins->size() >= 18 && arch->pcb->numSRTop < 2)
				sr->setYPos(arch->pcb->dmfbY - ((ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR + sr->fullBodyHeightMM) * arch->pcb->conversionFactor + sr->pinHeight));
			else
				sr->setYPos(arch->pcb->dmfbY - ((ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR + sr->fullBodyHeightMM) * arch->pcb->conversionFactor));

		}
	}
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft + arch->pcb->numSRRight; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);

		if(sr->yPos + sr->fullBodyHeightMM * conversionFactor > arch->pcb->dmfbY - ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * conversionFactor)
		{
			// This is because since sometimes the pins going to MC are too many and can overflow the top srs
			if(arch->pcb->topPins->size() >= 18 && arch->pcb->numSRTop < 2)
				sr->setYPos(arch->pcb->dmfbY - ((ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR + sr->fullBodyHeightMM) * arch->pcb->conversionFactor + sr->pinHeight));
			else
				sr->setYPos(arch->pcb->dmfbY - ((ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR + sr->fullBodyHeightMM) * arch->pcb->conversionFactor));

		}
	}

	// Make sure that bot Shift Register y doesn't overlap array
	for(int i = arch->pcb->numSRTop; i < arch->pcb->numSRTop + arch->pcb->numSRBot; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);

		if(sr->yPos < arch->pcb->dmfbY + ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * arch->pcb->conversionFactor + arch->pcb->arrayHeight)
		{
			sr->setYPos(arch->pcb->dmfbY + arch->pcb->arrayHeight + ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * arch->pcb->conversionFactor);
		}
	}

	// Make sure that left Shift Register x doesn't overlap array
	for(int i = arch->pcb->numSRTop + arch->pcb->numSRBot; i < arch->pcb->numSRTop + arch->pcb->numSRBot + arch->pcb->numSRLeft; i++)
	{
		ShiftRegister *sr = arch->shiftRegisters->at(i);

		if(sr->xPos + sr->fullBodyWidthMM * conversionFactor > arch->pcb->dmfbX - ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * arch->pcb->conversionFactor)
		{
			sr->setXPos(arch->pcb->dmfbX - ((ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR + sr->fullBodyWidthMM) * arch->pcb->conversionFactor));
		}
	}

	// Sort each group of pins
	std::sort(arch->pcb->topPins->begin(), arch->pcb->topPins->end(), PinIncreasingX());
	std::sort(arch->pcb->rightPins->begin(), arch->pcb->rightPins->end(), PinIncreasingY());
	std::sort(arch->pcb->botPins->begin(), arch->pcb->botPins->end(), PinDecreasingX());
	std::sort(arch->pcb->leftPins->begin(), arch->pcb->leftPins->end(), PinDecreasingY());

	// Group all pins into one complete vector
	vector<WireSegment *> *allArrayPins = new vector<WireSegment *>();
	for(unsigned pin = 0; pin < arch->pcb->topPins->size(); pin++)
	{
		arch->pcb->topPins->at(pin)->side = 0;
		arch->pcb->topPins->at(pin)->willRouteToMC = false;
		allArrayPins->push_back(arch->pcb->topPins->at(pin));
	}

	int num = 0;
	for(unsigned pin = 0; pin < arch->pcb->rightPins->size(); pin++)
	{
		arch->pcb->rightPins->at(pin)->side = 1;
		allArrayPins->push_back(arch->pcb->rightPins->at(pin));
	}
	for(unsigned pin = 0; pin < arch->pcb->botPins->size(); pin++)
	{
		arch->pcb->botPins->at(pin)->side = 2;
		arch->pcb->botPins->at(pin)->willRouteToMC = false;
		allArrayPins->push_back(arch->pcb->botPins->at(pin));
	}
	for(unsigned pin = 0; pin < arch->pcb->leftPins->size(); pin++)
	{
		arch->pcb->leftPins->at(pin)->side = 3;
		arch->pcb->leftPins->at(pin)->willRouteToMC = false;
		allArrayPins->push_back(arch->pcb->leftPins->at(pin));
	}

	// Create a buffer around array so that wires don't travel through it
	if(arch->pcb->leftPins->size() != 0 && arch->pcb->rightPins->size() != 0 && arch->pcb->topPins->size() != 0 && arch->pcb->botPins->size() != 0)
	{
		int topY = arch->pcb->topPins->at(0)->getSourceWireCellY(0) + arch->wrOffsetY + 2;
		int botY = arch->pcb->botPins->at(0)->getSourceWireCellY(0) + arch->wrOffsetY - 2;
		int leftX = arch->pcb->leftPins->at(0)->getSourceWireCellX(0) + arch->wrOffsetX + 2;
		int rightX = arch->pcb->rightPins->at(0)->getSourceWireCellX(0) + arch->wrOffsetX - 2;

		for(int lay = 0; lay < arch->maxLayer; lay++)
			for(int x = leftX; x <= rightX; x++)
				for(int y = topY; y < botY; y++)
					arch->wireGrid[lay][x][y] = true;
	}

	// Create a buffer around the microcontroller
	if(arch->pcb->numSRRight > 0)
		buffer = buffer / 2;

	createUnrouteableZoneHoriz(arch, arch->microcontroller->pins->at(0)->xPos - buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 4 - 1)->xPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 4 - 1)->yPos - buffer, -1);
	createUnrouteableZoneHoriz(arch, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 3)->xPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 3 - 1)->xPos + buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 3)->yPos - buffer, -1);
	createUnrouteableZoneHoriz(arch, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide - 1)->xPos - buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide)->xPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide)->yPos + buffer, -1);
	createUnrouteableZoneHoriz(arch, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 2 -1)->xPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 2)->xPos + buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide)->yPos + buffer, -1);
	createUnrouteableZoneVert(arch, arch->microcontroller->pins->at(0)->yPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 4 - 1)->yPos - buffer, arch->microcontroller->pins->at(0)->xPos - buffer, -1);
	createUnrouteableZoneVert(arch, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 3)->yPos - buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 3 - 1)->yPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 3 - 1)->xPos + buffer, -1);
	createUnrouteableZoneVert(arch, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide - 1)->yPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide)->yPos + buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide - 1)->xPos - buffer, -1);
	createUnrouteableZoneVert(arch, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 2 - 1)->yPos, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 2)->yPos + buffer, arch->microcontroller->pins->at(arch->microcontroller->numPinsPerSide * 2 - 1)->xPos + buffer, -1);

	if(arch->pcb->numSRRight > 0)
		buffer = buffer * 2;

	// Route pins that must go to sides other than their own
	int currentSide = -1;
	int finalSide = 0;
	int lastExcess = 0;	// The number of foreign pins on a given side (added to excess)
	int numExcessOnMCSide = 0;
	bool needsBotExcessRoute = false;
	int leftToMC = 0;
	while(currentSide != arch->pcb->mcSide /*&& routeExcessRoutes*/)
	{
		// Find the side below the Microcontroller
		if(currentSide == -1)
		{
			if(arch->pcb->mcSide == 3)
			{
				currentSide = 0;
			}
			else
			{
				currentSide = arch->pcb->mcSide + 1;
			}

			if(arch->pcb->mcSide== 0)
			{
				finalSide = 3;
			}
			else
			{
				finalSide = arch->pcb->mcSide - 1;
			}
		}

		// Route the excess pins to the other side
		if(currentSide == 0)	// Top
		{
			int numExcess = 0;
			if(arch->pcb->numSRTop > 0)
			{
				numExcess = arch->pcb->topPins->size() - (arch->pcb->numSRTop * topShiftRegs->at(topShiftRegs->size() - 1)->numOutputs) + lastExcess;
				lastExcess = numExcess;
			}
			else
			{
				numExcess = arch->pcb->topPins->size();
				lastExcess = 0;
			}

			if(currentSide == finalSide)
			{
				numExcessOnMCSide = numExcess;
			}

			if(numExcess > 0)
			{
				if(routeToMCSide && arch->pcb->numSRRight == 0)
					performExcessAreaRoute(arch, arch->pcb->topPins, arch->pcb->topPins->size() - numExcess, arch->pcb->topPins->size() - 1, NULL);
			}

			currentSide = 1;
		}
		else if(currentSide == 2)	// Bot
		{
			int numExcess = 0;
			if(arch->pcb->numSRBot > 0)
			{
				numExcess = arch->pcb->botPins->size() - (arch->pcb->numSRBot * botShiftRegs->at(botShiftRegs->size() - 1)->numOutputs) + lastExcess;
				lastExcess = numExcess;
			}
			else
			{
				numExcess = arch->pcb->botPins->size();
				lastExcess = 0;
			}

			if(currentSide == finalSide)
			{
				numExcessOnMCSide = numExcess;
			}

			if(numExcess > 0 && routeExcessRoutes)	// For this side if none on next side probably also none on side after that so just route straight down to bottom right corner and then to mc after all other wires complete
			{
				if(leftShiftRegs->size() == 0 && numExcess > 0 && arch->pcb->botPins->size() != numExcess)
				{
					for(int i = 0; i < numExcess; i++)
					{
						botPinsToMC->push_back(arch->pcb->botPins->at(i));
					}
				}
				if(leftShiftRegs->size() > 0 && arch->pcb->botPins->size() != numExcess)
					performExcessAreaRoute(arch, arch->pcb->botPins, arch->pcb->botPins->size() - numExcess, arch->pcb->botPins->size() - 1, leftShiftRegs->at(0));
				else if(arch->pcb->botPins->size() == numExcess)
				{
					for(int i = 0; i < arch->pcb->botPins->size(); i++)
						botPinsToMC->push_back(arch->pcb->botPins->at(i));
				}
			}

			currentSide = 3;
		}
		else if(currentSide == 3)	// Left
		{
			int numExcess = 0;
			if(arch->pcb->numSRLeft > 0)
			{
				numExcess = arch->pcb->leftPins->size() - (arch->pcb->numSRLeft * leftShiftRegs->at(leftShiftRegs->size() - 1)->numOutputs) + lastExcess;
				lastExcess = numExcess;
			}
			else
			{
				numExcess = arch->pcb->leftPins->size();
				lastExcess = 0;
			}


			if(currentSide == finalSide)
			{
				numExcessOnMCSide = numExcess;
			}

			if(numExcess > 0 && routeExcessRoutes)	// For this one if none on next side... route to above top left corner of array and then to to right corner and go from there before routing to mc before top side does
			{
				if(topShiftRegs->size() > 0)
					performExcessAreaRoute(arch, arch->pcb->leftPins, arch->pcb->leftPins->size() - numExcess, arch->pcb->leftPins->size() - 1, topShiftRegs->at(0));
				else
					leftToMC = numExcess;
			}
			currentSide = 0;
		}
	}

	// Route all pins that must go around mc around the array
	if(topShiftRegs->size() == 0 && leftToMC > 0)
	{
		// Find safe spot for wire
		int oCap = arch->getWireRouter()->getNumHorizTracks();
		int tileGridSize = ((oCap+2)*2 - 1);
		int buffer = ((dummySR.pinHeight * 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
		int beginXTopLeft = ((arch->pcb->dmfbX) / arch->pcb->cellDimPixels) * (tileGridSize - 1) - 4;
		int yTopLeft = ((arch->pcb->dmfbY) / arch->pcb->cellDimPixels) * (tileGridSize - 1) - arch->pcb->topPins->size() * 2 - 4;	// - 4 for good measure
		//int xTopRight = ((arch->pcb->dmfbX + arch->pcb->arrayWidth / 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1) + 2;
		int beginYTopRight = ((arch->pcb->dmfbY) / arch->pcb->cellDimPixels) * (tileGridSize - 1) - arch->pcb->topPins->size() * 2 - 4;
		int xMC = ((arch->microcontroller->xPos + 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
		int yMCStart = ((arch->microcontroller->yPos - arch->microcontroller->pinHeight - 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1) - buffer - arch->pcb->topPins->size() * 2;

		vector<PinDestLoc *> wiresToMCFromTop = vector<PinDestLoc *>();
		vector<PinDestLoc *> startingLoc = vector<PinDestLoc *>();
		int size = arch->pcb->leftPins->size();
		for(int i = size - 1; i >= size - leftToMC; i--)
		{
			if(i >= 0)
			{
				WireSegment *startingPoint = arch->pcb->leftPins->at(i);

				bool needsVia = false;
				if(startingPoint->getLayerNum() != 0)
					needsVia = true;

				routeWire(arch, arch->microcontroller->pinOutQueue, startingPoint->getLayerNum(), startingPoint->getPinNum(), startingPoint->getSourceWireCellX(0) + arch->wrOffsetX, startingPoint->getSourceWireCellY(0) + arch->wrOffsetY, beginXTopLeft, yTopLeft, false, true, false, startingPoint->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, startingPoint->getLayerNum(), startingPoint->getPinNum(), beginXTopLeft, yTopLeft, xMC, yMCStart, true, false, false, startingPoint->side, false);

				wiresToMCFromTop.push_back(new PinDestLoc(1, -1, startingPoint->getLayerNum(), startingPoint->getPinNum(), xMC, yMCStart, PinType::IO_OTHER, false));
				startingLoc.push_back(new PinDestLoc(1, -1, startingPoint->getLayerNum(), startingPoint->getPinNum(), startingPoint->getSourceWireCellX(0) + arch->wrOffsetX, startingPoint->getSourceWireCellY(0) + arch->wrOffsetY, PinType::IO_OTHER, false));

				beginXTopLeft -= 2;
				beginYTopRight -= 2;
				yMCStart -= 2;
			}
		}



		// Route wires from top side to pcb if needed
		int prevSide = -1;
		//for(int i = wiresToMCFromTop.size() - 1; i >= 0; i--)
		for(int i = 0; i < wiresToMCFromTop.size(); i++)
		{
			PinDestLoc *startingPoint = wiresToMCFromTop.at(i);
			PinDestLoc *endLoc = arch->microcontroller->getPinBack();

			// Increment total manhattan distance
			int distAcrossHoriz = abs(startingLoc.at(i)->xPos - endLoc->xPos);
			int distAcrossVert = abs(startingLoc.at(i)->yPos - endLoc->yPos);
			arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

			// See if needs helper (for the going back into array
			if(prevSide == -1)
				prevSide = endLoc->side;
			else if(endLoc->side != prevSide)
			{
				arch->xCoordToAimFor = endLoc->xPos;
				arch->yCoordToAimFor = endLoc->yPos;
				prevSide = endLoc->side;
			}

			bool needsVia = false;
			if(startingPoint->layer != 0)
				needsVia = true;

			// Find the offset for getting to the MC
			int offsetX = 0;
			int offsetY = 0;
			if(endLoc->side == 0)
			{
				offsetX = -1 * buffer;
				offsetY = 0;
			}
			else if(endLoc->side == 1)
			{
				offsetY = buffer;
				offsetX = 0;
			}
			else if(endLoc->side == 2)
			{
				offsetY = 0;
				offsetX = buffer;
			}
			else
			{
				offsetY = -1 * buffer;
				offsetX = 0;
			}

			routeWire(arch, arch->microcontroller->pinOutQueue, startingPoint->layer, startingPoint->pinNo, startingPoint->xPos, startingPoint->yPos, endLoc->xPos + offsetX, endLoc->yPos + offsetY, true, false, false, startingPoint->side, false);
			routeWire(arch, arch->microcontroller->pinOutQueue, startingPoint->layer, startingPoint->pinNo, endLoc->xPos + offsetX, endLoc->yPos + offsetY, endLoc->xPos, endLoc->yPos, false, false, needsVia, startingPoint->side, false);

			if(i == wiresToMCFromTop.size() - 1)
			{
				// Create a blockage on all layers to help the others route
				createUnrouteableZoneHoriz(arch, startingPoint->xPos, endLoc->xPos, startingPoint->yPos, -1);
				createUnrouteableZoneVert(arch, startingPoint->yPos, endLoc->yPos, endLoc->xPos, -1);
			}
		}
	}

	arch->xCoordToAimFor = -1;	// These two variables allow wires to curl back inwards to MC after going around one of its sides
	arch->yCoordToAimFor = -1;

	int numExcess = 0;

	for(unsigned pin = 0; pin < arch->pcb->rightPins->size(); pin++)
	{
		arch->pcb->rightPins->at(pin)->willRouteToMC = true;
	}

	if(arch->pcb->numSRTop != 0 && arch->pcb->numSRRight > 0)
		performFullSideAreaRoute(arch, arch->pcb->topPins, topShiftRegs, false, numExcessOnMCSide);

	// Route right pins to right Shift Registers (Side closest to MC)
	if(routeToMCSide)	// NOTE (Wires travel all along the bottom side of MC as to save up space on side with less mc pins so that the other types of vias (grnd, pow) need less vias)
		performFullSideAreaRoute(arch, arch->pcb->rightPins, rightShiftRegs, true, numExcessOnMCSide);

	// Increment total manhattan distance for array to MC
	int nextLoc = 0;
	for(int i = 0; i < arch->pcb->rightPins->size(); i++)
	{
		if(arch->pcb->rightPins->at(i)->willRouteToMC)
			for(int mcPinLoc = nextLoc; mcPinLoc < arch->microcontroller->pins->size(); mcPinLoc++)
			{
				if(arch->microcontroller->pins->at(mcPinLoc)->type == PinType::IO_PARALLEL_OUTPUT)
				{
					int distAcrossHoriz = abs(arch->pcb->rightPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX - arch->microcontroller->pins->at(mcPinLoc)->xPos);
					int distAcrossVert = abs(arch->pcb->rightPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY - arch->microcontroller->pins->at(mcPinLoc)->yPos);
					arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);


					nextLoc = mcPinLoc + 1;
					mcPinLoc = arch->microcontroller->pins->size();
				}
			}
	}

	// Route top pins to top Shift Registers
	if(routeFullSides)
	{
		if(arch->pcb->numSRTop != 0 && arch->pcb->numSRRight == 0)
			performFullSideAreaRoute(arch, arch->pcb->topPins, topShiftRegs, false, numExcessOnMCSide);

		// Route bot pins to bot Shift Registers
		if(arch->pcb->numSRBot != 0 && botPinsToMC->size() == 0)
			performFullSideAreaRoute(arch, arch->pcb->botPins, botShiftRegs, false, numExcessOnMCSide);

		// Route left pins to left Shift Registers
		if(arch->pcb->numSRLeft != 0)
			performFullSideAreaRoute(arch, arch->pcb->leftPins, leftShiftRegs, false, numExcessOnMCSide);
	}

	// Bot to MC
	for(int i = 0; i < botPinsToMC->size(); i++)
	{
		// Get dest and start
		WireSegment *source = botPinsToMC->at(i);
		PinDestLoc *dest = arch->microcontroller->getPinMinSide(1);

		if(dest != NULL)
		{
			// Increment total manhattan distance
			int distAcrossHoriz = abs(source->getSourceWireCellX(0) + arch->wrOffsetX - dest->xPos);
			int distAcrossVert = abs(source->getSourceWireCellY(0) + arch->wrOffsetY - dest->yPos);
			arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

			bool isMovHoriz = false;
			if(source->side == 0 || source->side == 2)
				isMovHoriz = true;
			bool needsVia = false;
			if(source->getLayerNum() != 0)
				needsVia = true;

			// Set up the visual offset
			int botX = ((arch->pcb->dmfbX + arch->pcb->arrayWidth) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
			int botY = ((arch->pcb->dmfbY + arch->pcb->arrayHeight) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
			int botYMC = ((arch->microcontroller->yPos + arch->microcontroller->fullBodyHeightMM * arch->pcb->conversionFactor) / arch->pcb->cellDimPixels) * (tileGridSize - 1);

			if(dest->side != 0)
				createUnrouteableZoneVert(arch, botY, botYMC + buffer, botX, dest->layer);
			else
				for(int j = botY; j <= botYMC + buffer; j++)
					arch->wireGrid[source->getLayerNum()][botX][j] = false;

			if(dest->side == 0)
			{
				if(arch->wireGrid[source->getLayerNum()][dest->xPos - buffer][dest->yPos - buffer])
					buffer--;

				// Route wire straight to mc
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), source->getSourceWireCellX(0) + arch->wrOffsetX, source->getSourceWireCellY(0) + arch->wrOffsetY, dest->xPos - buffer, dest->yPos, isMovHoriz, true, false, source->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), dest->xPos - buffer, dest->yPos, dest->xPos, dest->yPos, true, true, needsVia, source->side, false);
			}
			else if(dest->side == 1)
			{
				if(arch->wireGrid[source->getLayerNum()][dest->xPos][dest->yPos + buffer])
					buffer--;

				// Route wire straight to mc
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), source->getSourceWireCellX(0) + arch->wrOffsetX, source->getSourceWireCellY(0) + arch->wrOffsetY, dest->xPos, dest->yPos + buffer, isMovHoriz, true, false, source->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), dest->xPos, dest->yPos + buffer, dest->xPos, dest->yPos, false, true, needsVia, source->side, false);
			}
			else if(dest->side == 2)
			{
				if(arch->wireGrid[source->getLayerNum()][dest->xPos + buffer][dest->yPos])
					buffer--;

				// Route wire straight to mc
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), source->getSourceWireCellX(0) + arch->wrOffsetX, source->getSourceWireCellY(0) + arch->wrOffsetY, dest->xPos + buffer, dest->yPos, isMovHoriz, true, false, source->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), dest->xPos + buffer, dest->yPos, dest->xPos, dest->yPos, false, true, needsVia, source->side, false);
			}
			else if(dest->side == 3)
			{
				if(arch->wireGrid[source->getLayerNum()][dest->xPos][dest->yPos - buffer])
					buffer--;

				// Route wire straight to mc
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), source->getSourceWireCellX(0) + arch->wrOffsetX, source->getSourceWireCellY(0) + arch->wrOffsetY, dest->xPos, dest->yPos - buffer, isMovHoriz, true, false, source->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, source->getLayerNum(), source->getPinNum(), dest->xPos, dest->yPos - buffer, dest->xPos, dest->yPos, false, true, needsVia, source->side, false);
			}
		}
		else
		{

		}
	}

	// Finish off bottom routes if has some going to MC
	if(botPinsToMC->size() != arch->pcb->botPins->size() && botPinsToMC->size() != 0)
		routeWireGroup(arch, arch->pcb->botPins, botPinsToMC->size(), arch->pcb->botPins->size(), botShiftRegs, 0, numExcessOnMCSide);

	// Route the chain wire
	PinDestLoc *firstChainLoc;

	if(botShiftRegs->size() > 0)
		firstChainLoc = botShiftRegs->at(0)->getChainLoc(true);
	else if(leftShiftRegs->size() > 0)
		firstChainLoc = leftShiftRegs->at(0)->getChainLoc(true);

	if(arch->shiftRegisters->size() > 0 && routeChainWires)
	{
		PinDestLoc *nextMCPin = arch->microcontroller->pinOutQueue->front();

		if(nextMCPin->side == 0)
			nextMCPin = arch->microcontroller->getPinMinSide(1);
		else
			arch->microcontroller->pinOutQueue->pop();
		if(nextMCPin->side != 2)
		{
			routeWire(arch, arch->microcontroller->pinOutQueue, firstChainLoc->layer, firstChainLoc->pinNo, nextMCPin->xPos, nextMCPin->yPos, firstChainLoc->xPos, firstChainLoc->yPos, true, false, false, firstChainLoc->side, true);
		}
		else
		{
			// Add to the total distance
			int distAcrossHoriz = abs(nextMCPin->xPos - firstChainLoc->xPos);
			int distAcrossVert = abs(nextMCPin->yPos - firstChainLoc->yPos);
			arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

			routeWire(arch, arch->microcontroller->pinOutQueue, firstChainLoc->layer, firstChainLoc->pinNo, nextMCPin->xPos, nextMCPin->yPos, nextMCPin->xPos + buffer + 1, nextMCPin->yPos, true, false, false, firstChainLoc->side, false);
			routeWire(arch, arch->microcontroller->pinOutQueue, firstChainLoc->layer, firstChainLoc->pinNo, nextMCPin->xPos + buffer + 1, nextMCPin->yPos, firstChainLoc->xPos, firstChainLoc->yPos, false, false, false, firstChainLoc->side, false);
		}

		SRPinNode *begin = NULL;
		SRPinNode *curr = NULL;
		for(unsigned i = 0; i < botShiftRegs->size(); i++)
			if(begin == NULL)
			{
				begin = new SRPinNode(botShiftRegs->at(i)->getChainLoc(false));
				curr = begin;
			}
			else
			{
				curr->next = new SRPinNode(botShiftRegs->at(i)->getChainLoc(true));
				curr->next->next = new SRPinNode(botShiftRegs->at(i)->getChainLoc(false));
				curr = curr->next->next;
			}
		for(unsigned i = 0; i < leftShiftRegs->size(); i++)
			if(begin == NULL)
			{
				begin = new SRPinNode(leftShiftRegs->at(i)->getChainLoc(false));
				curr = begin;
			}
			else
			{
				curr->next = new SRPinNode(leftShiftRegs->at(i)->getChainLoc(true));
				curr->next->next = new SRPinNode(leftShiftRegs->at(i)->getChainLoc(false));
				curr = curr->next->next;
			}
		for(unsigned i = 0; i < topShiftRegs->size() - arch->pcb->numSRRight; i++)
			if(begin == NULL)
			{
				begin = new SRPinNode(topShiftRegs->at(i)->getChainLoc(false));
				curr = begin;
			}
			else
			{
				curr->next = new SRPinNode(topShiftRegs->at(i)->getChainLoc(true));
				curr->next->next = new SRPinNode(topShiftRegs->at(i)->getChainLoc(false));
				curr = curr->next->next;
			}

		if(routeChainWires)
			performChainRouting(arch, begin);
	}
}

// Routes the power wires
void PCBLayout::performChainRouting(DmfbArch *arch, SRPinNode *begin)
{
	bool test = true;

	if(begin->next != NULL)
	{
		PinDestLoc *out = begin->location;
		PinDestLoc *in = begin->next->location;

		// Add to the total distance
		int distAcrossHoriz = abs(out->xPos - in->xPos);
		int distAcrossVert = abs(out->yPos - in->yPos);
		arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

		int offsetX = 0;
		int offsetY = 0;
		bool movHoriz;

		if(out->side == 0)
		{
			offsetY = -8;
			movHoriz = true;
			test = false;
		}
		else if(out->side == 1)
		{
			offsetY = -8;
			movHoriz = true;
		}
		else if(out->side == 2)
		{
			offsetY = 8;
			movHoriz = true;
		}
		else
		{
			offsetX = -8;
			movHoriz = false;
		}

		// Offset
		routeWire(arch, arch->microcontroller->pinOutQueue, out->layer, out->pinNo, out->xPos, out->yPos, out->xPos + offsetX, out->yPos + offsetY, movHoriz, false, false, in->side, false);

		bool crawl = false;
		if(in->side != out->side)
		{
			if(in->side == 3)	// left
			{
				routeWire(arch, arch->microcontroller->pinOutQueue, out->layer, out->pinNo, out->xPos + offsetX, out->yPos + offsetY, in->xPos - 7, out->yPos + offsetY, true, false, false, in->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, out->layer, out->pinNo, in->xPos - 7, out->yPos + offsetY, in->xPos, in->yPos, false, false, false, in->side, false);
			}
			else
			{
				routeWire(arch, arch->microcontroller->pinOutQueue, out->layer, out->pinNo, out->xPos + offsetX, out->yPos + offsetY, out->xPos + offsetX, in->yPos - 7, false, false, false, in->side, false);
				routeWire(arch, arch->microcontroller->pinOutQueue, out->layer, out->pinNo, out->xPos + offsetX, in->yPos - 7, in->xPos, in->yPos, true, false, false, in->side, false);
			}
		}
		else
		{
			routeWire(arch, arch->microcontroller->pinOutQueue, out->layer, out->pinNo, out->xPos + offsetX, out->yPos + offsetY, in->xPos, in->yPos, movHoriz, false, false, in->side, crawl);
		}

		if(begin->next->next != NULL)
		{
			performChainRouting(arch, begin->next->next);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Area Routing Methods
//////////////////////////////////////////////////////////////////////////////////////////////

void PCBLayout::createUnrouteableZoneHoriz(DmfbArch *arch, int leftX, int rightX, int yCoord, int layerNum)
{
	// If going in other direction, change to make the smaller one correct
	if(leftX > rightX)
	{
		int temp = leftX;
		leftX = rightX;
		rightX = temp;
	}

	//	if(rightX > 0 && leftX < arch->numWRCellsHoriz)
	//	{
	// Go through each space on perimeter of zone and save each coordinate
	if(layerNum == -1)
	{
		for(int i = 0; i < arch->maxLayer; i++)
		{
			for(int j = leftX; j <= rightX; j++)
				if(i >= 0 && j > 0 && yCoord > 0)
					arch->wireGrid[i][j][yCoord] = true;
		}
	}
	else
	{
		for(int i = leftX; i <= rightX; i++)
		{
			if(layerNum >= 0 && i > 0 && yCoord > 0)
				arch->wireGrid[layerNum][i][yCoord] = true;
		}
	}
}

void PCBLayout::createUnrouteableZoneVert(DmfbArch *arch, int topY, int botY, int xCoord, int layerNum)
{
	// If going in other direction, change to make the smaller one correct
	if(topY > botY)
	{
		int temp = topY;
		topY = botY;
		botY = temp;
	}

	//	if(topY > 0 && botY < arch->numWRCellsVert)
	//	{
	// Go through each space on perimeter of zone and save each coordinate
	if(layerNum == -1)
	{
		for(int i = 0; i < arch->maxLayer; i++)
		{
			for(int j = topY; j <= botY; j++)
				if(i >= 0 && j > 0 && xCoord > 0)
					arch->wireGrid[i][xCoord][j] = true;
		}
	}
	else
	{
		for(int i = topY; i <= botY; i++)
		{
			if(layerNum >= 0 && i > 0 && xCoord > 0)
				arch->wireGrid[layerNum][xCoord][i] = true;
		}
	}
}

// Creates a wire from one array endpoint to the first output pin on the Shift Register
void PCBLayout::performAreaRoute(DmfbArch *arch, vector<WireSegment* > *arrayPins, int pinNo, int offset, PinDestLoc *srPin)
{
	// Find the direction to move first
	bool isMovingHorizontally = false;
	if(arrayPins->at(pinNo)->side == 1 && arrayPins->at(pinNo)->side == 3)
		isMovingHorizontally = true;

	// Route the pin
	routeWire(arch, NULL, arrayPins->at(pinNo)->getLayerNum(), arrayPins->at(pinNo)->getPinNum(), arrayPins->at(pinNo)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(pinNo)->getSourceWireCellY(0) + arch->wrOffsetY, srPin->xPos, srPin->yPos, isMovingHorizontally, false, false, arrayPins->at(pinNo)->side, false);
}

void PCBLayout::routeIndividualWire(DmfbArch *arch, PinDestLoc *destLoc, WireSegment *arrayBegin, ShiftRegister *currentSR, int sideSize)
{
	bool needsVia = false;
	bool isMovingHorizontally = false;
	int oCap = arch->getWireRouter()->getNumHorizTracks();
	int tileGridSize = ((oCap+2)*2 - 1);
	ShiftRegister dummySR = ShiftRegister(arch, arch->pcb->numShiftRegs, 0, true, 0);

	// Increment total manhattan distance
	int distAcrossHoriz = abs(arrayBegin->getSourceWireCellX(0) + arch->wrOffsetX - destLoc->xPos);
	int distAcrossVert = abs(arrayBegin->getSourceWireCellY(0) + arch->wrOffsetY - destLoc->yPos);
	arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

	if(arrayBegin->getLayerNum() != 0 && arrayBegin->getLayerNum() != -1)
		needsVia = true;
	else
		needsVia = false;

	// Find the initial direction to move
	if(arrayBegin->side == 0 || arrayBegin->side == 2)
		isMovingHorizontally = true;
	else
		isMovingHorizontally = false;

	if(destLoc->isOuterSR)
	{
		PinDestLoc *realPinLoc = currentSR->realOuterPinLocs->front();
		currentSR->realOuterPinLocs->pop();

		int testX = realPinLoc->xPos;
		int testY = realPinLoc->yPos;
		int testXX = destLoc->xPos;
		int testYY = destLoc->yPos;

		routeWire(arch, NULL, arrayBegin->getLayerNum(),  arrayBegin->getPinNum(), arrayBegin->getSourceWireCellX(0) + arch->wrOffsetX, arrayBegin->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos, isMovingHorizontally, false, false, arrayBegin->side, false);
		routeWire(arch, NULL, arrayBegin->getLayerNum(),  arrayBegin->getPinNum(), destLoc->xPos, destLoc->yPos, realPinLoc->xPos, realPinLoc->yPos, isMovingHorizontally, false, needsVia, arrayBegin->side, false);

		if(currentSR->rotateDeg == 0)	// This puts them each right on the side of SR so that it is easy to go around
		{
			currentSR->sideCoordY += 2;
			currentSR->sideCoordX += 2;
		}
		else if(currentSR->rotateDeg == 90)
		{
			currentSR->sideCoordX -= 2;
			currentSR->sideCoordY += 2;
		}
		else if(currentSR->rotateDeg == 180)
		{
			currentSR->sideCoordY -= 2;
			currentSR->sideCoordX -= 2;
		}
		else
		{
			currentSR->sideCoordX += 2;
			currentSR->sideCoordY -= 2;
		}
	}
	else if(arrayBegin->side != 1)	// Should always be case with current version
		routeWire(arch, NULL, arrayBegin->getLayerNum(),  arrayBegin->getPinNum(), arrayBegin->getSourceWireCellX(0) + arch->wrOffsetX, arrayBegin->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos, isMovingHorizontally, false, needsVia, arrayBegin->side, false);
}

void PCBLayout::routeWireGroup(DmfbArch *arch, vector<WireSegment* > *sidePins, int initPinNo, int endPinNo, vector<ShiftRegister* > *sideShiftRegs, int srNo, int numExcessOnMCSide)
{
	int numRouted = 0;

	////////////////////////////////////////////////////////////////////////
	// Route each pin on the given side
	bool endEarly = false;
	queue<PinDestLoc *> *mcPinQueue = new queue<PinDestLoc *>();

	// Variables for routing in reverse order
	bool isTravelingBackwards = false;
	int nextPinToRouteNormal = 0;
	stack<PinDestLoc *>	 backwardsPinStack = stack<PinDestLoc *>();
	stack<int> backwardsSRNumStack = stack<int>();	// In a perfect world this wouldn't exist
	int lastSafePin = 0;
	bool isMovingHorizontally = false;

	int oCap = arch->getWireRouter()->getNumHorizTracks();
	int tileGridSize = ((oCap+2)*2 - 1);
	int buffer = 0;
	if(arch->shiftRegisters->size() != 0)
		buffer = ((arch->shiftRegisters->at(0)->pinHeight * 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);

	if(arch->pcb->numSRRight == 1)
		buffer *= 2;

	for(unsigned arrayNo = initPinNo; arrayNo < endPinNo; arrayNo++)
		//for(unsigned arrayNo = endPinNo -2; arrayNo < endPinNo; arrayNo++)
	{
		bool needsVia = false;
		if(sidePins->at(arrayNo)->getLayerNum() != 0 && sidePins->at(arrayNo)->getLayerNum() != -1)
			needsVia = true;

		if(!sidePins->at(arrayNo)->willRouteToMC)
		{

			// If no possible outputs left, move to next Shift Register
			if(sideShiftRegs->at(srNo)->pinOutQueue->size() == 0)
			{
				if(srNo + 1 < sideShiftRegs->size())
					srNo++;
				else
					endEarly = true;
			}

			// If a pin must go downwards, figure out the furthest pin down that goes in that direction and route that one first
			if(!endEarly)
			{
				if((sidePins->at(0)->side == 0 || sidePins->at(0)->side == 2))
				{
					if(sidePins->at(0)->side == 2 && sideShiftRegs->at(srNo)->pinOutQueue->front()->xPos < sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX)
					{
						backwardsPinStack.push(sideShiftRegs->at(srNo)->pinOutQueue->front());
						backwardsSRNumStack.push(srNo);
						sideShiftRegs->at(srNo)->pinOutQueue->pop();
						isTravelingBackwards = true;

						if(!sidePins->at(arrayNo)->willRouteToMC)
							lastSafePin = arrayNo;
					}
					else if(sidePins->at(0)->side == 0 && sideShiftRegs->at(srNo)->pinOutQueue->front()->xPos > sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX)
					{
						backwardsPinStack.push(sideShiftRegs->at(srNo)->pinOutQueue->front());
						backwardsSRNumStack.push(srNo);
						sideShiftRegs->at(srNo)->pinOutQueue->pop();
						isTravelingBackwards = true;

						if(!sidePins->at(arrayNo)->willRouteToMC)
							lastSafePin = arrayNo;
					}
					else
					{
						if(isTravelingBackwards)
						{
							int arrayNum = arrayNo - 1;

							while(backwardsPinStack.size() != 0)
							{
								PinDestLoc *destLoc = backwardsPinStack.top();
								backwardsPinStack.pop();
								int srNumber = backwardsSRNumStack.top();
								backwardsSRNumStack.pop();
								WireSegment *wsDest = sidePins->at(arrayNum);

								routeIndividualWire(arch, destLoc, wsDest, sideShiftRegs->at(srNumber), sideShiftRegs->size());

								arrayNum--;
							}
						}

						isTravelingBackwards = false;
					}
				}
				else
				{
					if(sidePins->at(0)->side == 1 && sideShiftRegs->at(srNo)->pinOutQueue->front()->yPos > sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY)
					{
						backwardsPinStack.push(sideShiftRegs->at(srNo)->pinOutQueue->front());
						sideShiftRegs->at(srNo)->pinOutQueue->pop();
						backwardsSRNumStack.push(srNo);
						isTravelingBackwards = true;

						if(!sidePins->at(arrayNo)->willRouteToMC)
							lastSafePin = arrayNo;
					}
					else if(sidePins->at(0)->side == 3 && sideShiftRegs->at(srNo)->pinOutQueue->front()->yPos < sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY)
					{
						backwardsPinStack.push(sideShiftRegs->at(srNo)->pinOutQueue->front());
						sideShiftRegs->at(srNo)->pinOutQueue->pop();
						backwardsSRNumStack.push(srNo);
						isTravelingBackwards = true;

						if(!sidePins->at(arrayNo)->willRouteToMC)
							lastSafePin = arrayNo;
					}
					else
					{
						if(isTravelingBackwards)
						{
							int arrayNum = arrayNo - 1;

							while(backwardsPinStack.size() != 0)
							{
								PinDestLoc *destLoc = backwardsPinStack.top();
								backwardsPinStack.pop();
								int srNumber = backwardsSRNumStack.top();
								backwardsSRNumStack.pop();
								WireSegment *wsDest = sidePins->at(arrayNum);

								routeIndividualWire(arch, destLoc, wsDest, sideShiftRegs->at(srNumber), sideShiftRegs->size());

								arrayNum--;
							}
						}

						isTravelingBackwards = false;
					}
				}

				// Route the pin
				if(srNo < sideShiftRegs->size() && !sideShiftRegs->at(srNo)->pinOutQueue->size() == 0 && !isTravelingBackwards)
				{
					PinDestLoc *destLoc = sideShiftRegs->at(srNo)->pinOutQueue->front();
					sideShiftRegs->at(srNo)->pinOutQueue->pop();
					WireSegment *wsDest = sidePins->at(arrayNo);

					// Increment total manhattan distance
					int distAcrossHoriz = abs(sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX - destLoc->xPos);
					int distAcrossVert = abs(sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY - destLoc->yPos);
					arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

					// Find the initial direction to move
					if(sidePins->at(arrayNo)->side == 0 || sidePins->at(arrayNo)->side == 2)
						isMovingHorizontally = true;
					else
						isMovingHorizontally = false;

					// Perform the routing
					if(sidePins->at(0)->side != 1)
					{

						if(destLoc->isOuterSR)
						{
							PinDestLoc *realPinLoc = sideShiftRegs->at(srNo)->realOuterPinLocs->front();
							sideShiftRegs->at(srNo)->realOuterPinLocs->pop();

							routeWire(arch, NULL, wsDest->getLayerNum(),  wsDest->getPinNum(), wsDest->getSourceWireCellX(0) + arch->wrOffsetX, wsDest->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos, isMovingHorizontally, false, false, wsDest->side, false);
							routeWire(arch, NULL, wsDest->getLayerNum(),  wsDest->getPinNum(), destLoc->xPos, destLoc->yPos, realPinLoc->xPos, realPinLoc->yPos, isMovingHorizontally, false, needsVia, wsDest->side, false);

							if(sideShiftRegs->at(srNo)->rotateDeg == 0)	// This puts them each right on the side of SR so that it is easy to go around
							{
								sideShiftRegs->at(srNo)->sideCoordY += 2;
								sideShiftRegs->at(srNo)->sideCoordX += 2;
							}
							else if(sideShiftRegs->at(srNo)->rotateDeg == 90)
							{
								sideShiftRegs->at(srNo)->sideCoordX -= 2;
								sideShiftRegs->at(srNo)->sideCoordY += 2;
							}
							else if(sideShiftRegs->at(srNo)->rotateDeg == 180)
							{
								sideShiftRegs->at(srNo)->sideCoordY -= 2;
								sideShiftRegs->at(srNo)->sideCoordX -= 2;
							}
							else
							{
								sideShiftRegs->at(srNo)->sideCoordX += 2;
								sideShiftRegs->at(srNo)->sideCoordY -= 2;
							}
						}
						else
							routeWire(arch, NULL, sidePins->at(arrayNo)->getLayerNum(),  sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos, isMovingHorizontally, false, needsVia, sidePins->at(arrayNo)->side, false);
					}
					else if(sideShiftRegs->size() != 1)
						routeWire(arch, NULL, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos, isMovingHorizontally, false, needsVia, sidePins->at(arrayNo)->side, false);
					else if(sideShiftRegs->size() == 1)
					{
						routeWire(arch, NULL, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX + buffer, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, !isMovingHorizontally, false, false, sidePins->at(arrayNo)->side, false);
						routeWire(arch, NULL, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX + buffer, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos, isMovingHorizontally, false, needsVia, sidePins->at(arrayNo)->side, false);
					}
					numRouted++;
				}
			}
			else
			{
				// If ran out of pins, still go back to check that all reverse pins have been routed
				if(isTravelingBackwards)
				{
					int arrayNum = arrayNo - 1;

					while(backwardsPinStack.size() != 0)
					{
						PinDestLoc *destLoc = backwardsPinStack.top();
						backwardsPinStack.pop();
						int srNumber = backwardsSRNumStack.top();
						backwardsSRNumStack.pop();
						WireSegment *wsDest = sidePins->at(arrayNum);

						routeIndividualWire(arch, destLoc, wsDest, sideShiftRegs->at(srNumber), sideShiftRegs->size());

						arrayNum--;
					}
				}
			}

			if(isTravelingBackwards && arrayNo == endPinNo - 1)
			{
				int arrayNum = lastSafePin;

				while(backwardsPinStack.size() != 0)
				{
					PinDestLoc *destLoc = backwardsPinStack.top();
					backwardsPinStack.pop();
					int srNumber = backwardsSRNumStack.top();
					backwardsSRNumStack.pop();
					WireSegment *wsDest = sidePins->at(arrayNum);

					routeIndividualWire(arch, destLoc, wsDest, sideShiftRegs->at(srNumber), sideShiftRegs->size());

					arrayNum--;
				}
			}
		}
		else	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		{
			if(sideShiftRegs->size() != 0 && sideShiftRegs->at(srNo)->pinOutQueue->size() == 0)
			{
				if(srNo + 1 < sideShiftRegs->size())
					srNo++;
				else
					endEarly = true;
			}

			// If a pin must go downwards, figure out the furthest pin down that goes in that direction and route that one first
			if((sidePins->at(0)->side == 0 || sidePins->at(0)->side == 2))
			{
			}
			else
			{
				if(sideShiftRegs->size() != 0)
				{
					bool movHoriz = false;
					if(sidePins->at(0)->side == 0 || sidePins->at(0)->side == 2)
						movHoriz = true;
					int oCap = arch->getWireRouter()->getNumHorizTracks();
					int tileGridSize = ((oCap+2)*2 - 1);
					int xForMC = 0;
					if(sideShiftRegs->size() != 0 && sideShiftRegs->size() != 1)
						xForMC = ((sideShiftRegs->at(srNo - 1)->xPos + sideShiftRegs->at(srNo - 1)->fullBodyWidthMM * sideShiftRegs->at(srNo - 1)->conversionFactor) / sideShiftRegs->at(srNo - 1)->cellDimPixels) * (tileGridSize - 1);
					else if(sideShiftRegs->size() == 1)
						xForMC = ((sideShiftRegs->at(srNo)->xPos + sideShiftRegs->at(srNo)->fullBodyWidthMM * sideShiftRegs->at(srNo)->conversionFactor) / sideShiftRegs->at(srNo)->cellDimPixels) * (tileGridSize - 1);
					else
						xForMC = sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX + 5;

					// Calculate init coordinate to route to
					int yLoc = 0;
					if(sideShiftRegs->size() != 0 && sideShiftRegs->size() != 1)
						yLoc = calculateRelevantCoordUnderSR(arch, sidePins->at(arrayNo), sideShiftRegs->at(srNo - 1), sidePins->at(arrayNo)->numBeforeMC) + 1;	// + 1 for visual
					else if(sideShiftRegs->size() == 1)
						yLoc = calculateRelevantCoordUnderSR(arch, sidePins->at(arrayNo), sideShiftRegs->at(srNo), sidePins->at(arrayNo)->numBeforeMC) + 1;	// + 1 for visual
					else
						yLoc = calculateRelevantCoordUnderSR(arch, sidePins->at(arrayNo), NULL, sidePins->at(arrayNo)->numBeforeMC) + 1;
					arch->nextStepPositions->push_back(new PinDestLoc(arch->mcSide, -1, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), xForMC, yLoc, PinType::IO_OTHER, false));

					int test = sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY;
					if(sidePins->at(0)->side == 1 && yLoc > sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY)
					{
						// Make a dummy Shift Reg pin at the destination location
						backwardsPinStack.push(new PinDestLoc(arch->mcSide, -1, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), xForMC, yLoc, PinType::IO_OTHER, false));
						isTravelingBackwards = true;

						if(!sidePins->at(arrayNo)->willRouteToMC)
							lastSafePin = arrayNo;
					}
					else if(sidePins->at(0)->side == 3 && yLoc < sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY)
					{
						backwardsPinStack.push(new PinDestLoc(arch->mcSide, -1,sidePins->at(arrayNo)->getLayerNum(),  sidePins->at(arrayNo)->getPinNum(), xForMC, yLoc, PinType::IO_OTHER, false));
						isTravelingBackwards = true;

						if(!sidePins->at(arrayNo)->willRouteToMC)
							lastSafePin = arrayNo;
					}
					else
					{
						if(isTravelingBackwards)
						{
							int arrayNum = arrayNo - 1;

							while(backwardsPinStack.size() != 0)
							{
								PinDestLoc *next = backwardsPinStack.top();
								backwardsPinStack.pop();

								// Find the initial direction to move
								bool isMovingHorizontally;
								if(sidePins->at(arrayNum)->side == 0 || sidePins->at(arrayNum)->side == 2)
									isMovingHorizontally = true;
								else
									isMovingHorizontally = false;

								// Route the pin
								routeWire(arch, NULL, sidePins->at(arrayNum)->getLayerNum(), sidePins->at(arrayNum)->getPinNum(), sidePins->at(arrayNum)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNum)->getSourceWireCellY(0) + arch->wrOffsetY, next->xPos, next->yPos, isMovingHorizontally, false, false, sidePins->at(arrayNum)->side, false);

								arrayNum--;
							}
						}

						isTravelingBackwards = false;
					}
				}
				else
				{
					//					if(sideShiftRegs->size() != 1)
					//						arch->nextStepPositions->push_back(new PinDestLoc(arch->mcSide, -1, sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, PinType::IO_OTHER, false));
					//					else
					arch->nextStepPositions->push_back(new PinDestLoc(arch->mcSide, -1, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, PinType::IO_OTHER, false));
				}

				// Route the pin
				if(srNo < sideShiftRegs->size() && (!sideShiftRegs->at(srNo)->pinOutQueue->size() == 0 || arch->pcb->numSRRight == 1) && !isTravelingBackwards)
				{
					bool movHoriz = false;
					if(sidePins->at(0)->side == 0 || sidePins->at(0)->side == 2)
						movHoriz = true;
					int oCap = arch->getWireRouter()->getNumHorizTracks();
					int tileGridSize = ((oCap+2)*2 - 1);
					int xForMC = 0;
					if(sideShiftRegs->size() != 0 && sideShiftRegs->size() != 1)
						xForMC = ((sideShiftRegs->at(srNo - 1)->xPos + sideShiftRegs->at(srNo - 1)->fullBodyWidthMM * sideShiftRegs->at(srNo - 1)->conversionFactor) / sideShiftRegs->at(srNo - 1)->cellDimPixels) * (tileGridSize - 1);
					else if(sideShiftRegs->size() == 1)
						xForMC = ((sideShiftRegs->at(srNo)->xPos + sideShiftRegs->at(srNo)->fullBodyWidthMM * sideShiftRegs->at(srNo)->conversionFactor) / sideShiftRegs->at(srNo)->cellDimPixels) * (tileGridSize - 1);
					else
						xForMC = sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX + 5;
					// Calculate init coordinate to route to
					int yLoc = 0;
					if(sideShiftRegs->size() != 0 && sideShiftRegs->size() != 1)
						yLoc = calculateRelevantCoordUnderSR(arch, sidePins->at(arrayNo), sideShiftRegs->at(srNo - 1), sidePins->at(arrayNo)->numBeforeMC) + 1;	// + 1 for visual
					else if(sideShiftRegs->size() == 1)
						yLoc = calculateRelevantCoordUnderSR(arch, sidePins->at(arrayNo), sideShiftRegs->at(srNo), sidePins->at(arrayNo)->numBeforeMC) + 1;	// + 1 for visual
					else
						yLoc = calculateRelevantCoordUnderSR(arch, sidePins->at(arrayNo), NULL, sidePins->at(arrayNo)->numBeforeMC) + 1;


					// Find the initial direction to move
					bool isMovingHorizontally;
					if(sidePins->at(arrayNo)->side == 0 || sidePins->at(arrayNo)->side == 2)
						isMovingHorizontally = true;
					else
						isMovingHorizontally = false;

					// Perform the routing
					routeWire(arch, NULL, sidePins->at(arrayNo)->getLayerNum(), sidePins->at(arrayNo)->getPinNum(), sidePins->at(arrayNo)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNo)->getSourceWireCellY(0) + arch->wrOffsetY, xForMC, yLoc, isMovingHorizontally, false, false, sidePins->at(arrayNo)->side, false);
					numRouted++;
				}
			}
		}

		if(isTravelingBackwards && arrayNo == endPinNo - 1)
		{
			if(lastSafePin != 0)
			{
				int arrayNum = lastSafePin;

				while(backwardsPinStack.size() != 0)
				{
					PinDestLoc *sr = backwardsPinStack.top();
					backwardsPinStack.pop();

					if(sidePins->at(arrayNum)->getLayerNum() != 0 && sidePins->at(arrayNum)->getLayerNum() != -1)
						needsVia = true;
					else
						needsVia = false;

					// Find the initial direction to move
					bool isMovingHorizontally;
					if(sidePins->at(arrayNum)->side == 0 || sidePins->at(arrayNum)->side == 2)
						isMovingHorizontally = true;
					else
						isMovingHorizontally = false;

					// Route the pin
					routeWire(arch, NULL, sidePins->at(arrayNum)->getLayerNum(), sidePins->at(arrayNum)->getPinNum(), sidePins->at(arrayNum)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNum)->getSourceWireCellY(0) + arch->wrOffsetY, sr->xPos, sr->yPos, isMovingHorizontally, false, false, sidePins->at(arrayNum)->side, false);
					arrayNum--;
				}
			}
			else
			{
				int arrayNum = arrayNo;

				while(backwardsPinStack.size() != 0)
				{
					PinDestLoc *sr = backwardsPinStack.top();
					backwardsPinStack.pop();

					// Find the initial direction to move
					bool isMovingHorizontally;
					if(sidePins->at(arrayNo)->side == 0 || sidePins->at(arrayNo)->side == 2)
						isMovingHorizontally = true;
					else
						isMovingHorizontally = false;

					// Route the pin
					routeWire(arch, NULL, sidePins->at(arrayNum)->getLayerNum(), sidePins->at(arrayNum)->getPinNum(), sidePins->at(arrayNum)->getSourceWireCellX(0) + arch->wrOffsetX, sidePins->at(arrayNum)->getSourceWireCellY(0) + arch->wrOffsetY, sr->xPos, sr->yPos, isMovingHorizontally, false, false, sidePins->at(arrayNum)->side, false);
					arrayNum--;
				}
			}
		}
	}

	// Finish the routing to Microcontroller
	if(arch->nextStepPositions->size() != 0)
	{
		if(!arch->hasInitToAimFor)
		{
			arch->xCoordToAimFor = -1;
			arch->yCoordToAimFor = -1;
			arch->hasInitToAimFor = true;

		}

		stack<PinDestLoc *> *backwardsStartPosStack = new stack<PinDestLoc *>();
		for(unsigned arrayNo = 0; arrayNo < arch->nextStepPositions->size(); arrayNo++)
			//for(unsigned arrayNo = 0; arrayNo < 1; arrayNo++)
		{
			int a = 0;
			if(arrayNo == arch->nextStepPositions->size() - 1)
				a++;

			if(arch->microcontroller->pinOutQueue->front() != NULL)
			{
				PinDestLoc *pin = arch->nextStepPositions->at(arrayNo);
				int oCap = arch->getWireRouter()->getNumHorizTracks();
				int tileGridSize = ((oCap+2)*2 - 1);
				ShiftRegister dummySR = ShiftRegister(arch, arch->pcb->numShiftRegs, 0, true, 0);
				int buffer = ((dummySR.pinHeight * 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
				if(arch->pcb->numSRRight > 0)
					buffer /= 2;
				int offsetX = 0;
				int offsetY = 0;

				if((pin->side == 0 || pin->side == 2) && arch->microcontroller->pinOutQueue->size() > 0)
				{
					if(pin->side == 2 && (arch->microcontroller->pinOutQueue->front()->xPos > pin->xPos && arch->microcontroller->pinOutQueue->front()->side == 0))
					{
						backwardsPinStack.push(arch->microcontroller->pinOutQueue->front());
						arch->microcontroller->pinOutQueue->pop();
						backwardsStartPosStack->push(pin);
						isTravelingBackwards = true;
					}
					else if(pin->side == 0 && (arch->microcontroller->pinOutQueue->front()->xPos < pin->xPos && arch->microcontroller->pinOutQueue->front()->side == 0))
					{
						backwardsPinStack.push(arch->microcontroller->pinOutQueue->front());
						arch->microcontroller->pinOutQueue->pop();
						backwardsStartPosStack->push(pin);
						isTravelingBackwards = true;
					}
					else
					{
						if(isTravelingBackwards)
						{
							int arrayNum = arrayNo - 1;

							while(backwardsPinStack.size() != 0)
							{
								offsetX = 0;
								offsetY = 0;
								if(backwardsPinStack.top()->side == 0)
								{
									offsetX -= buffer;
								}
								else if(backwardsPinStack.top()->side == 1)
								{
									offsetY += buffer;
								}
								else if(backwardsPinStack.top()->side == 2)
								{
									offsetX += buffer;
								}
								else if(backwardsPinStack.top()->side == 3)
								{
									offsetY -= buffer;
								}

								PinDestLoc *mcPinPos = backwardsPinStack.top();
								backwardsPinStack.pop();
								PinDestLoc *startPos = backwardsStartPosStack->top();
								backwardsStartPosStack->pop();

								// Find the initial direction to move
								if(pin->side == 1 || pin->side == 3)
									isMovingHorizontally = true;
								else
									isMovingHorizontally = false;

								// Route the pin
								bool needsVia = false;
								if(startPos->layer != 0)
									needsVia = true;
								int i = -1 * startPos->pinNo;
								routeWire(arch, NULL, startPos->layer,  startPos->pinNo, startPos->xPos, startPos->yPos, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, isMovingHorizontally, false, false, pin->side, false);
								routeWire(arch, NULL, startPos->layer, startPos->pinNo, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos, isMovingHorizontally, false, needsVia, pin->side, false);
								//arch->areaRouteWires->push_back(new WireSegment(-1 * startPos->pinNo, 0, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos));
								arrayNum--;
							}
						}

						isTravelingBackwards = false;
					}
				}
				else
				{
					if(pin->side == 1 && (arch->microcontroller->pinOutQueue->front()->yPos < pin->yPos && arch->microcontroller->pinOutQueue->front()->side == 0))
					{
						int test = arch->microcontroller->pinOutQueue->front()->side;
						backwardsPinStack.push(arch->microcontroller->pinOutQueue->front());
						arch->microcontroller->pinOutQueue->pop();
						backwardsStartPosStack->push(pin);
						isTravelingBackwards = true;
					}
					else if(pin->side == 3 && (arch->microcontroller->pinOutQueue->front()->yPos > pin->yPos && arch->microcontroller->pinOutQueue->front()->side == 0))
					{
						backwardsPinStack.push(arch->microcontroller->pinOutQueue->front());
						arch->microcontroller->pinOutQueue->pop();
						backwardsStartPosStack->push(pin);
						isTravelingBackwards = true;
					}
					else
					{
						if(isTravelingBackwards)
						{
							int arrayNum = arrayNo - 1;

							while(backwardsPinStack.size() != 0)
							{
								offsetX = 0;
								offsetY = 0;
								if(backwardsPinStack.top()->side == 0)
								{
									offsetX -= buffer;
								}
								else if(backwardsPinStack.top()->side == 1)
								{
									offsetY += buffer;
								}
								else if(backwardsPinStack.top()->side == 2)
								{
									offsetX += buffer;
								}
								else if(backwardsPinStack.top()->side == 3)
								{
									offsetY -= buffer;
								}

								PinDestLoc *mcPinPos = backwardsPinStack.top();
								backwardsPinStack.pop();
								PinDestLoc *startPos = backwardsStartPosStack->top();
								backwardsStartPosStack->pop();


								// Find the initial direction to move
								if(pin->side == 1 || pin->side == 3)
									isMovingHorizontally = true;
								else
									isMovingHorizontally = false;

								// Route the pin
								bool shouldCrawl = false;
								if(mcPinPos->side != 0)
									shouldCrawl = true;

								bool needsVia = false;
								if(startPos->layer != 0)
									needsVia = true;
								routeWire(arch, NULL, startPos->layer,  startPos->pinNo, startPos->xPos, startPos->yPos, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, isMovingHorizontally, false, false, pin->side, shouldCrawl);
								routeWire(arch, NULL, startPos->layer, startPos->pinNo, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos, isMovingHorizontally, false, needsVia, pin->side, false);
								//arch->areaRouteWires->push_back(new WireSegment(-1 * startPos->pinNo, 0, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos));
								arrayNum--;
							}
						}

						isTravelingBackwards = false;
					}
				}

				// Route the pin
				if(!arch->microcontroller->pinOutQueue->size() == 0 && !isTravelingBackwards)
				{
					PinDestLoc *mcPinPos = arch->microcontroller->pinOutQueue->front();
					arch->microcontroller->pinOutQueue->pop();

					offsetX = 0;
					offsetY = 0;
					if(mcPinPos->side == 0)
					{
						offsetX -= buffer;
					}
					else if(mcPinPos->side == 1)
					{
						offsetY += buffer;
					}
					else if(mcPinPos->side == 2)
					{
						offsetX += buffer;
					}
					else if(mcPinPos->side == 3)
					{
						offsetY -= buffer;
					}

					// Find the initial direction to move
					if(pin->side == 1 || pin->side == 3)
						isMovingHorizontally = true;
					else
						isMovingHorizontally = false;

					// Route the pin
					bool shouldCrawl = false;
					if(mcPinPos->side != 0)
						shouldCrawl = true;

					// Route the pin
					bool needsVia = false;
					if(pin->layer != 0)
						needsVia = true;
					//arch->areaRouteWires->push_back(new WireSegment(-1 * mcPinPos->pinNo, 0, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos));
					routeWire(arch, NULL, pin->layer,  pin->pinNo, pin->xPos, pin->yPos, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, isMovingHorizontally, false, false, pin->side, shouldCrawl);
					routeWire(arch, NULL, pin->layer, pin->pinNo, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos, isMovingHorizontally, false, needsVia, pin->side, false);
					numRouted++;
				}

				if(isTravelingBackwards && (arrayNo == arch->nextStepPositions->size() - 1 || arch->microcontroller->pinOutQueue->size() == 0))
				{
					int arrayNum = lastSafePin;

					while(backwardsPinStack.size() != 0)
					{
						offsetX = 0;
						offsetY = 0;
						if(backwardsPinStack.top()->side == 0)
						{
							offsetX -= buffer;
						}
						else if(backwardsPinStack.top()->side == 1)
						{
							offsetY += buffer;
						}
						else if(backwardsPinStack.top()->side == 2)
						{
							offsetX += buffer;
						}
						else if(backwardsPinStack.top()->side == 3)
						{
							offsetY -= buffer;
						}

						PinDestLoc *mcPinPos = backwardsPinStack.top();
						backwardsPinStack.pop();
						PinDestLoc *startPos = backwardsStartPosStack->top();
						backwardsStartPosStack->pop();


						// Find the initial direction to move
						if(pin->side == 1 || pin->side == 3)
							isMovingHorizontally = true;
						else
							isMovingHorizontally = false;

						// Route the pin
						bool shouldCrawl = false;
						if(mcPinPos->side != 0)
							shouldCrawl = true;

						// Route the pin
						bool needsVia = false;
						if(startPos->layer != 0)
							needsVia = true;
						routeWire(arch, NULL, startPos->layer,  startPos->pinNo, startPos->xPos, startPos->yPos, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, isMovingHorizontally, false, false, pin->side, shouldCrawl);
						routeWire(arch, NULL, startPos->layer, startPos->pinNo, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos, isMovingHorizontally, false, needsVia, pin->side, false);
						//			arch->areaRouteWires->push_back(new WireSegment(-1 * startPos->pinNo, 0, mcPinPos->xPos + offsetX, mcPinPos->yPos + offsetY, mcPinPos->xPos, mcPinPos->yPos));
						arrayNum--;
					}
				}
			}
		}
	}

	arch->nextStepPositions->clear();
}

void PCBLayout::performFullSideAreaRoute(DmfbArch *arch, vector<WireSegment* > *sidePins, vector<ShiftRegister* > *sideShiftRegs, bool isMCSide, int numExcessOnMCSide)
{
	// Route each array pin to proper location
	int numSR = 1;//breakpoint
	if(!isMCSide)
	{
		routeWireGroup(arch, sidePins, 0, sidePins->size(), sideShiftRegs, 0, numExcessOnMCSide);
	}
	else
	{
		int nextUnroutedPin = 0;

		arch->pcb->numOutputsSRSide = 0;	// Unused, can be used to separate wires on side with MC using Shift Registers in between them

		arch->pcb->numToMCPerSection = 0;

		arch->pcb->numToMCPerSection = sidePins->size();

		if(arch->pcb->numSRRight != 0)
		{
			for(unsigned j = 0; j < sideShiftRegs->size(); j++)
			{
				ShiftRegister* srTop = sideShiftRegs->at(j);
				int temp = srTop->pinOutQueue->size();
				for(unsigned i = 0; i < temp; i++)
				{
					WireSegment* curr = sidePins->at(nextUnroutedPin);
					PinDestLoc* currEnd = srTop->pinOutQueue->front();
					srTop->pinOutQueue->pop();

					// Route up to the top
					routeWire(arch, NULL, curr->getLayerNum(), curr->getPinNum(), curr->getDestWireCellX(0) + arch->wrOffsetX, curr->getDestWireCellY(0) + arch->wrOffsetY, currEnd->xPos, currEnd->yPos + 5, false, false,false, curr->side, false);
					routeWire(arch, NULL, curr->getLayerNum(), curr->getPinNum(), currEnd->xPos, currEnd->yPos + 5, currEnd->xPos, currEnd->yPos, false, false,false, curr->side, false);
					nextUnroutedPin++;
				}
			}
		}


		if(arch->pcb->numSRRight >= 1)// Split the routing of MC in half (half upwards half downwards) to leave space for power wire
		{
			queue<PinDestLoc *> *tempQ = new queue<PinDestLoc *>();

			int oCap = arch->getWireRouter()->getNumHorizTracks();
			int tileGridSize = ((oCap+2)*2 - 1);
			int yPosCurr = ((arch->microcontroller->yPos - arch->microcontroller->pinHeight) / arch->pcb->cellDimPixels) * (tileGridSize - 1) - (2 * (35 - 7));
			int xPosCurr = ((arch->microcontroller->xPos) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
			stack<PinDestLoc *> *betweenS = new stack<PinDestLoc *>();
			for(unsigned i = 0; i < arch->microcontroller->numBackside - 3; i++)
			{
				if(nextUnroutedPin < sidePins->size())
				{
					WireSegment *currWire = sidePins->at(nextUnroutedPin++);
					betweenS->push(new PinDestLoc(0, currWire->getId(), currWire->getLayerNum(), currWire->getPinNum(), xPosCurr, yPosCurr, PinType::IO_OTHER, false));
					routeWire(arch, NULL, currWire->getLayerNum(), currWire->getPinNum(), currWire->getSourceWireCellX(0) + arch->wrOffsetX, currWire->getSourceWireCellY(0) + arch->wrOffsetY,xPosCurr, yPosCurr, false, false, false, 0, false);
					yPosCurr += 2;
				}
			}

			ShiftRegister dummySR = ShiftRegister(arch, arch->pcb->numShiftRegs, 0, true, 0);
			int buffer = ((dummySR.pinHeight * 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
			if(arch->pcb->numSRRight > 0)
				buffer /= 2;
			for(unsigned i = 0; i < arch->microcontroller->numBackside - 3; i++)
			{
				PinDestLoc *currS = betweenS->top();
				betweenS->pop();
				PinDestLoc *currE = arch->microcontroller->pinOutQueue->front();
				arch->microcontroller->pinOutQueue->pop();
				if(currE->side == 3)
				{
					routeWire(arch, NULL, currS->layer, currS->pinNo, currS->xPos, currS->yPos, currE->xPos, currE->yPos - buffer, false, false, false, currE->side, false);
					routeWire(arch, NULL, currS->layer, currS->pinNo, currE->xPos, currE->yPos - buffer, currE->xPos, currE->yPos, false, false, false, currE->side, false);
				}
				else
				{
					routeWire(arch, NULL, currS->layer, currS->pinNo, currS->xPos, currS->yPos, currE->xPos + buffer, currE->yPos, false, false, false, currE->side, false);
					routeWire(arch, NULL, currS->layer, currS->pinNo, currE->xPos + buffer, currE->yPos, currE->xPos, currE->yPos, false, false, false, currE->side, false);
				}
			}
		}

		if(arch->pcb->numSRRight > 0)
			routeWireGroup(arch, sidePins, nextUnroutedPin, sidePins->size(), new vector<ShiftRegister*>(), 0, 0);
		else
			routeWireGroup(arch, sidePins, nextUnroutedPin, sidePins->size(), new vector<ShiftRegister*>(), 0, 0);
	}
}

void PCBLayout::createVia(DmfbArch* arch, int xGridCoord, int yGridCoord, int beginLayer, int endLayer)
{
	Via* via = new Via(arch, xGridCoord, yGridCoord, beginLayer, endLayer);

	arch->vias->push_back(via);
}

void PCBLayout::routeWire(DmfbArch *arch, queue<PinDestLoc *> *mcPinQueue, int layer, int pinNo, int firstX, int firstY, int destX, int destY, bool movHoriz, bool isExcess, bool needsVia, int side, bool crawlAlongSide)
{
	bool keepXNormal = false;
	bool keepYNormal = false;
	bool notStartNewWire = false;
	if(crawlAlongSide)
	{
		notStartNewWire = true;
		if(firstX == destX)
			keepYNormal = true;
		else if(firstY == destY)
			keepXNormal = true;
		else
			keepXNormal = true;
	}

	int xCoord = firstX;
	int yCoord = firstY;
	int startingX = xCoord;
	int startingY = yCoord;
	int oneBackX = xCoord;
	int oneBackY = yCoord;
	int twoBackX = xCoord;
	int twoBackY = yCoord;
	int threeBackX = xCoord;
	int threeBackY = yCoord;
	int lockedXChange = 0;
	int lockedYChange = 0;
	int desiredX = destX;
	int desiredY = destY;
	Via *via = new Via(arch, 0, 0, 0, 1);
	int viaDistanceNeeded = (via->rightX - via->leftX) * 2;
	bool isRouting = true;
	bool shouldStartNewWire = false;
	bool justHitSomething = false;
	bool shouldCrawlExcess = isExcess;
	bool isFirst = true;
	bool isMovingHorizontally = movHoriz;
	int moveNum = 0;
	while(isRouting)
	{
		bool foo = true;
		if(xCoord > destX)
			foo = false;

		// Check the end condition for when crawling along a border
		if((crawlAlongSide && (keepXNormal && xCoord == desiredX)) || (keepYNormal && yCoord == desiredY))
			crawlAlongSide = false;

		// Check that should place via
		int sign = 1;
		if(firstX == destX)
		{
			if(firstY > destY)
				sign = -1;

		}
		else if(firstY == destY)
		{
			if(firstX > destX)
				sign = -1;
		}
		else if(!isExcess)
		{
			if(side == 0 || side == 3)
				sign = -1;
		}
		else
		{
			if(side == 1 || side == 2)
				sign = -1;
		}

		if(isMovingHorizontally)
		{
			if(needsVia && xCoord == desiredX - viaDistanceNeeded * sign && via->x == 0)
			{
				// Draw the next wire to the destination (in order to be able to have two layers)
				int change = 0;
				if(layer == 0)
					change = 1;
				else
					change = -1;

				via = new Via(arch, xCoord, yCoord, layer, 0);
				arch->vias->push_back(via);

				routeWire(arch, NULL, 0, pinNo, xCoord, yCoord, desiredX, desiredY, isMovingHorizontally, isExcess, false, side, false);

				// Shorten the wire to the middle
				desiredX = xCoord;
				desiredY = yCoord;
			}
		}
		else
			if(needsVia && yCoord == desiredY - viaDistanceNeeded * sign)
			{
				// Draw the next wire to the destination (in order to be able to have two layers)
				int change = 0;
				if(layer == 0)
					change = 1;
				else
					change = -1;

				via = new Via(arch, xCoord, yCoord, layer, 0);
				arch->vias->push_back(via);

				routeWire(arch, NULL, 0, pinNo, xCoord, yCoord, desiredX, desiredY, isMovingHorizontally, isExcess, false, side, false);


				desiredY = yCoord;
				desiredX = xCoord;
			}


		// Check for match
		if(xCoord == desiredX && yCoord == desiredY)
		{
			// Place wire up to this point
			arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));

			if(isMovingHorizontally)
				createUnrouteableZoneHoriz(arch, startingX, xCoord, yCoord, layer);
			else
				createUnrouteableZoneVert(arch, startingY, yCoord, xCoord, layer);

			if(arch->xCoordToAimFor == -1)
			{
				arch->xCoordToAimFor = xCoord;
				arch->yCoordToAimFor = yCoord;
			}

			isRouting = false;
		}
		else if(arch->wireGrid[layer][xCoord][yCoord] && !isFirst) // Check for hit
		{
			// Get rid of this condition as it is only used to prevent possible stack overflows
			notStartNewWire = false;

			// Check whether or not wire is stuck (hits one side, changes direction and backs up and immediately hits again)
			if(shouldCrawlExcess && justHitSomething && !(!notStartNewWire && (((desiredX == xCoord || desiredY == yCoord) && isRouting) || shouldStartNewWire)))
			{
				//isRouting = false;	// For debug purposes only
				//
				WireSegment *ws = arch->areaRouteWires->at(arch->areaRouteWires->size() - 1);
				arch->areaRouteWires->erase(arch->areaRouteWires->end() - 1);
				int goodX = ws->getSourceWireCellX(0);
				int goodY = ws->getSourceWireCellY(0);



				// Remove previous wires up to that point
				//				bool stillErasing = true;
				//				do
				//				{
				//ws = arch->areaRouteWires->at(arch->areaRouteWires->size() - 1);

				//					if((isMovingHorizontally && ws->getSourceWireCellY(0) != goodY) || (!isMovingHorizontally && ws->getSourceWireCellX(0) == goodX))
				//					{
				//						stillErasing = false;
				//					}

				//arch->areaRouteWires->erase(arch->areaRouteWires->end() - 1);
				//				} while(stillErasing && arch->areaRouteWires->size() > 0);

				// Set to crawl along side state
				crawlAlongSide = true;
				if(!isMovingHorizontally)
					keepXNormal = true;
				else
					keepYNormal = true;

				xCoord = ws->getSourceWireCellX(0);
				yCoord= ws->getSourceWireCellY(0);
				startingX = ws->getSourceWireCellX(0);
				startingY = ws->getSourceWireCellY(0);

				justHitSomething = true;
			}
			else if(isMovingHorizontally)
			{
				// Back up and change direction (back up 2 for space between wires
				if(crawlAlongSide)
				{
					if(keepXNormal)
					{
						if(xCoord < desiredX)
							xCoord -= 2;
						else
							xCoord += 2;
					}
					else if(arch->wireGrid[layer][xCoord + 1][yCoord] || arch->wireGrid[layer][xCoord + 2][yCoord])
						xCoord--;
					else
						xCoord++;
				}
				else if(xCoord == desiredX)
					xCoord =  threeBackX;	// xCoord = threeBackX;
				else if(xCoord < desiredX)
					xCoord -= 2;
				else
					xCoord += 2;

				isMovingHorizontally = !isMovingHorizontally;


				// Place wire up to this point
				if(startingX != xCoord || startingY != yCoord)
				{
					arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
					createUnrouteableZoneHoriz(arch, startingX, xCoord, yCoord, layer);

					// Begin next wire
					startingX = xCoord;
					startingY = yCoord;
				}

				// Jump up to avoid the hit
				if(crawlAlongSide)
				{
					if(keepYNormal)
					{
						if(desiredY >= yCoord)
							yCoord++;
						else
							yCoord--;
					}
					else if(arch->wireGrid[layer][xCoord][yCoord + 1] || arch->wireGrid[layer][xCoord][yCoord + 2])
						yCoord--;
					else
						yCoord++;
				}
				else if(!isExcess)
					if(desiredY > yCoord)
						yCoord++;
					else if(desiredY == yCoord)
						shouldStartNewWire = true;
					else
						yCoord--;
				else
				{
					if(side == 0)
						yCoord--;
					else if(side == 1)
						yCoord--;
					else if(side == 2)
						yCoord++;
					else if(side == 3)
						yCoord++;

					isExcess = false;
				}
				justHitSomething = true;
			}
			else
			{
				if(crawlAlongSide)
				{
					if(keepYNormal)
					{
						if(yCoord <= desiredY)
							yCoord -= 2;
						else
							yCoord += 2;
					}
					else if(arch->wireGrid[layer][xCoord][yCoord + 1] || arch->wireGrid[layer][xCoord][yCoord + 2])
						yCoord--;
					else
						yCoord++;
				}
				else if(yCoord == desiredY && threeBackY != yCoord)
					yCoord = threeBackY;	// yCoord = threeBackY
				else if(yCoord < desiredY)
					yCoord -= 2;
				else
					yCoord += 2;
				isMovingHorizontally = !isMovingHorizontally;

				if(startingX != xCoord || startingY != yCoord)
				{
					arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
					createUnrouteableZoneVert(arch, startingY, yCoord, xCoord, layer);

					startingX = xCoord;
					startingY = yCoord;
				}

				if(crawlAlongSide)
				{
					if(keepXNormal)
					{
						if(desiredX >= xCoord)
							xCoord++;
						else
							xCoord--;
					}
					else if(arch->wireGrid[layer][xCoord + 1][yCoord] || arch->wireGrid[layer][xCoord + 2][yCoord])
						xCoord--;
					else
						xCoord++;
				}
				else if(!isExcess)
					if(desiredX > xCoord)
						xCoord++;
					else if(desiredX == xCoord)
						shouldStartNewWire = true;
					else
						xCoord--;
				else
				{
					if(side == 0)
						xCoord++;
					else if(side == 1)
						xCoord++;
					else if(side == 2)
						xCoord--;
					else if(side == 3)
						xCoord--;

					isExcess = false;
				}
				justHitSomething = true;
			}
		}
		else if(xCoord == desiredX || yCoord == desiredY)	// Check that reached halfway to destination
		{
			if(xCoord == desiredX && isMovingHorizontally == true)
			{
				isMovingHorizontally = false;

				arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
				createUnrouteableZoneHoriz(arch, startingX, xCoord, yCoord, layer);

				startingX = xCoord;
				startingY = yCoord;
			}
			else if(yCoord == desiredY && isMovingHorizontally == false)
			{
				isMovingHorizontally = true;

				arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
				createUnrouteableZoneVert(arch, startingY, yCoord, xCoord, layer);

				startingX = xCoord;
				startingY = yCoord;
			}
		}
		else if(isMovingHorizontally && yCoord != desiredY && justHitSomething)// If is traveling in a direction to avoid something, move back
		{
			isMovingHorizontally = false;

			arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
			createUnrouteableZoneHoriz(arch, startingX, xCoord, yCoord, layer);

			startingX = xCoord;
			startingY = yCoord;

			justHitSomething = false;
		}
		else if(!isMovingHorizontally && xCoord != desiredX && justHitSomething)
		{
			isMovingHorizontally = true;

			arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
			createUnrouteableZoneVert(arch, startingY, yCoord, xCoord, layer);

			startingX = xCoord;
			startingY = yCoord;

			justHitSomething = false;
		}

		// Check end condition for crawl
		if (crawlAlongSide && keepXNormal)
		{
			if(xCoord == arch->xCoordToAimFor)
			{
				crawlAlongSide = false;
				isMovingHorizontally = !isMovingHorizontally;

				arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
				createUnrouteableZoneHoriz(arch, startingX, xCoord, yCoord, layer);


				startingX = xCoord;
				startingY = yCoord;

				arch->xCoordToAimFor = xCoord;
				arch->yCoordToAimFor = yCoord;
			}
		}
		else if(crawlAlongSide && keepYNormal)
		{
			if(yCoord == arch->yCoordToAimFor)
			{
				crawlAlongSide = false;
				isMovingHorizontally = !isMovingHorizontally;

				arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
				createUnrouteableZoneVert(arch, startingY, yCoord, xCoord, layer);

				startingX = xCoord;
				startingY = yCoord;

				arch->xCoordToAimFor = xCoord;
				arch->yCoordToAimFor = yCoord;
			}
		}

		// Increment positioning
		if(isMovingHorizontally)
		{
			if(crawlAlongSide)
			{
				if(keepXNormal)
				{
					if(desiredX > xCoord)
						xCoord ++;
					else
						xCoord--;
				}
				else if(arch->wireGrid[layer][xCoord + 1][yCoord] || arch->wireGrid[layer][xCoord + 2][yCoord])
					xCoord--;
				else
					xCoord++;
			}
			else if(!notStartNewWire && ((desiredX == xCoord && isRouting) || shouldStartNewWire))
			{
				if(yCoord - desiredY <= -5 || yCoord - desiredY >= 5)
				{
					// If trying to hit one coord (x or y) and hits the other while going around something, make a new wire to finish off the route
					arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
					routeWire(arch, mcPinQueue, layer, pinNo, xCoord, yCoord, desiredX, desiredY, movHoriz, false, false, side, true);
					isRouting = false;
				}
			}
			else if(desiredX > xCoord)
				xCoord ++;
			else if(desiredX == xCoord)
				isMovingHorizontally = !isMovingHorizontally;
			else
				xCoord--;
		}
		else
		{
			if(crawlAlongSide)
			{
				if(keepYNormal)
				{
					if(desiredY > yCoord)
						yCoord ++;
					else
						yCoord--;
				}
				else if(arch->wireGrid[layer][xCoord][yCoord + 1] || arch->wireGrid[layer][xCoord][yCoord + 2])
					yCoord--;
				else
					yCoord++;
			}
			else if(!notStartNewWire && ((desiredY == yCoord && isRouting) || shouldStartNewWire))
			{
				if(xCoord - desiredX >= 5 || xCoord - desiredX <= -5)
				{
					arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));
					routeWire(arch, mcPinQueue, layer, pinNo, xCoord, yCoord, desiredX, desiredY, movHoriz, false, false, side, true);
					isRouting = false;
				}
			}
			else if(desiredY > yCoord)
				yCoord ++;
			else if(desiredY == yCoord)
				isMovingHorizontally = !isMovingHorizontally;
			else
				yCoord--;
		}

		// Save move
		threeBackX = twoBackX;
		threeBackY = twoBackY;
		twoBackX = oneBackX;
		twoBackY = oneBackY;
		oneBackX = xCoord;
		oneBackY = yCoord;
		isFirst = false;

		// Increment variable used to prevent infinite loop in case of error
		moveNum++;

		if(moveNum == 5000)
		{
			isRouting = false;

			arch->areaRouteWires->push_back(new WireSegment(-1 * pinNo, layer, startingX, startingY, xCoord, yCoord));

			startingX = xCoord;
			startingY = yCoord;
		}
	}

	if(needsVia)
		via->createBounds(arch);
}

void PCBLayout::performExcessAreaRoute(DmfbArch *arch, vector<WireSegment* > *arrayPins, int firstArrayPin, int lastArrayPin, ShiftRegister *sr)
{
	// Variables for routing in reverse order
	stack<PinDestLoc *> backwardsPinStack = stack<PinDestLoc *>();

	// Switch order if not to MC
	if(sr != NULL)
	{
		for(int i = firstArrayPin; i <= lastArrayPin; i++)
		{
			if(sr != NULL)
			{
				backwardsPinStack.push(sr->pinOutQueue->front());
				sr->pinOutQueue->pop();
			}
			else
			{
				backwardsPinStack.push(arch->microcontroller->getPinBack());
			}
		}
	}

	// Perform the route
	int offset = 0;
	bool isFirst = true;
	// Calculate distance before routing excess to SR
	int oCap = arch->getWireRouter()->getNumHorizTracks();
	int tileGridSize = ((oCap+2)*2 - 1);
	ShiftRegister dummySR = ShiftRegister(arch, arch->pcb->numShiftRegs, 0, true, 0);

	int buffer = 0;
	if(sr == NULL)
		buffer = ((dummySR.pinHeight * 2) / arch->pcb->cellDimPixels) * (tileGridSize - 1);
	else
		buffer = lastArrayPin - firstArrayPin + 1;	// Just enough that offset won't be negative

	int a = 1;
	int prevSide = -1;
	for(int i = lastArrayPin; i >= firstArrayPin; i--)
	{


		bool needsVia = false;
		if(arrayPins->at(i)->getLayerNum() != 0 && arrayPins->at(i)->getLayerNum() != -1)
			needsVia = true;

		PinDestLoc *destLoc = NULL;
		if(sr != NULL)
			destLoc = backwardsPinStack.top();
		else
		{
			destLoc = arch->microcontroller->getPinBack();
		}

		// Increment total manhattan distance
		int distAcrossHoriz = abs(arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX - destLoc->xPos);
		int distAcrossVert = abs(arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY - destLoc->yPos);
		arch->totalManhatDistToWRDest += (distAcrossHoriz + distAcrossVert);

		// Find the direction to move first
		bool isMovingHorizontally = false;
		if(arrayPins->at(i)->side == 0 || arrayPins->at(i)->side == 2)
			isMovingHorizontally = true;

		// Check next dest
		int sfasdf = arrayPins->at(i)->side;
		bool isBlocked = false;
		if(sr == NULL) {
			if(destLoc->side == 0) {
				if(arch->wireGrid[destLoc->layer][destLoc->xPos - buffer][destLoc->yPos])
					isBlocked = true; }
			else if(destLoc->side == 1) {
				if(arch->wireGrid[destLoc->layer][destLoc->xPos][destLoc->yPos + buffer])
					isBlocked = true;}
			else if(destLoc->side == 2) {
				if(arch->wireGrid[destLoc->layer][destLoc->xPos + buffer][destLoc->yPos])
					isBlocked = true;}
			else if(destLoc->side == 3) {
				if(arch->wireGrid[destLoc->layer][destLoc->xPos][destLoc->yPos - buffer])
					isBlocked = true;}
		} else if(arrayPins->at(i)->side == 3) {
			if(arch->wireGrid[destLoc->layer][destLoc->xPos][destLoc->yPos + buffer]) {
				isBlocked = true; }
		} else if(arrayPins->at(i)->side == 0) {
			if(arch->wireGrid[destLoc->layer][destLoc->xPos - buffer][destLoc->yPos]) {
				isBlocked = true; }
		} else if(arrayPins->at(i)->side == 1) {
			if(arch->wireGrid[destLoc->layer][destLoc->xPos][destLoc->yPos - buffer]) {
				isBlocked = true; }
		} else if(arrayPins->at(i)->side == 2) {
			if(arch->wireGrid[destLoc->layer][destLoc->xPos + buffer][destLoc->yPos]) {
				isBlocked = true; } }

		// Figure out whether to ignore normal hit movement
		bool shouldCrawl = false;
		if(arrayPins->at(i)->side == 0) {
			if(arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY < destLoc->yPos)
				shouldCrawl = true; }
		else if(arrayPins->at(i)->side == 2) {
			if(arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY > destLoc->yPos)
				shouldCrawl = true; }
		else if(arrayPins->at(i)->side == 3) {
			if(arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX < destLoc->xPos)
				shouldCrawl = true; }

		// For Routes to MC, make sure that not routing over another wire
		if(isBlocked)
		{
			offset += 1;
		}

		// Flip the direction of movement on excess to avoid blocking off wiring when large number of SR's exist
		if(arch->pcb->numSRRight > 0)
			isMovingHorizontally = !isMovingHorizontally;

		// Route the pin
		int destX = 0;
		int destY = 0;
		if(sr == NULL)
		{
			if(destLoc->side == 0)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos - buffer, destLoc->yPos, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos - buffer, destLoc->yPos, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
				destX = destLoc->xPos - buffer;
				destY = destLoc->yPos;
			}
			else if(destLoc->side == 1)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos + buffer, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos, destLoc->yPos + buffer, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
				destX = destLoc->xPos;
				destY = destLoc->yPos + buffer;
			}
			else if(destLoc->side == 2)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos + buffer, destLoc->yPos, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos + buffer, destLoc->yPos, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
				destX = destLoc->xPos + buffer;
				destY = destLoc->yPos;
			}
			else if(destLoc->side == 3)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos - buffer, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos, destLoc->yPos - buffer, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
				destX = destLoc->xPos;
				destY = destLoc->yPos - buffer;
			}
		}
		else if(arrayPins->at(i)->side == 0)
		{
			if(isFirst)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX + buffer, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, !isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX + buffer, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos - buffer, destLoc->yPos, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos - buffer, destLoc->yPos, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);

				isFirst = false;
			}
			else
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos - buffer + offset, destLoc->yPos, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos - buffer + offset, destLoc->yPos, destLoc->xPos, destLoc->yPos, isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
			}
		}
		else if(arrayPins->at(i)->side == 1)
		{
			if(isFirst)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY + buffer, !isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY + buffer, destLoc->xPos, destLoc->yPos - buffer, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos, destLoc->yPos - buffer, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);

				isFirst = false;
			}
			else
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos - buffer + offset, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos, destLoc->yPos - buffer + offset, destLoc->xPos, destLoc->yPos, isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
			}
		}
		else if(arrayPins->at(i)->side == 2)
		{
			if(isFirst)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX - buffer, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, !isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX - buffer, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos + buffer, destLoc->yPos, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos + buffer, destLoc->yPos, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);

				isFirst = false;
			}
			else
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos + buffer - offset, destLoc->yPos, isMovingHorizontally, true, false, arrayPins->at(i)->side, shouldCrawl);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos + buffer - offset, destLoc->yPos, destLoc->xPos, destLoc->yPos, isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
			}
		}
		else if(arrayPins->at(i)->side == 3)
		{
			if(isFirst)
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY - buffer, !isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY - buffer, destLoc->xPos, destLoc->yPos + buffer, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos, destLoc->yPos + buffer, destLoc->xPos, destLoc->yPos, !isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);

				isFirst = false;
			}
			else
			{
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), arrayPins->at(i)->getSourceWireCellX(0) + arch->wrOffsetX, arrayPins->at(i)->getSourceWireCellY(0) + arch->wrOffsetY, destLoc->xPos, destLoc->yPos + buffer - offset, isMovingHorizontally, true, false, arrayPins->at(i)->side, false);
				routeWire(arch, NULL, arrayPins->at(i)->getLayerNum(), arrayPins->at(i)->getPinNum(), destLoc->xPos, destLoc->yPos + buffer - offset, destLoc->xPos, destLoc->yPos, isMovingHorizontally, true, needsVia, arrayPins->at(i)->side, false);
			}
		}

		//performAreaRoute(arch, arrayPins, i, offset, srPin);
		backwardsPinStack.pop();

		if(sr != NULL && destLoc->isOuterSR)
		{
			PinDestLoc* realLoc = sr->realOuterPinLocs->front();
			sr->realOuterPinLocs->pop();

			routeWire(arch, NULL, destLoc->layer, destLoc->pinNo, destLoc->xPos, destLoc->yPos, realLoc->xPos, realLoc->yPos, !isMovingHorizontally, false, false, realLoc->side, false);
		}

		// This gets the wires to turn back inwards on top instead of just going straight
		if(sr == NULL && destLoc->side != prevSide)
		{
			arch->xCoordToAimFor = destX;
			arch->yCoordToAimFor = destY;
			prevSide = destLoc->side;
		}
	}
}

// Calculates the y coordinate that an array pin must go through to go around a shift register and to the Microcontroller pin
int PCBLayout::calculateRelevantCoordUnderSR(DmfbArch *arch, WireSegment *arrayPin, ShiftRegister *shiftReg, int numBefore)
{
	int yCoord = 0;

	if(arch->pcb->numSRRight == 1)
	{
		// Calculate y coordinate of initial route location for array -> microcontroller pin when must go over or below the sole sr
		int oCap = arch->getWireRouter()->getNumHorizTracks();
		int tileGridSize = ((oCap+2)*2 - 1);
		ShiftRegister dummySR = ShiftRegister(arch, arch->pcb->numShiftRegs, 0, true, 0);
		int numBeforeRouteToSR = (arch->pcb->rightPins->size() - dummySR.pinOutQueue->size()) / 2;
		int numToRouteToSR = dummySR.pinOutQueue->size();
		if(numBefore < numBeforeRouteToSR)
			yCoord = ((shiftReg->yPos) / shiftReg->cellDimPixels) * (tileGridSize - 1) - (numBeforeRouteToSR - numBefore) * 2 - 3;
		else
			yCoord = ((shiftReg->yPos + shiftReg->fullBodyHeightMM * shiftReg->conversionFactor) / shiftReg->cellDimPixels) * (tileGridSize - 1) + (numBefore - numBeforeRouteToSR) * 2 + 3;
	}
	else if(shiftReg != NULL)
	{
		// Calculate y coordinate of initial route location for array -> microcontroller pin
		int oCap = arch->getWireRouter()->getNumHorizTracks();
		int tileGridSize = ((oCap+2)*2 - 1);

		int furthestXCoord = 0;

		// Calculate initial coordinate of wire under Shift Register
		if(arrayPin->side == 1 || arrayPin->side == 3)
		{
			yCoord = ((shiftReg->yPos + shiftReg->fullBodyHeightMM * shiftReg->conversionFactor) / shiftReg->cellDimPixels) * (tileGridSize - 1) + numBefore * 2 + 3;
		}
	}
	else
	{
		yCoord = arrayPin->getSourceWireCellY(0) + arch->wrOffsetY - 1;
	}

	return yCoord;
}

