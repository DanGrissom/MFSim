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
 * Source: BioSystem.cpp														*
 * Original Code Author(s): Chris Curtis										*
 * Original Completion/Release Date: April 23, 2014								*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/BioCoder/BioSystem.h"


BioSystem::BioSystem()
{
	IDs = new tNameBank("");
	BioDag = new map<BioCoder*, DAG*>();
	dagList = new vector<DAG*>();
	BioList = new vector<BioCoder *>();
	CGList = new vector<BioConditionalGroup*>();
	_PCRProbabiltyModel = NULL;
}

BioSystem::~BioSystem()
{
	delete IDs;

	for(int i=0;i<BioList->size();++i)
	{
		delete BioList->at(i);
	}
	delete BioList;

//	for(int i=0;i<dagList->size();++i)
//	{
//		delete dagList->at(i);
//	}
//	delete dagList;

	for(int i=0; i<CGList->size();++i)
	{
		delete CGList->at(i);
	}
	delete CGList;
	delete BioDag;

	if(_PCRProbabiltyModel)
		delete _PCRProbabiltyModel;
}

BioCoder* BioSystem::addBioCoder()
{
	//(IDs->accessName(DG));
	BioCoder * ret =new BioCoder(IDs->accessName(DG).c_str());
	ret->setOutputGraphs(outputGraphs);
	IDs->incNodeName(DG);
	BioList->push_back(ret);
	return ret;
}

BioCoder* BioSystem::addBioCoder(string name)
{
	//(IDs->accessName(DG));
	BioCoder * ret =new BioCoder(name);
	IDs->incNodeName(DG);
	BioList->push_back(ret);
	return ret;
}

BioConditionalGroup * BioSystem::addBioCondGroup()
{
	BioConditionalGroup *ret= new BioConditionalGroup();
	CGList->push_back(ret);
	return ret;
}

void BioSystem :: InitializePCRProbablityModel(double PofG, int MinCycle, double p, double thresh)
{
	_PCRProbabiltyModel = new PCRProbabilty(PofG,MinCycle,p,thresh);
}

////////////////////////////////////////////////////////////////////////////////
// This method converts the BioCoder internals data structures into a CFG that
// can be passed on to a DMFB simulator (specifically, MFSimSSA)
////////////////////////////////////////////////////////////////////////////////
CFG * BioSystem::GetDmfbExecutableCFG()
{
	CFG *cfg = new CFG();

	// Create new (but empty) DAG and associate with BioCoder assay protocol
	for (int i=0 ; i < BioList->size(); ++i)
	{
		//Creates assosiative array(map) from bioCoder->DAG
		DAG *newDAG = cfg->AddNewDAG(BioList->at(i)->getName());
		pair<BioCoder *,DAG *> mapHelp(BioList->at(i),newDAG);
		BioDag->insert(mapHelp);
		dagList->push_back(newDAG);
	}

	// Converts assay protocols to DAGs (created in last loop)
	for (int i = 0; i < BioList->size(); ++i)
	{
		DAG *convertedDAG = dagList->at(i);
		BioList->at(i)->Translator(convertedDAG);
		//dptr->ConvertHeatOpsToHeatAndCool();
	}

	// Convert BioCoder's internal conditional groups and add to CFG
	for (int i = 0; i < CGList->size(); ++i)
	{
		// Copy convertedCG's info into newCG to save in CFG
		ConditionalGroup *newCG = cfg->AddNewCG();
		ConditionalGroup *convertedCG = CGList->at(i)->convt2CG(dagList);
		for (int j = 0; j < convertedCG->getConditions()->size(); j++)
		{
			Condition *c = convertedCG->getConditions()->at(j);
			//Convert vector<BioTransferEdges *> to vector<TransferEdges *>
			newCG->addNewCondition(c->statement, c->branchIfTrue, c->transfers);
		}
	}

	cfg->ConstructAndValidateCFG();
	return cfg;
}

void BioSystem::printTransfers()
{
	cout << "FLOW/DROPLET TRANSFERS:" << endl;
	for (int i = 0; i < CGList->size(); i++)
	{
		BioConditionalGroup *bcg = CGList->at(i);
		for (int j = 0; j < bcg->getConditions()->size(); j++)
		{
			BioCondition *bc = bcg->getConditions()->at(j);

			cout << "Flow: " << "(";
			for (int jj = 0; jj < bcg->getConditions()->size(); jj++)
			{
				if (bcg->getConditions()->at(jj)->dependent != NULL)
					cout << bcg->getConditions()->at(jj)->dependent->getName() << ", ";
			}
			cout << ")" << " --> " << bc->branchIfTrue->getName() << endl;

			for (int k = 0; k < bc->transfers.size(); k++)
			{
				BioTransferEdge *bte = bc->transfers.at(k);
				cout << "\tDropletTrans: " << bte->transOut << " --> " << bte->transIn << endl;
			}
		}
	}
}



