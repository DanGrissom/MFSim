/*------------------------------------------------------------------------------*
 *                       (c)2012, All Rights Reserved.     						*
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
/*------------------------------Algorithm Details-------------------------------*
 * Type: Router																	*
 * Name: Lee Router																*
 *																				*
 * Inferred from the following paper:											*
 * Authors: 																	*
 * Title: 																		*
 *------------------------------------------------------------------------------*/

#ifndef LEE_ROUTER_H_
#define LEE_ROUTER_H_


#include "../Testing/elapsed_timer.h"
#include "post_subprob_compact_router.h"

#include <stack>
#include "router.h"

//Struct similar to SoukupCell but instead it holds an int called val which marks how far away the source cell is
//from the LeeCell.

struct LeeCell {
	int x;
	int y;
	bool block;
	int val;
};


struct RotuingPoint;

class LeeRouter : public PostSubproblemCompactionRouter
{
	public:
		// Constructors
		LeeRouter();
		LeeRouter(DmfbArch *dmfbArch);
		virtual ~LeeRouter();

	protected:
		void computeIndivSupProbRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops, map<Droplet *, vector<RoutePoint *> *> *routes);
		void routerSpecificInits();

	private:

		// Members
		vector<vector<LeeCell *> *> *board;

		//Methods
		void mark_cells(LeeCell * source, LeeCell * target);
};

#endif /* LEE_ROUTER_H_ */
