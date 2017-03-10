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
 * Name: Microcontroller															*
 *																				*
 * Details: This class represents the chip's microcontroller. 					*
 *-----------------------------------------------------------------------------*/

#ifndef HEADERS_MODELS_MICROCONTROLLER_H_
#define HEADERS_MODELS_MICROCONTROLLER_H_

#include <string>
#include <map>
#include <queue>
#include <stack>

class Component;

using namespace std;

class Microcontroller
{
private:
	DmfbArch *arch;
	int type;
	int buildType;
	int bodyWidth;
	int bodyHeight;
	int numSides;
	int pinHeight;
	int pinWidth;
	int pinBuffer;
	int edgeRadius;
	int numPinsPerSide;
	int arrayHeight;
	int arrayWidth;
	int pinBlank;
	int tileGridSize;
	int numPinsBeforeOffset;
	vector<PinType> *pinTypes;	// ALL PIN TYPE DEFINITIONS MUST BE IMPLEMENTED WITHIN THE CONSTRUCTOR

	// Electrode pitch of cells in DMFB (milimeters) (every other value is divided by this number to scale image)
	int cellDimPixels = 17;
	int electrodePitch;
	double cellDimMM;
	double conversionFactor;

	void updatePinLocations();
	int calculateMCPinY(int index);
	int calculateMCPinX(int index);
protected:
	double fullBodyWidthMM;
	double fullBodyHeightMM;
	int pcbHeight;
	int sectionWidth;
	int sectionX;
	int xPos;
	int yPos;
	int numBackside;

	vector<PinDestLoc *> *pins;
	queue<PinDestLoc *> *pinOutQueue;
	vector<int> *cornerXCoords;
	vector<int> *cornerYCoords;

public:
	// Constructors
	Microcontroller(DmfbArch *arch, int numShiftRegs);
	virtual ~Microcontroller();
	int getElectrodeMicrons();
	void setXPos(int xPosition);
	void setYPos(int yPosition);
	int getXGridBySide(int side);
	int getYGridBySide(int side);
	PinDestLoc * getPinBack();
	PinDestLoc * getPinMinSide(int side);
	int getOutputSize();


	friend class FileOut;
	friend class PCB;
	friend class ShiftRegister;
	friend class PCB;
	friend class Synthesis;
	friend class PCBLayout;
	friend class AreaRouter;
	friend class PCBLayoutTool;
};

#endif /* HEADERS_MODELS_MICROCONTROLLER_H_ */
