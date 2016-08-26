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
 * Source: BioConditionalGroup.cpp												*
 * Original Code Author(s): Chris Curtis										*
 * Original Completion/Release Date: April 23, 2014								*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/BioCoder/BioConditionalGroup.h"


/////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////
BioConditionalGroup::BioConditionalGroup()
{
	conditions = new vector<BioCondition *>();
}
/////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////
BioConditionalGroup::~BioConditionalGroup()
{
	while (conditions->size() > 0)
	{
		BioCondition *c = conditions->back();
		conditions->pop_back();
		delete c;
	}
	delete conditions;
}
/////////////////////////////////////////////////////////////////////
// Methods
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Adds new condition to the end of the conditional group.
/////////////////////////////////////////////////////////////////////
BioCondition * BioConditionalGroup::addNewCondition(BioExpression *e, BioCoder *bio)
{
	{	// Sanity check: If e is NULL, definitely an ELSE...must be 1+ conditions already
		std::stringstream msg;
		msg << "ERROR. Cannot add an ELSE w/o at least one other condition" << ends;
		claim(!(!e && conditions->size() == 0), &msg);
	}
	BioCondition *c = new BioCondition();
	c->statement = e;
	c->branchIfTrue = bio;
	c->order = conditions->size();
	conditions->push_back(c);
	return c;
}
/////////////////////////////////////////////////////////////////////
// Adds new condition to the end of the conditional group.
/////////////////////////////////////////////////////////////////////
void BioConditionalGroup::addNewCondition(BioExpression *e, BioCoder *bio, vector<BioTransferEdge *> transfers)
{
	{	// Sanity check: If e is NULL, definitely an ELSE...must be 1+ conditions already
		std::stringstream msg;
		msg << "ERROR. Cannot add an ELSE w/o at least one other condition" << ends;
		claim(!(!e && conditions->size() == 0), &msg);
	}
	BioCondition *c = new BioCondition();
	c->statement = e;
	c->branchIfTrue = bio;
	c->order = conditions->size();
	conditions->push_back(c);

	for (int i = 0; i < transfers.size(); i++){
		cout<<"IN: "<<transfers.at(i)->transIn<<" OUT: "<<transfers.at(i)->transOut<<endl;
		c->transfers.push_back(transfers.at(i));
	}

}
/////////////////////////////////////////////////////////////////////
// Delete based on 0-based index
/////////////////////////////////////////////////////////////////////
void BioConditionalGroup::deleteCondition(int i)
{
	{	// Sanity check: Must be within range
		std::stringstream msg;
		msg << "ERROR. Swap indeces not in range." << ends;
		claim(i >= 0 && i < conditions->size(), &msg);
	}
	conditions->erase(conditions->begin()+i);

	for (int j = i; j < conditions->size(); j++)
		conditions->at(j)->order--; // Update order numbers
}
/////////////////////////////////////////////////////////////////////
// Delete based on actual condition pointer
/////////////////////////////////////////////////////////////////////
void BioConditionalGroup::deleteCondition(BioCondition *c)
{
	for (int i = 0; i < conditions->size(); i++)
	{
		if (c == conditions->at(i))
		{
			deleteCondition(i);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////
// Print the IF/ELSE_IF/ELSE structure
/////////////////////////////////////////////////////////////////////
void BioConditionalGroup::printConditionalGroup()
{

	for (int i = 0; i < conditions->size(); i++)
	{
		BioExpression *s = conditions->at(i)->statement;
		if (i == 0)
			cout << "IF " << s->printBioExpression() << endl << "\tIf True Branch to: " << conditions->at(i)->branchIfTrue->getName() << endl;
		else if (i == conditions->size()-1)
			cout << "ELSE //" << s->printBioExpression() << endl << "\t// Do something" << endl;
		else
			cout << "ELSE IF " << s->printBioExpression() << endl << "\t// Do something" << endl;

	}
}

ConditionalGroup* BioConditionalGroup::convt2CG(vector<DAG*> *dags)
{
	Expression *e;
	AssayNode* AssayIn;
	AssayNode* AssayOut;
	AssayNode* Sensor1;
	AssayNode* Sensor2;
	DAG *dagInput;

	vector<TransferEdge*> te;
	TransferEdge* EdgePush;

	ConditionalGroup *ret =new ConditionalGroup();

	for(int i=0;i<this->conditions->size();++i)
	{
		te.clear();
		e = recursiveConvert(conditions->at(i)->statement, dags);
		/*if(conditions->at(i)->statement==NULL)
			e=NULL;
		else if(conditions->at(i)->statement->operandType==OP_ONE_SENSOR)
		{
			for(int j=0;j<dags->size();++j)
				for(int NodeIndex=0; NodeIndex < dags->at(j)->getAllNodes().size();++NodeIndex)
				{
					if(conditions->at(i)->statement->sensor1 ==dags->at(j)->getAllNodes().at(NodeIndex)->GetName())
						Sensor1=dags->at(j)->getAllNodes().at(NodeIndex);
				}
			e = new Expression(Sensor1,conditions->at(i)->statement->operationType ,conditions->at(i)->statement->constant);
		}
		else if(conditions->at(i)->statement->operationType==OP_UNCOND)
		{
			for(int j=0;j<dags->size();++j)
				if(conditions->at(i)->statement->unconditionalParent->getName()== dags->at(j)->getName())
					Sensor1=dags->at(j)->getAllNodes().at(0);
			if(conditions->at(i)->statement->operandType==OP_TRUE)
				e= new Expression(Sensor1->GetDAG(),true);
			else
				e= new Expression(Sensor1->GetDAG(),false);
		}
		else if(conditions->at(i)->statement->operandType==OP_TWO_SENSORS){
			for(int j=0;j<dags->size();++j)
				for(int NodeIndex=0; NodeIndex < dags->at(j)->getAllNodes().size();++NodeIndex)
				{
					if(conditions->at(i)->statement->sensor1 ==dags->at(j)->getAllNodes().at(NodeIndex)->GetName())
						Sensor1=dags->at(j)->getAllNodes().at(NodeIndex);
					if(conditions->at(i)->statement->sensor2 ==dags->at(j)->getAllNodes().at(NodeIndex)->GetName())
						Sensor2=dags->at(j)->getAllNodes().at(NodeIndex);
				}
			e=new Expression(Sensor1,conditions->at(i)->statement->operationType,Sensor2);
		}
		else if(conditions->at(i)->statement->operandType==OP_EXP)
		{
			e= new Expression(conditions->at(i)->statement->operationType);


		}

		else
			e=NULL;*/



		for(int j=0; j < dags->size(); ++j)
			if(conditions->at(i)->branchIfTrue->getName()==dags->at(j)->getName())
				dagInput=dags->at(j);


		//This get the AssayNode that are the Trans IN/OUT for the Conditional group.
		for(int j=0; j < conditions->at(i)->transfers.size();++j){
			string in= conditions->at(i)->transfers.at(j)->transIn;
			string out= conditions->at(i)->transfers.at(j)->transOut;
			for(int k=0;k<dags->size();k++)
			{
				for(int NodeIndex=0; NodeIndex < dags->at(k)->getAllNodes().size();NodeIndex++)
				{
					if(dags->at(k)->getAllNodes().at(NodeIndex)->GetName()==in)
						AssayIn=dags->at(k)->getAllNodes().at(NodeIndex);
					if(dags->at(k)->getAllNodes().at(NodeIndex)->GetName()==out)
						AssayOut=dags->at(k)->getAllNodes().at(NodeIndex);
				}
			}
			EdgePush=new TransferEdge();
			EdgePush->transIn=AssayIn;
			EdgePush->transOut=AssayOut;
			te.push_back(EdgePush);
		}
		ret->addNewCondition(e,dagInput,te);
	}

return ret;
}

Expression *BioConditionalGroup::recursiveConvert(BioExpression *be, vector<DAG*> *dags)
{
	AssayNode *Sensor1;
	AssayNode *Sensor2;
	AssayNode* node1;

	if(be == NULL)
		return NULL;
	else if (be->operandType == OP_ONE_SENSOR)
	{
		for(int j=0;j<dags->size();++j)
		{
			for(int NodeIndex=0; NodeIndex < dags->at(j)->getAllNodes().size();++NodeIndex)
			{
				if(be->sensor1 ==dags->at(j)->getAllNodes().at(NodeIndex)->GetName())
					Sensor1=dags->at(j)->getAllNodes().at(NodeIndex);
			}
		}
		return new Expression(Sensor1, be->operationType ,be->constant);
	}
	else if(be->operationType==OP_UNCOND)
	{
		for(int j=0;j<dags->size();++j)
			if(be->unconditionalParent->getName() == dags->at(j)->getName())
				node1=dags->at(j)->getAllNodes().at(0);
		if(be->operandType==OP_TRUE)
			return new Expression(node1->GetDAG(),true);
		else
			return new Expression(node1->GetDAG(),false);
	}
	else if(be->operandType==OP_TWO_SENSORS)
	{
		for(int j=0;j<dags->size();++j)
		{
			for(int NodeIndex=0; NodeIndex < dags->at(j)->getAllNodes().size();++NodeIndex)
			{
				if(be->sensor1 == dags->at(j)->getAllNodes().at(NodeIndex)->GetName())
					Sensor1=dags->at(j)->getAllNodes().at(NodeIndex);
				if(be->sensor2 ==dags->at(j)->getAllNodes().at(NodeIndex)->GetName())
					Sensor2=dags->at(j)->getAllNodes().at(NodeIndex);
			}
		}
		return new Expression(Sensor1,be->operationType,Sensor2);
	}
	else if(be->operandType==OP_SUB_EXP)
	{
		Expression *newExp = NULL;
		if (be->operationType == OP_NOT)
			newExp = new Expression(recursiveConvert(be->operands->front(), dags));
		else
		{
			newExp = new Expression(be->operationType);
			for (int i = 0; i < be->operands->size(); i++)
				newExp->addOperand(recursiveConvert(be->operands->at(i), dags));
		}
		return newExp;
	}

	else
		return NULL;
}



