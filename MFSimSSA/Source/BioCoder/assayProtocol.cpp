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
 * Source: assayProtocol.cpp													*
 * Original Code Author(s): Chris Curtis										*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: Originally taken and heavily modified from BioCoder:				*
 * http://research.microsoft.com/en-us/um/india/projects/biocoder/				*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/BioCoder/assayProtocol.h"

#include <fstream>
#include <string>

assayProtocol::assayProtocol(string FileName)
{
	dagDropRestrict = 100000; // The value of 13 causes a problem when 32+ outputs for protein
	stepNum=0;
	completeMixNum=0;
	nodes= new tNameBank(FileName);
	steps= new assayStep[100000];
	completeMixList= new aMixture[10000];
	name = FileName;
}

assayProtocol::~assayProtocol()
{
	delete nodes;
	delete completeMixList;
	delete steps;
}
int assayProtocol :: gcd( int a, int b)
{
	if(b==0)
		return a;
	else
		return gcd(b, a%b);
}

bool assayProtocol:: isInt(float f)
{
	int i =f;
	return(f==static_cast<float>(i));
}

void assayProtocol:: deciRatio2IntRatio(float &f,float &f2)
{
	int countf=0;
	int countf2=0;
	while(!isInt(f))
	{
		countf++;
		f*=10;
	}
	while(!isInt(f2))
	{
		countf2++;
		f2*=10;
	}
	if(countf > countf2)
		for(int i=0;i<countf-countf2;++i)
			f2*=10;
	else
		for(int i=0;i<countf2-countf;++i)
			f*=10;
}
bool assayProtocol:: ckSplit(int target ,int total, list<int> & splits,int dropletRestrict)
{
	int numDrop=dropletRestrict;
	/*int gcdTemp=gcd(total,target);
	if(gcdTemp>1){
		target/=gcdTemp;
		total/=gcdTemp;
	}*/ //MOVE THIS TO OUTSIDE ATTEMPT3 INTO SPLIT MANAGEMENT!!!!!!
	if(total<=13){
		splits.push_back(total);
		return true;
	}
	for (int i=numDrop;i>=2;--i){
		if(total%i==0&& (total/i)<=numDrop){
			splits.push_back(i);
			splits.push_back(total/i);
			return true;
		}
		else if(total%i==0){
			//there needs to be some check on target to find out how much is needed.
			splits.push_back(i);
			if(ckSplit(target,total/i,splits,dropletRestrict))
				return true;
			else
				splits.remove(total/i);
		}
	}
	return false;
}
int assayProtocol:: numPiecesUsed(int target, int pieces, int value)
{
	for (int i=1; i<=pieces;++i)
		if(target< i*value)
			return i-1;
	return pieces;
}
void assayProtocol:: splitCalls(Container & src,Container & dest, map<int,pair<int,string> > splitInst, map <int , string> mixDest, bool ensureMeasurement)
{
	string targetLinkName,tempLinkName;

	Container temp;
	Container target;
	map<int,string>::iterator mixDestIt;
	map<int,pair<int,string> >::iterator it=splitInst.begin();
	string linkName=assaySplit(it->second.first, ensureMeasurement);
	addToLinkMap(src,linkName);
	++it;
	for(;it!= splitInst.end();++it)
	{

		if(it->second.second=="s"){
			string linkName=assaySplit(it->second.first, ensureMeasurement);
			tempLinkName=linkName;
			addToLinkMap(temp,linkName);
		}
		if(it->second.second=="m"){
			mixDestIt=mixDest.find(it->first);
			if(mixDestIt!=mixDest.end())
			{

				if(mixDestIt->second=="dest"){
					for (int i=0;i<it->second.first;++i){
						target.tLinkName.push_back(linkName);
					}
					assayMixManagement(target,aMixture(),"mix");
				}
				else {
					for (int i=0;i<it->second.first-1;++i){
						src.tLinkName.push_back(linkName);
					}
					assayMixManagement(target,aMixture(),"mix");
					temp.tLinkName.push_back(tempLinkName);
				}
			}
		}
	}
	assayPreMix(target,dest);
	assayMixManagement(dest,aMixture(),"mix");
	//	linkName = assayMix(aMixture(),"mix");
	//	addToLinkMap(dest,linkName);
	temp.tLinkName.pop_front();
	assayPreMix(temp,src);
	assayMixManagement(src,aMixture(),"mix");
	if(conLookUp.find(target.name)!= conLookUp.end())
		conLookUp.erase(conLookUp.find(target.name));
	if(conLookUp.find(temp.name)!= conLookUp.end())
		conLookUp.erase(conLookUp.find(temp.name));
}
void assayProtocol:: splitManagment(Container & src,Container & dest,Volume targetVolume, bool ensureMeasurement)
{
	map<int,string> mixDestinations;
	map<int, pair<int,string> > splitInstructions;
	pair<int,pair<int,string> > map;
	pair<int ,string> helper;
	int stepNo=0;
	int dropCount=getDropCount();
	list<int> splitList;
	int value;
	int pieces;
	int largest=0;
	float totalVol = src.volume;
	float targetVol= targetVolume.conv2UL();
	deciRatio2IntRatio(totalVol,targetVol);
	int total=totalVol;
	int target=targetVol;
	int tot=totalVol;
	int tar=targetVol;
	int gcdTemp=gcd(total,target);
	if(gcdTemp>1){
		target/=gcdTemp;
		total/=gcdTemp;
	}
	if(! ckSplit(target,total,splitList,dagDropRestrict-dropCount))
	{
		cerr <<"Ratio of "<< target<< " / "<< total<< " is not possible to"
				<<" obtain, exiting program"<<endl;
		exit(-1);
	}
	//if true then splitList will have the necessary splits for the protocol.
	int piecesLeft;
	int debug = 0; //dtg debug
	while(!splitList.empty())
	{
		pieces=splitList.front();
		splitList.pop_front();

		if(largest<pieces)
			largest=pieces;
		dropCount+=(pieces-1);
		if (dropCount > dagDropRestrict)
		{
			total=totalVol/gcdTemp;
			target=targetVol/gcdTemp;
			int tot=totalVol;
			int tar=targetVol;
			splitList.clear();
			mixDestinations.clear();
			splitInstructions.clear();
			stepNo=0;
			if(! ckSplit(target,total,splitList,largest-1))
			{
				cerr <<"Ratio of "<< target<< " / "<< total<< " is not possible to"
						<<" obtain given number of drops, exiting program"<<endl;
				exit(-1);
			}
		}
		else{

			value=tot/pieces;
			int numUsed= numPiecesUsed(tar,pieces,value);

			helper.first=pieces;//this group splits the total into parts
			helper.second="s";
			map.first=stepNo++;
			map.second=helper;
			splitInstructions.insert(map);

			helper.first=stepNo;		//this group gives a tag for the mix for the destination
			helper.second="dest";
			mixDestinations.insert(helper);

			helper.first=numUsed;		// this is where the information is gathered for the mix
			helper.second="m";
			map.first=stepNo++;
			map.second=helper;
			splitInstructions.insert(map);
			tar-=numUsed*value;
			dropCount-=(numUsed-1);

			helper.first=stepNo;
			helper.second="src";
			mixDestinations.insert(helper);

			if(tar!=0){
				helper.first=pieces-(numUsed+1);
				helper.second="m";
				map.first=stepNo++;
				map.second=helper;
				splitInstructions.insert(map);

				dropCount-=(pieces- numUsed+1);

				total=value;
			}
			else{
				helper.first=pieces-(numUsed);
				helper.second="m";
				map.first=stepNo++;
				map.second=helper;
				splitInstructions.insert(map);

				dropCount-=(pieces- numUsed+1);

				tot=value;

			}
		}
	}
	//now that the split calls can be done correctly this will call all mixes and splits
	splitCalls(src, dest, splitInstructions,mixDestinations, ensureMeasurement);
}


void assayProtocol :: printassay(ostream & out)
{
	for (int i=0;i<=stepNum;++i){
		out<<"step " << i << ":"<<endl;
		if(steps[i].dispence){
			out<<"dispense:\n";
			for(map<string,aDispense>::iterator it= steps[i].dispList.begin();
					it!= steps[i].dispList.end();++it){
				out<<it->second.flu.new_name<<" ";
				out<<it->second.vol.value<<", ";
				switch(it->second.vol.unit_choice){
				case 0:out<< "uL "; break;
				case 1:out<< "mL "; break;
				case 2:out<< "L ";  break;
				}
				out<< " ("<< it->first<<")"<<endl;

			}
		}
		if(steps[i].mix){
			for( map<string,aMix>::iterator it= steps[i].mixList.begin();
					it !=steps[i].mixList.end();++it)
			{
				out<< "mix: (" << it->first<< ") ";
				out<< "total volume :"<< it->second.component.getTotalVol().value;
				for (int j=0 ; j < it->second.component.listNum ; j++)
				{
					out<<endl<<"	"<< it->second.component.list[j].original_name;
					out<< " "<<it->second.component.list[j].volume << ", ";
					switch(it->second.component.list[j].unit){
					case 0:out<< "uL "; break;
					case 1:out<< "mL "; break;
					case 2:out<< "L ";  break;
					}
				}
				out<<endl<<it->second.typeofmix<<endl;
			}
		}
		if(steps[i].heat){
			out<< "heat: "<<endl;
			for( map<string,aHeat>::iterator it= steps[i].heatList.begin();
					it !=steps[i].heatList.end();++it)
			{
				out << it->second.fluids.name << " for ";
				out << it->second.time.value<< " ";
				switch(it->second.time.unit_choice){
				case 0: out<<"S"; break;
				case 1: out<<"Min(s)"; break;
				case 2: out<< "Hr(s)"; break;
				}
				out << " at " << it->second.temp <<" degrees C";
				out<< " ("<< it->first<<")"<<endl;
			}
		}
		if(steps[i].store){
			out<< "Store: "<<endl;
			for( map<string,aStore>::iterator it= steps[i].storeList.begin();
					it !=steps[i].storeList.end();++it)
			{
				out << it->second.fluids.name << " for ";
				out << it->second.time.value<< " ";
				switch(it->second.time.unit_choice){
				case 0: out<<"S"; break;
				case 1: out<<"Min(s)"; break;
				case 2: out<< "Hr(s)"; break;
				}
				out << " at " << it->second.temp <<" degrees C";
				out<< " ("<< it->first<<")"<<endl;
			}
		}
		if(steps[i].detect){
			out<< "Detect: " <<endl;
			for(map<string,aDetect>::iterator it= steps[i].detectList.begin();
					it != steps[i].detectList.end();++it)
			{
				out<<it->second.detectType<< "for ";
				out << it->second.time.value<< " ";
				switch(it->second.time.unit_choice){
				case 0: out<<"S"; break;
				case 1: out<<"Min(s)"; break;
				case 2: out<< "Hr(s)"; break;
				}
			}
		}
		if(steps[i].output){
			out<< "output: "<<endl;
			for(map<string,aOutput>::iterator it= steps[i].outputList.begin();
					it != steps[i].outputList.end();++it)
				out<<"Sink Name: "<< it->second.sinkName<<endl;
		}
	}
	out<<endl;
}
void assayProtocol:: printLinks(ostream & out)
{
	for( map<string,list <string> > :: iterator it =  links.begin();
			it != links.end() ; ++it)
	{
		out << "child: "<<it->first<<endl << "Parent(s): \n";

		for (list <string>::iterator j = it->second.begin(); j != it->second.end(); ++j)
			out << *j << endl;
		out <<endl;
	}

}


//void assayProtocol :: printMixtures(ostream & out)
//{
//	for (int i =0; i<completeMixNum;++i){
//		out<<"Name: "<<completeMixList[i].name<<endl;
//		out<<"Cont Name: "<<completeMixList[i].container<<endl;
//		out<<"Total Volume: (UL)"<<completeMixList[i].getTotalVol().conv2UL()<<endl;
//		for(int j=0;j<completeMixList[i].listNum;++j){
//			out<<"	"<<completeMixList[i].list[j].original_name;
//			out<<"	";
//			Volume vol(completeMixList[i].list[j].volume,completeMixList[i].list[j].unit);
//			out<<vol.conv2UL()<<endl;
//		}
//	}
//}

void assayProtocol :: addToInitInputList(aDispense & dis)
{
	inputList.push_back(dis);
}

void assayProtocol :: addToInitOutputList(aOutput & out)
{
	outputList.push_back(out);
}

string assayProtocol:: assayDispense(Fluid fluid1,Volume volume1)
{
	string linkName=nodes->accessName(DIS);//index zero is the location of
	nodes->incNodeName(DIS);				  //the dispence link name

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	aDispense dis(fluid1,Volume(volume1));
	addToInitInputList(dis);

	pair<string,int> b(linkName,stepNum);
	nodeLookUp.insert(b);

	pair<string,aDispense> a (linkName, dis);
	steps[stepNum].dispence=true;
	steps[stepNum].dispList.insert(a);

	return linkName;
}
void assayProtocol :: assayPreMix(Container & Src, Container & Dest)
{
	addToMixtureList(Src, Dest);
	for(list <string>::iterator srcPtr =Src.tLinkName.begin(); srcPtr != Src.tLinkName.end(); ++srcPtr)
		Dest.tLinkName.push_back(*srcPtr);
	Src.tLinkName.clear();

}

//void assayProtocol::addToCondtionalMaps(BioConditionalGroup *bcg)
//{
//	for (int i = 0; i < bcg->getConditions()->size(); i++)
//	{
//		BioCondition *bCond = bcg->getConditions()->at(i);
//		if (bCond->statement->operandType == OP_ONE_SENSOR)
//		{
//			//			BioCondition *bCond = bcg->getConditions()->at(i);
//			pair<string,BioConditionalGroup*> condtion(bCond->statement->sensor1, bcg);
//			conditions.insert(condtion);
//			pair<string, BioCoder*> link(bCond->statement->sensor1, bCond->branchIfTrue);
//			cfgLink.insert(link);
//		}
//		else if (bCond->statement->operandType == OP_TWO_SENSORS)
//		{
//			//			BioCondition *bCond = bcg->getConditions()->at(i);
//			pair<string,BioConditionalGroup*> condtion(bCond->statement->sensor2, bcg);
//			conditions.insert(condtion);
//			pair<string, BioCoder*> link(bCond->statement->sensor2, bCond->branchIfTrue);
//			cfgLink.insert(link);
//		}
//		else if(bCond->statement->operationType == OP_UNCOND)
//		{
//			pair<string,BioConditionalGroup*> condtion("UNCOND", bcg);
//			conditions.insert(condtion);
//			pair<string, BioCoder*> link("UNCOND", bCond->branchIfTrue);
//			cfgLink.insert(link);
//		}
//	}
//
//
//
//
//}

string assayProtocol :: assayDetect(string typeDetect,Time time)
{
	steps[stepNum].detect=true;
	string linkName = nodes->accessName(DCT);
	nodes->incNodeName(DCT);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	/*if(e!=NULL){
		if(!bcg)
			bcg=new BioConditionalGroup();
		//if(e->operationType!=OP_UNCOND)
		if(e->operandType==OP_TWO_SENSORS)
			e->setSensor2(linkName);
		else
			e->setSensor1(linkName);
		bcg->addNewCondition(e,BioPtr);
		//pair<string,BioConditionalGroup*> condtion(linkName, bcg);
		//conditions.insert(condtion);
		//pair<string, BioCoder*> link(linkName, BioPtr);
		//cfgLink.insert(link);
	}*/
	pair<string ,int> b(linkName,stepNum);
	nodeLookUp.insert(b);

	aDetect detect(typeDetect, time);
	pair<string, aDetect> a (linkName, detect);
	steps[stepNum].detectList.insert(a);

	return linkName;
}
string assayProtocol :: assayStore(Container & con, float temp1, Time time1)
{
	steps[stepNum].store=true;
	string linkName=nodes->accessName(STR); //index 6 holds the place
	nodes->incNodeName(STR);				   //of store link name

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string,int> b(linkName,stepNum);
	nodeLookUp.insert(b);

	aStore str(time1,temp1,findMix(con));
	pair<string, aStore> a(linkName,str);
	steps[stepNum].storeList.insert(a);

	return linkName;
}
string assayProtocol :: assayHeat(Container &con, float temp1, Time time1, bool isCooling)
{
	if (isCooling)
	{
		steps[stepNum].cool=true;
		steps[stepNum].heat=false;
	}
	else
	{
		steps[stepNum].heat=true;
		steps[stepNum].cool=false;
	}

	string linkName=nodes->accessName(HT); //index 2 holds the place
	nodes->incNodeName(HT);				   //of heat link name

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string,int> b(linkName,stepNum);
	nodeLookUp.insert(b);

	if (isCooling)
	{
		aCool str(time1,temp1,findMix(con));
		pair<string, aCool> a(linkName,str);
		steps[stepNum].coolList.insert(a);
	}
	else
	{
		aHeat str(time1,temp1,findMix(con));
		pair<string, aHeat> a(linkName,str);
		steps[stepNum].heatList.insert(a);
	}

	return linkName;
}

string assayProtocol :: assayMix(aMixture mixture,string typeofmix)
{
	steps[stepNum].mix=true;
	string linkName=nodes->accessName(MX);
	nodes->incNodeName(MX);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string,int> b(linkName,stepNum);
	nodeLookUp.insert(b);

	aMix mix(mixture, typeofmix);
	pair<string,aMix> a ( linkName,mix);
	steps[stepNum].mixList.insert(a);

	return linkName;
}
void assayProtocol:: assayMixManagement(Container & con, aMixture mixture, string typeofmix)
{
	if(con.tLinkName.size() > 1){
		string linkName=assayMix(mixture,typeofmix);
		addToLinkMap(con,linkName);
	}
}
string assayProtocol :: assayOutput(string sinkName)
{
	steps[stepNum].output=true;
	string linkName= nodes->accessName(OT);
	nodes->incNodeName(OT);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string, int> b(linkName, stepNum);
	nodeLookUp.insert(b);

	aOutput out(sinkName, linkName);
	addToInitOutputList(out);

	pair<string, aOutput> a(linkName, out);
	steps[stepNum].outputList.insert(a);

	return linkName;
}
string assayProtocol :: assayWasteOutput(string sinkName)
{
	steps[stepNum].output=true;
	string linkName= nodes->accessName(WST);
	nodes->incNodeName(WST);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string, int> b(linkName, stepNum);
	nodeLookUp.insert(b);

	aOutput out(sinkName, linkName);
	addToInitOutputList(out);

	pair<string, aOutput> a(linkName, out);
	steps[stepNum].outputList.insert(a);

	return linkName;
}
string assayProtocol:: assaySplit(int n, bool ensureMeasurement)
{
	steps[stepNum].split=true;
	string linkName=nodes->accessName(SPLT);
	nodes->incNodeName(SPLT);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string, int> b (linkName, stepNum);
	nodeLookUp.insert(b);

	aSplit split(n);
	split.ensureMeasurement = ensureMeasurement;
	pair<string, aSplit>a(linkName,split);
	steps[stepNum].splitList.insert(a);

	return linkName;

}
string assayProtocol:: assayTransIn()
{
	steps[stepNum].transIn=true;
	string linkName =nodes->accessName(TrN);
	nodes->incNodeName(TrN);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string ,int> b(linkName, stepNum);
	nodeLookUp.insert(b);

	aTransIn transIn(linkName);
	pair<string,aTransIn> a(linkName, transIn);
	steps[stepNum].transInList.insert(a);

	return linkName;
}

string assayProtocol:: assayTransOut(Container &con)
{
	steps[stepNum].transOut=true;
	string linkName =nodes->accessName(TrO);
	nodes->incNodeName(TrO);

	// Quick fix by DTG to help make name unique across ENTIRE CFG!
	stringstream ss;
	ss << linkName << "_" << name;
	linkName = ss.str();

	pair<string ,int> b(linkName, stepNum);
	nodeLookUp.insert(b);

	aTransOut transOut(con);
	pair<string,aTransOut> a(linkName, transOut);
	steps[stepNum].transOutList.insert(a);

	return linkName;
}


//assayProtocol assayProtocol::mergeSteps()
//{
//	assayProtocol;
//	for (int i =0; i<stepNum;++i)
//
//}

void assayProtocol :: collectOutput()
{
	for(int i=0 ; i<completeMixNum ;++i){
		map<string, Container>::iterator it = conLookUp.find(completeMixList[i].container);
		if(it!= conLookUp.end()){
			string linkName = assayOutput(completeMixList[i].container);
			pair<string,list <string> > a (linkName , it->second.tLinkName);
			links.insert(a);
		}
	}
}
void assayProtocol :: addToLinkMap(Container & con, string linkName)
{
	if(linkName.substr(0,1)!= "M" && con.tLinkName.size()>1)
	{
		string newLinkName =nodes->accessName(MX);
		nodes->incNodeName(MX);

		// Quick fix by DTG to help make name unique across ENTIRE CFG!
		stringstream ss;
		ss << linkName << "_" << name;
		linkName = ss.str();

		addToLinkMap(con,newLinkName);
	}
	pair<string,list <string> > a (linkName , con.tLinkName);
	links.insert(a);
	con.tLinkName.clear();
	con.tLinkName.push_back(linkName);

	//this keeps track of the containers given for output list.
	//if the container doesnt exist in the list, it adds the container to the list.
	//otherwise it updates the old containter with the new contents.
	if(conLookUp.count(con.name)!=0){
		conLookUp.erase(conLookUp.find(con.name));
	}
	pair<string, Container> contIndex(con.name,con);
	conLookUp.insert(contIndex);


}
void assayProtocol :: assayDrain(Container & con, string outputSink)
{
	//empty the container
	con.volume=0;
	con.contents.volume=0;
	con.contents.original_name="";
	con.contents.new_name="";

	//assay link management
	//con.name;
	string linkName =assayWasteOutput(outputSink);
	addToLinkMap(con, linkName);
	clearMixture(con);
}

void assayProtocol :: clearMixture(Container& con)
{
	for( int i=0 ; i <completeMixNum;++i)
		if(completeMixList[i].container== con.name)
			completeMixList[i].container="";
}

aMixture assayProtocol:: findMix(Container &con)
{
	return completeMixList->find(con,completeMixNum);
}


aMixture assayProtocol:: addtoMixtureList(Container& con, Fluid& fluid)
{
	return completeMixList->addToMixList(con,fluid,completeMixNum);
}

aMixture assayProtocol:: addToMixtureList(Container& src, Container& dest)
{
	return completeMixList->addToMixList ( src,dest, completeMixNum);
}

//void assayProtocol :: initializeIO (int x, int y, VirtualLoC * LC)
//{
//	bool endFlag=false;
//	bool inputFlag= false;
//	bool outputFlag= false;
//	double dispenseTime=.1;//constant value for dispense time for the fluid of the assay.
//	map <string,list<pair<int,int> > > inputs;
//	map <string,list<pair<int,int> > > ::iterator mapIterator;
//	pair<string, list<pair<int,int> > > a;
//	list< pair<int,int> > coords;
//	pair<int, int> xy;
//
//	//this creates a map of all possible places that the initializations could go
//	//later this will probably be removed for a chip signal to tell where the inputs/outputs are
//
//	//NORTH
//	for (int i=0; i<x; ++i)
//	{
//		xy.first=i;
//		xy.second=0;
//		coords.push_back(xy);
//	}
//	a.first="NORTH";
//	a.second=coords;
//	inputs.insert(a);
//	coords.clear();
//
//	//SOUTH
//	for (int i=0; i<x; ++i)
//	{
//		xy.first=i;
//		xy.second=(y-1);
//		coords.push_back(xy);
//	}
//	a.first="SOUTH";
//	a.second=coords;
//	inputs.insert(a);
//	coords.clear();
//
//	//WEST
//	for (int i=0; i<y; ++i)
//	{
//		xy.first=0;
//		xy.second=i;
//		coords.push_back(xy);
//	}
//	a.first="WEST";
//	a.second=coords;
//	inputs.insert(a);
//	coords.clear();
//
//	//EAST
//	for (int i=0; i<y; ++i)
//	{
//		xy.first=x-1;
//		xy.second=i;
//		coords.push_back(xy);
//	}
//	a.first="EAST";
//	a.second=coords;
//	inputs.insert(a);
//
//	//PRINTS THE COORDINATES WITH THE COORESPONDING DIRECTIONS
//	//	for(map<string,list<pair<int,int> > >::iterator print= inputs.begin(); print != inputs.end();++print)
//	//	{
//	//		cout<<print->first<<": "<<endl;
//	//		for (list<pair<int,int> >::iterator j=print->second.begin(); j != print->second.end();++j)
//	//			cout<<" 	("<< j->first <<" ,"<<j->second<<")"<<endl;
//	//	}
//
//
//	while(! endFlag)
//	{
//		string Name,sinkName;
//		double vol;
//		if(! inputList.empty()){
//			inputFlag=true;
//			aDispense a(inputList.front());
//			inputList.pop_front();
//			vol= a.vol.conv2UL();
//			Name=a.flu.original_name;
//		}
//		else
//			inputFlag=false;
//
//		if(! outputList.empty()){
//			outputFlag=true;
//			aOutput b(outputList.front());
//			outputList.pop_front();
//			sinkName= b.sinkName;
//		}
//		else
//			outputFlag= false;
//
//		int xCoord,yCoord;//x,y coordinates for inputs.
//		mapIterator=inputs.begin();
//		if(mapIterator->second.empty())
//		{
//			if(mapIterator == inputs.end()){
//				cerr<<"Unable to initialize all Dispense Nodes due to lack of input sources\n";
//				exit(-1);
//			}
//			inputs.erase(mapIterator);
//			mapIterator= inputs.begin();
//		}
//		xCoord= mapIterator->second.front().first;
//		yCoord=mapIterator->second.front().second;
//		if(inputFlag){
//			if (mapIterator->first=="NORTH"){
//				LC->initializeInput(xCoord, yCoord, NORTH, Name, dispenseTime, vol);
//				cout<<"LC->initializeInput("<< xCoord<<", "<<yCoord<<", NORTH, "<<Name<<", "<<dispenseTime<<", "<<vol<<");"<<endl;
//			}
//			else if (mapIterator->first=="SOUTH")
//			{
//				LC->initializeInput(xCoord, yCoord, SOUTH, Name, dispenseTime, vol);
//				cout<<"LC->initializeInput("<< xCoord<<", "<<yCoord<<", SOUTH, "<<Name<<", "<<dispenseTime<<", "<<vol<<");"<<endl;
//			}
//			else if (mapIterator->first=="EAST"){
//				LC->initializeInput(xCoord, yCoord, EAST, Name, dispenseTime, vol);
//				cout<<"LC->initializeInput("<< xCoord<<", "<<yCoord<<", EAST, "<<Name<<", "<<dispenseTime<<", "<<vol<<");"<<endl;
//			}
//			else if (mapIterator->first=="WEST"){
//				LC->initializeInput(xCoord, yCoord, WEST, Name, dispenseTime, vol);
//				cout<<"LC->initializeInput("<< xCoord<<", "<<yCoord<<", WEST, "<<Name<<", "<<dispenseTime<<", "<<vol<<");"<<endl;
//			}
//		}
//		if(outputFlag){
//			if (mapIterator->first=="NORTH"){
//				LC->initializeOutput(xCoord,yCoord,NORTH,sinkName);
//				cout<<"LC->initializeOutput("<<xCoord<< "," <<yCoord<<" ,NORTH, "<<sinkName<<");"<<endl;
//			}
//			else if (mapIterator->first=="SOUTH"){
//				LC->initializeOutput(xCoord,yCoord,SOUTH,sinkName);
//				cout<<"LC->initializeOutput("<<xCoord<< "," <<yCoord<<" ,SOUTH, "<<sinkName<<");"<<endl;
//			}
//			else if (mapIterator->first=="EAST"){
//				LC->initializeOutput(xCoord,yCoord,EAST,sinkName);
//				cout<<"LC->initializeOutput("<<xCoord<< "," <<yCoord<<" ,EAST, "<<sinkName<<");"<<endl;
//			}
//			else if (mapIterator->first=="WEST"){
//				LC->initializeOutput(xCoord,yCoord,WEST,sinkName);
//				cout<<"LC->initializeOutput("<<xCoord<< "," <<yCoord<<" ,WEST, "<<sinkName<<");"<<endl;
//			}
//		}
//		mapIterator->second.pop_front();
//
//		if(inputList.empty()&& outputList.empty())
//			endFlag= true;
//	}
//
//}

void assayProtocol:: Translator(string filename ,DAG * dag, bool color, bool all, bool outputGraphs)
{
//	vector<BioConditionalGroup *> BCGList;

	stringstream ss;


	int step =0;
	ss << "digraph G {\n";
	ss << "  size=\"8.5,10.25\";\n";


	double splitTime = 2;
	vector<AssayNode*> assays;

	//running through the steps
	for(int i=0 ; i <= stepNum; ++i)
	{
		if(steps[i].detect){
			for( map<string,aDetect>::iterator it= steps[i].detectList.begin();
					it != steps[i].detectList.end();++it)
			{
				float time;
				if(it->second.time.value == 0)
					time=.1;//default length of time to detect
				else
					time=it->second.time.value;

				AssayNode *detect =dag->AddDetectNode(1, time, it->first);
				//cout<< "AssayNode *Disp =dag->AddDetectNode("<< time<<" ,"<< it->first <<");"<<endl;


				ss<< assays.size() <<" [label = \"Detect\"";
				if(color)
					ss<<" fillcolor= orange, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(detect);

				/*run2 collection of condtions*/
//				for(map<string,BioConditionalGroup*>::iterator ExprIterator =conditions.begin();
//						ExprIterator != conditions.end();
//						++ExprIterator)
//				{
//					//cout<<it->first<<" "<<ExprIterator->first<<endl;
//					if(it->first== ExprIterator->first)
//					{
//						ConditionalGroup * cg;
//
//
//						for(int i=0; i<ExprIterator->second->getConditions()->size();++i)/*going through the conditions*/
//						{
//							if(ExprIterator->second->getConditions()->at(i)->statement->operandType==OP_TWO_SENSORS)/*if the condition is a 2 sensor readings*/
//							{
//								//cycles through every Dag that has been Created and finds correct node.
//								for (map<BioCoder *, DAG*>::iterator dagCycle = BioDag.begin(); dagCycle != BioDag.end(); ++dagCycle)
//								{
//									for (int j = 0 ; j < dagCycle->second->getAllNodes().size(); j++)//finds sensor1
//									{
//										//cout<< dagCycle->second->getAllNodes().at(j)->GetName() <<" "<< ExprIterator->second->getConditions()->at(i)->statement->sensor1<<endl;
//										if(dagCycle->second->getAllNodes().at(j)->GetName() == ExprIterator->second->getConditions()->at(i)->statement->sensor1)/*find sensor1*/
//										{
//											ExprIterator->second->getConditions()->at(i)->statement->s1=dagCycle->second->getAllNodes().at(j);//assigns sensor1 into the expression
//											break;
//											//sensor1 = assays.at(j);
//										}
//									}
//								}
//							}
//						}
//						cg=ExprIterator->second->convt2CG(this,detect);
//						CGList.push_back(cg);
//					}
//				}
			}
		}
		if(steps[i].dispence){
			for( map<string,aDispense>::iterator it= steps[i].dispList.begin();
					it != steps[i].dispList.end(); ++it)
			{
				string fluid= it->second.flu.original_name;
				double vol= it->second.vol.conv2UL();
				AssayNode *Disp =dag->AddDispenseNode(fluid,vol ,it->first);
				//cout<< "AssayNode *Disp =dag->AddDispenseNode("<< fluid<<", "<<vol<<" ,"<< it->first <<");"<<endl;
				ss<<assays.size() <<" [label = \""<<fluid;
				if(all)
					ss<< " "<< it->second.vol.conv2UL()<<"UL \"";
				else
					ss<<"\"";
				if(color)
					ss<<" fillcolor=cyan, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Disp);
			}
		}
		if(steps[i].mix){
			for( map<string,aMix>::iterator it= steps[i].mixList.begin();
					it !=steps[i].mixList.end();++it)
			{
				int numDrops= links.find(it->first)->second.size();
				//				for(list<string>::iterator it2=links.find(it->first)->second.begin(); it2!=links.find(it->first)->second.end();++it2)
				//					cout<< *it2<<endl;
				AssayNode* Mix= dag->AddMixNode(numDrops,it->second.component.time.conv2secs(),it->first);
				//cout << "AssayNode* Mix= dag->AddMixNode( " <<numDrops << "," << it->second.component.time.conv2secs() << "," << it->first << ");"<<endl;
				ss<<assays.size() <<" [label = \"Mix\"";
				if(color)
					ss<<" fillcolor=yellow, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Mix);
			}
		}
		if(steps[i].heat){
			for( map<string,aHeat>::iterator it= steps[i].heatList.begin();
					it !=steps[i].heatList.end();++it)
			{
				double time= it->second.time.conv2secs();
				//				cout<<"TIME: ----->>>>>>>>"<<time<<endl;
				AssayNode * Heat= dag->AddHeatNode(time,it->first);
				Heat->SetTemperature(it->second.temp);
				//cout<<"AssayNode * Heat= dag->AddHeatNode("<<time<<","<<it->first<<");"<<endl;
				ss<<assays.size() <<" [label = \"Heat";
				if(all)
					ss<<" ("<<it->second.temp<< "C, "<<time<<" s)\"";
				else
					ss<<"\"";
				if(color)
					ss<<" fillcolor=lightcoral, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Heat);
			}
		}
		if(steps[i].cool){
			for( map<string,aCool>::iterator it= steps[i].coolList.begin();
					it !=steps[i].coolList.end();++it)
			{
				double time= it->second.time.conv2secs();
				//				cout<<"TIME: ----->>>>>>>>"<<time<<endl;
				AssayNode * Cool= dag->AddCoolNode(time,it->first);
				Cool->SetTemperature(it->second.temp);
				//cout<<"AssayNode * Heat= dag->AddHeatNode("<<time<<","<<it->first<<");"<<endl;
				ss<<assays.size() <<" [label = \"Cool";
				if(all)
					ss<<" ("<<it->second.temp<< "C, "<<time<<" s)\"";
				else
					ss<<"\"";
				if(color)
					ss<<" fillcolor=lightskyblue1, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Cool);
			}
		}
		if(steps[i].split){
			for(map<string,aSplit>::iterator it = steps[i].splitList.begin();
					it!= steps[i].splitList.end();++it)
			{

				AssayNode * Split = dag->AddSplitNode(it->second.ensureMeasurement, it->second.numSplit,splitTime,it->first);
				//cout<< "AssayNode * Split = dag->AddSplitNode("<<it->second.numSplit<<","<<splitTime<<","<<it->first<<")"<<endl;

				ss<<assays.size() <<" [label = \"Split\"";
				if(color)
					ss<<" fillcolor=seagreen1, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Split);
			}
		}
		if(steps[i].store){
			//			cout<< "Store: "<<endl;
			for( map<string,aStore>::iterator it= steps[i].storeList.begin();
					it !=steps[i].storeList.end();++it)
			{
				AssayNode* Store= dag->AddStorageNode(it->first);
				//cout<<"AssayNode* Store= dag->AddStorageNode("<< it->first <<");"<<endl;
				ss<<assays.size() <<" [label = \"Store\"";
				if(color)
					ss<<" fillcolor=burlywood, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Store);
			}
		}
		if(steps[i].output){
			//			cout<< "Output: "<<endl;
			for( map<string,aOutput> :: iterator it= steps[i].outputList.begin();
					it !=steps[i].outputList.end();++it)
			{
				aOutput foo = it->second;

				AssayNode* Output= dag->AddOutputNode(it->second.sinkName, it->first);
				//cout<<"AssayNode* Output= dag->AddOutputNode("<< it->second.sinkName<<", "<< it->first<<");"<<endl;
				ss<<assays.size() <<" [label = \"" << it->second.sinkName << "\"";
				if(color)
					ss<<" fillcolor=orchid1, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(Output);
			}
		}
		if(steps[i].transOut){
			for( map<string,aTransOut> :: iterator it= steps[i].transOutList.begin();
					it !=steps[i].transOutList.end();++it)
			{
//				aTransOut foo = it->second;

				AssayNode* tOut= dag->AddTransOutNode(it->first);

				ss<<assays.size() <<" [label = \"TransferOut\"";
				if(color)
					ss<<" fillcolor=orchid3, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(tOut);
			}
		}
		if(steps[i].transIn){
			for( map<string,aTransIn> :: iterator it= steps[i].transInList.begin();
					it !=steps[i].transInList.end();++it)
			{
//				aTransIn foo = it->second;

				AssayNode* tIn= dag->AddTransInNode(it->first);

				ss<<assays.size() <<" [label = \"TransferIn\"";
				if(color)
					ss<<" fillcolor=cyan3, style=filled];\n";
				else
					ss<<"];\n";
				assays.push_back(tIn);
			}
		}
	}
//	map<string,BioConditionalGroup*>::iterator ExprIterator =conditions.find("UNCOND");
//	if(ExprIterator!=conditions.end())
//	{
////		if(ExprIterator->first == "UNCOND"){
//			ConditionalGroup * cg;
//			cg=ExprIterator->second->convt2CG(this,assays.at(0));
//			CGList.push_back(cg);
////		}
//	}


	int graphNum=-1;
	int c;
	//Establising the links between the nodes
	for(map<string,list<string> >::iterator it= links.begin();
			it != links.end(); ++it)
	{
		AssayNode *child;
		for (int i = 0 ; i < assays.size(); i++)
		{
			if(assays.at(i)->GetName() == it->first)
			{
				c = i;
				child = assays.at(i);
			}
		}

		for (list<string> :: iterator j=it->second.begin(); j != it->second.end(); ++j)
		{
			AssayNode *parent;
			for (int i = 0 ; i < assays.size(); i++)
			{
				if(assays.at(i)->GetName() == *j)
				{
					graphNum=i;
					parent = assays.at(i);
				}
			}
			dag->ParentChild(parent,child);
			ss << graphNum <<" -> "<< c <<";\n";
			//cout<<"dag->ParentChild(parent,child);"<<endl;
			//cout<< "parent: "<< parent->GetName()<<endl;
			//cout<< "child: "<< child ->GetName()<<endl;

		}
	}

	ss<<"}\n";

	if (outputGraphs)
	{
		ofstream out;
		string temp = "Output/Bio_DMFB_";
		out.open(temp.c_str());
		temp+= filename;
		temp+= ".dot";
		out << ss.str();
		out.close();
	}
//	return BCGList;
}
int assayProtocol :: getDropCount()
{
	int count=0;
	for(int i=0 ; i<completeMixNum ;++i){
		map<string, Container>::iterator it = conLookUp.find(completeMixList[i].container);
		if(it!= conLookUp.end()){
			count++;
		}
	}
	return count;
}

