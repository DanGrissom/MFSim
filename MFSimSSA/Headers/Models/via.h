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
 * Name: Via																	*
 *																				*
 * Details: This class represents a Via. A Via is a hole in the PCB allowing	*
 * wires to move between layers.												*
 *-----------------------------------------------------------------------------*/

#ifndef HEADERS_MODELS_VIA_H_
#define HEADERS_MODELS_VIA_H_

#include "dag.h"
#include "../synthesis.h"
#include "dmfb_arch.h"

class Via
{
	private:
	double conversionFactor;
	double cellDimMM;
	int electrodePitch;
	int cellDimPixels = 17;

	protected:

		// Coordinate for the middle of the via
		int x;
		int y;
		int leftX;
		int rightX;
		int topY;
		int botY;
		int beginLayer;
		int endLayer;

	public:
		// Constructors
		Via(DmfbArch *arch, int xGridCoord, int yGridCoord, int beginLayerNum, int endLayerNum);	// Grid coordinate is passed in, and is then translated to a pixel coordinate for ease of drawing
		virtual ~Via();

		// Getters/Setters
		int getElectrodeMicrons();
		void createBounds(DmfbArch *arch);

		// Methods

		friend class FileOut;
		friend class Synthesis;
		friend class PCBLayout;
		friend class AreaRouter;
		friend class PCBLayoutTool;
};

#endif /* HEADERS_MODELS_VIA_H_ */
