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
 * Name: BioExpression															*
 *																				*
 * Code Developed and Described in:												*
 * Authors: Dan Grissom, Chris Curtis & Philip Brisk							*
 * Title: Interpreting Assays with Control Flow on Digital Microfluidic			*
 * 			Biochips															*
 * Publication Details: In ACM JETC in Computing Systems, Vol. 10, No. 3,		*
 * 					    Apr 2014, Article No. 24								*
 * 																				*
 * Details: This class describes a BioCoder version of a Expression,			*
 * which is used as an internal data structure to help mimic control-flow and 	*
 * conditions/expressions.														*
 *																				*
 *-----------------------------------------------------------------------------*/
#ifndef BIOExpression_H_
#define BIOExpression_H_

class BioCoder;

#include "../Models/assay_node.h"
#include "../Resources/enums.h"
#include "../Testing/claim.h"
#include <sstream>
#include <string>
#include <vector>
#include "../Models/dag.h"

/////////////////////////////////////////////////////////////////
//BioBioExpression: Node Structure for holding an Expression/value/
// operand, as well as the operation type and children.  Together,
// forms a tree which represents an entire boolean Expression
/////////////////////////////////////////////////////////////////
struct BioExpression
{
public:
        // Variables
		ExOperationType operationType;
		std::vector<BioExpression*> *operands;
		ExOperandType operandType;
		double constant;
		string sensor1;
		string sensor2;
		AssayNode * s1;
		BioCoder *unconditionalParent;

		// Methods
		static bool recursiveValidate(BioExpression *e);
		static void recursivePrint(BioExpression *e, std::stringstream *ss);
		//static bool recursiveEvaluate(BioExpression *e);
		//static void recursiveGetParents(BioExpression *e, list<DAG *> *parents);


    //public:
        // Constructors
		BioExpression(string s1, ExOperationType ot, string s2); // Two-sensor constructor
		BioExpression(string s1, ExOperationType ot, double c); // One-sensor constructor
		BioExpression(BioCoder *repeatableAssay, ExOperationType ot, double runCount); // Run-count constructor
		BioExpression(BioExpression *notExp); // NOT constructor
		BioExpression(ExOperationType andOr); // AND/OR constructor
		BioExpression(BioCoder *uncondPar, bool unconditional); // Either unconditionally true or false
        ~BioExpression();

        // Getters/Setters
        std::vector<BioExpression*> * getOperands() { return operands; }
        void setSensor1(string s);
        void setSensor2(string s);
        string getSensor1();
        string getSensor2();

        // Methods
        void addOperand(BioExpression *op);
        bool isValidBioExpression();
        std::string printBioExpression();
     //   bool evaluateBioExpression();
       // void getParentDags(list<DAG *> *parents);

 };

#endif /* BIOExpression_H_ */
