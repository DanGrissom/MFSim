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
#include "../Headers/Testing/test.h"
#include "../Headers/Testing/demo.h"
#include "../Headers/command_line.h"
#include "../Headers/Resources/enums.h"
#include "../Headers/compatibility_check.h"
#include <math.h>

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

			//////////////////////////////////////////////////////////////////////
			// Demos executing DAGs on individually addressable DMFBs
			Demo::IndividuallyAddressableDMFB(true, true, true, PCR_BM);
			//Demo::RoutingBasedSynthesis(true, PROTEIN_BM);

			//////////////////////////////////////////////////////////////////////
			// Demos executing DAGs on pin-constrained DMFBs
			//Demo::FieldProgrammablePinConstrainedDMFB(true, true, true, PROTEIN_SPLIT_1_BM);
			//Demo::CliquePartitionedDMFB(true, false, false, PCR_BM);

			//////////////////////////////////////////////////////////////////////
			// Demos performing wire routing on DMFBs
			//for (int i = 20; i <= 20; i++)
			//	Demo::WireRoutingCase(3, i);
			//Demo::PinMapWireRouteFlow(SWITCH_PM, PIN_MAPPER_INHERENT_WR, 3, 10);
			//Demo::PinMapWireRouteFlow(CLIQUE_PM, PATH_FINDER_WR, 3, 10);

			//////////////////////////////////////////////////////////////////////
			// Method for trying your own implementation flows
			//Test::MySandboxCode();

			//////////////////////////////////////////////////////////////////////
			// Demos executing CFGs on individually addressable DMFBs
			//Demo::Create_Compile_Execute_Biocoder_CFG(1);

			//////////////////////////////////////////////////////////////////////
			// Other Utility Methods
			//Test::CFGFileIOTest();

			cout << "-----------------------------------------------" << endl << "Exiting the UCR DMFB SSS Simulator" << endl;
		}
	}

	return 0;
}

#endif
