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
 * Source: list_scheduler.cc													*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/Scheduler/list_scheduler.h"
#include "../../Headers/Util/sort.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
ListScheduler::ListScheduler()
{
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
ListScheduler::~ListScheduler()
{
}

/////////////////////////////////////////////////////////////////
// Tells whether a dispense well containing the specified fluid
// is available at the current timestep
/////////////////////////////////////////////////////////////////
IoResource * ListScheduler::getReadyDispenseWell(string fluidName, unsigned long long schedTS)
{
	for (unsigned i = 0; i < dispRes->size(); i++)
	{
		IoResource *dr = dispRes->at(i);
		if (strcmp(Util::StringToUpper(fluidName).c_str(), Util::StringToUpper(dr->name).c_str()) == 0)
		{
			if (dr->lastEndTS + dr->durationInTS <= schedTS)
				return dr;
		}
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////
// Schedules the DAG (contained by synthesis) according to the list scheduling
// algorithm.
///////////////////////////////////////////////////////////////////////////////////
unsigned long long ListScheduler::schedule(DmfbArch *arch, DAG *dag)
{
	int numModules = getAvailResources(arch);
	int maxLocDrops = ((numModules-1)*getMaxStoragePerModule()) + 1;
	int dropsInLoc = 0;
	int dropsInStorage = 0;
	int numStorageModules = 0;
	resetResourcesForSchedulerReuse(arch);

	if (internalPriorities)
	{
		Priority::setAsCritPathDist(dag, arch);
		//Priority::setAsNumIndPaths(dag);
		//Priority::setAsLongestPathDist(dag);
		//Priority::setAsNumIndPaths(dag);
	}

	commissionDAG(dag);

	unsigned long long schedTS = 0;

	// If we have transfer-ins with immediate transfer-outs, take care of them first b/c we'll need to account for storage;
	// Also take care of transfer-ins with immediate outputs as they won't be scheduled otherwise
	//	unsigned maxOutTS = 0;
	//	for (int i = 0; i < dag->tails.size(); i++)
	//		if (maxOutTS < dag->tails.at(i)->startTimeStep && dag->tails.at(i)->parents.front()->GetType() != TRANSFER_IN && dag->tails.at(i)->parents.front()->GetType() != DISPENSE)
	//			maxOutTS = dag->tails.at(i)->startTimeStep;
	//	unsigned minInTS = maxOutTS;
	//	for (int i = 0; i < dag->heads.size(); i++)
	//		if (minInTS > dag->heads.at(i)->startTimeStep && dag->heads.at(i)->children.front()->GetType() != TRANSFER_OUT && dag->heads.at(i)->children.front()->GetType() != OUTPUT)
	//			minInTS = dag->heads.at(i)->startTimeStep;
	//	if (minInTS < timeStep + 1)
	//		minInTS = timeStep + 1;
	for (int i = 0; i < dag->heads.size(); i++)
	{
		AssayNode *in = dag->heads.at(i);
		if (in->GetType() == TRANSFER_IN)
		{
			in->startTimeStep = in->endTimeStep = schedTS;
			in->status = SCHEDULED;
			if (in->children.front()->GetType() == OUTPUT)
			{
				in->children.front()->startTimeStep = schedTS;
				in->children.front()->endTimeStep = schedTS + 1;
				in->children.front()->status = SCHEDULED;
			}
			else //if (in->children.front()->GetType() == TRANSFER_OUT)
			{
				// Droplets will remain on DMFB for some time (possibly entire time if child is TRANSFER_OUT), so account for
				dropsInStorage++;
				dropsInLoc++;
			}
		}
	}




	// If we have transfer-outs, make sure they're scheduled at very end of
	// DAG so we can insert storage nodes between them and their parent
	//	unsigned maxOutTS = 0;
	//	for (int i = 0; i < dag->tails.size(); i++)
	//		if (maxOutTS < dag->tails.at(i)->startTimeStep && dag->tails.at(i)->parents.front()->GetType() != TRANSFER_IN && dag->tails.at(i)->parents.front()->GetType() != DISPENSE)
	//			maxOutTS = dag->tails.at(i)->startTimeStep;
	//	unsigned minInTS = maxOutTS;
	//	for (int i = 0; i < dag->heads.size(); i++)
	//		if (minInTS > dag->heads.at(i)->startTimeStep && dag->heads.at(i)->children.front()->GetType() != TRANSFER_OUT && dag->heads.at(i)->children.front()->GetType() != OUTPUT)
	//			minInTS = dag->heads.at(i)->startTimeStep;
	//	if (minInTS < timeStep + 1)
	//		minInTS = timeStep + 1;
	//	for (int i = 0; i < dag->tails.size(); i++)
	//	{
	//		AssayNode *out = dag->tails.at(i);
	//		if (out->parents.front()->GetType() == TRANSFER_IN) // This is a pass through droplet, adjust start-stop times
	//			out->parents.front()->startTimeStep = out->parents.front()->endTimeStep = out->startTimeStep = out->endTimeStep	= minInTS;
	//		if (out->GetType() == TRANSFER_OUT && maxOutTS > out->startTimeStep)
	//		{
	//			for (unsigned j = out->startTimeStep; j < maxOutTS; j++)
	//				availResAtTS->find(j)->second->dropsInStorage++;
	//			out->startTimeStep = out->endTimeStep = maxOutTS;
	//		}
	//	}
	//	for (int i = 0; i < dag->heads.size(); i++)
	//	{
	//		AssayNode *in = dag->heads.at(i);
	//		if (in->GetType() == TRANSFER_IN && in->endTimeStep > minInTS &&
	//				in->children.front()->GetType() != TRANSFER_OUT && in->children.front()->GetType() != OUTPUT)
	//		{
	//			for (unsigned j = minInTS; j < in->endTimeStep; j++)
	//				availResAtTS->find(j)->second->dropsInStorage++;
	//			in->startTimeStep = in->endTimeStep = minInTS;
	//		}
	//	}

	// If no nodes to schedule, increment TS for proper accounting at end
	// since following loop will never be executed
	if (!moreNodesToSchedule())
		schedTS++;

	while(moreNodesToSchedule())
	{
		//cout << "DB: Scheduling Time Step " << schedTS << endl << "----------------------------" << endl;
		//cout << "\tlocDrops " << dropsInLoc << " -- stDrops " << dropsInStorage << " -- stChams " << numStorageModules << endl;

		// If any ops just finished, their droplets go back into "storage" until otherwise claimed
		vector<AssayNode*> finishedOps;
		vector<AssayNode*> scheduledOps;
		list<AssayNode*>::iterator	it = unfinishedOps->begin();

		// Determine which ops just finished so we can reclaim resources and account for droplets
		for (; it != unfinishedOps->end(); it++)
		{
			AssayNode *node = *it;

			//cout << "\tUnfinished Operation: " << node->GetName() << endl;

			if (node->GetEndTS() == schedTS)
			{
				finishedOps.push_back(node);
				if (node->boundedResType == BASIC_RES)
					availRes[BASIC_RES]++;
				else if (node->boundedResType == D_RES)
					availRes[D_RES]++;
				else if (node->boundedResType == H_RES)
					availRes[H_RES]++;
				else if (node->boundedResType == DH_RES)
					availRes[DH_RES]++;

				for (unsigned i = 0; i < node->GetChildren().size(); i++)
				{
					if (node->GetChildren().at(i)->GetType() != OUTPUT)
						dropsInStorage++;
				}
			}
		}
		for (unsigned i = 0; i < finishedOps.size(); i++)
			unfinishedOps->remove(finishedOps.at(i));

		// Arrange by priority
		//Sort::sortNodesByPriorityHiFirst(candidateOps); // MLS_DEC
		Sort::sortNodesByPriorityLoFirst(candidateOps); // MLS_INC

		// Now, see if there is an operation we can schedule
		it = candidateOps->begin();
		for (; it != candidateOps->end(); it++)
		{
			AssayNode *n = *it;
			//cout << "Candidate Operation: " << n->GetName() << endl;
			int netStorageDropsGain = 0;
			int netLocDropsGain = 0;

			// Determine what the net gain in droplets would be by scheduling this node at this TS
			bool parentsDone = true;
			for (unsigned p = 0; p < n->GetParents().size(); p++)
			{
				AssayNode *par = n->GetParents().at(p);
				if (par->GetType() == DISPENSE)
				{
					//netStorageDropsGain++;// Pretend like this came from storage for a moment to cancel out later
					if (!getReadyDispenseWell(par->GetPortName(), schedTS))
						parentsDone = false;
				}
				else
				{
					// Droplets being output next are NEVER considered as in the system/storage;
					// Also, if this is a transfer-out node, then it is ALWAYS considered as in
					// storage so we do not decrement here (already considred in storage b/c of
					// it's parent node)
					if (n->GetType() != OUTPUT && n->GetType() != TRANSFER_OUT)
					{
						netStorageDropsGain--;
						netLocDropsGain--;
					}
					if (!(par->GetStatus() == SCHEDULED && par->GetEndTS() <= schedTS))
						parentsDone = false;
				}

			}
			for (unsigned c = 0; c < n->GetChildren().size(); c++)
				if (n->GetChildren().at(c)->GetType() != OUTPUT)
					netLocDropsGain++;

			// If the proposed operation doesn't cause too many droplets/chambers, schedule it
			numStorageModules = ceil(((double)dropsInStorage + (double)netStorageDropsGain)/ (double)getMaxStoragePerModule());
			if (parentsDone && dropsInLoc + netLocDropsGain <= maxLocDrops &&
					(unfinishedOps->size() + 1) + numStorageModules <= numModules)
			{
				// Determine if there is an applicable, available resource
				bool canSchedule = false;
				if ((n->type == SPLIT || n->type == MIX || n->type == DILUTE) && (availRes[BASIC_RES] + availRes[D_RES] + availRes[H_RES] + availRes[DH_RES]) > 0)
				{
					canSchedule = true;
					if (availRes[BASIC_RES] > 0)
					{
						availRes[BASIC_RES]--;
						n->boundedResType = BASIC_RES;
					}
					else if (availRes[H_RES] > 0)
					{
						availRes[H_RES]--;
						n->boundedResType = H_RES;
					}
					else if (availRes[D_RES] > 0)
					{
						availRes[D_RES]--;
						n->boundedResType = D_RES;
					}
					else
					{
						availRes[DH_RES]--;
						n->boundedResType = DH_RES;
					}
				}
				else if (n->type == DETECT && (availRes[D_RES] + availRes[DH_RES]) > 0)
				{
					canSchedule = true;
					if (availRes[D_RES] > 0)
					{
						availRes[D_RES]--;
						n->boundedResType = D_RES;
					}
					else
					{
						availRes[DH_RES]--;
						n->boundedResType = DH_RES;
					}
				}
				else if ((n->type == HEAT || n->type == COOL) && (availRes[H_RES] + availRes[DH_RES]) > 0)
				{
					canSchedule = true;
					if (availRes[H_RES] > 0)
					{
						availRes[H_RES]--;
						n->boundedResType = H_RES;
					}
					else
					{
						availRes[DH_RES]--;
						n->boundedResType = DH_RES;
					}
				}
				else if (n->type == OUTPUT || n->type == TRANSFER_OUT)
					canSchedule = true;

				if (canSchedule)
				{
					dropsInLoc += netLocDropsGain;
					dropsInStorage += netStorageDropsGain;
					n->startTimeStep = schedTS;
					n->endTimeStep = schedTS + ceil((double)n->cycles/(double)arch->getFreqInHz()/arch->getSecPerTS());
					n->status = SCHEDULED;
					if (n->GetType() != OUTPUT)
						unfinishedOps->push_back(n);
					else
						n->endTimeStep = schedTS+1;		// Increment output end time by 1 TS (is this necessary?)
					scheduledOps.push_back(n);

					//cout << "DEBUG: " << n->name << ": [" << n->startTimeStep << ", " << n->endTimeStep << "]" << endl;// dtg debug

					// Update any dispense parents & insert any necessary storage nodes into the DAG
					vector<AssayNode*> pInsert;
					vector<AssayNode*> sInsert;

					for (unsigned p = 0; p < n->GetParents().size(); p++)
					{
						AssayNode *parent = n->GetParents().at(p);
						if (parent->GetType() == DISPENSE)
						{
							IoResource *dr = getReadyDispenseWell(parent->GetPortName(), schedTS);
							parent->startTimeStep = schedTS - dr->durationInTS;
							parent->endTimeStep = parent->startTimeStep + dr->durationInTS;
							parent->status = SCHEDULED;
							dr->lastEndTS = parent->endTimeStep;
						}

						if (parent->endTimeStep < schedTS)
						{
							AssayNode *store = dag->AddStorageNode();
							store->status = SCHEDULED;
							store->startTimeStep = parent->endTimeStep;
							store->endTimeStep = schedTS;
							pInsert.push_back(parent); // Insert later so we don't mess up for loop
							sInsert.push_back(store);
							//dag->InsertNode(parent, n, store);
						}
					}

					// Now do actual insert of any necessary storage nodes
					for (unsigned s = 0; s < pInsert.size(); s++)
						dag->InsertNode(pInsert.at(s), n, sInsert.at(s));

					// If each child's parents are all scheduled, we can add it to the candidate list
					for (unsigned c = 0; c < n->GetChildren().size(); c++)
					{
						bool canAddChild = true;
						AssayNode *child = n->GetChildren().at(c);
						for (unsigned p = 0; p < child->GetParents().size(); p++)
						{
							if (child->GetParents().at(p)->GetType() != DISPENSE && child->GetParents().at(p)->GetStatus() != SCHEDULED)
								canAddChild = false;
						}
						if (canAddChild)
							candidateOps->push_back(child);
					}
				}
			}
		}
		// Remove scheduled ops from the candidate list
		for (unsigned i = 0; i < scheduledOps.size(); i++)
			candidateOps->remove(scheduledOps.at(i));

		// Create nodes for storage
		int ar[RES_TYPE_MAX+1];
		for (int i = 0; i <= RES_TYPE_MAX; i++)
			ar[i] = availRes[i];

		int dis = dropsInStorage;
		while (dis > 0)
		{
			//AssayNode *node = dag->AddStorageHolderNode();
			//node->startTimeStep = schedTS;
			//node->endTimeStep = schedTS + 1;
			//node->cycles = (node->GetEndTS()-node->GetStartTS())* (arch->getFreqInHz() * arch->getSecPerTS());
			if (dis >= getMaxStoragePerModule())
				dis -= getMaxStoragePerModule();
			else
				dis -= dis;

			// Reserve a resource type, but don't need to assign here (will do in placer/binder)
			if (ar[BASIC_RES] > 0)
			{
				ar[BASIC_RES]--;
				//node->boundedResType = BASIC_RES;
			}
			else if (ar[H_RES] > 0)
			{
				ar[H_RES]--;
				//node->boundedResType = H_RES;
			}
			else if (ar[D_RES] > 0)
			{
				ar[D_RES]--;
				//node->boundedResType = D_RES;
			}
			else
			{
				ar[DH_RES]--;
				//node->boundedResType = DH_RES;
			}
		}


		schedTS++;

		// If we failed to schedule any operations, and there are more left,
		// then LS cannot schedule this assay given the current priorities
		int freeModules = 0;
		for (int i = 0; i <= RES_TYPE_MAX; i++)
			freeModules += ar[i];
		if (unfinishedOps->empty() && moreNodesToSchedule() && freeModules == 0)
		{
			claim(!internalPriorities, "List Scheduler failed to produce a valid schedule.");
			return 1000000;
		}

		//if (schedTS > 1000)
		//	return 10000;

		//cout << "storage: " << dropsInStorage << endl; // dtg debug
		//cout << "loc: " << dropsInLoc << endl; // dtg debug
	}

	// If we have transfer-outs w/ non dispense/transfer-in parents, make sure they're scheduled at
	// very end of the DAG so we can insert storage nodes between them and their parent
	for (int i = 0; i < dag->tails.size(); i++)
	{
		AssayNode *tOut = dag->tails.at(i);
		if (tOut->GetType() == TRANSFER_OUT)
		{
			AssayNode *tIn = tOut->GetParents().front();
			if (tIn->GetType() == TRANSFER_IN)
			{
				tOut->startTimeStep = tOut->endTimeStep = schedTS-1;
				tOut->status = SCHEDULED;

				if (tIn->endTimeStep < tOut->startTimeStep)
				{
					AssayNode *store = dag->AddStorageNode();
					store->status = SCHEDULED;
					store->startTimeStep = tIn->endTimeStep;
					store->endTimeStep = tOut->startTimeStep;
					dag->InsertNode(tIn, tOut, store);
				}
			}
		}
	}




	if (internalPriorities)
		cout << "LS Time: " << schedTS-1 << endl;

	return schedTS-1;
}


