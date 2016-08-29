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
 * Source: main.cc																*
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
#ifndef _MAIN
#define _MAIN

using namespace std;
#include "../Headers/Testing/biocode_test.h"
#include "../Headers/Testing/test.h"
#include "../Headers/Testing/demo.h"
#include "../Headers/command_line.h"
#include "../Headers/synthesis.h"
#include "../Headers/Resources/enums.h"
#include "../Headers/compatibility_check.h"
#include <math.h>

#include "../Headers/Models/compiled_cfg.h"
#include "../Headers/Models/cfg.h"

// TEMPORARY - DTG
#include "../Headers/Util/file_out.h"

///////////////////////////////////////////////////////////////////////////////////
// Main function called when starting program.
///////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	////////////////////////////////////////////////////////////////////////////////////
	// Should be set to true if this C++ simulator is being compiled with the intention
	// to call the executable from the Java GUI. If you are merely running C++ from an
	// IDE (e.g., Eclipse) with hardcoded simulations in the "sandbox" below, then set
	// this variable to false to prevent the program from demanding command-line params.
	boolean executedViaJavaGUI = false;

	CmdLine cmdLine;
	if (cmdLine.IsUsingCommandLine(argc))
		cmdLine.ExecuteCommand(argc, argv);
	else // Hard-coded Sandbox
	{
		if (executedViaJavaGUI)
			cmdLine.ForceCorrectUsage(); // Comment out to be able to run following code
		else
		{
			cout << "Executing Hard-coded DMFB Simulation" << endl << "-----------------------------------------------"<< endl;

			//Demo::FieldProgrammablePinConstrainedDMFB(true, true, true, PROTEIN_SPLIT_1_BM);
			//Test::RoutingBasedSynthesis(true, PROTEIN_BM);
			//Demo::IndividuallyAddressableDMFB(true, true, false, PCR_BM);
			//Demo::CliquePartitionedDMFB(true, false, false, PCR_BM);
			//for (int i = 1; i <= 19; i++)
			//	Demo::WireRoutingCase(3, i);
			//Demo::PinMapWireRouteFlow(SWITCH_PM, PIN_MAPPER_INHERENT_WR, 3, 10);
			//Demo::PinMapWireRouteFlow(CLIQUE_PM, PATH_FINDER_WR, 3, 10);
			//Demo::MySandboxCode();

			// Non-BioCoder Conditional Tests
			//CFG *cfg = Demo::Create_Conditional_Demo_CFG();
			//CFG *cfg = Demo::Create_FaultTolerant_Split_Demo1_CFG();
			//CFG *cfg = Demo::Create_FaultTolerant_Split_Demo2_CFG();

			// SSA Dynamic System
			Synthesis *synthesisEngine = Synthesis::CreateNewSynthesisEngine("DmfbArchs/IndividuallyAddressable/Testing/Arch_15_19_SampleReagent.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, false, GRISSOM_FIX_0_RA, INDIVADDR_PM, PATH_FINDER_WR, BEG_COMP, FIXED_PE, ALL_EX, 2, 1, 3, 3);
			//CFG *cfg = BiocodeTest::Create_Conditional_Demo_CFG(false);										// LS Pass; LEB Pass; RMR-R Pass; File I/O Pass
			//CFG *cfg = BiocodeTest::Create_Simple_Conditional_With_Transfer_Droplets_Demo_CFG(false);			// LS Pass; LEB Pass; RMR-R Pass; File I/O Pass
			//CFG *cfg = BiocodeTest::Create_Compound_Conditional_With_Transfer_Droplets_Demo_CFG(false);		// LS Pass; LEB Pass; RMR-R Pass; File I/O Pass
			CFG *cfg = BiocodeTest::Create_Conditional_Runcount_With_Transfer_Droplets_Demo_CFG(false);			// LS Pass; LEB Pass; RMR-R Pass; File I/O Pass
			//CFG *cfg = BiocodeTest::Create_Conditional_With_Transfer_Droplets_Demo_CFG(false);				// LS Pass; LEB Pass; RMR-R Pass (Merge case); Complete; File I/O Pass	 	//*Good Example
			//CFG *cfg = BiocodeTest::Create_Conditional_PCR_Droplet_Replacement_CFG(.8, false);					// LS Pass; LEB Pass; RMR-R Pass; File I/O Pass
			//CFG *cfg = BiocodeTest::Create_Conditional_Probabilistic_PCR_CFG(.8, false);						// LS Pass; LEB Pass; RMR-R Pass; File I/O Pass

			//Synthesis *synthesisEngine = Synthesis::CreateNewSynthesisEngine("DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, false, GRISSOM_FIX_0_RA, INDIVADDR_PM, PATH_FINDER_WR, BEG_COMP, FIXED_PE, ALL_EX, 2, 1, 3, 3);
			//CFG *cfg = BiocodeTest::Create_Conditional_B2_InVitroDiag_CFG(4, 4, "none", 1, 1, 1, false);		// LS Pass; LEB Pass; RMR-R Pass

			//Synthesis *synthesisEngine = Synthesis::CreateNewSynthesisEngine("DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, false, GRISSOM_FIX_0_RA, INDIVADDR_PM, PATH_FINDER_WR, BEG_COMP, FIXED_PE, ALL_EX, 2, 1, 3, 3);
			//CFG *cfg = BiocodeTest::Create_Conditional_B3_Protein_FaultTolerant_CFG(1, 1, 1, false);			// LS Pass; LEB Pass; RMR-R Pass (Merge cases)

			// Testing CFG file I/O methods
			//FileOut::WriteCfgToFile(cfg, "Test/", cfg->getName() + ".cfg");
			//CFG *cfgIn = FileIn::ReadCfgFromFile("Test/" +  cfg->getName() + ".cfg");

			CompiledCFG *compiledCFG = new CompiledCFG(synthesisEngine, cfg);
			compiledCFG->execute();
			compiledCFG->outputSimulationFiles();
			delete compiledCFG;
			delete cfg;
			//delete cfgIn;

			cout << "-----------------------------------------------" << endl << "Exiting the UCR DMFB SSS Simulator" << endl;
		}
	}

	return 0;
}

#endif
