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
/*--------------------------------Class Details---------------------------------*
 * Name: Shift Register															*
 *																				*
 * Details: This class represents a Shift Register. A Shift Register outputs	*
 * signals to the array electrodes.												*
 *-----------------------------------------------------------------------------*/

#ifndef HEADERS_MODELS_SHIFT_REGISTER_H_
#define HEADERS_MODELS_SHIFT_REGISTER_H_

using namespace std;

#include "../Resources/enums.h"
#include "../Resources/structs.h"
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <algorithm>


class Component;
struct PinDestLoc;

class ShiftRegister
{
private:
	DmfbArch *arch;
	int type;
	int bodyWidth;
	int bodyHeight;
	int pinHeight;
	int pinWidth;
	int pinBuffer;
	int edgeRadius;
	int numPinsPerSide;
	int arrayHeight;
	int arrayWidth;
	int tileGridSize;
	int pinBlank;

	vector<PinType> *pinTypes;

	PinType SOIC_PIN_TYPES[16] = {IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT,
		IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT,
		IO_GROUND, IO_SERIAL_OUTPUT, IO_RESET_INPUT, IO_SR_CLOCK_INPUT,
		IO_STORAGE_CLOCK_INPUT, IO_3STATE_INPUT, IO_SERIAL_INPUT, IO_PARALLEL_OUTPUT, IO_VOLTAGE_EMITTER};
	PinType SOP_PIN_TYPES[8] = {IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT,
			IO_GROUND, IO_SERIAL_OUTPUT, IO_SR_CLOCK_INPUT,
			IO_SERIAL_INPUT, IO_VOLTAGE_EMITTER};
	PinType TSSOP_PIN_TYPES[16] = {IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT,
		IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT, IO_PARALLEL_OUTPUT,
		IO_GROUND, IO_SERIAL_OUTPUT, IO_RESET_INPUT, IO_SR_CLOCK_INPUT,
		IO_STORAGE_CLOCK_INPUT, IO_3STATE_INPUT, IO_SERIAL_INPUT, IO_PARALLEL_OUTPUT, IO_VOLTAGE_EMITTER};


	// Electrode pitch of cells in DMFB (milimeters) (every other value is divided by this number to scale image)
	int electrodePitch;
	double cellDimMM;
	double conversionFactor;
	vector<int> *xPositions = new vector<int>;	// Helpers for PCB size calculations
	vector<int> *yPositions = new vector<int>;

	void updatePinLocations();
	void findInitSRPositioning();

protected:
	int side;
	int cellDimPixels = 17;
	int numShiftRegs;
	double fullBodyWidthMM;
	double fullBodyHeightMM;
	double rotationFullBodyWidthMM;	// Always is the length from left to right
	double rotationFullBodyHeightMM;	// Always is the length from top to bottom
	int sideCoordX;
	int sideCoordY;
	int numOutOuterSide = 0;
	int xPos;
	int yPos;
	int numSR;
	int initX;
	int initY;
	int width;
	int height;
	int yStep;
	int xStep;
	int numOutputs;
	int rotateDeg;
	int sectionWidth;
	int pcbHeight;
	bool ignoreOuter;

	vector<PinDestLoc *> *pins;	// List of all pins in architecture order (top-right to bot to top-left)
	queue<PinDestLoc *> *pinOutQueue;	// Queue of all possible output pins (in architectural order)
	queue<PinDestLoc *> *realOuterPinLocs;	// The actual location of the outer pins as these must not be used in comparison with others

public:
	// Constructors
	ShiftRegister();
	ShiftRegister(DmfbArch *architecture, int numShiftReg, int rotateDegrees, bool ignoreOuterOutputs, int srSide);
	virtual ~ShiftRegister();
	int getElectrodeMicrons();
	bool canAddShiftReg(int x, int y);
	void setXPos(int xPosition);
	void setYPos(int yPosition);
	PinDestLoc* getUnroutedPinByType(PinType pinType, int preferredSide);
	PinDestLoc *getChainLoc(bool isInput);	// IsInput should be true when referring to serial input and false when referring to serial output
	void resetOutputQueue();
	PinType getFirstShiftRegPin();

	friend class FileOut;
	friend class PCB;
	friend class Microcontroller;
	friend class Synthesis;
	friend class PCBLayout;
	friend class PCBLayoutTool;
	friend class AreaRouter;
};
#endif /* HEADERS_MODELS_SHIFT_REGISTER_H_ */
