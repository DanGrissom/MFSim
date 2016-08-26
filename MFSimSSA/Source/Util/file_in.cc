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
 * Source: file_in.cc															*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 17, 2013							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/Util/file_in.h"
#include "../../Headers/Util/util.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
FileIn::FileIn() {}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
FileIn::~FileIn(){}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the CFG structure saved in the specified file so it
// can be processed by the system (scheduling, placement, routing, etc.)
////////////////////////////////////////////////////////////////////////////
CFG *FileIn::ReadCfgFromFile(string fileName)
{
	// Init data structures/variables
	CFG *cfg = new CFG();
	map<string, DAG *> dags;
	map<int, Expression *> *exps = new map<int, Expression *>();
	map<Condition *, int> condToExpId;
	vector<ConditionalGroup *> cgs;
	ConditionalGroup *currentCg = NULL;
	Condition *currentCondition = NULL;
	int highestDagId = 0;

	// Open file and read
	ifstream is;
	is.open(fileName.c_str());
	{
		stringstream str;
		str << "Failed to properly read CFG file: " << fileName << endl;
		claim (is.good(), &str);
	}

	// Get prefix file name for DAG files
	string dagFileNamePrefix = fileName.substr(0, fileName.length() - 4); // Remove ".cfg"
	string dagFileExt = ".dag";

	// Parse each line
	string line;
	while (!is.eof())
	{
		line = GetLine(&is);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//" ))
		{
			ParseLine(line, &tag, &p);

			tag = Util::StringToUpper(tag);

			// Recreate CFG Structure
			if (tag == "NAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for NAME.");
				std::replace(p.at(0).begin(), p.at(0).end(), ' ', '_');
				cfg->setName(p.at(0));
			}
			else if (tag == "DAG")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAG.");
				string dagName = p.at(0);
				string dagFileName = dagFileNamePrefix + "_" + dagName + dagFileExt;

				// Read DAG from file and add to CFG....
				DAG *d = FileIn::ReadDagFromFile(dagFileName);
				string dagId = dagName.substr(3, string::npos);
				cfg->AddExistingDAG(d);

				// ...and place into map
				d->id = atoi(dagId.c_str());
				dags[dagName] = d;

				// Update highestDagId for internal maintenance
				if (d->id > highestDagId)
					highestDagId = d->id;
			}
			else if (tag == "NUMCGS")
			{
				claim(p.size() == 1, "Invalid number of parameters for NUMCGS.");

				// Get number of conditional groups and add that number of CGs
				// to the CFG; keep CGs in vector for easy reference
				int numCgs = (atoi(p.at(0).c_str()));
				for (int i = 0; i < numCgs; i++)
				{
					ConditionalGroup *cg = cfg->AddNewCG();
					cgs.push_back(cg);
				}
			}
			else if (tag == "COND")
			{
				///////////////////////////////////////////////////////////////////////
				// Every time we hit a condition tag, it means it's time to parse a new
				// condition. The COND() tag will be followed by optional EXP() and TD()
				// tags before the next COND() tag. First, add the last condition if
				// one exists before creating a new condition.
				if (currentCondition)
					currentCg->addExistingCondition(currentCondition);
				currentCondition = new Condition();

				// Get conditional group
				currentCg = cgs.at(atoi(p.at(0).c_str()));

				// Read in Dependent DAG names and fetch DAG;
				// add to condition
				int numDepDags = atoi(p.at(1).c_str());
				for (int i = 0; i < numDepDags; i++)
				{
					string depDagName = p.at(2 + i);
					DAG *d = dags[depDagName];
					currentCondition->dependents.push_back(d);
				}

				// Read in branchIfTrue DAG names and fetch DAGs;
				// add to condition
				int numBranchDags = atoi(p.at(2 + numDepDags).c_str());
				claim(numBranchDags == 1, "Simulator currently only supports 1 brahchIfTrue DAG.");
				for (int i = 0; i < numBranchDags; i++)
				{
					string branchDagName = p.at(3 + numDepDags + i);
					DAG *d = dags[branchDagName];
					currentCondition->branchIfTrue = d;
				}

				// Get expression id and map condition to id (for later association)
				int expId = atoi(p.at(3 + numDepDags + numBranchDags).c_str());
				if (expId != -1)
					condToExpId[currentCondition] = expId;

			}
			else if (tag == "EXP")
			{
				int expId = atoi(p.at(0).c_str());
				string operandType = p.at(1);

				// Either get an already existent (pre-created) expression,
				// or create it now
				Expression *e = AddOrGetUniqueExpressionFromMap(exps, expId);

				if (operandType == "SUB_EXP")
				{

					e->operandType = OP_SUB_EXP;
					e->operationType = GetExOperationTypeFromString(p.at(2));
					e->operands = new vector<Expression *>();

					// Either get an already existent (pre-creation) expression, or create it now
					int subExp1Id = atoi(p.at(3).c_str());
					Expression *se1 = AddOrGetUniqueExpressionFromMap(exps, subExp1Id);
					e->operands->push_back(se1);

					if (e->operationType == OP_NOT)
						claim(p.size() == 4, "Invalid number of parameters for EXP w/ SUB_EXP/OP_NOT.");
					else
					{
						claim(p.size() >= 5, "Invalid number of parameters for EXP w/ SUB_EXP/OP_AND/OP_OR.");

						// Either get an already existent (pre-creation) expression, or create it now
						int subExp2Id = atoi(p.at(4).c_str());
						Expression *se2 = AddOrGetUniqueExpressionFromMap(exps, subExp2Id);
						e->operands->push_back(se2);
					}
				}
				else if (operandType == "RUN_COUNT")
				{
					claim(p.size() == 4, "Invalid number of parameters for EXP w/ RUN_COUNT.");
					e->operandType = OP_SUB_EXP;
					e->operationType = GetExOperationTypeFromString(p.at(2));
					e->constant = strtod(p.at(3).c_str(), NULL); // run count
				}
				else if (operandType == "ONE_SENSOR")
				{
					claim(p.size() == 6, "Invalid number of parameters for EXP w/ ONE_SENSOR.");
					e->operandType = OP_ONE_SENSOR;
					e->operationType = GetExOperationTypeFromString(p.at(2));
					DAG *dag1 = dags[p.at(3)];
					e->sensor1 = dag1->getNode(atoi(p.at(4).c_str())); // op1SensorNodeId
					e->constant = strtod(p.at(5).c_str(), NULL); // op2StaticVal
				}
				else if (operandType == "TWO_SENSORS")
				{
					claim(p.size() == 7, "Invalid number of parameters for EXP w/ TWO_SENSORS.");
					e->operandType = OP_TWO_SENSORS;
					e->operationType = GetExOperationTypeFromString(p.at(2));
					DAG *dag1 = dags[p.at(3)];
					e->sensor1 = dag1->getNode(atoi(p.at(4).c_str())); // op1SensorNodeId
					DAG *dag2 = dags[p.at(5)];
					e->sensor2 = dag2->getNode(atoi(p.at(6).c_str())); // op2SensorNodeId
				}
				else if (operandType == "TRUE")
				{
					claim(p.size() == 4, "Invalid number of parameters for EXP w/ TRUE.");
					e->operandType = OP_TRUE;
					e->operationType = GetExOperationTypeFromString(p.at(2));
					e->unconditionalParent = dags[p.at(3)]; // op1UncondDagName
				}
				else if (operandType == "FALSE")
				{
					claim(p.size() == 4, "Invalid number of parameters for EXP w/ FALSE.");
					e->operandType = OP_FALSE;
					e->operationType = GetExOperationTypeFromString(p.at(2));
					e->unconditionalParent = dags[p.at(3)]; // op1UncondDagName
				}
				else
					claim(false, operandType + " is an unsupported operand type.");
			}
			else if (tag == "TD")
			{
				// Create droplet transfer edge and add to condition
				TransferEdge * te = new TransferEdge();
				currentCondition->transfers.push_back(te);

				// Get transfer out node
				DAG *toDag = dags[p.at(0)];
				te->transOut = toDag->getNode(atoi(p.at(1).c_str())); // toNodeId

				// Get transfer in node
				DAG *tiDag = dags[p.at(2)];
				te->transIn = tiDag->getNode(atoi(p.at(3).c_str())); // tiNodeId
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Add last condition to conditional group
	if (currentCondition)
		currentCg->addExistingCondition(currentCondition);
	currentCg = NULL;
	currentCondition = NULL;

	// Finally, associate the conditions with the top-level expressions
	map<Condition *, int>::iterator condIt = condToExpId.begin();
	for (; condIt != condToExpId.end(); condIt++)
	{
		Condition *c = condIt->first;
		c->statement = (*exps)[condIt->second];
	}

	// Make sure DAG id's created are higher than anything seen to guarantee uniqueness
	if (highestDagId + 1 > DAG::next_id)
		DAG::next_id = highestDagId+1;

	// Cleanup
	is.close();
	delete exps;

	// Construct the CFG (internal maintenance) and return
	cfg->ConstructAndValidateCFG();
	return cfg;
}

////////////////////////////////////////////////////////////////////////////
// Takes in a map of expression ids (int) to expressions. The method returns
// the expression with expId as its id, if it is found. If not, it creates
// a new expression with expId as its id, adds it to the map, and returns it.
////////////////////////////////////////////////////////////////////////////
Expression * FileIn::AddOrGetUniqueExpressionFromMap(map<int, Expression *> *exps, int expId)
{
	Expression *e = NULL;

	// Get the expression if found...
	if (exps->find(expId) != exps->end())
		e = (*exps)[expId];
	else
	{
		e = new Expression(expId); //...otherwise create a new one
		(*exps)[expId] = e;
	}

	return e;
}
////////////////////////////////////////////////////////////////////////////
// Converts a string to ExOperationType enum type.
////////////////////////////////////////////////////////////////////////////
ExOperationType FileIn::GetExOperationTypeFromString(string str)
{
	if (str == "GT")
		return OP_GT;
	else if (str == "GoE")
		return OP_GoE;
	else if (str == "LT")
		return OP_LT;
	else if (str == "LoE")
		return OP_LoE;
	else if (str == "EQUAL")
		return OP_EQUAL;
	else if (str == "NOT")
		return OP_NOT;
	else if (str == "AND")
		return OP_AND;
	else if (str == "OR")
		return OP_OR;
	else if (str == "UNCOND")
		return OP_UNCOND;
	else
		claim(false, str + " is an unknown expression operation type.");

}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DAG structure saved in the specified file so it
// can be processed by a scheduler.
////////////////////////////////////////////////////////////////////////////
DAG *FileIn::ReadDagFromFile(string fileName)
{
	DAG *dag = new DAG();
	map<int, AssayNode *> nodes;
	vector<int> edges;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read DAG file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	int highestNodeId = 0;
	while (!is.eof())
	{
		line = GetLine(&is);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//" ))
		{
			ParseLine(line, &tag, &p);

			tag = Util::StringToUpper(tag);
			// Recreate Nodes and save edges for later
			if (tag == "DAGNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAGNAME.");
				std::replace(p.at(0).begin(), p.at(0).end(), ' ', '_');
				dag->setName(p.at(0));
			}
			else if (tag == "NODE")
			{
				AssayNode *n;
				if (Util::StringToUpper(p.at(1)) == "DISPENSE")
				{
					claim(p.size() == 5, "Invalid number of parameters for DISPENSE.");
					n = dag->AddDispenseNode(p.at(2), atof(p.at(3).c_str()), p.at(4));
				}
				else if (Util::StringToUpper(p.at(1)) == "MIX")
				{
					claim(p.size() == 5, "Invalid number of parameters for MIX.");
					n = dag->AddMixNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (Util::StringToUpper(p.at(1)) == "DILUTE")
				{
					claim(p.size() == 5, "Invalid number of parameters for DILUTE.");
					n = dag->AddDiluteNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (Util::StringToUpper(p.at(1)) == "SPLIT")
				{
					claim(p.size() == 5, "Invalid number of parameters for SPLIT.");
					n = dag->AddSplitNode(false, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (Util::StringToUpper(p.at(1)) == "HEAT")
				{
					claim(p.size() == 4, "Invalid number of parameters for HEAT.");
					n = dag->AddHeatNode(atof(p.at(2).c_str()), p.at(3));
				}
				else if (Util::StringToUpper(p.at(1)) == "COOL")
				{
					claim(p.size() == 4, "Invalid number of parameters for COOL.");
					n = dag->AddCoolNode(atof(p.at(2).c_str()), p.at(3));
				}
				else if (Util::StringToUpper(p.at(1)) == "DETECT")
				{
					claim(p.size() == 5, "Invalid number of parameters for DETECT.");
					n = dag->AddDetectNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (Util::StringToUpper(p.at(1)) == "OUTPUT")
				{
					claim(p.size() == 4, "Invalid number of parameters for OUTPUT.");
					n = dag->AddOutputNode(p.at(2), p.at(3));
				}
				else if (Util::StringToUpper(p.at(1)) == "STORAGE")
				{
					claim(p.size() == 3, "Invalid number of parameters for STORAGE.");
					n = dag->AddStorageNode(p.at(2));
				}
				else if (Util::StringToUpper(p.at(1)) == "TRANSFER_IN")
				{
					claim(p.size() == 3, "Invalid number of parameters for TRANSFER_IN.");
					n = dag->AddTransInNode(p.at(2));
				}
				else if (Util::StringToUpper(p.at(1)) == "TRANSFER_OUT")
				{
					claim(p.size() == 3, "Invalid number of parameters for TRANSFER_OUT.");
					n = dag->AddTransOutNode(p.at(2));
				}
				else // GENERAL
				{
					claim(p.size() == 3, "Invalid number of parameters for GENERAL.");
					n = dag->AddGeneralNode(p.at(2));
				}
				n->id = atoi(p.at(0).c_str());
				nodes[n->id] = n;

				if (n->id > highestNodeId)
					highestNodeId = n->id;
			}
			else if (tag == "EDGE")
			{
				claim(p.size() == 2, "Invalid number of parameters for an EDGE.");
				edges.push_back(atoi(p.at(0).c_str()));
				edges.push_back(atoi(p.at(1).c_str()));
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Make sure node id's created are higher than anything seen to guarantee uniqueness
	if (highestNodeId + 1 > AssayNode::next_id)
		AssayNode::next_id = highestNodeId+1;

	// Recreate Edges
	while (!edges.empty())
	{
		dag->ParentChild(nodes[edges[0]], nodes[edges[1]]);
		edges.erase(edges.begin());
		edges.erase(edges.begin());
	}

	is.close();
	return dag;
}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DmfbArch structure saved in the specified file
// so it can be processed used during synthesis to know the target DMFB.
////////////////////////////////////////////////////////////////////////////
DmfbArch *FileIn::ReadDmfbArchFromFile(string fileName)
{
	DmfbArch *arch = new DmfbArch;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read DMFB Architecture file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		line = GetLine(&is);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = Util::StringToUpper(tag);
			// Recreates Architecture Details
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
				std::replace(arch->name.begin(), arch->name.end(), ' ', '_');
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				if (Util::StringToUpper(p.at(0)) == "HEAT")
					arch->externalResources->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else if (Util::StringToUpper(p.at(0)) == "DETECT")
					arch->externalResources->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else // GENERAL
					claim(false, "Invalid type of EXTERNAL module.");
			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 4 || p.size() == 5, "Invalid number of parameters for an INPUT.");
				bool wash = false;
				if (p.size() == 5)
				{
					if (Util::StringToUpper(p.at(4)) == "TRUE")
						wash = true;
					else if (Util::StringToUpper(p.at(4)) == "FALSE")
						wash = false;
					else
						claim(false, "Invalid washing mode; should be 'True' or 'False'.");
				}
				if (Util::StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(true, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else if (Util::StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(true, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else if (Util::StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(true, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else if (Util::StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(true, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else
					claim(false, "Invalid side for an INPUT.");
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 4 || p.size() == 5, "Invalid number of parameters for an OUTPUT.");
				bool wash = false;
				if (p.size() == 5)
				{
					if (Util::StringToUpper(p.at(4)) == "TRUE")
						wash = true;
					else if (Util::StringToUpper(p.at(4)) == "FALSE")
						wash = false;
					else
						claim(false, "Invalid washing mode; should be 'True' or 'False'.");
				}
				if (Util::StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(false, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else if (Util::StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(false, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else if (Util::StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(false, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else if (Util::StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(false, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3), wash));
				else
					claim(false, "Invalid side for an OUTPUT.");
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	is.close();
	return arch;
}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DAG structure and ARCH structure saved in the
// specified file so it can be processed by a placer.
////////////////////////////////////////////////////////////////////////////
void FileIn::ReadScheduledDagAndArchFromFile(DAG *dag, DmfbArch *arch, Placer *placer, string fileName)
{
	map<int, AssayNode*> nodes;
	vector<int> edges;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read Scheduled DAG/Arch file: " << fileName << endl;
		claim (is.good(), &str);
	}

	int highestNodeId = 0;
	string line;
	while (!is.eof())
	{
		line = GetLine(&is);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = Util::StringToUpper(tag);

			// Recreate Nodes and save edges for later
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				/*if ((ResourceType)atoi(p.at(0).c_str()) == H_RES)
					arch->fixedModules->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else if ((ResourceType)atoi(p.at(0).c_str()) == D_RES)
					arch->fixedModules->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else // GENERAL
					claim(false, "Invalid type of fixed module.");*/
				arch->externalResources->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));

			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 6, "Invalid number of parameters for an INPUT.");

				bool wash = false;
				if (Util::StringToUpper(p.at(5)) == "TRUE")
					wash = true;
				else if (Util::StringToUpper(p.at(5)) == "FALSE")
					wash = false;
				else
					claim(false, "Invalid washing mode; should be 'True' or 'False'.");

				if (Util::StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(true, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else if (Util::StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(true, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else if (Util::StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(true, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else if (Util::StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(true, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else
					claim(false, "Invalid side for an INPUT.");
				arch->ioPorts->back()->setPinNo(atoi(p.at(3).c_str()));
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 6, "Invalid number of parameters for an OUTPUT.");

				bool wash = false;
				if (Util::StringToUpper(p.at(5)) == "TRUE")
					wash = true;
				else if (Util::StringToUpper(p.at(5)) == "FALSE")
					wash = false;
				else
					claim(false, "Invalid washing mode; should be 'True' or 'False'.");

				if (Util::StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(false, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else if (Util::StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(false, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else if (Util::StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(false, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else if (Util::StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(false, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4), wash));
				else
					claim(false, "Invalid side for an OUTPUT.");
				arch->ioPorts->back()->setPinNo(atoi(p.at(3).c_str()));
			}
			else if (tag == "DAGNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAGNAME.");
				dag->setName(p.at(0));
			}
			else if (tag == "NODE")
			{
				/*ResourceType rt;
				if (p.back() == "B")
					rt = BASIC_RES;
				else if (p.back() == "D")
					rt = D_RES;
				else if (p.back() == "H")
					rt = H_RES;
				else if (p.back() == "DH")
					rt = DH_RES;
				else if (p.back() == "SS")
					rt = SSD_RES;
				else
					rt = UNKNOWN_RES;
				 */

				AssayNode *n;
				if (Util::StringToUpper(p.at(1)) == "DISPENSE")
				{
					claim(p.size() == 7, "Invalid number of parameters for DISPENSE.");
					n = dag->AddDispenseNode(p.at(2), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
				}
				else if (Util::StringToUpper(p.at(1)) == "MIX")
				{
					claim(p.size() == 8, "Invalid number of parameters for MIX.");
					n = dag->AddMixNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());//rt;
				}
				else if (Util::StringToUpper(p.at(1)) == "DILUTE")
				{
					claim(p.size() == 8, "Invalid number of parameters for DILUTE.");
					n = dag->AddDiluteNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());//rt;
				}
				else if (Util::StringToUpper(p.at(1)) == "SPLIT")
				{
					claim(p.size() == 8, "Invalid number of parameters for SPLIT.");
					n = dag->AddSplitNode(false, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (Util::StringToUpper(p.at(1)) == "HEAT")
				{
					claim(p.size() == 7, "Invalid number of parameters for HEAT.");
					n = dag->AddHeatNode(atof(p.at(2).c_str()), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (Util::StringToUpper(p.at(1)) == "COOL")
				{
					claim(p.size() == 7, "Invalid number of parameters for COOL.");
					n = dag->AddCoolNode(atof(p.at(2).c_str()), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (Util::StringToUpper(p.at(1)) == "DETECT")
				{
					claim(p.size() == 8, "Invalid number of parameters for DETECT.");
					n = dag->AddDetectNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (Util::StringToUpper(p.at(1)) == "OUTPUT")
				{
					claim(p.size() == 6, "Invalid number of parameters for OUTPUT.");
					n = dag->AddOutputNode(p.at(2), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
				}
				else if (Util::StringToUpper(p.at(1)) == "STORAGE")
				{
					claim(p.size() == 5, "Invalid number of parameters for STORAGE.");
					n = dag->AddStorageNode();
					n->startTimeStep = strtoul(p.at(2).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (Util::StringToUpper(p.at(1)) == "STORAGE_HOLDER")
				{
					claim(p.size() == 5, "Invalid number of parameters for STORAGE_HOLDER.");
					n = dag->AddStorageHolderNode();
					n->startTimeStep = strtoul(p.at(2).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (Util::StringToUpper(p.at(1)) == "TRANSFER_IN")
				{
					claim(p.size() == 5, "Invalid number of parameters for TRANSFER_IN.");
					n = dag->AddTransInNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
				}
				else if (Util::StringToUpper(p.at(1)) == "TRANSFER_OUT")
				{
					claim(p.size() == 5, "Invalid number of parameters for TRANSFER_OUT.");
					n = dag->AddTransOutNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
				}
				else // GENERAL
				{
					claim(p.size() == 6, "Invalid number of parameters for GENERAL.");
					n = dag->AddGeneralNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				n->id = atoi(p.at(0).c_str());
				n->status = SCHEDULED;
				nodes[n->id] = n;

				if (n->id > highestNodeId)
					highestNodeId = n->id;
			}
			else if (tag == "EDGE")
			{
				claim(p.size() == 2, "Invalid number of parameters for an EDGE.");
				edges.push_back(atoi(p.at(0).c_str()));
				edges.push_back(atoi(p.at(1).c_str()));
			}
			else if (tag == "PINMAP")
			{
				claim((int)p.size() == arch->numCellsX*arch->numCellsY, "PinMap should be the same size of the array.");
				int i = 0;
				for (int y = 0; y < arch->numCellsY; y++)
					for (int x = 0; x < arch->numCellsX; x++)
						arch->pinMapper->getPinMapping()->at(x)->at(y) = atoi(p.at(i++).c_str());
			}
			else if (tag == "SPECIALPINS")
			{
				for (unsigned i = 0; i < p.size(); i++)
					arch->pinMapper->getSpecialPurposePins()->push_back(atoi(p.at(i).c_str()));
				arch->getPinMapper()->unflattenSpecialPurposePins();
			}
			else if (tag == "RESOURCECOUNT")
			{
				for (unsigned i = 0; i < p.size(); i++)
					arch->pinMapper->getAvailResCount()->at(i) = atoi(p.at(i).c_str());
			}
			else if (tag == "RESOURCELOCATION")
			{
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->back()->setTileNum(atoi(p.at(5).c_str()));
			}
			else if (tag == "SCHEDTYPE")
				placer->setPastSchedType(FileIn::GetTypeFromSchedKey(p.at(0)));
			else if (tag == "RESOURCEALLOCATIONTYPE")
				arch->getPinMapper()->setResAllocType(FileIn::GetTypeFromResourceAllocationKey(p.at(0)));
			else if (tag == "PINMAPTYPE")
			{
				PinMapType pmt = FileIn::GetTypeFromPinMapKey(p.at(0));
				Synthesis::setPinMappingMethod(pmt, INHERIT_RA, arch);
				arch->getPinMapper()->setType(pmt);
				arch->getPinMapper()->initPinMapping();
			}
			else if (tag == "MAXSTORAGEDROPSPERMOD")
				placer->setMaxStoragePerModule(atoi(p.at(0).c_str()));
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Make sure node id's created are higher than anything seen to guarantee uniqueness
	if (highestNodeId + 1 > AssayNode::next_id)
		AssayNode::next_id = highestNodeId+1;

	// Recreate Edges
	while (!edges.empty())
	{
		dag->ParentChild(nodes[edges[0]], nodes[edges[1]]);
		//dag->ParentChild(dag->getAllNodes().at(edges.at(0)-1), dag->getAllNodes().at(edges.at(1)-1));
		edges.erase(edges.begin());
		edges.erase(edges.begin());
	}

	is.close();
}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DAG structure and ARCH structure saved in the
// specified file so it can be processed by a placer.
////////////////////////////////////////////////////////////////////////////
void FileIn::ReadPlacedDagAndArchFromFile(DAG *dag, DmfbArch *arch, Router *router, vector<ReconfigModule *> *rModules, string fileName)
{
	map<int, AssayNode*> nodes;
	map<int, IoPort*> ports;
	map<int, ReconfigModule*> rMods;
	vector<int> edges;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read Placed DAG/Arch file: " << fileName << endl;
		claim (is.good(), &str);
	}

	int highestNodeId = 0;
	string line;
	while (!is.eof())
	{
		line = GetLine(&is);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = Util::StringToUpper(tag);

			// Recreate Nodes and save edges for later
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				/*if (Util::StringToUpper(p.at(0)) == "HEAT")
					arch->fixedModules->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else if (Util::StringToUpper(p.at(0)) == "DETECT")
					arch->fixedModules->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else // GENERAL
					claim(false, "Invalid type of EXTERNAL module.");*/
				arch->externalResources->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
			}
			else if (tag == "RECONFIG")
			{
				claim(p.size() == 10, "Invalid number of parameters for reconfigurable module/resource.");
				ReconfigModule *rm = new ReconfigModule((ResourceType)atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str()), atoi(p.at(5).c_str()), atoi(p.at(6).c_str()));
				rm->id = atoi(p.at(0).c_str());
				rm->startTimeStep = strtoul(p.at(7).c_str(), NULL, 0);
				rm->endTimeStep = strtoul(p.at(8).c_str(), NULL, 0);
				rm->tiledNum = atoi(p.at(9).c_str());
				//rm->boundNode->type = getOpTypeFromString(p.at(1).c_str());

				rMods[rm->id] = rm;
			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 7, "Invalid number of parameters for an INPUT.");
				Direction dir = DIR_UNINIT;
				if (Util::StringToUpper(p.at(1)) == "NORTH")
					dir = NORTH;
				else if (Util::StringToUpper(p.at(1)) == "SOUTH")
					dir = SOUTH;
				else if (Util::StringToUpper(p.at(1)) == "EAST")
					dir = EAST;
				else if (Util::StringToUpper(p.at(1)) == "WEST")
					dir = WEST;
				else
					claim(false, "Invalid side for an INPUT.");

				bool wash = false;
				if (Util::StringToUpper(p.at(6)) == "TRUE")
					wash = true;
				else if (Util::StringToUpper(p.at(6)) == "FALSE")
					wash = false;
				else
					claim(false, "Invalid washing mode; should be 'True' or 'False'.");

				IoPort *iop = new IoPort(true, dir, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(5), wash);
				iop->setPinNo(atoi(p.at(4).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 7, "Invalid number of parameters for an OUTPUT.");
				Direction dir = DIR_UNINIT;
				if (Util::StringToUpper(p.at(1)) == "NORTH")
					dir = NORTH;
				else if (Util::StringToUpper(p.at(1)) == "SOUTH")
					dir = SOUTH;
				else if (Util::StringToUpper(p.at(1)) == "EAST")
					dir = EAST;
				else if (Util::StringToUpper(p.at(1)) == "WEST")
					dir = WEST;
				else
					claim(false, "Invalid side for an OUTPUT.");

				bool wash = false;
				if (Util::StringToUpper(p.at(6)) == "TRUE")
					wash = true;
				else if (Util::StringToUpper(p.at(6)) == "FALSE")
					wash = false;
				else
					claim(false, "Invalid washing mode; should be 'True' or 'False'.");

				IoPort *iop = new IoPort(false, dir, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(5), wash);
				iop->setPinNo(atoi(p.at(4).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "DAGNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAGNAME.");
				dag->setName(p.at(0));
			}
			else if (tag == "NODE")
			{
				AssayNode *n;
				if (Util::StringToUpper(p.at(1)) == "DISPENSE")
				{
					claim(p.size() == 8, "Invalid number of parameters for DISPENSE.");
					n = dag->AddDispenseNode(p.at(2), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->ioPort = ports[atoi(p.at(7).c_str())];
				}
				else if (Util::StringToUpper(p.at(1)) == "MIX")
				{
					claim(p.size() == 8, "Invalid number of parameters for MIX.");
					n = dag->AddMixNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "DILUTE")
				{
					claim(p.size() == 8, "Invalid number of parameters for DILUTE.");
					n = dag->AddDiluteNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "SPLIT")
				{
					claim(p.size() == 8, "Invalid number of parameters for SPLIT.");
					n = dag->AddSplitNode(false, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "HEAT")
				{
					claim(p.size() == 7, "Invalid number of parameters for HEAT.");
					n = dag->AddHeatNode(atof(p.at(2).c_str()), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(6).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "COOL")
				{
					claim(p.size() == 7, "Invalid number of parameters for COOL.");
					n = dag->AddCoolNode(atof(p.at(2).c_str()), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(6).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "DETECT")
				{
					claim(p.size() == 8, "Invalid number of parameters for DETECT.");
					n = dag->AddDetectNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "OUTPUT")
				{
					claim(p.size() == 7, "Invalid number of parameters for OUTPUT.");
					n = dag->AddOutputNode(p.at(2), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->ioPort = ports[atoi(p.at(6).c_str())];
				}
				else if (Util::StringToUpper(p.at(1)) == "STORAGE")
				{
					claim(p.size() == 5, "Invalid number of parameters for STORAGE.");
					n = dag->AddStorageNode();
					n->startTimeStep = strtoul(p.at(2).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(4).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (Util::StringToUpper(p.at(1)) == "TRANSFER_IN")
				{
					claim(p.size() == 5, "Invalid number of parameters for TRANSFER_IN.");
					n = dag->AddTransInNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
				}
				else if (Util::StringToUpper(p.at(1)) == "TRANSFER_OUT")
				{
					claim(p.size() == 5, "Invalid number of parameters for TRANSFER_OUT.");
					n = dag->AddTransOutNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
				}
				else // GENERAL
				{
					claim(p.size() == 6, "Invalid number of parameters for GENERAL.");
					n = dag->AddGeneralNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(5).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				n->id = atoi(p.at(0).c_str());
				n->status = BOUND;
				nodes[n->id] = n;

				if (n->id > highestNodeId)
					highestNodeId = n->id;
			}
			else if (tag == "EDGE")
			{
				claim(p.size() == 2, "Invalid number of parameters for an EDGE.");
				edges.push_back(atoi(p.at(0).c_str()));
				edges.push_back(atoi(p.at(1).c_str()));
			}
			else if (tag == "PINMAP")
			{
				arch->getPinMapper()->initPinMapping();
				claim(p.size() == arch->numCellsX*arch->numCellsY, "PinMap should be the same size of the array.");
				int i = 0;
				for (int y = 0; y < arch->numCellsY; y++)
					for (int x = 0; x < arch->numCellsX; x++)
						arch->pinMapper->getPinMapping()->at(x)->at(y) = atoi(p.at(i++).c_str());
			}
			else if (tag == "SPECIALPINS")
			{
				for (unsigned i = 0; i < p.size(); i++)
					arch->pinMapper->getSpecialPurposePins()->push_back(atoi(p.at(i).c_str()));
				arch->getPinMapper()->unflattenSpecialPurposePins();
			}
			else if (tag == "RESOURCECOUNT")
			{
				for (unsigned i = 0; i < p.size(); i++)
					arch->pinMapper->getAvailResCount()->at(i) = atoi(p.at(i).c_str());
			}
			else if (tag == "RESOURCELOCATION")
			{
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->back()->setTileNum(atoi(p.at(5).c_str()));
			}
			else if (tag == "SCHEDTYPE")
				router->setPastSchedType(FileIn::GetTypeFromSchedKey(p.at(0)));
			else if (tag == "RESOURCEALLOCATIONTYPE")
				arch->getPinMapper()->setResAllocType(FileIn::GetTypeFromResourceAllocationKey(p.at(0)));
			else if (tag == "PINMAPTYPE")
			{
				PinMapType pmt = FileIn::GetTypeFromPinMapKey(p.at(0));
				Synthesis::setPinMappingMethod(pmt, INHERIT_RA, arch);
				arch->getPinMapper()->setType(pmt);
				arch->getPinMapper()->initPinMapping();
			}
			else if (tag == "PLACERTYPE")
				router->setPastPlacerType(FileIn::GetTypeFromPlaceKey(p.at(0)));
			else if (tag == "HCELLSBETWEENMODIR")
				router->setHCellsBetweenModIR(atoi(p.at(0).c_str()));
			else if (tag == "VCELLSBETWEENMODIR")
				router->setVCellsBetweenModIR(atoi(p.at(0).c_str()));
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Make sure node id's created are higher than anything seen to guarantee uniqueness
	if (highestNodeId + 1 > AssayNode::next_id)
		AssayNode::next_id = highestNodeId+1;

	// Recreate Edges
	while (!edges.empty())
	{
		dag->ParentChild(nodes[edges[0]], nodes[edges[1]]);
		//dag->ParentChild(dag->getAllNodes().at(edges.at(0)-1), dag->getAllNodes().at(edges.at(1)-1));
		edges.erase(edges.begin());
		edges.erase(edges.begin());
	}

	// Now add IOPorts and Reconfigurable Modules to the synthesis members
	map<int, IoPort*>::iterator pIt = ports.begin();
	for (; pIt != ports.end(); pIt++)
		arch->ioPorts->push_back(pIt->second);

	map<int, ReconfigModule*>::iterator rIt = rMods.begin();
	for (; rIt != rMods.end(); rIt++)
		rModules->push_back(rIt->second);

	is.close();
}

///////////////////////////////////////////////////////////////
// Reads the routed file to gather info about the pin-mapping
// (which is potentially in multiple interface files) and the
// pin-activations, which is only in the routed file.
///////////////////////////////////////////////////////////////
void FileIn::ReadRoutedSimPinsAndArchFromFile(DmfbArch *arch, vector<vector<int> *> *pinActivations, string fileName)
{
	map<int, IoPort*> ports;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read Routed DAG/Arch file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		line = GetLine(&is);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = Util::StringToUpper(tag);

			// Recreate Nodes and save edges for later
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
				arch->getPinMapper()->initPinMapping();
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNALDETECTOR")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				arch->externalResources->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
			}
			else if (tag == "EXTERNALHEATER")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				arch->externalResources->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 5, "Invalid number of parameters for an INPUT.");
				Direction dir = DIR_UNINIT;
				if (Util::StringToUpper(p.at(1)) == "TOP")
					dir = NORTH;
				else if (Util::StringToUpper(p.at(1)) == "BOTTOM")
					dir = SOUTH;
				else if (Util::StringToUpper(p.at(1)) == "RIGHT")
					dir = EAST;
				else if (Util::StringToUpper(p.at(1)) == "LEFT")
					dir = WEST;
				else
					claim(false, "Invalid side for an INPUT.");

				bool wash = false;
				if (Util::StringToUpper(p.at(4)) == "TRUE")
					wash = true;
				else if (Util::StringToUpper(p.at(4)) == "FALSE")
					wash = false;
				else
					claim(false, "Invalid washing mode; should be 'True' or 'False'.");

				IoPort *iop = new IoPort(true, dir, atoi(p.at(2).c_str()), 0, p.at(3), wash);
				//iop->setPinNo(atoi(p.at(0).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 5, "Invalid number of parameters for an OUTPUT.");
				Direction dir = DIR_UNINIT;
				if (Util::StringToUpper(p.at(1)) == "TOP")
					dir = NORTH;
				else if (Util::StringToUpper(p.at(1)) == "BOTTOM")
					dir = SOUTH;
				else if (Util::StringToUpper(p.at(1)) == "RIGHT")
					dir = EAST;
				else if (Util::StringToUpper(p.at(1)) == "LEFT")
					dir = WEST;
				else
					claim(false, "Invalid side for an OUTPUT.");

				bool wash = false;
				if (Util::StringToUpper(p.at(4)) == "TRUE")
					wash = true;
				else if (Util::StringToUpper(p.at(4)) == "FALSE")
					wash = false;
				else
					claim(false, "Invalid washing mode; should be 'True' or 'False'.");

				IoPort *iop = new IoPort(false, dir, atoi(p.at(2).c_str()), 0, p.at(3), wash);
				//iop->setPinNo(atoi(p.at(4).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "PINMAP")
			{
				arch->getPinMapper()->initPinMapping();
				claim(p.size() == arch->numCellsX*arch->numCellsY, "PinMap should be the same size of the array.");
				int i = 0;
				for (int y = 0; y < arch->numCellsY; y++)
					for (int x = 0; x < arch->numCellsX; x++)
						arch->pinMapper->getPinMapping()->at(x)->at(y) = atoi(p.at(i++).c_str());
			}
			else if (tag == "SPECIALPINS")
			{
				for (unsigned i = 0; i < p.size(); i++)
					arch->pinMapper->getSpecialPurposePins()->push_back(atoi(p.at(i).c_str()));
				arch->getPinMapper()->unflattenSpecialPurposePins();
			}
			else if (tag == "RESOURCECOUNT")
			{
				for (unsigned i = 0; i < p.size(); i++)
					arch->pinMapper->getAvailResCount()->at(i) = atoi(p.at(i).c_str());
			}
			else if (tag == "RESOURCELOCATION")
			{
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->back()->setTileNum(atoi(p.at(5).c_str()));
			}
			else if (tag == "ACTIVEPINS")
			{
				vector<int> *activePins = new vector<int>();
				for (unsigned i = 0; i < p.size(); i++)
					activePins->push_back(atoi(p.at(i).c_str()));
				pinActivations->push_back(activePins);
			}
			else if (tag == "PINMAPTYPE")
			{
				PinMapType pmt = FileIn::GetTypeFromPinMapKey(p.at(0));
				Synthesis::setPinMappingMethod(pmt, INHERIT_RA, arch);
				arch->getPinMapper()->setType(pmt);
				arch->getPinMapper()->initPinMapping();
			}
			else
			{
				//stringstream ss;
				//ss << "Invalid tag type: " << tag << endl;
				//claim(false, &ss);
			}
		}
	}


	// Now add IOPorts to the architecture
	map<int, IoPort*>::iterator pIt = ports.begin();
	for (; pIt != ports.end(); pIt++)
		arch->ioPorts->push_back(pIt->second);

	is.close();
}

////////////////////////////////////////////////////////////////////////////
// Gets the line using the standard getline function, but also trims carriage
// return characters for Linux compatability
////////////////////////////////////////////////////////////////////////////
string FileIn::GetLine(ifstream *ifs)
{
	string line;
	getline(*ifs, line);

	if (!line.empty())
	    if (isspace(line.at(line.size()-1)))
	        line = line.substr(0,line.size()-1);

	return line;
}

////////////////////////////////////////////////////////////////////////////
// Given a line, parses and finds the tag and parameters and passes
// back by reference.
////////////////////////////////////////////////////////////////////////////
void FileIn::ParseLine(string line, string *tag, vector<string> *parameters)
{
	string params;
	size_t begin = line.find("(");
	size_t end = line.find(")");
	*tag = Util::TrimString(line.substr(0, begin));
	params = line.substr(begin+1,end-begin-1);
	TokenizeString(params, ",", parameters);
}

////////////////////////////////////////////////////////////////////////////
// Divide str into small substrings, based on token, and place in dest.
////////////////////////////////////////////////////////////////////////////
void FileIn::TokenizeString(string str, string token, vector<string> *dest)
{
	size_t begin = 0;
	size_t end = str.find(token);
	string next;
	string remaining = str;

	while (end != string::npos)
	{

		next = remaining.substr(begin, end-begin);
		remaining = remaining.substr(end+1);
		dest->push_back(Util::TrimString(next));
		next.clear();
		end = remaining.find(token);
	}
	dest->push_back(Util::TrimString(remaining));
}

////////////////////////////////////////////////////////////////////////////
// Returns the operation-type given a string.
////////////////////////////////////////////////////////////////////////////
OperationType FileIn::GetOpTypeFromString(string ot)
{
	ot = Util::StringToUpper(Util::TrimString(ot));
	if (ot == "MIX")
		return MIX;
	else if (ot == "DILUTE")
		return DILUTE;
	else if (ot == "SPLIT")
		return SPLIT;
	else if (ot == "HEAT")
		return HEAT;
	else if (ot == "COOL")
		return COOL;
	else if (ot == "DETECT")
		return DETECT;
	else if (ot == "STORAGE")
		return STORAGE;
	else if (ot == "STORAGE_HOLDER")
		return STORAGE_HOLDER;
	else if (ot == "TRANSFER_IN")
		return TRANSFER_IN;
	else if (ot == "TRANSFER_OUT")
		return TRANSFER_OUT;
	else if (ot == "GENERAL")
		return GENERAL;
	else
	{
		claim(false, "Invalid operation type.");
		return GENERAL; // Unreachable, Eliminate Warning
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Given a key, the following "GetTypeFrom...Key" functions search the
// appropriate library to find and return the requested type
//////////////////////////////////////////////////////////////////////////////
SchedulerType FileIn::GetTypeFromSchedKey(string sKey)
{
	CmdLine c;
	return c.sEnums.at(c.GetLibIndexFromSchedKey(sKey));
}
PlacerType FileIn::GetTypeFromPlaceKey(string pKey)
{
	CmdLine c;
	return c.pEnums.at(c.GetLibIndexFromPlaceKey(pKey));
}
RouterType FileIn::GetTypeFromRouteKey(string rKey)
{
	CmdLine c;
	return c.rEnums.at(c.GetLibIndexFromRouteKey(rKey));
}
ResourceAllocationType FileIn::GetTypeFromResourceAllocationKey(string raKey)
{
	CmdLine c;
	return c.raEnums.at(c.GetLibIndexFromResourceAllocationKey(raKey));
}
PinMapType FileIn::GetTypeFromPinMapKey(string pmKey)
{
	CmdLine c;
	return c.pmEnums.at(c.GetLibIndexFromPinMapKey(pmKey));
}
WireRouteType FileIn::GetTypeFromWrKey(string wrKey)
{
	CmdLine c;
	return c.wrEnums.at(c.GetLibIndexFromWrKey(wrKey));
}
CompactionType FileIn::GetTypeFromCompKey(string cKey)
{
	CmdLine c;
	return c.cEnums.at(c.GetLibIndexFromCompKey(cKey));
}
ProcessEngineType FileIn::GetTypeFromPeKey(string peKey)
{
	CmdLine c;
	return c.peEnums.at(c.GetLibIndexFromPeKey(peKey));
}
ExecutionType FileIn::GetTypeFromEtKey(string etKey)
{
	CmdLine c;
	return c.etEnums.at(c.GetLibIndexFromEtKey(etKey));
}
