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
 * Name: DmfbArch (Dmfb Architecture)											*
 *																				*
 * Details: Contains the basic information needed to represent a DMFB			*
 * architecture such as dimensions, heating/detecting/io locations, etc.		*
 *-----------------------------------------------------------------------------*/
#ifndef _DMFB_ARCH_H
#define _DMFB_ARCH_H

#include "../PinMapper/pin_mapper.h"
#include "../WireRouter/wire_router.h"
#include "../WireRouter/wire_segment.h"
#include "fixed_module.h"
#include "shift_register.h"
#include "microcontroller.h"
#include "unroutable_locations.h"
#include "pcb.h"
#include "../Util/util.h"
#include "io_port.h"
#include "via.h"
#include <map>
//#include <vector>

class PinMapper;
class WireRouter;
class PCB;

class DmfbArch
{
	protected:
		// Variables
		string name;
		int numCellsX;
		int numCellsY;
		double freqInHz;
		double secPerTS;
		vector<FixedModule *> *externalResources;
		vector<IoPort *> *ioPorts;
		PinMapper *pinMapper;
		WireRouter *wireRouter;

		// Variables to help routing IC's in PCB
		int wrOffsetX;
		int wrOffsetY;
		int numWRCellsHoriz;	// The number of wire routing cells contained vertically and horizontally on the PCB
		int numWRCellsVert;
		int totalManhatDistToWRDest;	// The total Manhattan distance from start of wires to ends (optimal wire routing)
		int totalWireLengthGrid;
		int srType;
		int mcType;
		int mcSide;
		int maxLayer;
		int xCoordToAimFor;
		int yCoordToAimFor;
		bool hasInitToAimFor;
		double electrodePitchMicrons;
		bool ***wireGrid;	// The grid that contains the status of PCB locations (whether a wire can be placed)
		vector<ShiftRegister *> *shiftRegisters;
		vector<WireSegment *> *areaRouteWires;
		vector<PinDestLoc *> *nextStepPositions;	// Helper for wire routing
		Microcontroller *microcontroller;
		vector<Via *>  *vias;
		PCB *pcb;

	public:
		// Constructors
		DmfbArch();
		virtual ~DmfbArch();

		// Getters/Setters
		int getNumCellsX() { return numCellsX; }
		int getNumCellsY() { return numCellsY; }
		double getFreqInHz() { return freqInHz; }
		double getSecPerTS() { return secPerTS; }
		vector<FixedModule *> *getExternalResources() { return externalResources; }
		vector<IoPort *> *getIoPorts() { return ioPorts; }
		IoPort *getIoPort(string fluidName);
		string getName() { return name; }
		void setPinMapper (PinMapper *pm) { pinMapper = pm; }
		void setWireRouter (WireRouter *wr) { wireRouter = wr; }
		PinMapper *getPinMapper () { return pinMapper; }
		WireRouter *getWireRouter() { return wireRouter; }
		void setSecPerTS(double secondsPerTS) { secPerTS = secondsPerTS; }
		bool hasHeaters();
		bool hasDetectors();

		// Methods

		// Friend Classes
		friend class FileOut;
		friend class FileIn;
		friend class Test;
		friend class Synthesis;
		friend class PCBLayout;
		friend class ShiftRegister;
		friend class Microcontroller;
		friend class PCB;
		friend class Via;
		friend class PCBLayoutTool;
		friend class AreaRouter;
};
#endif /* DMFB_ARCH_H_ */
