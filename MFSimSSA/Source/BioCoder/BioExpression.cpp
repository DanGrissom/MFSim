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
 * Source: BioExpression.cpp													*
 * Original Code Author(s): Chris Curtis										*
 * Original Completion/Release Date: April 23, 2014								*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/BioCoder/BioExpression.h"
#include <string>
/////////////////////////////////////////////////////////////////////
// Constructors
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Creating an BioExpression that compares the readings from two sensors.
/////////////////////////////////////////////////////////////////////

BioExpression::BioExpression(string s1, ExOperationType ot, string s2)
{
	{	// Sanity check: Must be proper operation type
		if (!(ot == OP_GT || ot == OP_LT || ot == OP_GoE || ot == OP_LoE || ot == OP_EQUAL) )
		{
			std::stringstream msg;
			msg << "ERROR. >, <, <=, >=, == operations allowed for a sensor-sensor comparison. Must be valid sensors." << ends;
			claim(false, &msg);
		}
	}
	operationType = ot;
	operands = NULL;
	operandType = OP_TWO_SENSORS;
	constant = 0;
	sensor1 = s1;
	sensor2 = s2;
	this->s1=NULL;
}
/////////////////////////////////////////////////////////////////////
// Creating an BioExpression that compares the reading from a sensor to
// a constant value
/////////////////////////////////////////////////////////////////////
BioExpression::BioExpression(string s1, ExOperationType ot, double c)
{
	{	// Sanity check: Must be proper operation type
		if (!(ot == OP_GT || ot == OP_LT || ot == OP_GoE || ot == OP_LoE || ot == OP_EQUAL)	)
		{
			std::stringstream msg;
			msg << "ERROR. >, <, <=, >=, == operations allowed for a sensor-sensor comparison. Must be valid sensors." << ends;
			claim(false, &msg);
		}
	}
	operationType = ot;
	operands = NULL;
	operandType = OP_ONE_SENSOR;
	constant = c;
	sensor1 = s1;
	sensor2 = "";
	this->s1=NULL;
}
/////////////////////////////////////////////////////////////////////
// Creating an BioExpression that performs a NOT operation
/////////////////////////////////////////////////////////////////////
BioExpression::BioExpression(BioExpression *notExp)
{
	operationType = OP_NOT;
	operands = new std::vector<BioExpression*>();
	operands->push_back(notExp);
	operandType = OP_SUB_EXP;
	constant = 0;
	sensor1 = "";
	sensor2 = "";
	this->s1=NULL;
}

/////////////////////////////////////////////////////////////////////
// Creating an BioExpression that performs an AND or OR operation
/////////////////////////////////////////////////////////////////////
BioExpression::BioExpression(ExOperationType andOr)
{
	{	// Sanity check: Must be proper operation type
		std::stringstream msg;
		msg << "ERROR. Only AND, OR operations allowed for this BioExpression." << ends;
		claim(andOr == OP_AND || andOr == OP_OR, &msg);
	}
	operationType = andOr;
	operands = new std::vector<BioExpression*>();
	operandType = OP_SUB_EXP;
	constant = 0;
	sensor1 = "";
	sensor2 = "";
	this->s1=NULL;
}
BioExpression::BioExpression(BioCoder *uncondPar, bool unconditional)
{
	if (unconditional)
		operandType = OP_TRUE;
	else
		operandType = OP_FALSE;
	//operands = new vector<Expression*>();
	operationType = OP_UNCOND;
	constant = 0;
	sensor1 = "";
	sensor2 = "";
	this->s1=NULL;
	unconditionalParent = uncondPar;
}
/////////////////////////////////////////////////////////////////////
// Destructor.
/////////////////////////////////////////////////////////////////////
BioExpression::~BioExpression()
{
	if (operands)
	{

		//for (int i = 0; i < operands->size(); i++)
			//recursiveDelete();

		operands->clear();
		delete operands;
	}

	delete s1;
}
/////////////////////////////////////////////////////////////////////
// Methods
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Add an operand to an AND/OR statement.
/////////////////////////////////////////////////////////////////////
void BioExpression::addOperand(BioExpression *op)
{
	{	// Sanity check: Must be AND/OR to add operand
		std::stringstream msg;
		msg << "ERROR. Only AND, OR operations allowed to add more operands." << ends;
		claim(operationType == OP_AND || operationType == OP_OR, &msg);
	}
	{	// Sanity check: BioExpression must not be NULL
		std::stringstream msg;
		msg << "ERROR. BioExpression is not valid." << ends;
		claim(op, &msg);
	}
	operands->push_back(op);
}
void BioExpression::setSensor1(string s)
{
	sensor1=s;
}
void BioExpression::setSensor2(string s)
{
	sensor2=s;
}

string BioExpression::getSensor1()
{
	return sensor1;
}
string BioExpression::getSensor2()
{
	return sensor2;
}

/////////////////////////////////////////////////////////////////////
// Evaluates if the BioExpression is valid.  That is, if all the leaves
// actually evaluate to a true or false (the leaves all compare
// sensor/constant values).
/////////////////////////////////////////////////////////////////////
bool BioExpression::isValidBioExpression()
{
	return recursiveValidate(this);
}
bool BioExpression::recursiveValidate(BioExpression *e)
{
	if (!e)
		return false;
	if (e->operationType == OP_GT || e->operationType == OP_LT
			|| e->operationType == OP_GoE || e->operationType == OP_LoE || e->operationType == OP_EQUAL)
		return true;
	else if (e->operationType == OP_AND && e->operationType == OP_OR && e->operands->size() <= 1)
		return false;
	else if (e->operands->size() == 0) // e->operationType == OP_NOT
			return false;

	bool isValid = true;
	for (int i = 0; i < e->operands->size(); i++)
	{
		isValid = isValid && (recursiveValidate(e->operands->at(i)));
		if (!isValid)
			return false;
	}
	return true;
}
/////////////////////////////////////////////////////////////////////
// Prints the boolean BioExpression.
/////////////////////////////////////////////////////////////////////
string BioExpression::printBioExpression()
{
	std::stringstream ss;
	recursivePrint(this, &ss);
	return ss.str();
}
void BioExpression::recursivePrint(BioExpression *e, std::stringstream *ss)
{
	if (!e)
	{
		*ss << "(No Condition)";
		return;
	}

	*ss << "(";
	if (e->operationType == OP_GT || e->operationType == OP_LT
			|| e->operationType == OP_GoE || e->operationType == OP_LoE || e->operationType == OP_EQUAL)
	{
		*ss << e->sensor1 ;
		if (e->operationType == OP_GT)
			*ss << " > ";
		else if (e->operationType == OP_LT)
			*ss << " < ";
		else if (e->operationType == OP_GoE)
			*ss << " >= ";
		else if (e->operationType == OP_LoE)
			*ss << " <= ";
		else if (e->operationType == OP_EQUAL)
			*ss << " = ";
		else
			*ss << " ??? ";

		if (e->operandType == OP_ONE_SENSOR)
			*ss << e->constant;
		else if (e->operandType == OP_TWO_SENSORS)
			*ss << e->sensor2;
		else
			*ss << "---ERROR---";
	}
	else if (e->operationType == OP_AND || e->operationType == OP_OR)
	{
		for (int i = 0; i < e->operands->size(); i++)
		{
			recursivePrint(e->operands->at(i), ss);
			if (i < e->operands->size()-1 && e->operationType == OP_AND)
				*ss << " AND ";
			else if (i < e->operands->size()-1 && e->operationType == OP_OR)
				*ss << " OR ";
		}

	}
	else if (e->operationType == OP_NOT)
	{
		*ss << " NOT";
		recursivePrint(e->operands->front(), ss);
	}
	else
		*ss << "---ERROR---";
	*ss << ")";
}

/////////////////////////////////////////////////////////////////////
// Evaluates the BioExpression and returns the value.
/////////////////////////////////////////////////////////////////////
//bool BioExpression::evaluateBioExpression()
//{
//	return recursiveEvaluate(this);
//}


/////////////////////////////////////////////////////////////////////
// Gets the unique DAG parents for this BioExpression
/////////////////////////////////////////////////////////////////////
//void BioExpression::getParentDags(list<DAG *> *parents)
//{
//	parents->clear();
//	recursiveGetParents(this, parents);
//}
//void BioExpression::recursiveGetParents(BioExpression *e, list<DAG *> *parents)
//{
//	if (e->operationType == OP_GT || e->operationType == OP_LT
//			|| e->operationType == OP_GoE || e->operationType == OP_LoE || e->operationType == OP_EQUAL)
//	{
//		parents->remove(e->sensor1->GetDAG());
//		parents->push_back(e->sensor1->GetDAG());
//
//		if (e->operandType == OP_TWO_SENSORS)
//		{
//			parents->remove(e->sensor2->GetDAG());
//			parents->push_back(e->sensor2->GetDAG());
//		}
//	}
//	else
//	{
//		for (int i = 0; i < e->operands->size(); i++)
//			recursiveGetParents(e->operands->at(i), parents);
//	}
//}
