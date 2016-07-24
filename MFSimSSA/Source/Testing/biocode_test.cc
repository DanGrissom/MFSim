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
 * Source: biocode_test.cc														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/Testing/biocode_test.h"


///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the PCR mixing stage detailed
// in Chakrabarty's benchmarks. DAG created using BIOCODER.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_B1_PCRMix(double mult)
{
	DAG *dag = new DAG();
	dag->setName("B1_PCR_Mix");
	vector<Container> tubes;

	BioCoder * bio= new BioCoder("B1_PCR_Mix");
	Volume dispVol = bio->vol(100, ML);
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3*mult, SECS);
	Fluid beosynucleotide = bio->new_fluid("beosynucleotide", dispVol);
	Fluid amplitag = bio->new_fluid("amplitag", dispVol);
	Fluid trishcl = bio->new_fluid("tris-hcl", dispVol);
	Fluid gelatin = bio->new_fluid("gelatin", dispVol);
	Fluid bovine = bio->new_fluid("bovine", dispVol);
	Fluid primer = bio->new_fluid("primer", dispVol);
	Fluid lambda = bio->new_fluid("lambda", dispVol);
	Fluid kcl = bio->new_fluid("kcl", dispVol);

	for (int i = 0; i < 4; i++)
		tubes.push_back(bio->new_container(STERILE_MICROFUGE_TUBE2ML));

	bio->first_step();
	bio->measure_fluid(trishcl, dropVol, tubes[0]);
	bio->measure_fluid(kcl, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);

	bio->next_step();
	bio->measure_fluid(bovine, dropVol, tubes[1]);
	bio->measure_fluid(gelatin, dropVol, tubes[1]);
	bio->vortex(tubes[1], time);

	bio->next_step();
	bio->measure_fluid(primer, dropVol, tubes[2]);
	bio->measure_fluid(beosynucleotide, dropVol, tubes[2]);
	bio->vortex(tubes[2], time);

	bio->next_step();
	bio->measure_fluid(amplitag, dropVol, tubes[3]);
	bio->measure_fluid(lambda, dropVol, tubes[3]);
	bio->vortex(tubes[3], time);

	bio->next_step();
	bio->measure_fluid(tubes[1], tubes[0]);
	bio->vortex(tubes[0], time);

	bio->next_step();
	bio->measure_fluid(tubes[3], tubes[2]);
	bio->vortex(tubes[2], time);

	bio->next_step();
	bio->measure_fluid(tubes[2], tubes[0]);
	bio->vortex(tubes[0], time);

	bio->next_step();
	bio->drain(tubes[0], "output");

	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCode Bench1_PCRMix CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum are assayed
// for glucose/lactate measurements. DAG created using BIOCODER.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_B2_InVitroDiag(int numSamples, int numReagents)
{
	// S1-S4 = Plasma, Serum, Saliva, Urine
	// R1-R4 = Glucose, Lactate, Pyruvate, Glutamate
	// Mixing Times: S1 = 5, S2 = 3, S3 = 4, S4 = 6
	// Detect Times: R1 = 5, R2 = 4, R3 = 6, R4 = 5

	// Bench Stats:
	// B1 - #S = 2, #R = 2, #Detectors = 2
	// B2 - #S = 2, #R = 3, #Detectors = 3
	// B3 - #S = 3, #R = 3, #Detectors = 3
	// B4 - #S = 3, #R = 4, #Detectors = 4
	// B5 - #S = 4, #R = 4, #Detectors = 4

	{	// Sanity check: We only have mix/detect data for the four samples/reagents
		stringstream msg;
		msg << "ERROR. Number of samples/reagents/detectors must each be between 1 and 7." << ends;
		claim(numSamples > 0 && numReagents > 0 && numSamples <= 7 && numReagents <= 7, &msg);
	}

	BioCoder * bio= new BioCoder("B2_InVitro_Mix");
	double sampleMixTimes[] = {5, 3, 4, 6, 3, 7, 6};
	double reagentDetectTimes[] = {5, 4, 6, 5, 8, 6, 7};
	char* sampleNames[] = {"Plasma", "Serum", "Saliva", "Urine", "S1", "S2", "S3"};
	char* reagentNames[] = {"Glucose", "Lactate", "Pyruvate", "Glutamate", "R1", "R2", "R3"};

	DAG *dag = new DAG();
	dag->setName("B2_InVitro_Mix");

	int tubeNum = 0;
	for (int s = 0; s < numSamples; s++)
	{
		for (int r = 0; r < numReagents; r++)
		{
			Container tube = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
			Fluid S = bio->new_fluid(sampleNames[s] ,bio->vol(100,ML));
			Fluid R = bio->new_fluid(reagentNames[r] ,bio->vol(100,ML));

			if (s == 0 && r == 0)
				bio->first_step();
			else
				bio->next_step();

			bio->measure_fluid(S, bio->vol(10, UL), tube);
			bio->measure_fluid(R, bio->vol(10, UL), tube);
			bio->vortex(tube, bio->time(sampleMixTimes[s], SECS));

			bio->next_step();
			bio->measure_fluorescence(tube, bio->time(sampleMixTimes[s], SECS));

			bio->next_step();
			bio->drain(tube, "output");

			tubeNum++;
		}
	}
	bio->end_protocol();
	bio->Translator(dag);

    cout << "BioCoder Bench_InVitroDiag(Samples=" << numSamples << ", Reagents=" << numReagents << ")  CREATED" << endl;
    return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the protein assay (based on Bradford
// reaction) detailed in Chakrabarty's benchmarks. Specified in Biocoder.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_B3_Protein(double mult)
{
	DAG *dag = new DAG();
	dag->setName("B3_Protein_Mix");

	BioCoder * bio = new BioCoder("B3_Protein_Mix");
	Fluid DsS = bio->new_fluid("DsS", bio->vol(1000,ML));
	Fluid DsB = bio->new_fluid("DsB", bio->vol(1000,ML));
	Fluid DsR = bio->new_fluid("DsR", bio->vol(1000,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3*mult, SECS);

	vector<Container> tubes;
	int numOuts = 8;
	for (int i = 0; i < numOuts; i++)
		tubes.push_back(bio->new_container(STERILE_MICROFUGE_TUBE2ML));

	// Level 1
	bio->first_step();
	bio->measure_fluid(DsS, dropVol, tubes[0]);
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[4], false);

	// Level 2
	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[2], false);

	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tubes[4]);
	bio->vortex(tubes[4], time);
	bio->measure_fluid(tubes[4], dropVol, tubes[6], false);

	// Level 3
	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[1], false);

	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tubes[2]);
	bio->vortex(tubes[2], time);
	bio->measure_fluid(tubes[2], dropVol, tubes[3], false);

	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tubes[4]);
	bio->vortex(tubes[4], time);
	bio->measure_fluid(tubes[4], dropVol, tubes[5], false);

	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tubes[6]);
	bio->vortex(tubes[6], time);
	bio->measure_fluid(tubes[6], dropVol, tubes[7], false);


	// Level 4-END
	for (int i = 0; i < numOuts; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bio->next_step();
			bio->measure_fluid(DsB, dropVol, tubes[i]);
			bio->vortex(tubes[i], time);
		}
		bio->next_step();
		bio->measure_fluid(DsR, dropVol, tubes[i]);
		bio->vortex(tubes[i], time);

		bio->next_step();
		bio->measure_fluorescence(tubes[i], bio->time(30*mult, SECS));
	}

	bio->next_step();
	for (int i = 0; i < numOuts; i ++)
		bio->drain(tubes[i], "output");

	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCoder B3_Protein CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the protein assay (based on Bradford
// reaction) detailed in Chakrabarty's benchmarks. However, instead of doing
// the default 3 levels of splits, we can specify how many levels of splits
// we want to do.  Also, we can use eq1_inc2_dec3 to specify if the detects
// are equal (1), increasing (2) or decreasing (3) from left to right.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_B3_Protein_Levels(int splitLevels, int eq1_inc2_dec3, double mult)
{
	DAG *dag = new DAG();
	dag->setName("B3_Protein_Mix_Levels");

	BioCoder * bio = new BioCoder("B3_Protein_Mix");
	Fluid DsS = bio->new_fluid("DsS", bio->vol(10000,ML));
	Fluid DsB = bio->new_fluid("DsB", bio->vol(10000,ML));
	Fluid DsR = bio->new_fluid("DsR", bio->vol(10000,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3*mult, SECS);

	vector<Container> tubes;
	int numOuts = (int)pow((float)2, splitLevels);
	for (int i = 0; i < numOuts; i++)
		tubes.push_back(bio->new_container(STERILE_MICROFUGE_TUBE2ML));

	bio->first_step();
	bio->measure_fluid(DsS, dropVol, tubes[0]);
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[numOuts/2], false);

	for (int l = 1; l < splitLevels; l++)
	{
		int inc1 = (int)pow((float)2, splitLevels-l);
		int inc2 = (int)pow((float)2, splitLevels-l-1);

		for (int sNum = 0; sNum < (int)pow((float)2, l); sNum++)
		{
			int base = sNum * inc1;
			bio->next_step();
			bio->measure_fluid(DsB, dropVol, tubes[base]);
			bio->vortex(tubes[base], time);
			//bio->measure_fluid(tubes[base], dropVol, tubes[base+inc2], false);
			bio->measure_fluid(tubes[base], 2, 1, tubes[base+inc2], false);
		}
	}

	// Level numLevels+1 to END
	for (int i = 0; i < numOuts; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bio->next_step();
			bio->measure_fluid(DsB, dropVol, tubes[i]);
			bio->vortex(tubes[i], time);
		}
		bio->next_step();
		bio->measure_fluid(DsR, dropVol, tubes[i]);
		bio->vortex(tubes[i], time);

		bio->next_step();

		if (eq1_inc2_dec3 == 1)
			bio->measure_fluorescence(tubes[i], bio->time(30*mult, SECS));
		else if (eq1_inc2_dec3 == 2)
			bio->measure_fluorescence(tubes[i], bio->time(30+i, SECS));
		else if (eq1_inc2_dec3 == 3)
			bio->measure_fluorescence(tubes[i], bio->time(30+(numOuts-i-1), SECS));
		else
		{
			cerr << "Specified a value for eq1_inc2_dec3 that was not valid for B3_Protein_Levels()" << endl;
			exit(1);
		}
	}

	bio->next_step();
	for (int i = 0; i < numOuts; i ++)
		bio->drain(tubes[i], "output");

	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCoder B3_Protein_Levels CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// Simply shows how the dispense, mix, heat, detect, split, output operations
// are called
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_Operation_Demonstration_Bench()
{
	DAG *dag = new DAG();
	dag->setName("Operation_Demo_Benchmark");

	// BioCoder Assay Protocol
	BioCoder * bio = new BioCoder("Operation_Demo_Benchmark");
	Volume reservoirVol = bio->vol(100,ML);
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(1, SECS);
	Container tube1 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube2 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Fluid reagent = bio->new_fluid("reagent", reservoirVol);
	Fluid sample = bio->new_fluid("sample", reservoirVol);

	bio->first_step("Dispense Fluids");
	bio->measure_fluid(sample, dropVol, tube1);
	bio->measure_fluid(reagent, dropVol, tube1);

	bio->next_step("Mix Fluids");
	bio->vortex(tube1, time);

	bio->next_step("Heat Fluids at 50 C");
	bio->store_for(tube1, 50, time, false);

	bio->next_step("Detect Fluorescence");
	bio->measure_fluorescence(tube1, time);

	bio->next_step("Split Fluid");
	bio->measure_fluid(tube1, dropVol, tube2, false);

	bio->next_step("Output Fluid");
	bio->drain(tube1, "waste");
	bio->drain(tube2, "output");

	bio->end_protocol();
	bio->Translator(dag);

    cout << "Operation_Demo_Benchmark CREATED" << endl;
    return dag;
}


///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the protein assay (based on Bradford
// reaction) detailed in Chakrabarty's benchmarks. However, instead of doing
// the default 3 levels of splits, it does 2 levels and then has 5s detect
// operations after each split.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_2LevelProtein_With_SplitVolumeDetects()
{
	int splitLevels = 2;

	DAG *dag = new DAG();
	dag->setName("B3_2LevelProtein_With_SplitVolumeDetects");

	BioCoder * bio = new BioCoder("B3_2LevelProtein_With_SplitVolumeDetects");
	Fluid DsS = bio->new_fluid("DsS", bio->vol(10000,ML));
	Fluid DsB = bio->new_fluid("DsB", bio->vol(10000,ML));
	Fluid DsR = bio->new_fluid("DsR", bio->vol(10000,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3, SECS);
	Time detTime = bio->time(5, SECS);

	vector<Container> tubes;
	int numOuts = (int)pow((float)2, splitLevels);
	for (int i = 0; i < numOuts; i++)
		tubes.push_back(bio->new_container(STERILE_MICROFUGE_TUBE2ML));

	bio->first_step();
	bio->measure_fluid(DsS, dropVol, tubes[0]);
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[numOuts/2], false);
	bio->measure_fluorescence(tubes[0], detTime);
	//bio->measure_fluorescence(tubes[numOuts/2], detTime); // For timings...really only need one detect

	for (int l = 1; l < splitLevels; l++)
	{
		int inc1 = (int)pow((float)2, splitLevels-l);
		int inc2 = (int)pow((float)2, splitLevels-l-1);

		for (int sNum = 0; sNum < (int)pow((float)2, l); sNum++)
		{
			int base = sNum * inc1;
			bio->next_step();
			bio->measure_fluid(DsB, dropVol, tubes[base]);
			bio->vortex(tubes[base], time);
			//bio->measure_fluid(tubes[base], dropVol, tubes[base+inc2], false);
			bio->measure_fluid(tubes[base], 2, 1, tubes[base+inc2], false);
			bio->measure_fluorescence(tubes[base], detTime);
			//bio->measure_fluorescence(tubes[base+inc2], detTime); // For timings...really only need one detect
		}
	}

	// Level numLevels+1 to END
	for (int i = 0; i < numOuts; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bio->next_step();
			bio->measure_fluid(DsB, dropVol, tubes[i]);
			bio->vortex(tubes[i], time);
		}
		bio->next_step();
		bio->measure_fluid(DsR, dropVol, tubes[i]);
		bio->vortex(tubes[i], time);

		bio->next_step();
		bio->measure_fluorescence(tubes[i], bio->time(30, SECS));

	}

	bio->next_step();
	for (int i = 0; i < numOuts; i ++)
		bio->drain(tubes[i], "output");

	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCoder B3_2LevelProtein_With_SplitVolumeDetects CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// Time-redundant recovery sub-graph for a failure occurring in first level.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_2LevelProtein_Lev1FailureSubGraph()
{
	int splitLevels = 1;

	DAG *dag = new DAG();
	dag->setName("B3_2LevelProtein_Lev1FailureSubGraph");

	BioCoder * bio = new BioCoder("B3_2LevelProtein_Lev1FailureSubGraph");
	Fluid DsS = bio->new_fluid("DsS", bio->vol(10000,ML));
	Fluid DsB = bio->new_fluid("DsB", bio->vol(10000,ML));
	Fluid DsR = bio->new_fluid("DsR", bio->vol(10000,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3, SECS);
	Time detTime = bio->time(5, SECS);

	vector<Container> tubes;
	int numOuts = (int)pow((float)2, splitLevels);
	for (int i = 0; i < numOuts; i++)
		tubes.push_back(bio->new_container(STERILE_MICROFUGE_TUBE2ML));

	bio->first_step();
	bio->measure_fluid(DsS, dropVol, tubes[0]);
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[numOuts/2], false);
	bio->measure_fluorescence(tubes[0], detTime);
	bio->measure_fluorescence(tubes[numOuts/2], detTime); // For timings...really only need one detect

	bio->next_step();
	for (int i = 0; i < numOuts; i ++)
		bio->drain(tubes[i], "output");

	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCoder B3_2LevelProtein_Lev1FailureSubGraph CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// Time-redundant recovery sub-graph for a single failure occurring in
// second level.
///////////////////////////////////////////////////////////////////////////
DAG *BiocodeTest::Create_2LevelProtein_Lev2SingleFailureSubGraph()
{
	int splitLevels = 2;

	DAG *dag = new DAG();
	dag->setName("B3_2LevelProtein_Lev2SingleFailureSubGraph");

	BioCoder * bio = new BioCoder("B3_2LevelProtein_Lev2SingleFailureSubGraph");
	Fluid DsS = bio->new_fluid("DsS", bio->vol(10000,ML));
	Fluid DsB = bio->new_fluid("DsB", bio->vol(10000,ML));
	Fluid DsR = bio->new_fluid("DsR", bio->vol(10000,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3, SECS);
	Time detTime = bio->time(5, SECS);

	Container tube0 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube1 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube2 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);


	bio->first_step();
	bio->measure_fluid(DsS, dropVol, tube0);
	bio->measure_fluid(DsB, dropVol, tube0);
	bio->vortex(tube0, time);
	bio->measure_fluid(tube0, dropVol, tube2, false);
	bio->measure_fluorescence(tube0, detTime);
	//bio->measure_fluorescence(tubes[numOuts/2], detTime); // For timings...really only need one detect
	bio->drain(tube2, "output");



	bio->next_step();
	bio->measure_fluid(DsB, dropVol, tube0);
	bio->vortex(tube0, time);
	//bio->measure_fluid(tubes[base], dropVol, tubes[base+inc2], false);
	bio->measure_fluid(tube0, 2, 1, tube1, false);
	bio->measure_fluorescence(tube0, detTime);
	bio->drain(tube0, "output");
	bio->drain(tube1, "output");


	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCoder B3_2LevelProtein_Lev2SingleFailureSubGraph CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// Time-redundant recovery sub-graph for a double failure occurring in
// second level.
///////////////////////////////////////////////////////////////////////////
DAG * BiocodeTest::Create_2LevelProtein_Lev2DoubleFailureSubGraph()
{
	int splitLevels = 2;

	DAG *dag = new DAG();
	dag->setName("B3_2LevelProtein_Lev2DoubleFailureSubGraph");

	BioCoder * bio = new BioCoder("B3_2LevelProtein_Lev2DoubleFailureSubGraph");
	Fluid DsS = bio->new_fluid("DsS", bio->vol(10000,ML));
	Fluid DsB = bio->new_fluid("DsB", bio->vol(10000,ML));
	Fluid DsR = bio->new_fluid("DsR", bio->vol(10000,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(3, SECS);
	Time detTime = bio->time(5, SECS);

	vector<Container> tubes;
	int numOuts = (int)pow((float)2, splitLevels);
	for (int i = 0; i < numOuts; i++)
		tubes.push_back(bio->new_container(STERILE_MICROFUGE_TUBE2ML));

	bio->first_step();
	bio->measure_fluid(DsS, dropVol, tubes[0]);
	bio->measure_fluid(DsB, dropVol, tubes[0]);
	bio->vortex(tubes[0], time);
	bio->measure_fluid(tubes[0], dropVol, tubes[numOuts/2], false);
	bio->measure_fluorescence(tubes[0], detTime);
	bio->measure_fluorescence(tubes[numOuts/2], detTime); // For timings...really only need one detect

	for (int l = 1; l < splitLevels; l++)
	{
		int inc1 = (int)pow((float)2, splitLevels-l);
		int inc2 = (int)pow((float)2, splitLevels-l-1);

		for (int sNum = 0; sNum < (int)pow((float)2, l); sNum++)
		{
			int base = sNum * inc1;
			bio->next_step();
			bio->measure_fluid(DsB, dropVol, tubes[base]);
			bio->vortex(tubes[base], time);
			//bio->measure_fluid(tubes[base], dropVol, tubes[base+inc2], false);
			bio->measure_fluid(tubes[base], 2, 1, tubes[base+inc2], false);
			bio->measure_fluorescence(tubes[base], detTime);
			bio->measure_fluorescence(tubes[base+inc2], detTime); // For timings...really only need one detect
		}
	}

	bio->next_step();
	for (int i = 0; i < numOuts; i ++)
		bio->drain(tubes[i], "output");

	bio->end_protocol();
	bio->Translator(dag);

	cout << "BioCoder B3_2LevelProtein_Lev2DoubleFailureSubGraph CREATED" << endl;
	return dag;
}














///////////////////////////////////////////////////////////////////////////
// Creates a simple dependency from DAG1 to DAG2, where DAG2 cannot run
// unless a sensor in DAG1 is below a certain threshold (.5 in this case)
///////////////////////////////////////////////////////////////////////////
CFG * BiocodeTest::Create_Conditional_Demo_CFG(bool outputBiocoderGraphs)
{
	double mult = 1; // TimeStep multiplier
	double secPerTS = .5; //seconds per timestep

	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);
	string sensor1;

	// BioCoder 1 Protocol
	BioCoder * bio= BioSys->addBioCoder();
	Container tube1 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube2 = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Fluid reagent = bio->new_fluid("reagent",bio->vol(100,ML));
	Fluid sample = bio->new_fluid("sample",bio->vol(100,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(1, SECS);

	bio->first_step();
	bio->measure_fluid(sample, dropVol, tube1);
	bio->measure_fluid(reagent, dropVol, tube1);
	bio->vortex(tube1, time);

	bio->next_step();
	bio->store_for(tube1, 50, time, true);

	bio->next_step();
	bio->measure_fluid(tube1, dropVol, tube2, false);
	bio->drain(tube2, "waste");

	bio->next_step();
	//bio->store_for(tube1, 60, time);
	sensor1 = bio->measure_fluorescence(tube1, time);

	bio->next_step();
	bio->drain(tube1, "save");

	bio->end_protocol();


	// BioCoder 2 Protocol
	BioCoder * bio2 = BioSys->addBioCoder();
	Container tube3 = bio2->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube4 = bio2->new_container(STERILE_MICROFUGE_TUBE2ML);
	Fluid reagent2 = bio2->new_fluid("reagent",bio2->vol(100,ML));
	Fluid sample2 = bio2->new_fluid("sample",bio2->vol(100,ML));
	Volume dropVol2 = bio2->vol(10, UL);
	Time time2 = bio2->time(2, SECS);

	bio2->first_step();
	bio2->measure_fluid(sample2, dropVol2, tube3);
	bio2->measure_fluid(reagent2, dropVol2, tube3);
	bio2->vortex(tube3, time2);

	bio2->next_step();
	bio2->store_for(tube3, 50, time2, true);

	bio2->next_step();
	bio2->measure_fluid(tube3, dropVol2, tube4, false);
	bio2->drain(tube4, "waste");

	bio2->next_step();
	bio2->store_for(tube3, 60, time2, false);

	bio2->next_step();
	bio2->drain(tube3, "save");

	bio2->end_protocol();

	// Conditional Groups
	BioConditionalGroup *bcg = BioSys->addBioCondGroup();
	BioExpression *be = new BioExpression(sensor1, OP_LT, .5);
	bcg->addNewCondition(be, bio2);

	// Generate CFG from Biocoder and output CFG and all DAGs
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->setName("BC_Conditional_CFG");
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
    cout << "BioCoder Conditional CFG Generated" << endl;
    return cfg;
}

///////////////////////////////////////////////////////////////////////////
// Manually creates the DAGs (using Biocoder) for the case where we have a
// single fault-tolerant case (SYSTEM->convertFTSplits() does this auto-
// matically, but we want to show it's possible with BioCoder.)
///////////////////////////////////////////////////////////////////////////
CFG * BiocodeTest::Create_Conditional_With_Transfer_Droplets_Demo_CFG(bool outputBiocoderGraphs)
{
	double mult = 1; // TimeStep multiplier
	double secPerTS = 1; //seconds per timestep

	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);
	string d1;
	string at_to1;
	string at_to2;
	string d_to1;
	string d_to2;
	string ms_to1;
	string ms_to2;

	string ab_ti1;
	string ab_ti2;
	string d_ti1;
	string d_ti2;
	string ms_ti1;
	string ms_ti2;

	// BioCoder AssayTop Protocol
	BioCoder * bioAT = BioSys->addBioCoder();
	Container tube1 = bioAT->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube2 = bioAT->new_container(STERILE_MICROFUGE_TUBE2ML);
	Fluid reagent = bioAT->new_fluid("reagent",bioAT->vol(100,ML));
	Fluid sample = bioAT->new_fluid("sample",bioAT->vol(100,ML));
	Volume dropVol = bioAT->vol(10, UL);
	Time time = bioAT->time(1, SECS);
	bioAT->first_step();
	bioAT->measure_fluid(sample, dropVol, tube1);
	bioAT->measure_fluid(reagent, dropVol, tube1);
	bioAT->next_step();
	bioAT->vortex(tube1, time);
	bioAT->next_step();
	bioAT->store_for(tube1, 50, time, true);
	bioAT->next_step();
	bioAT->measure_fluid(tube1, dropVol, tube2, false);
	bioAT->next_step();
	at_to1 = bioAT->save_fluid(tube1);
	at_to2 = bioAT->save_fluid(tube2);
	bioAT->end_protocol();

	// BioCoder AssayBottom Protocol
	BioCoder * bioAB = BioSys->addBioCoder();
	tube1 = bioAB->new_container(STERILE_MICROFUGE_TUBE2ML);
	tube2 = bioAB->new_container(STERILE_MICROFUGE_TUBE2ML);
	dropVol = bioAB->vol(10, UL);
	time = bioAB->time(1, SECS);
	bioAB->first_step();
	ab_ti1 = bioAB->reuse_fluid(tube1);
	bioAB->next_step();
	bioAB->drain(tube1, "waste");
	bioAB->next_step();
	ab_ti2 = bioAB->reuse_fluid(tube2);
	bioAB->next_step();
	bioAB->store_for(tube2, 60, time, false);
	bioAB->next_step();
	bioAB->drain(tube2, "save");
	bioAB->end_protocol();

	// BioCoder Detect Protocol
	BioCoder * bioD = BioSys->addBioCoder();
	tube1 = bioD->new_container(STERILE_MICROFUGE_TUBE2ML);
	tube2 = bioD->new_container(STERILE_MICROFUGE_TUBE2ML);
	dropVol = bioD->vol(10, UL);
	time = bioD->time(1, SECS);
	bioD->first_step();
	d_ti1 = bioD->reuse_fluid(tube1);
	d_ti2 = bioD->reuse_fluid(tube2);
	bioD->next_step();
	d1 = bioD->measure_fluorescence(tube1, time);
	bioD->next_step();
	d_to1 = bioD->save_fluid(tube1);
	d_to2 = bioD->save_fluid(tube2);
	bioD->end_protocol();

	// BioCoder Merge-Resplit Protocol
	BioCoder * bioMS = BioSys->addBioCoder();
	tube1 = bioMS->new_container(STERILE_MICROFUGE_TUBE2ML);
	tube2 = bioMS->new_container(STERILE_MICROFUGE_TUBE2ML);
	dropVol = bioMS->vol(10, UL);
	time = bioMS->time(1, SECS);
	bioMS->first_step();
	ms_ti1 = bioMS->reuse_fluid(tube1);
	ms_ti2 = bioMS->reuse_fluid(tube1);
	bioMS->next_step();
	bioMS->vortex(tube1, time);
	bioMS->next_step();
	bioMS->measure_fluid(tube1, 2, 1, tube2, false);
	bioMS->next_step();
	ms_to1 = bioMS->save_fluid(tube1);
	ms_to2 = bioMS->save_fluid(tube2);
	bioMS->end_protocol();

	// Conditional Groups
	// AssayTop-->DetectAssay
	BioConditionalGroup *bcg =BioSys->addBioCondGroup();
	BioExpression *e = new BioExpression(bioAT, true);
	BioCondition* bioCond = bcg->addNewCondition(e, bioD);
	bioCond->addTransferDroplet(at_to1,d_ti1);
	bioCond->addTransferDroplet(at_to2,d_ti2);

	// DetectAssay-->MergeSplitAssay
	bcg = BioSys->addBioCondGroup();
	e = new BioExpression (d1,OP_LT,.5);
	bioCond= bcg->addNewCondition(e, bioMS);
	bioCond->addTransferDroplet(d_to1,ms_ti1);
	bioCond->addTransferDroplet(d_to2,ms_ti2);

	// DetectAssay-->AssayBottom
	bioCond = bcg->addNewCondition(NULL, bioAB);
	bioCond->addTransferDroplet(d_to1,ab_ti1);
	bioCond->addTransferDroplet(d_to2,ab_ti2);

	// MergeSplitAssay-->DetectAssay
	bcg = BioSys->addBioCondGroup();
	e = new BioExpression(bioMS,true);
	bioCond=bcg->addNewCondition(e,bioD);
	bioCond->addTransferDroplet(ms_to1,d_ti1);
	bioCond->addTransferDroplet(ms_to2,d_ti2);

	// Generate CFG from Biocoder and output CFG and all DAGs
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->setName("BC_Conditional_TransferDrops_CFG");
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
    cout << "BioCoder Conditional w/ Transfer Droplets CFG Generated" << endl;
    return cfg;
}

///////////////////////////////////////////////////////////////////////////
// Creates a DAG that transfers a droplet to one of two simple child DAGs
// based on a sensor reading from the parent DAG.
///////////////////////////////////////////////////////////////////////////
CFG * BiocodeTest::Create_Simple_Conditional_With_Transfer_Droplets_Demo_CFG(bool outputBiocoderGraphs)
{
	double mult = 1; // TimeStep multiplier
	double secPerTS = .5; //seconds per timestep

	string d1;
	string to;
	string ti_save;
	string ti_waste;

	// Creation of BioSystem
	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);

	// BioCoder Mix/Detect Protocol
	BioCoder * bio = BioSys->addBioCoder();
	Container tube = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
	Fluid reagent = bio->new_fluid("reagent",bio->vol(100,ML));
	Fluid sample = bio->new_fluid("sample",bio->vol(100,ML));
	Volume dropVol = bio->vol(10, UL);
	Time time = bio->time(1, SECS);
	bio->first_step("Dispense sample/reagent");
	bio->measure_fluid(sample, dropVol, tube);
	bio->measure_fluid(reagent, dropVol, tube);
	bio->next_step("Mix");
	bio->vortex(tube, time);
	bio->next_step("Detect");
	d1 = bio->measure_fluorescence(tube, time);
	bio->next_step("Transfer Out");
	to = bio->save_fluid(tube);
	bio->end_protocol();

	// BioCoder Save Protocol
	BioCoder * bioSave = BioSys->addBioCoder();
	tube = bioSave->new_container(STERILE_MICROFUGE_TUBE2ML);
	dropVol = bioSave->vol(10, UL);
	bioSave->first_step("Transfer In");
	ti_save = bioSave->reuse_fluid(tube);
	bioSave->next_step("Save this fluid");
	bioSave->drain(tube, "save");
	bioSave->end_protocol();

	// BioCoder Waste Protocol
	BioCoder * bioWaste = BioSys->addBioCoder();
	tube = bioWaste->new_container(STERILE_MICROFUGE_TUBE2ML);
	dropVol = bioWaste->vol(10, UL);
	bioWaste->first_step("Transfer In");
	ti_waste = bioWaste->reuse_fluid(tube);
	bioWaste->next_step("Discard this fluid");
	bioWaste->drain(tube, "waste");
	bioWaste->end_protocol();

	// Conditional Groups
	BioConditionalGroup *bcg = BioSys->addBioCondGroup();
	BioExpression *be = new BioExpression(d1, OP_LT, 0.5);
	BioCondition *bc = bcg->addNewCondition(be, bioSave);
	bc->addTransferDroplet(to, ti_save);
	bc = bcg->addNewCondition(NULL, bioWaste);
	bc->addTransferDroplet(to, ti_waste);

	// Generate CFG from Biocoder and output CFG and all DAGs
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->setName("BC_Simple_Conditional_TransferDrops_CFG");
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
    cout << "BioCoder Simple Conditional w/ Transfer Droplets CFG Generated" << endl;
    return cfg;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum are assayed
// for glucose/lactate measurements. DAG created using BIOCODER. This
// is also modified to include control flow
///////////////////////////////////////////////////////////////////////////
CFG * BiocodeTest::Create_Conditional_B2_InVitroDiag_CFG(int numSamples, int numReagents, string inputArchFile, double mult, double secPerTS, int repeat, bool outputBiocoderGraphs)
{
	// S1-S4 = Plasma, Serum, Saliva, Urine
	// R1-R4 = Glucose, Lactate, Pyruvate, Glutamate
	// Mixing Times: S1 = 5, S2 = 3, S3 = 4, S4 = 6
	// Detect Times: R1 = 5, R2 = 4, R3 = 6, R4 = 5

	// Bench Stats:
	// B1 - #S = 2, #R = 2
	// B2 - #S = 2, #R = 3
	// B3 - #S = 3, #R = 3
	// B4 - #S = 3, #R = 4
	// B5 - #S = 4, #R = 4

	{	// Sanity check: We only have mix/detect data for the four samples/reagents
		stringstream msg;
		msg << "ERROR. Number of samples/reagents/detectors must each be between 1 and 4." << ends;
		claim(numSamples > 0 && numReagents > 0 && numSamples <= 4 && numReagents <= 4, &msg);
	}

	double sampleMixTimes[] = {5, 3, 4, 6};
	double reagentDetectTimes[] = {5, 4, 6, 5};
	char* sampleNames[] = {"Plasma", "Serum", "Saliva", "Urine"};
	char* reagentNames[] = {"Glucose", "Lactate", "Pyruvate", "Glutamate"};
	vector<string> detects;
	vector<BioCoder *> assays;

	// Creation of BioSystem
	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);

	// Create protocol
	for (int r = 0; r < numReagents; r++)
	{
		BioCoder * bio = BioSys->addBioCoder();
		for (int s = 0; s < numSamples; s++)
		{
			Container tube = bio->new_container(STERILE_MICROFUGE_TUBE2ML);
			Fluid S = bio->new_fluid(sampleNames[s] ,bio->vol(100,ML));
			Fluid R = bio->new_fluid(reagentNames[r] ,bio->vol(100,ML));

			if (s == 0 && r == 0)
				bio->first_step();
			else
				bio->next_step();

			bio->measure_fluid(S, bio->vol(10, UL), tube);
			bio->measure_fluid(R, bio->vol(10, UL), tube);
			bio->vortex(tube, bio->time(sampleMixTimes[s], SECS));

			bio->next_step();
			detects.push_back(bio->measure_fluorescence
					(tube, bio->time(sampleMixTimes[s], SECS)));

			bio->next_step();
			bio->drain(tube, "output");
		}
		bio->end_protocol();
		assays.push_back(bio);
	}
	// Conditional Groups
	for (int r = 0; r < numReagents-1; r++)
	{
		int i = numSamples * r;
		BioConditionalGroup *bcg = BioSys->addBioCondGroup();
		BioExpression *outer = new BioExpression(OP_AND);
		outer->addOperand(new BioExpression(detects[i], OP_LT, 0.99));
		outer->addOperand(new BioExpression(detects[i+1], OP_LT, 0.99));
		outer->addOperand(new BioExpression(detects[i+2], OP_LT, 0.99));
		outer->addOperand(new BioExpression(detects[i+3], OP_LT, 0.99));
		BioCondition *bc = bcg->addNewCondition(outer, assays[r+1]);
	}

	// Generate CFG from Biocoder and output CFG and all DAGs
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->setName("BC_Conditional_B2_InVitro_CFG");
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
	cout << "BioCoder Conditional B2_InVitroDiag(Samples=" << numSamples << ", Reagents=" << numReagents /*<< ", Detectors= " << numDetectors*/ << ") CFG Generated" << endl;
	return cfg;
}


///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the protein assay (based on Bradford
// reaction) detailed in Chakrabarty's benchmarks. Specified in Biocoder.
// We add conditional code to account for fault-tolerant splits.
///////////////////////////////////////////////////////////////////////////
CFG * BiocodeTest::Create_Conditional_B3_Protein_FaultTolerant_CFG(double mult, double secPerTS, int repeat, bool outputBiocoderGraphs)
{
	double splitFailThresh = 0.5; // If detect reading is lower than this, then the detect "fails"

	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);

	/////////////////////////////////////////////////////////
	// LEVEL 1
	/////////////////////////////////////////////////////////
	// Level 1 Transfer/Detect tags
	string lev1_det;

	string lev1Sp_to1;
	string lev1Sp_to2;

	string lev1Det_ti1;
	string lev1Det_ti2;
	string lev1Det_to1;
	string lev1Det_to2;

	string lev1RMS_ti1;
	string lev1RMS_ti2;
	string lev1RMS_to1;
	string lev1RMS_to2;
	/////////////////////////////////////////////////////////
	// Level_1_Splits
	BioCoder * bioLev1Split = BioSys->addBioCoder("bioLev1Split");
	Fluid DsS = bioLev1Split->new_fluid("DsS", bioLev1Split->vol(1000,ML));
	Fluid DsB = bioLev1Split->new_fluid("DsB", bioLev1Split->vol(1000,ML));
	Fluid DsR = bioLev1Split->new_fluid("DsR", bioLev1Split->vol(1000,ML));
	Volume dropVol = bioLev1Split->vol(10, UL);
	Time time = bioLev1Split->time(3*mult, SECS);
	Time detTime = bioLev1Split->time(5*mult, SECS);
	Container tube1 = bioLev1Split->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube2 = bioLev1Split->new_container(STERILE_MICROFUGE_TUBE2ML);
	bioLev1Split->first_step();
	bioLev1Split->measure_fluid(DsS, dropVol, tube1);
	bioLev1Split->measure_fluid(DsB, dropVol, tube1);
	bioLev1Split->next_step();
	bioLev1Split->vortex(tube1, time);
	bioLev1Split->next_step();
	bioLev1Split->measure_fluid(tube1, 2, 1, tube2, false);
	bioLev1Split->next_step();
	lev1Sp_to1 = bioLev1Split->save_fluid(tube1);
	lev1Sp_to2 = bioLev1Split->save_fluid(tube2);
	bioLev1Split->end_protocol();
	/////////////////////////////////////////////////////////
	// Level_1_Det
	BioCoder * bioLev1Det = BioSys->addBioCoder("bioLev1Det");
	tube1 = bioLev1Det->new_container(STERILE_MICROFUGE_TUBE2ML);
	tube2 = bioLev1Det->new_container(STERILE_MICROFUGE_TUBE2ML);
	//time = bioLev1Det->time(1, SECS);
	bioLev1Det->first_step();
	lev1Det_ti1 = bioLev1Det->reuse_fluid(tube1);
	lev1Det_ti2 = bioLev1Det->reuse_fluid(tube2);
	bioLev1Det->next_step();
	lev1_det = bioLev1Det->measure_fluorescence(tube1, detTime);
	bioLev1Det->next_step();
	lev1Det_to1 = bioLev1Det->save_fluid(tube1);
	lev1Det_to2 = bioLev1Det->save_fluid(tube2);
	bioLev1Det->end_protocol();
	/////////////////////////////////////////////////////////
	// Level_1_Merge-ReSplit
	BioCoder * bioLev1MRS = BioSys->addBioCoder("bioLev1MRS");
	tube1 = bioLev1MRS->new_container(STERILE_MICROFUGE_TUBE2ML);
	tube2 = bioLev1MRS->new_container(STERILE_MICROFUGE_TUBE2ML);
	bioLev1MRS->first_step();
	lev1RMS_ti1 = bioLev1MRS->reuse_fluid(tube1);
	lev1RMS_ti2 = bioLev1MRS->reuse_fluid(tube1);
	bioLev1MRS->next_step();
	bioLev1MRS->vortex(tube1, time);
	bioLev1MRS->next_step();
	bioLev1MRS->measure_fluid(tube1, 2, 1, tube2, false);
	bioLev1MRS->next_step();
	lev1RMS_to1 = bioLev1MRS->save_fluid(tube1);
	lev1RMS_to2 = bioLev1MRS->save_fluid(tube2);
	bioLev1MRS->end_protocol();

	/////////////////////////////////////////////////////////
	// LEVEL 2
	/////////////////////////////////////////////////////////
	// Level 2 Transfer/Detect tags
	string lev2Sp_ti1;
	string lev2Sp_ti2;
	string lev2Sp_to1;
	string lev2Sp_to2;
	string lev2Sp_to3;
	string lev2Sp_to4;

	/////////////////////////////////////////////////////////
	// Level_2_Splits
	BioCoder * bioLev2Split = BioSys->addBioCoder("bioLev2Split");
	tube1 = bioLev2Split->new_container(STERILE_MICROFUGE_TUBE2ML);
	tube2 = bioLev2Split->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube3 = bioLev2Split->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube4 = bioLev2Split->new_container(STERILE_MICROFUGE_TUBE2ML);
	bioLev2Split->first_step("Merge previous with new dispense droplets");
	bioLev2Split->measure_fluid(DsB, dropVol, tube1);
	lev2Sp_ti1 = bioLev2Split->reuse_fluid(tube1);
	bioLev2Split->measure_fluid(DsB, dropVol, tube2);
	lev2Sp_ti2 = bioLev2Split->reuse_fluid(tube2);
	bioLev2Split->next_step("Mix droplets");
	bioLev2Split->vortex(tube1, time);
	bioLev2Split->vortex(tube2, time);
	bioLev2Split->next_step("Split Droplets");
	bioLev2Split->measure_fluid(tube1, 2, 1, tube3, false);
	bioLev2Split->measure_fluid(tube2, 2, 1, tube4, false);
	bioLev2Split->next_step("Transfer droplets out");
	lev2Sp_to1 = bioLev2Split->save_fluid(tube1);
	lev2Sp_to2 = bioLev2Split->save_fluid(tube2);
	lev2Sp_to3 = bioLev2Split->save_fluid(tube3);
	lev2Sp_to4 = bioLev2Split->save_fluid(tube4);
	bioLev2Split->end_protocol();

	int levNum = 2;
	int numTransPerSplit = 2;
	int numSplitsPerLevel = (int)pow(2.0, levNum-1);
	int numCombosPerLevel = (int)pow(2.0, numSplitsPerLevel); // Combinations per level
	int numTransfers = (numTransPerSplit * numSplitsPerLevel * numCombosPerLevel);

	vector<string> *tiRmsL2 = new vector<string>();
	vector<string> *toRmsL2 = new vector<string>();
	vector<string> *tiDetL2 = new vector<string>();
	vector<string> *toDetL2 = new vector<string>();
	for (int i = 0; i < numTransfers; i++)
	{
		tiRmsL2->push_back(" ");
		toRmsL2->push_back(" ");
		tiDetL2->push_back(" ");
		toDetL2->push_back(" ");
	}
	vector<string> *detsL2 = new vector<string>();
	for (int i = 0; i < numTransfers/2; i++)
		detsL2->push_back(" ");

	vector<BioCoder *> bioLev2RMS;
	for (int i = 1; i < numCombosPerLevel; i++)
		bioLev2RMS.push_back(B3_GenerateFtLevelCase_RMS(BioSys, "bioLev2RMS", i, levNum, tiRmsL2, toRmsL2, mult));

	vector<BioCoder *> bioLev2Det;
	for (int i = 1; i < numCombosPerLevel; i++)
		bioLev2Det.push_back(B3_GenerateFtLevelCase_Det(BioSys, "bioLev2Det", i, levNum, tiDetL2, toDetL2, detsL2, mult));

	/////////////////////////////////////////////////////////
	// LEVEL 3 (non-split level)
	/////////////////////////////////////////////////////////
	vector<string> *tiL3 = new vector<string>();
	for (int i = 0; i < 4; i++)
		tiL3->push_back(" ");

	/////////////////////////////////////////////////////////
	// Level 3-END
	BioCoder * bioLev3End = BioSys->addBioCoder("bioLev3End");
	vector<Container> tubes;
	int numOuts = 4;
	for (int i = 0; i < numOuts; i++)
		tubes.push_back(bioLev3End->new_container(STERILE_MICROFUGE_TUBE2ML));

	for (int i = 0; i < numOuts; i++)
	{
		bioLev3End->measure_fluid(DsB, dropVol, tubes[i]);
		tiL3->at(i) = bioLev3End->reuse_fluid(tubes[i]);
		bioLev3End->vortex(tubes[i], time);

		for (int j = 1; j < 4; j++)
		{
			bioLev3End->next_step();
			bioLev3End->measure_fluid(DsB, dropVol, tubes[i]);
			bioLev3End->vortex(tubes[i], time);
		}
		bioLev3End->next_step();
		bioLev3End->measure_fluid(DsR, dropVol, tubes[i]);
		bioLev3End->vortex(tubes[i], time);

		bioLev3End->next_step();
		bioLev3End->measure_fluorescence(tubes[i], bioLev3End->time(30*mult, SECS));
	}

	bioLev3End->next_step();
	for (int i = 0; i < numOuts; i ++)
		bioLev3End->drain(tubes[i], "output");

	bioLev3End->end_protocol();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// Conditional Groups ////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// Level 1 Conditional Statements ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// Lev1Split-->Lev1Det
	BioConditionalGroup *bcg = BioSys->addBioCondGroup();
	BioExpression *e = new BioExpression(bioLev1Split, true);
	BioCondition* bioCond = bcg->addNewCondition(e, bioLev1Det);
	bioCond->addTransferDroplet(lev1Sp_to1, lev1Det_ti1);
	bioCond->addTransferDroplet(lev1Sp_to2, lev1Det_ti2);
	//bcg->printConditionalGroup();

	// Lev1Det-->Lev1MRS
	bcg = BioSys->addBioCondGroup();
	e = new BioExpression (lev1_det, OP_LT, splitFailThresh);
	bioCond = bcg->addNewCondition(e, bioLev1MRS);
	bioCond->addTransferDroplet(lev1Det_to1, lev1RMS_ti1);
	bioCond->addTransferDroplet(lev1Det_to2, lev1RMS_ti2);

	// Lev1Det-->Lev2Split
	bioCond = bcg->addNewCondition(NULL, bioLev2Split);
	bioCond->addTransferDroplet(lev1Det_to1, lev2Sp_ti1);
	bioCond->addTransferDroplet(lev1Det_to2, lev2Sp_ti2);
	//bcg->printConditionalGroup();

	// Lev1MRS-->Lev1Det
	bcg = BioSys->addBioCondGroup();
	e = new BioExpression(bioLev1MRS, true);
	bioCond = bcg->addNewCondition(e, bioLev1Det);
	bioCond->addTransferDroplet(lev1RMS_to1, lev1Det_ti1);
	bioCond->addTransferDroplet(lev1RMS_to2, lev1Det_ti2);
	//bcg->printConditionalGroup();

	//////////////////////////////////////////////////////////////////////////
	// Level 2 Conditional Statements ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// Lev2Split-->Lev2Det3
	int detSgNum = 3; // Detect Sub-graph number
	levNum = 2;
	numSplitsPerLevel = (int)pow(2.0, levNum-1);
	numTransPerSplit = 2;
	bcg = BioSys->addBioCondGroup();
	e = new BioExpression(bioLev2Split, true);
	bioCond = bcg->addNewCondition(e, bioLev2Det.at(detSgNum-1));
	int detOff = detSgNum*numSplitsPerLevel*numTransPerSplit;
	//for (int i = start; i < start + (numTransPerCc * numCcsPerSubGraph); i++)
	//	bioCond->addTransferDroplet(tiDetL2[i])
	bioCond->addTransferDroplet(lev2Sp_to1, tiDetL2->at(detOff++));
	bioCond->addTransferDroplet(lev2Sp_to2, tiDetL2->at(detOff++));
	bioCond->addTransferDroplet(lev2Sp_to3, tiDetL2->at(detOff++));
	bioCond->addTransferDroplet(lev2Sp_to4, tiDetL2->at(detOff++));

	//////////////////////////////////////////////////////////////////////////
	// Lev2Det3-->.........
	detSgNum = 3; // Detect Sub-graph number
	int rmsSgNum; // RMS Sub-graph number
	levNum = 2;
	numSplitsPerLevel = (int)pow(2.0, levNum-1);
	numTransPerSplit = 2;
	detOff = detSgNum*numSplitsPerLevel*numTransPerSplit;
	BioExpression *e1 = new BioExpression(detsL2->at(numSplitsPerLevel*detSgNum), OP_LT, splitFailThresh);
	BioExpression *e2 = new BioExpression(detsL2->at(numSplitsPerLevel*detSgNum+1), OP_LT, splitFailThresh); // +1 for right-detect
	e = new BioExpression(OP_AND);
	e->addOperand(e1);
	e->addOperand(e2);
	bcg = BioSys->addBioCondGroup();
	// Lev2Det3-->Lev2Rms3
	rmsSgNum = 3;
	bioCond = bcg->addNewCondition(e, bioLev2RMS.at(rmsSgNum-1)); // (send flow)
	int rmsOff = rmsSgNum*numSplitsPerLevel*numTransPerSplit;
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiRmsL2->at(rmsOff+i));
	// Lev2Det3-->Lev2Rms2
	rmsSgNum = 2;
	bioCond = bcg->addNewCondition(e1, bioLev2RMS.at(rmsSgNum-1)); // (send flow)
	rmsOff = rmsSgNum*numSplitsPerLevel*numTransPerSplit;
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiRmsL2->at(rmsOff+i));
	// Lev2Det3-->Lev2Rms1
	rmsSgNum = 1;
	bioCond = bcg->addNewCondition(e2, bioLev2RMS.at(rmsSgNum-1)); // (send flow)
	rmsOff = rmsSgNum*numSplitsPerLevel*numTransPerSplit;
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiRmsL2->at(rmsOff+i));
	// Lev2Det3-->Lev3End
	e = new BioExpression(bioLev2Det.at(detSgNum-1), true);
	bioCond = bcg->addNewCondition(e, bioLev3End); // (send flow)
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiL3->at(i));
	//bioCond->addTransferDroplet(toDetL2->at(detOff), tiL3->at(0)); //(send droplets)
	//bioCond->addTransferDroplet(toDetL2->at(detOff+1), tiL3->at(0)); //(send droplets)
	//bioCond->addTransferDroplet(toDetL2->at(detOff+2), tiL3->at(0)); //(send droplets)
	//bioCond->addTransferDroplet(toDetL2->at(detOff+3), tiL3->at(0)); //(send droplets)

	//////////////////////////////////////////////////////////////////////////
	// Lev2Det2-->.........
	detSgNum = 2; // Detect Sub-graph number
	levNum = 2;
	numSplitsPerLevel = (int)pow(2.0, levNum-1);
	numTransPerSplit = 2;
	detOff = detSgNum*numSplitsPerLevel*numTransPerSplit;
	e = new BioExpression(detsL2->at(numSplitsPerLevel*detSgNum), OP_LT, splitFailThresh);
	bcg = BioSys->addBioCondGroup();
	// Lev2Det2-->Lev2Rms2
	rmsSgNum = 2;
	bioCond = bcg->addNewCondition(e, bioLev2RMS.at(rmsSgNum-1)); // (send flow)
	rmsOff = rmsSgNum*numSplitsPerLevel*numTransPerSplit;
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiRmsL2->at(rmsOff+i));
	// Lev2Det2-->Lev3End
	e = new BioExpression(bioLev2Det.at(detSgNum-1), true);
	bioCond = bcg->addNewCondition(e, bioLev3End); // (send flow)
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiL3->at(i));

	//////////////////////////////////////////////////////////////////////////
	// Lev2Det1-->.........
	detSgNum = 1; // Detect Sub-graph number
	levNum = 2;
	numSplitsPerLevel = (int)pow(2.0, levNum-1);
	numTransPerSplit = 2;
	detOff = detSgNum*numSplitsPerLevel*numTransPerSplit;
	e = new BioExpression(detsL2->at(numSplitsPerLevel*detSgNum+1), OP_LT, splitFailThresh);
	bcg = BioSys->addBioCondGroup();
	// Lev2Det1-->Lev2Rms1
	rmsSgNum = 1;
	bioCond = bcg->addNewCondition(e, bioLev2RMS.at(rmsSgNum-1)); // (send flow)
	rmsOff = rmsSgNum*numSplitsPerLevel*numTransPerSplit;
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiRmsL2->at(rmsOff+i));
	// Lev2Det1-->Lev3End
	e = new BioExpression(bioLev2Det.at(detSgNum-1), true);
	bioCond = bcg->addNewCondition(e, bioLev3End); // (send flow)
	for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
		bioCond->addTransferDroplet(toDetL2->at(detOff+i), tiL3->at(i));

	//////////////////////////////////////////////////////////////////////////
	// Lev2Rms1-->Lev2Det1, Lev2Rms2-->Lev2Det2, Lev2Rms3-->Lev2Det3
	for (int sgNum = 1; sgNum <= 3; sgNum++)
	{
		detOff = sgNum*numSplitsPerLevel*numTransPerSplit;
		rmsOff = sgNum*numSplitsPerLevel*numTransPerSplit;
		e = new BioExpression(bioLev2RMS.at(sgNum-1), true);
		bcg = BioSys->addBioCondGroup();
		bioCond = bcg->addNewCondition(e, bioLev2Det.at(sgNum-1)); // (send flow)
		for (int i = 0; i < (numTransPerSplit * numSplitsPerLevel); i++) //(send droplets)
			bioCond->addTransferDroplet(toRmsL2->at(rmsOff+i), tiDetL2->at(detOff+i));
	}

	// Generate CFG from Biocoder and output CFG and all DAGs
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->setName("BC_Conditional_B3_Protein_FaultTolerant_CFG");
	cfg->OutputGraphFile(cfg->getName(), true, true, true);

	// Cleanup
	delete tiRmsL2;
	delete toRmsL2;
	delete tiDetL2;
	delete toDetL2;
	delete detsL2;
	delete tiL3;

	cout << "Biocoder B3_Protein_FaultTolerant CFG Generated" << endl;
	return cfg;
}


///////////////////////////////////////////////////////////////////////////
// Helper method for B3_Protein_FaultTolerant()
///////////////////////////////////////////////////////////////////////////
BioCoder * BiocodeTest::B3_GenerateFtLevelCase_RMS(BioSystem *bs, string bcBaseName, int splitCode, int levelNum, vector<string> *tIns, vector<string> *tOuts, int mult)
{
	// Generate name of entire DAG
	stringstream ss;
	ss << bcBaseName << splitCode;
	string name = ss.str();
	BioCoder * rms = bs->addBioCoder(name);

	// Now decode the splitCode so we know which splits to do and which to just store
	//bool doSplit[(int)pow(2.0, levelNum-1)];
	vector<bool> doSplit;
	for (int i = 0; i < (int)pow(2.0, levelNum-1); i++)
	{
		doSplit.push_back((bool)((splitCode >> ( ((int)pow(2.0, levelNum-1)-1) - i )) & 1));
		//cout << doSplit[i];
	}
	//cout << endl;

	// Now, generate BioCode...either a split or just a store.
	int numTransPerRms = 2;
	int numRmsPerLevel = (int)pow(2.0, levelNum-1);
	for (int i = 0; i < (int)pow(2.0, levelNum-1); i++)
	{
		int transIndexOffset = (numTransPerRms * numRmsPerLevel * splitCode) + (i * numTransPerRms);
		Time time = rms->time(3*mult, SECS);
		Container tube1 = rms->new_container(STERILE_MICROFUGE_TUBE2ML);
		Container tube2 = rms->new_container(STERILE_MICROFUGE_TUBE2ML);

		if (i == 0)
			rms->first_step();
		else
			rms->next_step();

		if (doSplit.at(i))
		{	// MERGE AND RE-SPLIT (RMS)
			tIns->at(transIndexOffset) = rms->reuse_fluid(tube1);
			tIns->at(transIndexOffset+1) = rms->reuse_fluid(tube1);
			rms->next_step();
			rms->vortex(tube1, time);
			rms->next_step();
			rms->measure_fluid(tube1, 2, 1, tube2, false);
			rms->next_step();
			tOuts->at(transIndexOffset) = rms->save_fluid(tube1);
			tOuts->at(transIndexOffset+1) = rms->save_fluid(tube2);
		}
		else
		{	// JUST STORE DROPLET
			tIns->at(transIndexOffset) = rms->reuse_fluid(tube1);
			tIns->at(transIndexOffset+1) = rms->reuse_fluid(tube2);
			rms->next_step();
			tOuts->at(transIndexOffset) = rms->save_fluid(tube1);
			tOuts->at(transIndexOffset+1) = rms->save_fluid(tube2);
		}
	}
	rms->end_protocol();
	return rms;
}

///////////////////////////////////////////////////////////////////////////
// Helper method for B3_Protein_FaultTolerant()
///////////////////////////////////////////////////////////////////////////
BioCoder * BiocodeTest::B3_GenerateFtLevelCase_Det(BioSystem *bs, string bcBaseName, int detectCode, int levelNum, vector<string> *tIns, vector<string> *tOuts, vector<string> *dets, int mult)
{
	// Generate name of entire DAG
	stringstream ss;
	ss << bcBaseName << detectCode;
	string name = ss.str();
	BioCoder * det = bs->addBioCoder(name);

	// Now decode the splitCode so we know which splits to do and which to just store
	//bool doDetect[(int)pow(2.0, levelNum-1)];
	vector<bool> doDetect;
	for (int i = 0; i < (int)pow(2.0, levelNum-1); i++)
	{
		doDetect.push_back((bool)((detectCode >> ( ((int)pow(2.0, levelNum-1)-1) - i )) & 1));
		//cout << doSplit[i];
	}
	//cout << endl;

	// Now, generate BioCode...either a split or just a store.
	int numTransPerDet = 2;
	int numDetPerLevel = (int)pow(2.0, levelNum-1);
	for (int i = 0; i < (int)pow(2.0, levelNum-1); i++)
	{
		int transIndexOffset = (numTransPerDet * numDetPerLevel * detectCode) + (i * numTransPerDet);
		int detIndexOffset = (numDetPerLevel * detectCode) + i;
		Time time = det->time(3*mult, SECS);
		Time detTime = det->time(5*mult, SECS);
		Container tube1 = det->new_container(STERILE_MICROFUGE_TUBE2ML);
		Container tube2 = det->new_container(STERILE_MICROFUGE_TUBE2ML);

		if (i == 0)
			det->first_step();
		else
			det->next_step();

		if (doDetect.at(i))
		{	// DETECT ONE OF THE DROPLETS, STORE THE OTHER
			tIns->at(transIndexOffset) = det->reuse_fluid(tube1);
			tIns->at(transIndexOffset+1) = det->reuse_fluid(tube2);
			det->next_step();
			dets->at(detIndexOffset) = det->measure_fluorescence(tube1, detTime); // Need to update time accordingly
			det->next_step();
			tOuts->at(transIndexOffset) = det->save_fluid(tube1);
			tOuts->at(transIndexOffset+1) = det->save_fluid(tube2);
		}
		else
		{	// JUST STORE DROPLETS
			tIns->at(transIndexOffset) = det->reuse_fluid(tube1);
			tIns->at(transIndexOffset+1) = det->reuse_fluid(tube2);
			det->next_step();
			tOuts->at(transIndexOffset) = det->save_fluid(tube1);
			tOuts->at(transIndexOffset+1) = det->save_fluid(tube2);
		}
	}
	det->end_protocol();
	return det;
}

///////////////////////////////////////////////////////////////////////////
// Biocoder protocol containing conditional flow for Probabilistic PCR
///////////////////////////////////////////////////////////////////////////
static CFG *Create_Conditional_Probabilistic_PCR_CFG(double dnaThreshold, bool outputBiocoderGraphs)
{
	double mult = 1; // TimeStep multiplier
	double secPerTS = .5; //seconds per timestep
	int maxDim = 2;

	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);
	BioSys->InitializePCRProbablityModel(.8,0,pow(10,-4),.95);

	//TOTAL THRESH AND INITVALUES NEED TO BE FOUND.
	int Total = 20;
	int Threshold = 10;
	int Init = 9;//BioSys->PCRProbabiltyModel()->minNumberOfcyclesNeeded;

	///////////////////////////////////////////////////////////////////////////////
	//			Inital ThermoCycles
	///////////////////////////////////////////////////////////////////////////////
	vector<BioCoder*> InitDags;
	vector<string> InitDagsInputs;
	vector<string> InitDagsOutput;

	cout<<"Inital ThermoCycles" <<endl;

	for( int i = 1; i < Init; ++i)
	{
		cout<< i << " of " << Init << endl;
		char buffer[25];
		sprintf(buffer,"InitialThermocyclingStep%i",i);

		BioCoder * initialThermocycler= BioSys->addBioCoder(buffer);
		Time time = initialThermocycler->time(45, SECS);

		Container tube1 = initialThermocycler->new_container(STERILE_MICROFUGE_TUBE2ML);

		string transferIn;
		string transferOut;
		initialThermocycler->first_step();
		transferIn = initialThermocycler->reuse_fluid(tube1);

		//Heat
		initialThermocycler->next_step();
		initialThermocycler->store_for(tube1, 94, time, false);

		//Cool
		initialThermocycler->next_step();
		initialThermocycler->store_for(tube1, 65, time, true);

		initialThermocycler->next_step();
		transferOut = initialThermocycler->save_fluid(tube1);
		initialThermocycler->end_protocol();

		InitDags.push_back(initialThermocycler);
		InitDagsInputs.push_back(transferIn);
		InitDagsOutput.push_back(transferOut);

		initialThermocycler->printAssaySteps();
	}


	///////////////////////////////////////////////////////////////////////////////
	//			Inital Dispenses
	///////////////////////////////////////////////////////////////////////////////
	BioCoder * begin= BioSys->addBioCoder("Begin Dispense");
	Fluid PCRMix = begin->new_fluid("PCR",begin->vol(100,ML));
	Volume dropVol = begin->vol(10, UL);
	Time time = begin->time(45, SECS);

	//cout<<"Initial Dispenses" <<endl;

	Container tube1 = begin->new_container(STERILE_MICROFUGE_TUBE2ML);
	string initalTransferOut;
	begin->first_step(); 							//Dispense
	begin->measure_fluid(PCRMix, dropVol, tube1);
	//Heat
	begin->next_step();
	begin->store_for(tube1, 94, time, false);

	//Cool
	begin->next_step();
	begin->store_for(tube1, 65, time, true);

	begin->next_step();
	initalTransferOut = begin->save_fluid(tube1);
	begin->end_protocol();

	///////////////////////////////////////////////////////////////////////////////
	//				Thermocycle with Detect.
	///////////////////////////////////////////////////////////////////////////////
	vector<BioCoder*> EarlyDetectDags;
	vector<string> EarlyDetectDagsInputs;
	vector<string> EarlyDetectDagsOutputs;
	vector<string> EarlyDetectDagsSensors;
	cout<<"Thermocycle with Detect." <<endl;
	for (int i= Init; i < Threshold;++i)
	{
		cout<< i << " of " << Threshold << endl;
		char buffer[25];
		sprintf(buffer,"EarlyExitDetectionStep%i",i);

		BioCoder * EarlyExitDetection = BioSys->addBioCoder(buffer);
		Time time = EarlyExitDetection->time(45, SECS);

		tube1 = EarlyExitDetection->new_container(STERILE_MICROFUGE_TUBE2ML);
		string transferIn;
		string transferOut;
		string sensor1;

		EarlyExitDetection->first_step();
		transferIn = EarlyExitDetection->reuse_fluid(tube1);

		//Heat
		EarlyExitDetection->next_step();
		EarlyExitDetection->store_for(tube1, 94, time, false);

		//Cool
		EarlyExitDetection->next_step();
		EarlyExitDetection->store_for(tube1, 65, time, true);

		//Detect
		EarlyExitDetection->next_step();
		sensor1 = EarlyExitDetection->measure_fluorescence(tube1, EarlyExitDetection->time(5, SECS));

		EarlyExitDetection->next_step();
		transferOut = EarlyExitDetection->save_fluid(tube1);

		EarlyExitDetection->end_protocol();
		EarlyDetectDags.push_back(EarlyExitDetection);
		EarlyDetectDagsInputs.push_back(transferIn);
		EarlyDetectDagsOutputs.push_back(transferOut);
		EarlyDetectDagsSensors.push_back(sensor1);
	}

	///////////////////////////////////////////////////////////////////////////////
	//				Finish THermo
	///////////////////////////////////////////////////////////////////////////////
	vector<BioCoder*> PCRDags;
	vector<string> PCRDagsInputs;
	vector<string> PCRDagsOutput;
	cout<<"Finish THermo"<<endl;
	for( int i = Init+1; i < Total; ++i)
	{
		cout<< i << " of " << Total << endl;

		char buffer[25];
		sprintf(buffer,"FinishThermocyclingStep%i",i);
		BioCoder * finalThermocycler= BioSys->addBioCoder(buffer);
		Time time = finalThermocycler->time(45, SECS);

		tube1 = finalThermocycler->new_container(STERILE_MICROFUGE_TUBE2ML);

		string transferIn ="";
		string transferOut;
		finalThermocycler->first_step();
		transferIn = finalThermocycler->reuse_fluid(tube1);

		//Heat
		finalThermocycler->next_step();
		finalThermocycler->store_for(tube1, 94, time, false);

		//Cool
		finalThermocycler->next_step();
		finalThermocycler->store_for(tube1, 65, time, true);

		finalThermocycler->next_step();
		transferOut = finalThermocycler->save_fluid(tube1);
		finalThermocycler->end_protocol();

		PCRDags.push_back(finalThermocycler);
		PCRDagsInputs.push_back(transferIn);
		PCRDagsOutput.push_back(transferOut);
	}

	///////////////////////////////////////////////////////////////////////////////
	//				PROCESS PCR
	///////////////////////////////////////////////////////////////////////////////
	cout<<"ProcessPCR" <<endl;
	BioCoder * processPCR= BioSys->addBioCoder("PROCESS PCR");
	tube1 = processPCR->new_container(STERILE_MICROFUGE_TUBE2ML);

	string processTransferIn;
	processPCR->first_step();
	processTransferIn = processPCR->reuse_fluid(tube1);

	processPCR->next_step();
	processPCR->drain(tube1, "AmplifiedPCR");

	processPCR->end_protocol();

	///////////////////////////////////////////////////////////////////////////////
	//				Exit Early
	///////////////////////////////////////////////////////////////////////////////
	cout<<"Exit Early" <<endl;
	BioCoder * bioQuit= BioSys->addBioCoder("QUIT EARLY");
	tube1 = bioQuit->new_container(STERILE_MICROFUGE_TUBE2ML);

	string quitTranferIn;
	bioQuit->first_step();
	quitTranferIn = bioQuit->reuse_fluid(tube1);

	bioQuit->next_step();
	bioQuit->drain(tube1, "waste");

	bioQuit->end_protocol();

	///////////////////////////////////////////////////////////////////////////////
	//			Inital CFG controls  Dispense to Initial THermo
	///////////////////////////////////////////////////////////////////////////////
	BioConditionalGroup *bcg = BioSys->addBioCondGroup();

	BioExpression *e = new BioExpression(begin,true);  //unconditional branches.
	BioCondition *bioCond = bcg->addNewCondition(e, InitDags[0]); //jump to next dag.
	bioCond->addTransferDroplet(initalTransferOut, InitDagsInputs[0]);

	///////////////////////////////////////////////////////////////////////////////
	//				  CFG control : Initial THermo to Early Exit detection
	///////////////////////////////////////////////////////////////////////////////
	for(unsigned int i = 0 ; i < InitDags.size()-1;++i)
	{
		BioConditionalGroup *bcg = BioSys->addBioCondGroup();

		BioExpression *e = new BioExpression(InitDags[i],true);  //unconditional branches.
		BioCondition* bioCond = bcg->addNewCondition(e, InitDags[i+1]); //jump to next dag.
		bioCond->addTransferDroplet(InitDagsOutput[i],InitDagsInputs[i+1]);
	}
	bcg = BioSys->addBioCondGroup();

	e = new BioExpression(InitDags[InitDags.size()-1],true);  //unconditional branches.
	bioCond = bcg->addNewCondition(e, EarlyDetectDags[0]); //jump to next dag.
	bioCond->addTransferDroplet(InitDagsOutput[InitDagsOutput.size()-1], EarlyDetectDagsInputs[0]);


	///////////////////////////////////////////////////////////////////////////////
	//		 CFGs controls EarlyDetection to early Quit OR FinishTHermo
	///////////////////////////////////////////////////////////////////////////////
	for(unsigned int i= 0 ; i < EarlyDetectDags.size()-1; ++i)
	{
		BioConditionalGroup *bcg = BioSys->addBioCondGroup();
		e = new BioExpression(EarlyDetectDagsSensors[i],OP_LT,dnaThreshold);
		bioCond = bcg->addNewCondition(e,PCRDags[i]);//if branch
		bioCond->addTransferDroplet(EarlyDetectDagsOutputs[i], PCRDagsInputs[i]);
		bioCond = bcg->addNewCondition(NULL,EarlyDetectDags[i+1]); //else branch
		bioCond->addTransferDroplet(EarlyDetectDagsOutputs[i], EarlyDetectDagsInputs[i+1]);

	}

	bcg = BioSys->addBioCondGroup();
	e = new BioExpression(EarlyDetectDagsSensors[EarlyDetectDags.size()-1],OP_LT,dnaThreshold);
	bioCond = bcg->addNewCondition(e,PCRDags[EarlyDetectDags.size()-1]);//if branch
	bioCond->addTransferDroplet(EarlyDetectDagsOutputs[EarlyDetectDags.size()-1], PCRDagsInputs[EarlyDetectDags.size()-1]);
	bioCond = bcg->addNewCondition(NULL,bioQuit); //else branch
	bioCond->addTransferDroplet(EarlyDetectDagsOutputs[EarlyDetectDags.size()-1], quitTranferIn);

	///////////////////////////////////////////////////////////////////////////////
	//				 CFGs control Finish Thermo to process PCR
	///////////////////////////////////////////////////////////////////////////////
	for(int i = 0 ; i < PCRDags.size()-1;++i)
	{
		BioConditionalGroup *bcg = BioSys->addBioCondGroup();

		BioExpression *e = new BioExpression(PCRDags[i],true);  //unconditional branches.
		BioCondition* bioCond = bcg->addNewCondition(e, PCRDags[i+1]); //jump to next dag.
		bioCond->addTransferDroplet(PCRDagsOutput[i],PCRDagsInputs[i+1]);
	}
	bcg = BioSys->addBioCondGroup();

	e = new BioExpression(PCRDags[PCRDags.size()-1],true);  //unconditional branches.
	bioCond = bcg->addNewCondition(e, processPCR); //jump to next dag.
	bioCond->addTransferDroplet(PCRDagsOutput[PCRDagsOutput.size()-1],processTransferIn);

	///////////////////////////////////////////////////////////////////////////////
	//			Generate CFG from Biocoder and output CFG and all DAGs
	///////////////////////////////////////////////////////////////////////////////
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->OutputGraphFile("BC_Conditional_ProbabilisticPCR_CFG", true, true, true);
	cout << "BioCoder Conditional Probabilistic PCR CFG Generated" << endl;
	return cfg;
}

///////////////////////////////////////////////////////////////////////////
// Biocoder protocol containing conditional flow for PCR Droplet Replacement
///////////////////////////////////////////////////////////////////////////
CFG * BiocodeTest::Create_Conditional_PCR_Droplet_Replacement_CFG(double volumeTolerance, bool outputBiocoderGraphs)
{
	const int THERMOCYCLESTEPS=9;

	double mult = 1; // TimeStep multiplier
	double secPerTS = .5; //seconds per timestep
	int maxDim = 2;

	BioSystem * BioSys= new BioSystem();
	BioSys->setOutputGraphs(outputBiocoderGraphs);

	///////////////////////////////////////////////////////////////////////////////
	//			Inital Dispenses
	///////////////////////////////////////////////////////////////////////////////
	BioCoder * begin= BioSys->addBioCoder("Dispense");
	Fluid Replacement = begin->new_fluid("ReplacementMix",begin->vol(100,ML));
	Fluid PCRMasterMix = begin->new_fluid("PCRMasterMix",begin->vol(100,ML));
	Fluid Primers = begin->new_fluid("Primers",begin->vol(100,ML));
	Fluid Template = begin->new_fluid("template",begin->vol(100,ML));

	Volume dropVol = begin->vol(10, UL);
	Time seconds45 = begin->time(45, SECS);
	Time seconds30 = begin->time(30, SECS);
	Time seconds20 = begin->time(20, SECS);
	Time minutes5 = begin->time(5, MINS);
	Time seconds1 = begin->time(1,SECS);

	//cout<<"Initial Dispenses" <<endl;

	Container tube = begin->new_container(STERILE_MICROFUGE_TUBE2ML);
	Container tube1 = begin->new_container(STERILE_MICROFUGE_TUBE2ML);

	string initalTransferOut;
	begin->first_step(); 							//Dispense
	begin->measure_fluid(Replacement, dropVol, tube1);
	//begin->measure_fluid(Primers, dropVol, tube1);

	begin-> next_step();
	begin->vortex(tube1, seconds1);

	begin-> next_step();
	begin->measure_fluid(Template, dropVol,tube1);

	begin-> next_step();
	begin->vortex(tube1,seconds1);

	//Heat
	begin->next_step();
	begin->store_for(tube1, 95, seconds45, false);
	begin->next_step();
	//begin->drain(tube1, "AmplifiedPCR");
	initalTransferOut = begin->save_fluid(tube1);
	begin->end_protocol();

	///////////////////////////////////////////////////////////////////////////////
	//			Refresh volume
	///////////////////////////////////////////////////////////////////////////////
	vector<BioCoder* > refreshDAGS;
	vector<string > refreshDAGSInputs;
	vector<string > refreshDAGSOutputs;
	for(int i = 0; i<THERMOCYCLESTEPS;++i)
	{
		char buffer[25];
		sprintf(buffer,"ReactantReplenishStep%i",i);
		//cout << buffer <<endl;
		BioCoder * bioRefresh= BioSys->addBioCoder(buffer);
		//Fluid Replacement = bioRefresh->new_fluid("ReplacementMix",bioRefresh->vol(100,ML));
		tube1 = bioRefresh->new_container(STERILE_MICROFUGE_TUBE2ML);
		tube = bioRefresh->new_container(STERILE_MICROFUGE_TUBE2ML);

		string transferOut;
		bioRefresh->first_step(); 							//Dispense
		bioRefresh->measure_fluid(Replacement, dropVol, tube1);

		bioRefresh->next_step();
		bioRefresh->store_for(tube1, 95, seconds45, false);

		bioRefresh->next_step();
		string transferIn = bioRefresh->reuse_fluid(tube);

		bioRefresh->next_step();
		bioRefresh->measure_fluid(tube, tube1);
		bioRefresh->vortex(tube1,seconds1);

		bioRefresh->next_step();
		transferOut = bioRefresh->save_fluid(tube1);
		bioRefresh->end_protocol();

		refreshDAGS.push_back(bioRefresh);
		refreshDAGSInputs.push_back(transferIn);
		refreshDAGSOutputs.push_back(transferOut);
	}
	///////////////////////////////////////////////////////////////////////////////
	//			Cycle with droplet replacement
	///////////////////////////////////////////////////////////////////////////////
	vector<BioCoder* > thermoHeadDAGS;
	vector<string > thermoHeadDAGSensors;
	vector<string> thermoHeadDAGInputs;
	vector<string > thermoHeadDAGOutputs;
	for(int i = 0; i<THERMOCYCLESTEPS;++i)
	{
		char buffer[25];
		sprintf(buffer,"ThermoHeadStep%i",i);
		//cout << buffer <<endl;
		BioCoder * Thermocycler= BioSys->addBioCoder(buffer);

		tube1 = Thermocycler->new_container(STERILE_MICROFUGE_TUBE2ML);

		string transferIn;

		Thermocycler->first_step();
		transferIn = Thermocycler->reuse_fluid(tube1);

		//Thermocycler-> first_step();
		//Thermocycler->measure_fluid(Replacement, dropVol,tube1);


		Thermocycler->next_step();
		Thermocycler->store_for(tube1, 95, seconds20, false);

		Thermocycler->next_step();
		string sensor = Thermocycler->weigh(tube1);
		thermoHeadDAGSensors.push_back(sensor);

		Thermocycler->next_step();
		string transferOut = Thermocycler->save_fluid(tube1);
		Thermocycler->end_protocol();
		thermoHeadDAGS.push_back(Thermocycler);
		thermoHeadDAGOutputs.push_back(transferOut);
		thermoHeadDAGInputs.push_back(transferIn);
	}

	///////////////////////////////////////////////////////////////////////////////
	//			Tail TermoCycler
	///////////////////////////////////////////////////////////////////////////////
	vector<BioCoder* > thermoTailDAGS;

	vector<string> thermoTailDAGInputs;
	vector<string> thermoTailDAGOutputs;
	for(int i = 0; i<THERMOCYCLESTEPS;++i)
	{
		char buffer[25];
		sprintf(buffer,"TailStep%i",i);
		//cout << buffer <<endl;

		BioCoder * tailThermo= BioSys->addBioCoder(buffer);
		tube1 = tailThermo->new_container(STERILE_MICROFUGE_TUBE2ML);


		tailThermo->first_step();
		string transferIn = tailThermo->reuse_fluid(tube1);

		tailThermo->next_step();
		tailThermo->store_for(tube1, 50, seconds30, false);

		tailThermo->next_step();
		tailThermo->store_for(tube1, 68, seconds45, false);


		if(i==THERMOCYCLESTEPS-1)
		{
			tailThermo->next_step();
			tailThermo->store_for(tube1, 68, minutes5, false);

			tailThermo->next_step();
			tailThermo->drain(tube1, "AmplifiedPCR");
		}
		else
		{
			tailThermo->next_step();

			string transferOut = tailThermo->save_fluid(tube1);
			thermoTailDAGOutputs.push_back(transferOut);
		}

		tailThermo->end_protocol();
		//tailThermo->printAssaySteps();
		//tailThermo->printLinks();

		thermoTailDAGS.push_back(tailThermo);

		thermoTailDAGInputs.push_back(transferIn);
	}

	//Thermocycler->printAssaySteps();
	//Thermocycler->printLinks();
	///////////////////////////////////////////////////////////////////////////////
	//			Inital CFG controls  Dispense to Initial THermo
	///////////////////////////////////////////////////////////////////////////////
	//cout <<"Initial CFG"<<endl;
	BioConditionalGroup *bcg = BioSys->addBioCondGroup();

	BioExpression *e = new BioExpression(begin,true);  //unconditional branches.
	BioCondition *bioCond = bcg->addNewCondition(e, thermoHeadDAGS[0]); //jump to next dag.
	bioCond->addTransferDroplet(initalTransferOut, thermoHeadDAGInputs[0]);

	///////////////////////////////////////////////////////////////////////////////
	//			Tail Thermo to Head Thermo Loop
	///////////////////////////////////////////////////////////////////////////////
	//cout <<"Tail to head"<<endl;
	for(int i = 0 ; i<THERMOCYCLESTEPS-1; ++i )
	{
		BioConditionalGroup *bcg = BioSys->addBioCondGroup();

		BioExpression *e = new BioExpression(thermoTailDAGS[i],true);  //unconditional branches.
		BioCondition *bioCond = bcg->addNewCondition(e, thermoHeadDAGS[i+1]); //jump to next dag.
		bioCond->addTransferDroplet(thermoTailDAGOutputs[i], thermoHeadDAGInputs[i+1]);
	}
	///////////////////////////////////////////////////////////////////////////////
	//			Refresh Dag to Tail Thermo
	///////////////////////////////////////////////////////////////////////////////
	//cout <<"Refresh to tail"<<endl;
	for(int i = 0 ; i<THERMOCYCLESTEPS-1; ++i )
	{
		BioConditionalGroup *bcg = BioSys->addBioCondGroup();

		BioExpression *e = new BioExpression(refreshDAGS[i],true);  //unconditional branches.
		BioCondition *bioCond = bcg->addNewCondition(e, thermoTailDAGS[i]); //jump to next dag.
		bioCond->addTransferDroplet(refreshDAGSOutputs[i], thermoTailDAGInputs[i]);
	}

	///////////////////////////////////////////////////////////////////////////////
	//			Head thermo to replenish or tail Thermo
	///////////////////////////////////////////////////////////////////////////////
	//cout <<"head to tail"<<endl;
	for(int i = 0 ; i<THERMOCYCLESTEPS; ++i )
	{
		bcg = BioSys->addBioCondGroup();
		if(i==THERMOCYCLESTEPS -1)
			e = new BioExpression(thermoHeadDAGSensors[i],OP_GT,.99);
		else
			e = new BioExpression(thermoHeadDAGSensors[i],OP_LT,volumeTolerance);

		bioCond = bcg->addNewCondition(e,refreshDAGS[i]);//if branch
		bioCond->addTransferDroplet(thermoHeadDAGOutputs[i],refreshDAGSInputs[i]);
		bioCond = bcg->addNewCondition(NULL,thermoTailDAGS[i]); //else branch
		bioCond->addTransferDroplet(thermoHeadDAGOutputs[i], thermoTailDAGInputs[i]);
	}

	///////////////////////////////////////////////////////////////////////////////
	//			Generate CFG from Biocoder and output CFG and all DAGs
	///////////////////////////////////////////////////////////////////////////////
	CFG *cfg = BioSys->GetDmfbExecutableCFG();
	cfg->setName("BC_Conditional_PCRDropletReplacement_CFG");
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
	cout << "BioCoder Conditional PCR Droplet Replacement CFG Generated" << endl;
	return cfg;
}
