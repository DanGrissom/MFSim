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
 * Name: PCRProbability															*
 * 																				*
 * Details: This class helps with PCR assay probabilities.						*
 *																				*
 *-----------------------------------------------------------------------------*/


#ifndef PCRPROBABILTY_H_
#define PCRPROBABILTY_H_

#include<iostream>
#include<map>
#include<cstdlib>


struct PCRProbabilty {
	double probabiltyofGoodDroplet;
	int minNumberOfcyclesNeeded;
	double pStar;
	double threshhold;
	std::map<int,double> listOfProbabilities;

	PCRProbabilty(double PofG, int MinCycle, double p, double thresh): probabiltyofGoodDroplet(PofG), minNumberOfcyclesNeeded(MinCycle), pStar(p), threshhold(thresh)
	{
		if(PofG >1 || PofG<0){
			std::cerr << "Probability of G should be between 0 and 1"<<std::endl;
			std::exit(-1);
		}
		if(threshhold >1 || threshhold<0){
			std::cerr << "Threshold of G should be between 0 and 1"<<std::endl;
			std::exit(-1);
		}
		for(int i=0; i< minNumberOfcyclesNeeded; ++i)
		{
			listOfProbabilities.insert(std::pair<int,double>(i,0));
		}
	}

	double ProbabiltyofPi(int );
	double ProbabilityofGGivenAk(int );
	double ProbabilityofNOTGGivenAk(int );
};

#endif /* PCRPROBABILTY_H_ */
