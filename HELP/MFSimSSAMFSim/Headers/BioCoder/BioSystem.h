/*------------------------------------------------------------------------------*
 *                   	(c)2016, All Rights Reserved.                			*
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
/*-------------------------------Class Details----------------------------------*
 * Type: UCR Created BioCoder-Related Class										*
 * Name: BioSystem																*
 *																				*
 * Code Developed and Described in:												*
 * Authors: Dan Grissom, Chris Curtis & Philip Brisk							*
 * Title: Interpreting Assays with Control Flow on Digital Microfluidic			*
 * 			Biochips															*
 * Publication Details: In ACM JETC in Computing Systems, Vol. 10, No. 3,		*
 * 					    Apr 2014, Article No. 24								*
 * 																				*
 * Details: This class describes a BioCoder version of a System; its main		*
 * purpose is to translate the internal CFG-like datastructure within BioCoder	*
 * to a similar structure that can be manipulated by the DMFB simulator.		*
 *																				*
 *-----------------------------------------------------------------------------*/

#ifndef BIOSYSTEM_H_
#define BIOSYSTEM_H_

#include<iostream>
#include<map>
#include<vector>
#include "BioCoder.h"
#include "BioExpression.h"
#include "BioCoderStructs.h"
#include "assayProtocol.h"
#include "BioConditionalGroup.h"
#include "PCRprobabilty.h"

#include "../Models/assay_node.h"
#include "../Resources/enums.h"
#include "../Models/dag.h"
#include "../Models/cfg.h"
#include "../Models/conditional_group.h"
#include "../Models/expression.h"

using namespace std;
class BioSystem
{
private:
	tNameBank *IDs;
	map<BioCoder* ,DAG*> *BioDag;

	vector<DAG*> *dagList;
	vector<BioCoder*> *BioList;
	vector<BioConditionalGroup*> *CGList;
	PCRProbabilty* _PCRProbabiltyModel;
	bool outputGraphs;

public:
	BioSystem();
	~BioSystem();

	BioCoder * addBioCoder();
	BioCoder * addBioCoder(string name);
	BioConditionalGroup* addBioCondGroup();
	void InitializePCRProbablityModel(double PofG, int MinCycle, double p, double thresh);
	CFG * GetDmfbExecutableCFG();
	void printTransfers();
	PCRProbabilty* PCRProbabiltyModel() { return _PCRProbabiltyModel; }
	void setOutputGraphs(bool outputAll) { outputGraphs = outputAll; }

};

#endif /* BIOSYSTEM_H_ */
