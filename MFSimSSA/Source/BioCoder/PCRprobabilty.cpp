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
 * Source: PCRProbability.cpp													*
 * Original Code Author(s): Chris Curtis										*
 * Original Completion/Release Date: October 30, 2014							*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/

#include "../../Headers/BioCoder/PCRprobabilty.h"
#include <cmath>
	double PCRProbabilty:: ProbabiltyofPi(int i)
	{
		//std:: cout<< "Entering ProbabiltyofPi"<< i <<std::endl;

		if(listOfProbabilities.find(i) != listOfProbabilities.end())
		{
		//	std:: cout<< " ProbabiltyofPi"<< i <<" in the map, so returning"<<std::endl;
			return listOfProbabilities.find(i)->second;
		}

		double log2P = log2(pStar);

		if( minNumberOfcyclesNeeded - log2P < i){
			//std:: cout<< "Exiting ProbabiltyofPi"<< i <<std::endl;
			listOfProbabilities.insert(std::pair<int,double>(i,1));
			return 1;
		}

	//	std:: cout<< "Exiting ProbabiltyofPi"<< i <<std::endl;

		double ret = pStar * pow(2,i-minNumberOfcyclesNeeded);
		listOfProbabilities.insert(std::pair<int,double>(i,ret));
		return ret;

	}
	double PCRProbabilty :: ProbabilityofGGivenAk(int k)
	{
		//std:: cout<< "Entering ProbabilityofGGivenAk "<< std::endl;
		double numResult=1;
		double denomResult=1;
		for(int i=1; i<k; ++i)
		{
			//std::cout << "calculating num at i:" << i<<std::endl;
			numResult *= ((1-this->ProbabiltyofPi(i)));
			//std::cout << "calculating denom at i:" << i<<std::endl;
			denomResult *= ((1-this->ProbabiltyofPi(i)));
		}
		numResult *= probabiltyofGoodDroplet;
		denomResult = denomResult * probabiltyofGoodDroplet + (1 - probabiltyofGoodDroplet);

		return numResult/denomResult;
	}
	double PCRProbabilty:: ProbabilityofNOTGGivenAk(int k)
	{
		double denomResult=1;
		double numResult = (1-probabiltyofGoodDroplet);

		for(int i=1; i<k; ++i)
		{
			denomResult *= ((1-this->ProbabiltyofPi(i)) );
		}
		denomResult = probabiltyofGoodDroplet * denomResult + (1 - probabiltyofGoodDroplet);
		return numResult/denomResult;
	}
