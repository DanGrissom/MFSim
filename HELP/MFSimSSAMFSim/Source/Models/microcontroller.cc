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
 * Source: microcontroller.cc															*
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


Microcontroller::Microcontroller(DmfbArch *architecture, int numShiftRegs)
{
	arch = architecture;

	pinTypes = new vector<PinType>();
	pins = new vector<PinDestLoc *>();
	pinOutQueue = new queue<PinDestLoc *>();
	cornerXCoords = new vector<int>();
	cornerYCoords = new vector<int>();

	// Parse the architecture
	int numCellsX = arch->numCellsX;
	int numCellsY = arch->numCellsY;
	int microType = arch->mcType;
	electrodePitch = arch->electrodePitchMicrons;

	// Calculate area of PCB already placed
	sectionX = 0;
	if(arch->pcb != NULL)
	{
		sectionX += arch->pcb->dmfbSideWidth;
	}
	if(arch->shiftRegisters != NULL)
	{
		sectionX += ShiftRegister(arch, numShiftRegs, 0, false, 0).sectionWidth;
	}

	cellDimMM = electrodePitch / 1000.0;

	// Calculate conversion factor between milimeters and pixels
	conversionFactor = cellDimPixels / cellDimMM;

	// Calculate dimensions of array
	arrayHeight = 17 * numCellsY;
	arrayWidth =  17 * numCellsX;

	// Instantiate type-dependent Microcontroller variables
	switch(microType)
	{
	case 0:
		type = 0;
		buildType = ComponentDimensions::TQFP_BUILD_TYPE;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::TQFP_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::TQFP_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::TQFP_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::TQFP_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::TQFP_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::TQFP_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::TQFP_BODY_WIDTH + 2 * ComponentDimensions::TQFP_PIN_HEIGHT;
		fullBodyHeightMM = ComponentDimensions::TQFP_BODY_HEIGHT + 2 * ComponentDimensions::TQFP_PIN_HEIGHT;

		numPinsPerSide = ComponentDimensions::TQFP_NUM_PINS;

		if(buildType == 0)
			numSides = 4;
		else if(buildType == 1)
			numSides = 2;
		else if (buildType == 2)
			numSides = 4;
		else if(buildType == 3)
			numSides = 4;

		// Define the pin types
		for(unsigned i = 0; i < numPinsPerSide * numSides; i++)
		{
			if(i == 20 || i == 51 || i == 63)
				pinTypes->push_back(PinType::MC_VCC);
			else if(i == 21 || i == 52 || i == 62)
				pinTypes->push_back(PinType::MC_GROUND);
			else if(i == 22 || i == 23)
				pinTypes->push_back(PinType::MC_TXAL);
			else if(i == 19)
				pinTypes->push_back(PinType::MC_RESET);
			else if(i == 61)
				pinTypes->push_back(PinType::MC_OTHER);
			else
				pinTypes->push_back(PinType::MC_DIGITAL_IO);
		}
		break;
	case 1:
		type = 1;
		buildType = ComponentDimensions::CBGA_BUILD_TYPE;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::CBGA_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::CBGA_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::CBGA_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::CBGA_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::CBGA_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::CBGA_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::CBGA_BODY_WIDTH;
		fullBodyHeightMM = ComponentDimensions::CBGA_BODY_HEIGHT;

		numPinsPerSide = ComponentDimensions::CBGA_NUM_PINS;

		if(buildType == 0)
			numSides = 4;
		else if(buildType == 1)
			numSides = 2;
		else if (buildType == 2)
			numSides = 4;
		else if(buildType == 3)
			numSides = 4;

		// Define the pin types
		for(unsigned i = 0; i < numPinsPerSide * numSides; i++)
		{
			pinTypes->push_back(PinType::MC_DIGITAL_IO);
		}
		break;
	case 2:
		type = 2;
		buildType = ComponentDimensions::SS_BUILD_TYPE;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::SS_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::SS_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::SS_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::SS_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::SS_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::SS_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::SS_BODY_WIDTH + ComponentDimensions::SS_PIN_HEIGHT * 2;
		fullBodyHeightMM = ComponentDimensions::SS_BODY_HEIGHT;

		numPinsPerSide = ComponentDimensions::SS_NUM_PINS;

		if(buildType == 0)
			numSides = 4;
		else if(buildType == 1)
			numSides = 2;
		else if (buildType == 2)
			numSides = 4;
		else if(buildType == 3)
			numSides = 4;

		// Define the pin types
		for(unsigned i = 0; i < numPinsPerSide * numSides; i++)
		{
			pinTypes->push_back(PinType::MC_DIGITAL_IO);
		}
		break;
	case 3:
		type = 3;
		buildType = ComponentDimensions::ML_BUILD_TYPE;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::ML_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::ML_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::ML_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::ML_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::ML_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::ML_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::ML_BODY_WIDTH;
		fullBodyHeightMM = ComponentDimensions::ML_BODY_HEIGHT;

		numPinsPerSide = ComponentDimensions::ML_NUM_PINS;

		if(buildType == 0)
			numSides = 4;
		else if(buildType == 1)
			numSides = 2;
		else if (buildType == 2)
			numSides = 4;
		else if(buildType == 3)
			numSides = 4;

		// Define the pin types
		for(unsigned i = 0; i < numPinsPerSide * numSides; i++)
		{
			pinTypes->push_back(PinType::MC_DIGITAL_IO);
		}
	}

	pinBlank = (bodyHeight - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);
	int oCap = arch->getWireRouter()->getNumHorizTracks();
	tileGridSize = ((oCap+2)*2 - 1);

	sectionWidth = (int) (((ComponentDimensions::PCB_BUFFER_DMFB * 2) + fullBodyWidthMM) * conversionFactor);

	// Find the error in placement
	int pinSpan = (pinWidth + pinBlank) * numPinsPerSide + 2 * pinBuffer - pinBlank;
	int correctSpan = bodyHeight;
	numPinsBeforeOffset = (correctSpan - pinSpan)/ 2;

	updatePinLocations();
}

Microcontroller::~Microcontroller()
{
	int temp = 0;

//	if(pins != NULL)
//	{
//		while(pins->size() > 0)
//		{
//			PinDestLoc *rm = pins->back();
//			pins->pop_back();
//
//			delete rm;
//		}
//
//		delete pins;
//	}

//	if(pinOutQueue != NULL)
//	{
//		temp = pinOutQueue->size();
//		for(int i = 0; i < temp; i++)
//		{
//			pinOutQueue->pop();
//		}
//		delete pinOutQueue;
//	}

	delete pinTypes;
	delete cornerXCoords;
	delete cornerYCoords;
}

int Microcontroller::getElectrodeMicrons()
{
	return electrodePitch;
}

////////////////////////////////////////////////////////////////////////////////////
// Sets the X Position of the Shift Register and updates the location of every pin
////////////////////////////////////////////////////////////////////////////////////
void Microcontroller::setXPos(int xPosition)
{
	xPos = xPosition;

	updatePinLocations();
}

////////////////////////////////////////////////////////////////////////////////////
// Sets the Y Position of the Shift Register and updates the location of every pin
////////////////////////////////////////////////////////////////////////////////////
void Microcontroller::setYPos(int yPosition)
{
	yPos = yPosition;

	updatePinLocations();
}

////////////////////////////////////////////////////////////////////////////////////
// Gets pin in back of list and takes it out of pin queue... definitely should
// Replace pinOutQueue with something like this
////////////////////////////////////////////////////////////////////////////////////
PinDestLoc * Microcontroller::getPinBack()
{
	queue<PinDestLoc *> *temp = new queue<PinDestLoc *>;

	int startSize = pinOutQueue->size() - 1;

	int i = 0;
	for(i = 0; i < startSize; i++)
	{
		temp->push(pinOutQueue->front());
		pinOutQueue->pop();
	}

	PinDestLoc *tempPin = pinOutQueue->front();
	pinOutQueue->pop();

	pinOutQueue = temp;

	return tempPin;
}

////////////////////////////////////////////////////////////////////////////////////
// Gets pin in front of list but with a minimum side specified
////////////////////////////////////////////////////////////////////////////////////
PinDestLoc * Microcontroller::getPinMinSide(int side)
{
	queue<PinDestLoc *> *temp = new queue<PinDestLoc *>;
	PinDestLoc *tempPin = NULL;

	int startSize = pinOutQueue->size();

	int i = 0;
	bool hasFound = false;
	for(i = 0; i < startSize; i++)
	{
		if(pinOutQueue->front()->side < side || hasFound)
		{
			temp->push(pinOutQueue->front());
			pinOutQueue->pop();
		}
		else
		{
			tempPin = pinOutQueue->front();
			pinOutQueue->pop();
			hasFound = true;
		}
	}

	pinOutQueue = temp;

	return tempPin;
}

////////////////////////////////////////////////////////////////////////////////////
// Sets the wire routing grid locations of each pin in the given Microcontroller
////////////////////////////////////////////////////////////////////////////////////
void Microcontroller::updatePinLocations()
{
	bool useRemainder = true;	// MUST BE SET TO TRUE ONLY ON HIGH QUALITY IMAGES (determined within dmfb drawer java file)

	while(!pinOutQueue->empty())
		pinOutQueue->pop();
	pins->clear();

	// Set initial position of pins
	int direction = 1;
	int pinX = xPos - pinHeight;
	int pinY = yPos + pinBuffer + pinWidth / 2;	// Make this a definite position on the array of nodes

	// Adjust pin coordinates for rotation
	int side = 0;
	int counter = 0;
	for(int i = 0; i < numPinsPerSide * numSides; i++)
	{
		// Used to make up for potential arithmetic differences
		int offsetX = 0;
		int offsetY = 0;
		if(i < numPinsPerSide)
			offsetY = 1;
		else if(i < numPinsPerSide * 2)
			offsetX = 1;
		else if(i < numPinsPerSide * 3)
			offsetY = -1;
		else
			offsetX = -1;

		if(i == numPinsPerSide)
		{
			pinX = (xPos + pinBuffer + pinWidth / 2);	// + pinHeight + pinBuffer + pinWidth / 2
			pinY = (yPos + fullBodyHeightMM * conversionFactor - pinHeight);	// +
		}
		else if(i == numPinsPerSide * 2)
		{
			pinX = (xPos + fullBodyWidthMM * conversionFactor - pinHeight);	// +
			pinY = (yPos + bodyHeight - (pinBuffer + pinWidth / 2 + pinWidth));	// -

			direction *= -1;
		}
		else if(i == numPinsPerSide * 3)
		{
			pinX = (xPos + fullBodyWidthMM * conversionFactor - (pinHeight * 2 + pinBuffer + pinWidth / 2 + pinWidth));	// -
			pinY = (yPos - pinHeight);	// -
		}

		// Create pins if not already made, or just update their locations
		if(pins->size() != numPinsPerSide * numSides)
		{
			// Create the corner coordinates for boundaries (actually easier to just have a box)
			if(i == 0 || counter == numPinsPerSide || counter == numPinsPerSide - 1)
			{
				if(cornerXCoords->size() > 0)
				{
					cornerXCoords->clear();
					cornerYCoords->clear();
				}

				int offsetXCorner = 0;
				int offsetYCorner = -1;
				if(side == 0 && i == numPinsPerSide - 1)
				{
					offsetYCorner = 1;
				}
				else if(side == 0 && i == numPinsPerSide)
				{
					offsetXCorner = -1;
					offsetYCorner = 0;
				}
				else if(side == 1 && i == numPinsPerSide - 1)
				{
					offsetXCorner = 1;
					offsetYCorner = 0;
				}
				else if(side == 1 && i == numPinsPerSide)
				{
					offsetXCorner = 0;
					offsetYCorner = -1;
				}
				else if(side == 2 && i == numPinsPerSide - 1)
				{
					offsetXCorner = 0;
					offsetYCorner = 1;
				}
				else if(side == 2 && i == numPinsPerSide)
				{
					offsetXCorner = 1;
					offsetYCorner = 0;
				}
				else if(side == 3 && i == numPinsPerSide - 1)
				{
					offsetXCorner = -1;
					offsetYCorner = 0;
				}

				cornerXCoords->push_back((int) (pinX / (double) (cellDimPixels) * (tileGridSize - 1)) + offsetX + offsetXCorner);
				cornerYCoords->push_back((int) (pinY / (double) (cellDimPixels) * (tileGridSize - 1)) + offsetY + offsetYCorner);
			}

			if(counter == numPinsPerSide)
			{
				side++;
				counter = 1;
			}
			else
				counter++;

			int pinXGrid = (int) (pinX / (double) (cellDimPixels) * (tileGridSize - 1)) + offsetX;
			int pinYGrid = (int) (pinY / (double) (cellDimPixels) * (tileGridSize - 1)) + offsetY;
			pins->push_back(new PinDestLoc(arch->mcSide, -1, 0, -1, (int) (pinX / (double) (cellDimPixels) * (tileGridSize - 1)) + offsetX, (int) (pinY / (double) (cellDimPixels) * (tileGridSize - 1)) + offsetY, pinTypes->at(i), false));
			pins->at(i)->side = side;

			if(pins->at(i)->type == PinType::MC_DIGITAL_IO)
				pinOutQueue->push(pins->at(i));
		}

		// Increment them
		if(i < numPinsPerSide || (i >= numPinsPerSide * 2 && i < numPinsPerSide * 3))
		{
			pinY += (pinWidth + pinBlank) * direction;

			if(i % numPinsBeforeOffset == 0 && useRemainder)
				pinY += 1 * direction;
		}
		else
		{
			pinX += (pinWidth + pinBlank) * direction;

			if(i % numPinsBeforeOffset == 0 && useRemainder)
				pinX += 1 * direction;
		}
	}

	// If there are SR's on top, partition microcontroller pin order in two to allow chain wire to go to bottom
	if(arch->pcb != NULL && arch->pcb->numSRRight > 0)
	{
		queue<PinDestLoc *> *tempQueueBack = new queue<PinDestLoc *>();
		stack<PinDestLoc *> *tempStackFront = new stack<PinDestLoc *>();

		PinDestLoc *curr = pinOutQueue->front();
		while(curr->side < 2)
		{
			tempQueueBack->push(curr);
			pinOutQueue->pop();
			if(pinOutQueue->size() > 0)
				curr = pinOutQueue->front();
			else
				break;
		}

		numBackside = 0;
		while(curr->side > 1)
		{
			tempStackFront->push(curr);
			numBackside++;
			pinOutQueue->pop();
			if(pinOutQueue->size() > 0)
				curr = pinOutQueue->front();
			else
				break;
		}

		unsigned total = tempQueueBack->size() + tempStackFront->size();
		for(unsigned i = 0; i < total; i++)
		{
			if(tempStackFront->size() > 3)
			{
				pinOutQueue->push(tempStackFront->top());
				tempStackFront->pop();
			}
			else
			{
				pinOutQueue->push(tempQueueBack->front());
				tempQueueBack->pop();
			}
		}
	}
}

int Microcontroller::calculateMCPinY(int index)
{
	int y = yPos + pinBuffer;

	// Calculate the size of the blank spaces between pins
	int pinBlank = (bodyHeight - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);

	y += (pinWidth * (index)) + (pinBlank * index);

	return y;
}

int Microcontroller::calculateMCPinX(int index)
{
	int x = xPos + pinBuffer;

	// Calculate the size of the blank spaces between pins
	int pinBlank = (bodyWidth - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);

	x += (pinWidth * (index)) + (pinBlank * index);

	return x;
}

// Gets the x coordinate of the specified side of MC (useful for wire routing)
int Microcontroller::getXGridBySide(int side)
{
	int x = 0;

	for(int i = 0; i < pins->size(); i++)
	{
		if(pins->at(i)->side == side)
		{
			x = pins->at(i)->xPos;
			i = pins->size();
		}
	}

	return x;
}

// Gets the y coordinate of the specified side of MC (useful for wire routing)
int Microcontroller::getYGridBySide(int side)
{
	int y = 0;

	for(int i = 0; i < pins->size(); i++)
	{
		if(pins->at(i)->side == side)
		{
			y = pins->at(i)->yPos;
			i = pins->size();
		}
	}

	return y;
}
