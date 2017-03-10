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

#ifndef HEADERS_MODELS_PCB_LAYOUT_H_
#define HEADERS_MODELS_PCB_LAYOUT_H_

using namespace std;

#include "../Models/dmfb_arch.h"
#include "../Resources/enums.h"
#include "../Models/dag.h"
#include "../Models/shift_register.h"
#include "../Models/unroutable_locations.h"
#include <map>
#include <stack>
#include <string>


class UnroutableLocations;

class PCBLayout
{
	private:
		static void routeWire(DmfbArch *arch, queue<PinDestLoc *> *mcPinQueue, int layer, int pinNo, int firstX, int firstY, int destX, int destY, bool movHoriz, bool isExcess, bool needsVia, int side, bool lockMovementOnHit);
		static void routeWireGroup(DmfbArch *arch, vector<WireSegment* > *sidePins, int initX, int finalX, vector<ShiftRegister* > *sideShiftRegs, int srNo, int numExcessOnMCSide);
		static void routeGroundPin(DmfbArch *arch, PinType pinType);
		static void performChainRouting(DmfbArch *arch, SRPinNode *node);

	protected:
		DmfbArch *arch;
		enum ComponentSectionTypes
		{
			ARRAY,
			SHIFT_REGISTER,
			MICROCONTROLLER
		};

	public:
		// Area Routing Methods
		static void calculatePCBLayout(DmfbArch *arch);
		static void performAreaRoute(DmfbArch *arch, vector<WireSegment* > *arrayPins, int pinNo, int offset, PinDestLoc *srPin);
		static void performExcessAreaRoute(DmfbArch *arch, vector<WireSegment* > *arrayPins, int firstArrayPin, int lastArrayPin, ShiftRegister *sr);
		static void performFullSideAreaRoute(DmfbArch *arch, vector<WireSegment* > *sidePins, vector<ShiftRegister* > *sideShiftRegs, bool isMCSide, int numExcessOnMCSide);
		static void performAreaRouteToMC(DmfbArch *arch, ShiftRegister *shiftReg, WireSegment *arrayPin, int offsetY, int offsetX);
		static int calculateRelevantCoordUnderSR(DmfbArch *arch, WireSegment *arrayPin, ShiftRegister *shiftReg, int numBefore);
		static void createUnrouteableZoneVert(DmfbArch *arch, int topY, int botY, int xCoord, int layerNum);
		static void createUnrouteableZoneHoriz(DmfbArch *arch, int leftX, int rightX, int yCoord, int layerNum);
		static void createVia(DmfbArch *arch, int xGridCoord, int yGridCoord, int beginLayer, int endLayer);
		static void routeIndividualWire(DmfbArch *arch, PinDestLoc *destLoc, WireSegment *arrayBegin, ShiftRegister *currentSR, int sideSize);

	friend class Via;
	friend class DmfbArch;
};
#endif /* HEADERS_MODELS_PCB_LAYOUT_H_ */
