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
 * Name: Unroutable Locations													*
 *																				*
 * Details: This class can hold the wire locations in the PCB layout            *
 *  calculation.  However, it is not curently used.								*
 *-----------------------------------------------------------------------------*/

#ifndef HEADERS_MODELS_UNROUTABLE_LOCATIONS_H_
#define HEADERS_MODELS_UNROUTABLE_LOCATIONS_H_

using namespace std;

#include <vector>

class UnroutableLocations
{
private:

	struct HorizBound
		{
			HorizBound(int xCoord1, int xCoord2, int yCoord, int layerNum)
			{
				if(xCoord1 > xCoord2)
				{
					x1 = xCoord2;
					x2 = xCoord1;
				}
				else
				{
					x1 = xCoord1;
					x2 = xCoord2;
				}

				x1 = xCoord1;
				x2 = xCoord2;
				y = yCoord;
				layer = layerNum;
			}

			int x1;
			int x2;
			int y;
			int layer;

			bool check(int xCoord, int yCoord, int layerNum)
			{
				bool isFilled = false;

				if(layer == -1)
					layerNum = layer;

				if(xCoord >= x1 && xCoord <= x2 && yCoord == y && layerNum == layer)
					isFilled = true;

				return isFilled;
			}
		};

		struct VertBound
		{
			VertBound(int yCoord1, int yCoord2, int xCoord, int layerNum)
			{
				if(yCoord1 > yCoord2)
				{
					y1 = yCoord2;
					y2 = yCoord1;
				}
				else
				{
					y1 = yCoord1;
					y2 = yCoord2;
				}

				x = xCoord;
				layer = layerNum;
			}

			int y1;
			int y2;
			int x;
			int layer;

			bool check(int xCoord, int yCoord, int layerNum)
			{
				bool isFilled = false;

				if(layer == -1)
					layerNum = layer;

				if(yCoord >= y1 && yCoord <= y2 && xCoord == x && layerNum == layer)
					isFilled = true;

				return isFilled;
			}
		};

	// Data in the same positions represent parts of the same unit (Coords may be passed through if desiredLocation)
	vector<HorizBound> *hBounds;
	vector<VertBound> *vBounds;

public:
	UnroutableLocations();
	virtual ~UnroutableLocations();
	bool isClaimed(int xCoord, int yCoord, int layerNum);
	void setClaimedHoriz(int x1, int x2, int y, int layerNum);
	void setClaimedVert(int y1, int y2, int x, int layerNum);
};

#endif /* HEADERS_MODELS_UNROUTABLE_LOCATIONS_H_ */
