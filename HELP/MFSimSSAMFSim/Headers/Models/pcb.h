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
 * Name: PCB																	*
 *																				*
 * Details: This class represents the PCB of the chip. 							*
 *-----------------------------------------------------------------------------*/

#ifndef HEADERS_MODELS_PCB_H_
#define HEADERS_MODELS_PCB_H_

#include "dmfb_arch.h"
#include <map>
#include <string>

class Component;

using namespace std;

class PCB
{

protected:
	int numSRTop;
	int numSRBot;
	int numSRLeft;
	int numSRRight;
	vector<WireSegment *> *topPins;
	vector<WireSegment *> *leftPins;
	vector<WireSegment *> *rightPins;
	vector<WireSegment *> *botPins;
	int mcSide;
	double pcbBufferSR;
	double pcbBufferToMC;
	int numOutputsSRSide;
	int difference;
	int numToMCPerSection;
	int numExcess;
	bool ignoreOuterOutputs;

	int xPos;
	int yPos;
	int height;
	int heightCondensed;
	int dmfbSideWidth;
	int mcSideWidth;
	int dmfbX;
	int dmfbY;
	int numShiftRegs;
	int srSideWidth;
	int arrayHeight;
	int arrayWidth;

	// Electrode pitch of cells in DMFB (milimeters) (every other value is divided by this number to scale image)
	int cellDimPixels = 17;
	int electrodePitch;
	double cellDimMM;
	double conversionFactor;

public:
	// Constructors
	PCB(DmfbArch *arch);
	virtual ~PCB();

	// Getters/Setters

	// Methods
	int getElectrodeMicrons();

	friend class FileOut;
	friend class Synthesis;
	friend class PCBLayout;
	friend class Microcontroller;
	friend class ShiftRegister;
	friend class AreaRouter;
	friend class PCBLayoutTool;
};



#endif /* HEADERS_MODELS_PCB_H_ */
