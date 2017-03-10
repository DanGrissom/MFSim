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
 * Name: BioConditionalGroup													*
 *																				*
 * Code Developed and Described in:												*
 * Authors: Dan Grissom, Chris Curtis & Philip Brisk							*
 * Title: Interpreting Assays with Control Flow on Digital Microfluidic			*
 * 			Biochips															*
 * Publication Details: In ACM JETC in Computing Systems, Vol. 10, No. 3,		*
 * 					    Apr 2014, Article No. 24								*
 * 																				*
 * Details: This class describes a BioCoder version of a ConditionalGroup,		*
 * which is used as an internal data structure to help mimic control-flow and 	*
 * conditions/expressions.														*
 *																				*
 *-----------------------------------------------------------------------------*/
#ifndef BIOCONDITIONALGROUP_H_
#define BIOCONDITIONALGROUP_H_

#include "../Resources/enums.h"
#include "../Testing/claim.h"
#include "BioCoder.h"
#include "BioExpression.h"
#include "BioCoderStructs.h"
#include "assayProtocol.h"
#include "../Models/conditional_group.h"
#include "../Models/expression.h"

struct BioTransferEdge;
struct BioCoder;
struct BioCondition;
struct ConditionalGroup;
struct assayProtocol;

class BioConditionalGroup
{
    protected:
        // Variables
		std::vector<BioCondition *> *conditions;
		Expression *recursiveConvert(BioExpression *be, vector<DAG*> *dags);

		// Methods



    public:
        // Constructors
		BioConditionalGroup();
        ~BioConditionalGroup();

        // Getters/Setters
        std::vector<BioCondition *> * getConditions() { return conditions; }


        // Methods
        BioCondition* addNewCondition(BioExpression *e, BioCoder *bio);
        void addNewCondition(BioExpression *e, BioCoder *bio, vector<BioTransferEdge *> transfers);
        void deleteCondition(int i);
        void deleteCondition(BioCondition *c);
        void printConditionalGroup();


        ConditionalGroup * convt2CG(vector<DAG*> *dags);


 };
#endif /* BIOCONDITIONALGROUP_H_ */
