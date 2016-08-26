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
/*--------------------------------Class Details---------------------------------*
 * Name: Biocode Test (BioCode Test)											*
 *																				*
 * Details: Creates DAGs for several assays using the BioCoder language.		*
 *-----------------------------------------------------------------------------*/
#ifndef _BIOCODE_TEST_H
#define _BIOCODE_TEST_H

using namespace std;
#include "../../Headers/BioCoder/BioSystem.h"
#include "../../Headers/BioCoder/BioCoder.h"
#include "../../Headers/Models/assay_node.h"
#include "../../Headers/Resources/enums.h"
#include "../../Headers/Models/dag.h"
#include <stdlib.h>
#include <stdio.h>
#include<list>
#include <map>

class BiocodeTest
{
    public:
        // Constructors
		BiocodeTest();
        virtual ~BiocodeTest ();

        // Benchmarks from http://people.ee.duke.edu/~fs/Benchmark.pdf
        static DAG *Create_B1_PCRMix(double mult);
        static DAG *Create_B2_InVitroDiag(int numSamples, int numReagents);
        static DAG *Create_B3_Protein(double mult);
        static DAG *Create_B3_Protein_Levels(int splitLevels, int eq1_inc2_dec3, double mult);

        // Other homemade benchmarks and tests
        static DAG *Create_Operation_Demonstration_Bench();
        static DAG *Create_2LevelProtein_With_SplitVolumeDetects(); // Used for JETC journal
        static DAG *Create_2LevelProtein_Lev1FailureSubGraph(); // Used for JETC journal
        static DAG *Create_2LevelProtein_Lev2SingleFailureSubGraph(); // Used for JETC journal
        static DAG *Create_2LevelProtein_Lev2DoubleFailureSubGraph(); // Used for JETC journal


        // Biocode Conditional benchmarks
        static CFG *Create_Conditional_Demo_CFG(bool outputBiocoderGraphs);
        static CFG *Create_Simple_Conditional_With_Transfer_Droplets_Demo_CFG(bool outputBiocoderGraphs);
        static CFG *Create_Compound_Conditional_With_Transfer_Droplets_Demo_CFG(bool outputBiocoderGraphs);
        static CFG *Create_Conditional_With_Transfer_Droplets_Demo_CFG(bool outputBiocoderGraphs);
        static CFG *Create_Conditional_B2_InVitroDiag_CFG(int numSamples, int numReagents, string inputArchFile, double mult, double secPerTS, int repeat, bool outputBiocoderGraphs);
        static CFG *Create_Conditional_B3_Protein_FaultTolerant_CFG(double mult, double secPerTS, int repeat, bool outputBiocoderGraphs);
        static CFG *Create_Conditional_Probabilistic_PCR_CFG(double dnaThreshold, bool outputBiocoderGraphs);
        static CFG *Create_Conditional_PCR_Droplet_Replacement_CFG(double VolumeTolerance, bool outputBiocoderGraphs);

    private:
        // Internal methods (used by
        static BioCoder * B3_GenerateFtLevelCase_RMS(BioSystem *bs, string bcBaseName, int splitCode, int levelNum, vector<string> *tIns, vector<string> *tOuts, int mult);
        static BioCoder * B3_GenerateFtLevelCase_Det(BioSystem *bs, string bcBaseName, int detectCode, int levelNum, vector<string> *tIns, vector<string> *tOuts, vector<string> *dets, int mult);
};
#endif
