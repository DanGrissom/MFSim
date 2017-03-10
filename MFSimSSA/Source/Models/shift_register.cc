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

ShiftRegister::ShiftRegister()
{

}

ShiftRegister::~ShiftRegister()
{
//	if(pins != NULL)
//	{
//		int temp = pins->size();
//		for(int i = 0; i < temp; i++)
//		{
//			pins->pop_back();
//		}
//
//		delete pins;
//	}
//
//	if(pinOutQueue != NULL)
//	{
//		int temp = pinOutQueue->size();
//		for(int i = 0; i < temp; i++)
//		{
//			pinOutQueue->pop();
//		}
//		delete pinOutQueue;
//	}
//
//	if(realOuterPinLocs != NULL)
//	{
//		int temp = realOuterPinLocs->size();
//		for(int i = 0; i < temp; i++)
//		{
//			realOuterPinLocs->pop();
//		}
//		delete realOuterPinLocs;
//	}

	delete pinTypes;
	delete xPositions;
	delete yPositions;
}

ShiftRegister::ShiftRegister(DmfbArch *architecture, int numShiftReg, int rotateDegrees, bool ignoreOuterOutputs, int srSide)
{
	arch = architecture;

	rotateDeg = rotateDegrees;

	side = srSide;
	pins = new vector<PinDestLoc *>();
	pinOutQueue = new queue<PinDestLoc *>();
	realOuterPinLocs = new queue<PinDestLoc *>();

	numOutputs = 0;
	numSR = numShiftReg;

	ignoreOuter = ignoreOuterOutputs;

	// Parse architecture
	int numCellsX = arch->numCellsX;
	int numCellsY = arch->numCellsY;
	int shiftRegType = arch->srType;
	electrodePitch = arch->electrodePitchMicrons;

	// Calculate area of PCB already placed
	int sectionX = 0;
	if(arch->pcb != NULL)
	{
		sectionX += arch->pcb->dmfbSideWidth;
	}
	if(arch->microcontroller != NULL)
	{
		sectionX += arch->microcontroller->sectionWidth;
	}

	// Note: bodyWidths do not include pin dimensions, however bodyWidthMM does

	cellDimMM = electrodePitch / 1000.0;

	// Calculate conversion factor between milimeters and pixels
	conversionFactor = cellDimPixels / cellDimMM;

	// Calculate dimensions of array
	arrayHeight = 17 * numCellsY;
	arrayWidth =  17 * numCellsX;

	// Instantiate type-dependent Shift Register variables
	switch(shiftRegType)
	{
	case 0:
		type = 0;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::SOIC_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::SOIC_BODY_HEIGHT / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::SOIC_PIN_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::SOIC_PIN_WIDTH / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::SOIC_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::SOIC_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::SOIC_BODY_WIDTH + ComponentDimensions::SOIC_PIN_HEIGHT * 2;
		fullBodyHeightMM = ComponentDimensions::SOIC_BODY_HEIGHT;

		numPinsPerSide = ComponentDimensions::SOIC_NUM_PINS;

		pinTypes = new vector<PinType>();

		for(int i = 0; i < ComponentDimensions::SOIC_PIN_TYPES_SIZE; i++)
		{
			pinTypes->push_back(SOIC_PIN_TYPES[i]);

			if(SOIC_PIN_TYPES[i] == PinType::IO_PARALLEL_OUTPUT)
			{
				//if(!ignoreOuterOutputs || (ignoreOuterOutputs && i < numPinsPerSide))
				numOutputs++;

				if(i >= ComponentDimensions::SOIC_PIN_TYPES_SIZE / 2)
					numOutOuterSide++;
			}
		}
		break;
	case 1:
		type = 1;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::SOP_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::SOP_BODY_HEIGHT / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::SOP_PIN_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::SOP_PIN_WIDTH / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::SOP_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::SOP_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::SOP_BODY_WIDTH + ComponentDimensions::SOP_PIN_HEIGHT * 2;
		fullBodyHeightMM = ComponentDimensions::SOP_BODY_HEIGHT;

		numPinsPerSide = ComponentDimensions::SOP_NUM_PINS;

		pinTypes = new vector<PinType>();

		for(int i = 0; i < ComponentDimensions::SOP_PIN_TYPES_SIZE; i++)
		{
			pinTypes->push_back(SOP_PIN_TYPES[i]);

			if(SOP_PIN_TYPES[i] == PinType::IO_PARALLEL_OUTPUT)
			{
				//if(!ignoreOuterOutputs || (ignoreOuterOutputs && i < numPinsPerSide))
				numOutputs++;

				if(i >= ComponentDimensions::SOIC_PIN_TYPES_SIZE / 2)
					numOutOuterSide++;
			}
		}
		break;
	case 2:
		type = 2;
		bodyWidth = (int) (cellDimPixels * ComponentDimensions::TSSOP_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * ComponentDimensions::TSSOP_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * ComponentDimensions::TSSOP_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * ComponentDimensions::TSSOP_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * ComponentDimensions::TSSOP_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * ComponentDimensions::TSSOP_PIN_BUFFER / cellDimMM);
		fullBodyWidthMM = ComponentDimensions::TSSOP_BODY_WIDTH + ComponentDimensions::TSSOP_PIN_HEIGHT * 2;
		fullBodyHeightMM = ComponentDimensions::TSSOP_BODY_HEIGHT;

		numPinsPerSide = ComponentDimensions::TSSOP_NUM_PINS;

		pinTypes = new vector<PinType>();

		for(int i = 0; i < ComponentDimensions::TSSOP_PIN_TYPES_SIZE; i++)
		{
			pinTypes->push_back(TSSOP_PIN_TYPES[i]);

			if(TSSOP_PIN_TYPES[i] == PinType::IO_PARALLEL_OUTPUT)
			{
				//if(!ignoreOuterOutputs || (ignoreOuterOutputs && i < numPinsPerSide))
				numOutputs++;

				if(i >= ComponentDimensions::SOIC_PIN_TYPES_SIZE / 2)
					numOutOuterSide++;
			}
		}
	}

	pinBlank = (bodyHeight - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);
	int oCap = arch->getWireRouter()->getNumHorizTracks();
	tileGridSize = ((oCap+2)*2 - 1);

	// Calculate number of Shift Registers necessary
	if(arch->pcb != NULL)
		numShiftRegs = arch->pcb->numShiftRegs;
	else
		numShiftRegs = 0;

	// Calculate location of initial Shift Register
	initX = sectionX;
	initY = ComponentDimensions::PCB_BUFFER_PCB * conversionFactor;

	// Calculate width and height of full Shift Register in pixels
	width = (fullBodyWidthMM) * conversionFactor;
	height = (fullBodyHeightMM) * conversionFactor;

	// Change dimensions on rotate
	if(rotateDegrees == 90 || rotateDegrees == 270)
	{
		double temp = fullBodyWidthMM;
		fullBodyWidthMM = fullBodyHeightMM;
		fullBodyHeightMM = temp;
	}

	// Calculate amount to be added to x and y
	int buffer = ComponentDimensions::PCB_BUFFER_PCB * conversionFactor;
	yStep = height + buffer;
	xStep = width + buffer;

	findInitSRPositioning();

	if(arch->pcb != NULL)
	{
		// Add each part individually, but init first
		sectionWidth = 0;

		int mostTop;
		if(arch->pcb->numSRTop > arch->pcb->numSRBot)
			mostTop = arch->pcb->numSRTop;
		else
			mostTop = arch->pcb->numSRBot;
		int containsSRLeft = 1;
		int containsSRRight = 1;
		if(arch->pcb->numSRLeft == 0 && arch->pcb->numSRRight == 0)
		{
			containsSRLeft = 0;
			containsSRRight = 0;
		}


		// Potential widths for if the total sr positioning is larger horizontally than the default buffered positioning
		int potentialArraySpreadSideWidth = ComponentDimensions::PCB_BUFFER_ARRAY_TO_SR * conversionFactor * 2 + arrayWidth + (fullBodyWidthMM * conversionFactor + ComponentDimensions::PCB_BUFFER_SR * conversionFactor) * (containsSRLeft + containsSRRight);
		int potentialSRSpreadSideWidth = mostTop * (fullBodyHeightMM * conversionFactor + arch->pcb->pcbBufferSR * conversionFactor) + containsSRLeft * (fullBodyWidthMM * conversionFactor + ComponentDimensions::PCB_BUFFER_SR * conversionFactor) + containsSRRight * (fullBodyWidthMM * conversionFactor + ComponentDimensions::PCB_BUFFER_SR * conversionFactor);
		if(potentialArraySpreadSideWidth > potentialSRSpreadSideWidth)
			sectionWidth = potentialArraySpreadSideWidth;
		else
			sectionWidth = potentialSRSpreadSideWidth;

		//sectionWidth = (int) ((ComponentDimensions::PCB_BUFFER_DMFB * 2) * conversionFactor) + arrayWidth;

	}
	else
		sectionWidth = 2.5 * conversionFactor;

	// Calculate full PCB height (NOT USED OR ACCURATE)
	pcbHeight = (int) (((ComponentDimensions::PCB_BUFFER_DMFB * 2) * conversionFactor) + arrayWidth + ((ComponentDimensions::PCB_BUFFER_DMFB * 2) * conversionFactor) + arrayHeight);

	updatePinLocations();
}

////////////////////////////////////////////////////////////////////////////////////
// Determines whether a Shift Register can be added vertically given the last shift register position
////////////////////////////////////////////////////////////////////////////////////
bool ShiftRegister::canAddShiftReg(int x, int y)
{
	bool canAdd = false;

	int pcbHeight = (int) ((ComponentDimensions::PCB_BUFFER_DMFB * 2) * conversionFactor) + arrayHeight;
	int buffer = ComponentDimensions::PCB_BUFFER_PCB * conversionFactor;

	if(pcbHeight >= y + bodyHeight * 2 + buffer)
	{
		canAdd = true;
	}

	return canAdd;
}

////////////////////////////////////////////////////////////////////////////////////
// Gets the electrode pitch in microns (available in every IC class)
////////////////////////////////////////////////////////////////////////////////////
int ShiftRegister::getElectrodeMicrons()
{
	return electrodePitch;
}

////////////////////////////////////////////////////////////////////////////////////
// Sets the X Position of the Shift Register and updates the location of every pin
////////////////////////////////////////////////////////////////////////////////////
void ShiftRegister::setXPos(int xPosition)
{
	xPos = xPosition;

	updatePinLocations();
}
////////////////////////////////////////////////////////////////////////////////////
// Sets the Y Position of the Shift Register and updates the location of every pin
////////////////////////////////////////////////////////////////////////////////////
void ShiftRegister::setYPos(int yPosition)
{
	yPos = yPosition;

	updatePinLocations();
}

PinDestLoc *ShiftRegister::getChainLoc(bool isInput)
{
	PinDestLoc *chainPin;

	PinType desiredPinType;
	if(isInput)
		desiredPinType = PinType::IO_SERIAL_INPUT;
	else
		desiredPinType = PinType::IO_SERIAL_OUTPUT;

	for(unsigned i = 0; i < pins->size(); i++)
	{
		if(pins->at(i)->type == desiredPinType)
		{
			chainPin = pins->at(i);
			i = pins->size();
		}
	}

	return chainPin;
}

////////////////////////////////////////////////////////////////////////////////////
// Sets the wire routing grid locations of each pin in the given Shift Register
////////////////////////////////////////////////////////////////////////////////////
void ShiftRegister::updatePinLocations()
{
	// Redo pin location calculations
	int direction = 0;
	int pinX = 0;
	int pinY = 0;

	// Set initial position of pins
	if(rotateDeg == 0)
	{
		direction = 1;
		pinX = xPos;
		pinY = yPos + pinBuffer + pinWidth / 2;	// Make this a definite position on the array of nodes
	}
	else if(rotateDeg == 90)
	{
		direction = -1;
		pinX = xPos + fullBodyWidthMM * conversionFactor - (pinBuffer + pinWidth / 2);
		pinY = yPos;
	}
	else if(rotateDeg == 180)
	{
		direction = -1;
		pinX = xPos + fullBodyWidthMM * conversionFactor;
		pinY = yPos + fullBodyHeightMM * conversionFactor - (pinBuffer + pinWidth / 2);

	}
	else if(rotateDeg == 270)
	{
		direction = 1;
		pinX = xPos + pinBuffer + pinWidth / 2;
		pinY = yPos + fullBodyHeightMM * conversionFactor;
	}

	// Adjust pin coordinates for rotation
	for(int i = 0; i < numPinsPerSide * 2; i++)
	{
		if(i == numPinsPerSide)
		{
			if(rotateDeg == 90)
			{
				pinY += fullBodyHeightMM * conversionFactor;
			}
			else if(rotateDeg == 270)
			{
				pinY -= fullBodyHeightMM * conversionFactor;
			}
			else if(rotateDeg == 0)
			{
				pinX += fullBodyWidthMM * conversionFactor;
			}
			else if(rotateDeg == 180)
			{
				pinX -= fullBodyWidthMM * conversionFactor;
			}

			direction *= -1;
		}

		if(pins->size() != numPinsPerSide * 2)
		{
			pins->push_back(new PinDestLoc(arch->mcSide, i, 0, -1, (int) (pinX / (double) (cellDimPixels) * (tileGridSize - 1)), (int) (pinY / (double) (cellDimPixels) * (tileGridSize - 1)), pinTypes->at(i), false));

			if(i >= numPinsPerSide)
			{
				pins->at(i)->isOuterSR = true;
			}

			if(pins->at(i)->type == PinType::IO_PARALLEL_OUTPUT)
			{
				//				if(!ignoreOuter || (ignoreOuter && !pins->at(i)->isOuterSR))
				pinOutQueue->push(pins->at(i));
			}
		}
		else
		{
			pins->at(i)->xPos = (int) (pinX / (double) (cellDimPixels) * (tileGridSize - 1));
			pins->at(i)->yPos = (int) (pinY / (double) (cellDimPixels) * (tileGridSize - 1));

			if(pins->at(i)->type == PinType::IO_PARALLEL_OUTPUT)
			{
				pinOutQueue->pop();
				pinOutQueue->push(pins->at(i));
			}
		}

		pins->at(i)->side = side;


		// Increment them
		int buffer = ((pinHeight * 2) / cellDimPixels) * (tileGridSize - 1) / 3;	// numPinsPS = first pin on side 1... numPinsPS - 1 = last pin on side 0...
		if(i >= numPinsPerSide && pins->at(i)->type == PinType::IO_PARALLEL_OUTPUT)
		{
			if(rotateDeg ==0)	// This puts them each right on the side of SR so that it is easy to go around
			{
				realOuterPinLocs->push(pins->at(i)->copy());

				if(realOuterPinLocs->size() == 2)	// Makes sure that old ones are erased
					realOuterPinLocs->pop();

				pins->at(i)->yPos = pins->at(0)->yPos - 3;	// HARDCODED... FIX	FAIRY DEBUG JOIN
				pins->at(i)->xPos = pins->at(i)->xPos + buffer;
			}
			else if(rotateDeg == 90)
			{
				realOuterPinLocs->push(pins->at(i)->copy());

				if(realOuterPinLocs->size() == 2)	// Makes sure that old ones are erased
					realOuterPinLocs->pop();

				pins->at(i)->xPos = pins->at(0)->xPos + 3;
				pins->at(i)->yPos = pins->at(i)->yPos + buffer;
			}
			else if(rotateDeg == 180)
			{
				realOuterPinLocs->push(pins->at(i)->copy());

				if(realOuterPinLocs->size() == 2)	// Makes sure that old ones are erased
					realOuterPinLocs->pop();

				pins->at(i)->yPos = pins->at(0)->yPos + 3;
				pins->at(i)->xPos = pins->at(i)->xPos - buffer;
			}
			else
			{
				realOuterPinLocs->push(pins->at(i)->copy());

				if(realOuterPinLocs->size() == 2)	// Makes sure that old ones are erased
					realOuterPinLocs->pop();

				pins->at(i)->xPos = pins->at(0)->xPos - 3;
				pins->at(i)->yPos = pins->at(i)->yPos - buffer;
			}
		}

		if(i != numPinsPerSide - 1)
		{
			if(rotateDeg == 90 || rotateDeg == 270)
			{
				pinX += (pinWidth + pinBlank) * direction;
			}
			else
			{
				pinY += (pinWidth + pinBlank) * direction;
			}
		}
	}

	PinDestLoc *outer;
	queue<PinDestLoc *> *tempQueue = new queue<PinDestLoc *>();

	int piSize = pinOutQueue->size();

	for(unsigned i = 0; i < numOutputs; i++)
	{
		if(i < numOutputs - numOutOuterSide)
		{
			tempQueue->push(pinOutQueue->front());
			pinOutQueue->pop();
		}
		else
		{
			outer = pinOutQueue->front();
			pinOutQueue->pop();
		}
	}
	pinOutQueue->push(outer);
	int size = tempQueue->size();
	for(unsigned i = 0; i < size; i++)
	{
		pinOutQueue->push(tempQueue->front());
		tempQueue->pop();
	}

	piSize = pinOutQueue->size();
	piSize++;
}

PinDestLoc* ShiftRegister::getUnroutedPinByType(PinType pinType, int preferredSide)
{
	PinDestLoc *pin = NULL;

	for(unsigned i = 0; i < pins->size(); i++)
	{
		if(pins->at(i)->type == pinType)
		{
			pin = pins->at(i);
			pins->at(i)->isAreaRouted = true;
		}
	}

	return pin;
}

////////////////////////////////////////////////////////////////////////////////////
// Reset the output queue to the initial values
////////////////////////////////////////////////////////////////////////////////////
void ShiftRegister::resetOutputQueue()
{
	// Get rid of all still in
	for(unsigned i = 0; i < pinOutQueue->size(); i++)
	{
		pinOutQueue->pop();
	}

	// Replace all
	for(unsigned i = 0; i < pins->size(); i++)
	{
		if(pins->at(i)->type == PinType::IO_PARALLEL_OUTPUT )
		{
			//			if(!ignoreOuter || (ignoreOuter && !pins->at(i)->isOuterSR))
			pinOutQueue->push(pins->at(i));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Finds the initial box Shift Register positioning used to calculate the sizes
// of each PCB section and distance between IC's
////////////////////////////////////////////////////////////////////////////////////
void ShiftRegister::findInitSRPositioning()
{
	// Calculate position for each Shift Register (INITIAL ROW-COLUMN POSITIONING)
	int shiftRegX = initX;
	int shiftRegY = initY;
	for(int i = 0; i < numShiftRegs; i++)
	{
		if(i == numSR)
		{
			xPos = shiftRegX;
			yPos = shiftRegY;
		}

		xPositions->push_back(shiftRegX);
		yPositions->push_back(shiftRegY);

		// Calculate Shift Register x and y
		if(canAddShiftReg(shiftRegX, shiftRegY))
		{
			shiftRegY += yStep;
		}
		else
		{
			shiftRegX += xStep;
			shiftRegY = initY;
		}
	}
}
