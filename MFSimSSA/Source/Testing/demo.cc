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
 * Source: demo.cc																*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: July 6, 2015								*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/

#include "../../Headers/Testing/demo.h"
#include "../../Headers/Resources/enums.h"
#include "../../Headers/synthesis.h"
#include "../../Headers/Models/cfg.h"

///////////////////////////////////////////////////////////////////////////
// An empty method for you (me, who?) to write your own code in a clean,
// contained method for trying out specific cases.
///////////////////////////////////////////////////////////////////////////
void Demo::MySandboxCode()
{

}

///////////////////////////////////////////////////////////////////////////
// This method provides a demo of the Field-programmable, pin-constrained DMFB,
// which is explained in detail in the following publications:
//
// D. Grissom, J. McDaniel and P. Brisk
// A Low-cost Field-Programmable Pin-Constrained Digital Microfluidic Biochip
// IEEE Transactions on Computer-Aided Design (TCAD) of Integrated Circuits and Systems
// Vol. 33, No. 11, October, 2014, pp. 1657-1670
//
// D. Grissom and P. Brisk
// A Field-Programmable Pin-Constrained Digital Microfluidic Biochip
// 50th Design Automation Conference (DAC)
// Austin, TX, USA, June 2-6, 2013, Article No. 46
//
// This demo uses the newer paper's architecture files found in
// "DmfbArchs/FPPC/Enhanced_TCAD/"; the older architecture files can be found
// and used in "DmfbArchs/FPPC/Original_DAC_13/" if desired. Please note,
// however, that the older DAC 2013 version does not have the capability for
// parallel routing, wash droplets or specialized FPPC wire routing (standard routing
// techniques can be applied by setting userFPPCWireRouter to false).
//
// In light of this, if the older architecture files are used, fppcPinMapType should
// be set to ORIGINAL_FPPC_PM and fppcRouterType to FPPC_SEQUENTIAL_R.
///////////////////////////////////////////////////////////////////////////
void Demo::FieldProgrammablePinConstrainedDMFB(bool isParallel, bool performWash, bool useFPPCWireRouter, CommonBenchmarkType benchmark)
{
	// Setup pin-mapping, droplet-routing and wire-routing type....
	PinMapType fppcPinMapType;
	RouterType fppcRouterType;
	WireRouteType wireRouterType;

	// ...based on parameters
	if (isParallel)
	{
		fppcPinMapType = ENHANCED_FPPC_ROUTE_OPT_PM;
		fppcRouterType = FPPC_PARALLEL_R;
	}
	else
	{
		fppcPinMapType = ENHANCED_FPPC_PIN_OPT_PM;
		fppcRouterType = FPPC_SEQUENTIAL_R;
	}

	if (useFPPCWireRouter)
		wireRouterType = ENHANCED_FPPC_WR;
	else
		wireRouterType = PATH_FINDER_WR;


	// Schedule the requested benchmark...
	switch (benchmark)
	{
	case PCR_BM:
		Synthesis::Schedule("Assays/B1/MixSplit/PCR.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B1.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case IN_VITRO_1_BM:
		Synthesis::Schedule("Assays/B2/InVitro_Ex1_2s_2r.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case IN_VITRO_2_BM:
		Synthesis::Schedule("Assays/B2/InVitro_Ex2_2s_3r.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case IN_VITRO_3_BM:
		Synthesis::Schedule("Assays/B2/InVitro_Ex3_3s_3r.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case IN_VITRO_4_BM:
		Synthesis::Schedule("Assays/B2/InVitro_Ex4_3s_4r.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case IN_VITRO_5_BM:
		Synthesis::Schedule("Assays/B2/InVitro_Ex5_4s_4r.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case PROTEIN_SPLIT_1_BM:
		Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_01_Eq.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B3.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_PATH_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case PROTEIN_SPLIT_2_BM:
		Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_02_Eq.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B3.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_PATH_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case PROTEIN_SPLIT_3_BM:
		Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_03_Eq.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B3.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_PATH_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	case PROTEIN_BM:
		Synthesis::Schedule("Assays/B3/MixSplit/Protein.txt", "DmfbArchs/FPPC/Enhanced_TCAD/Arch_FPPC2_B3.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_PATH_S, PC_INHERENT_RA, fppcPinMapType, 1);
		break;
	default:
		claim(false, "Unknown benchmark not currently implemented into the FieldProgrammablePinConstrainedDMFB() demo.");
		return;
	}

	// ...place, droplet route and wire-route the scheduled assay
	Synthesis::Place("Output/1_SCHED_to_PLACE.txt", "Output/2_PLACE_to_ROUTE.txt", FPPC_LE_B, 1);
	Synthesis::Route("Output/2_PLACE_to_ROUTE.txt", fppcRouterType, performWash, INHERENT_COMP, FPPC_PE, ALL_EX);
	Synthesis::WireRoute("Output/3_ROUTE_to_SIM.txt", wireRouterType, 3, 3);
}

///////////////////////////////////////////////////////////////////////////
// This method provides demos for free-placed and virtual-topology bound
// DMFB simulations for individually addressable and clique-partioned pin-
// mapped DMFBs.
///////////////////////////////////////////////////////////////////////////
void Demo::ProgrammableDMFB(bool isParallel, bool useVirtualTopology, PinMapType pmType, bool performWireRouting, CommonBenchmarkType benchmark)
{
	////////////////////////////////////////////////////////////////////////////
	// Synthesis Variables
	////////////////////////////////////////////////////////////////////////////
	int maxDropsPerStorageMod = 2;
	int minCellsBetweenIrMods = 1;
	int numHorizTracks = 3;
	int numVertTracks = 3;

	// Setup types....
	CompactionType compactionType;
	PlacerType placerType;
	ProcessEngineType peType;

	// ...based on parameters
	if (isParallel)
		compactionType = BEG_COMP;
	else
		compactionType = NO_COMP;

	if (useVirtualTopology)
	{
		placerType = GRISSOM_PATH_B;
		peType = FIXED_PE;
	}
	else
	{
		placerType = KAMER_LL_P;
		peType = FREE_PE;
	}

	// Schedule the requested benchmark... (notice the use of Path Scheduler (PATH_S) for the protein assays)
	switch (benchmark)
	{
		case PCR_BM:
			Synthesis::Schedule("Assays/B1/MixSplit/PCR.txt", "DmfbArchs/IndividuallyAddressable/B1/Arch_15_19_B1.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case IN_VITRO_1_BM:
			Synthesis::Schedule("Assays/B2/InVitro_Ex1_2s_2r.txt", "DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case IN_VITRO_2_BM:
			Synthesis::Schedule("Assays/B2/InVitro_Ex2_2s_3r.txt", "DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case IN_VITRO_3_BM:
			Synthesis::Schedule("Assays/B2/InVitro_Ex3_3s_3r.txt", "DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case IN_VITRO_4_BM:
			Synthesis::Schedule("Assays/B2/InVitro_Ex4_3s_4r.txt", "DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case IN_VITRO_5_BM:
			Synthesis::Schedule("Assays/B2/InVitro_Ex5_4s_4r.txt", "DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case PROTEIN_SPLIT_1_BM:
			Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_01_Eq.txt", "DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case PROTEIN_SPLIT_3_BM:
			Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_03_Eq.txt", "DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case PROTEIN_SPLIT_5_BM:
			Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_05_Eq.txt", "DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		case PROTEIN_BM:
			Synthesis::Schedule("Assays/B3/MixSplit/Protein.txt", "DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_0_RA, pmType, maxDropsPerStorageMod);
			break;
		default:
			claim(false, "Unknown benchmark not currently implemented into the IndividuallyAddressableDMFB() demo.");
			return;
	}

	// ...place, droplet route and wire-route the scheduled assay
	Synthesis::Place("Output/1_SCHED_to_PLACE.txt", "Output/2_PLACE_to_ROUTE.txt", placerType, minCellsBetweenIrMods);
	Synthesis::Route("Output/2_PLACE_to_ROUTE.txt", ROY_MAZE_R, false, compactionType, peType, SIM_EX);
	if (performWireRouting)
		Synthesis::WireRoute("Output/3_ROUTE_to_SIM.txt", PATH_FINDER_WR, numHorizTracks, numVertTracks);
}

///////////////////////////////////////////////////////////////////////////
// Easy interface for individually addressable DMFBs
///////////////////////////////////////////////////////////////////////////
void Demo::IndividuallyAddressableDMFB(bool isParallel, bool useVirtualTopology, bool performWireRouting, CommonBenchmarkType benchmark)
{
	ProgrammableDMFB(isParallel, useVirtualTopology, INDIVADDR_PM, performWireRouting, benchmark);
}

///////////////////////////////////////////////////////////////////////////
// Easy interface for clique-partitioned pin-mapped DMFBs
///////////////////////////////////////////////////////////////////////////
void Demo::CliquePartitionedDMFB(bool isParallel, bool useVirtualTopology, bool performWireRouting, CommonBenchmarkType benchmark)
{
	ProgrammableDMFB(isParallel, useVirtualTopology, CLIQUE_PM, performWireRouting, benchmark);
}

///////////////////////////////////////////////////////////////////////////
// This method calls a wire-routing method on a number of different test
// cases (based on the testNum parameter). The oCap (orthogonal capacity) represents
// the number of wires that can fit under each electrode in a given orthogonal
// direction.
///////////////////////////////////////////////////////////////////////////
void Demo::WireRoutingCase(int oCap, int testNum)
{
	string inFileName = "Output/InputArch.txt";

	////////////////////////////////////////////////////////////////////////////
	// Pin-constrained wire-routing Tests
	////////////////////////////////////////////////////////////////////////////
	if (testNum == 1)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2008/B1_PCR_08.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 2)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2008/B2_InVitro_08.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 3)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2008/B3_Protein_08.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 4)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2008/B123_MultiFunctional_08.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 5)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2012/B1_PCR_12.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 6)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2012/B2_InVitro_12.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 7)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2012/B3_Protein_12.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 8)
	{
		CopyArchFile("DmfbArchs/PinConstrained/DAC2012/B123_MultiFunctional_12.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 9)
	{
		CopyArchFile("DmfbArchs/PinConstrained/IA_PinConstrained/B1_PCR_IA.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 10)
	{
		CopyArchFile("DmfbArchs/PinConstrained/IA_PinConstrained/B2_InVitro_IA.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 11)
	{
		CopyArchFile("DmfbArchs/PinConstrained/IA_PinConstrained/B3_Protein_IA.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 12)
	{
		CopyArchFile("DmfbArchs/PinConstrained/IA_PinConstrained/B123_MultiFunctional_IA.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}

	////////////////////////////////////////////////////////////////////////////
	// FPPC Wire-Routing Tests
	////////////////////////////////////////////////////////////////////////////
	else if (testNum == 13)
	{
		CopyArchFile("DmfbArchs/FPPC/Enhanced_TCAD/WireRoutingTesting/EnhancedFPPC_PinOpt_4.txt");
		Synthesis::WireRoute(inFileName, ENHANCED_FPPC_WR, oCap, oCap);
	}
	else if (testNum == 14)
	{
		CopyArchFile("DmfbArchs/FPPC/Enhanced_TCAD/WireRoutingTesting/EnhancedFPPC_PinOpt_8.txt");
		Synthesis::WireRoute(inFileName, ENHANCED_FPPC_WR, oCap, oCap);
	}
	else if (testNum == 15)
	{
		CopyArchFile("DmfbArchs/FPPC/Enhanced_TCAD/WireRoutingTesting/EnhancedFPPC_RouteOpt_4.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 16)
	{
		CopyArchFile("DmfbArchs/FPPC/Enhanced_TCAD/WireRoutingTesting/EnhancedFPPC_RouteOpt_8.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 17)
	{
		CopyArchFile("DmfbArchs/FPPC/Enhanced_TCAD/WireRoutingTesting/EnhancedFPPC_IA_4.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else if (testNum == 18) // Broke
	{
		CopyArchFile("DmfbArchs/FPPC/Enhanced_TCAD/WireRoutingTesting/EnhancedFPPC_IA_8.txt");
		Synthesis::WireRoute(inFileName, PATH_FINDER_WR, oCap, oCap);
	}
	else
		claim(false, "Unknown test-case number for IndividuallyAddressableDMFB() demo.");
}

///////////////////////////////////////////////////////////////////////////
// Helper method which copies file to output directory
///////////////////////////////////////////////////////////////////////////
void Demo::CopyArchFile(string fileName)
{
	const char * c = fileName.c_str();
	std::ifstream src(c, std::ios::binary);
	std::ofstream dst("Output/InputArch.txt", std::ios::binary);
	dst << src.rdbuf();
}

///////////////////////////////////////////////////////////////////////////
// This method demonstrates pin-mapping and wire-routing capabilities on
// a number of pre-defined test cases (more can be added if desired).
///////////////////////////////////////////////////////////////////////////
void Demo::PinMapWireRouteFlow(PinMapType pmt, WireRouteType wrt, int oCap, int testNum)
{
	// Don't touch these
	int maxDropsPerStorageMod = 2;
	int minCellsBetweenIrMods = 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ten different assay/dmfb combination test cases assigned randomly to testNums 1-10. Can add more cases if
	// desired.
	if (testNum == 1)
		Synthesis::Schedule("Assays/B1/MixSplit/PCR.txt", "DmfbArchs/IndividuallyAddressable/B1/Arch_15_19_B1.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 2)
		Synthesis::Schedule("Assays/B2/InVitro_Ex5_4s_4r.txt", "DmfbArchs/IndividuallyAddressable/B2/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 3)
		Synthesis::Schedule("Assays/B3/MixSplit/Protein.txt", "DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 4)
		Synthesis::Schedule("Assays/B4/MixSplit/ProteinSplit_05_Eq.txt", "DmfbArchs/IndividuallyAddressable/B3and4/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 5)
		Synthesis::Schedule("Assays/B5/CoDos5React2_45_23_67_93.txt", "DmfbArchs/IndividuallyAddressable/B5/Arch_15_19_B5.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 6)
		Synthesis::Schedule("Assays/B5/Gorma23_256.txt", "DmfbArchs/IndividuallyAddressable/B5/Arch_15_19_B5.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 7)
		Synthesis::Schedule("Assays/B5/griffith.txt", "DmfbArchs/IndividuallyAddressable/B5/Arch_15_19_B5.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 8)
		Synthesis::Schedule("Assays/B5/Remia191_1024.txt", "DmfbArchs/IndividuallyAddressable/B5/Arch_15_19_B5.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 9)
		Synthesis::Schedule("Assays/B5/Remia641_1024.txt", "DmfbArchs/IndividuallyAddressable/B5/Arch_15_19_B5.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else if (testNum == 10)
		Synthesis::Schedule("Assays/B5/Remia850_1024.txt", "DmfbArchs/IndividuallyAddressable/B5/Arch_15_19_B5.txt", "Output/1_SCHED_to_PLACE.txt", FD_LIST_S, GRISSOM_FIX_0_RA, pmt, maxDropsPerStorageMod);
	else
		claim(false, "Unknown test-case number for PinMapWireRouteFlow() demo.");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Can choose the virtual topology (GRISSOM_PATH_B/FIXED_PE...recommended!!!) or free placement (KAMER_LL_P/FREE_PE)
	PlacerType pt = GRISSOM_PATH_B; // If using the virtual topology flow...
	ProcessEngineType pet = FIXED_PE; //...use these two lines instead
	//PlacerType pt = KAMER_LL_P; // If using the KAMER flow....
	//ProcessEngineType pet = FREE_PE; // .....use these two lines

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Finish synthesis flow
	Synthesis::Place("Output/1_SCHED_to_PLACE.txt", "Output/2_PLACE_to_ROUTE.txt", pt, minCellsBetweenIrMods);
	Synthesis::Route("Output/2_PLACE_to_ROUTE.txt", ROY_MAZE_R, false, BEG_COMP, pet, SIM_EX);
	Synthesis::WireRoute("Output/3_ROUTE_to_SIM.txt", wrt, oCap, oCap); // Call wire-routing here if you'd like
}

///////////////////////////////////////////////////////////////////////////
// This test demonstrates conditional branches/dependencies in action on a
// simple example. It also demonstrates 2 LoCs being used at the same time.
///////////////////////////////////////////////////////////////////////////
CFG * Demo::Create_Conditional_Demo_CFG()
{
	CFG *cfg = new CFG();
	cfg->setName("Conditional_Demo_CFG");

	// DAG 1
	DAG *dag1 = cfg->AddNewDAG("dag1");
	AssayNode *D1 = dag1->AddDispenseNode("blood", 10, "D1_1");
	AssayNode *H1 = dag1->AddHeatNode(.1, "H1_1");
	AssayNode *d1 = dag1->AddDetectNode(1, .1, "d1_1");
	AssayNode *O1 = dag1->AddOutputNode("waste", "O1_1");
	dag1->ParentChild(D1, d1);
	dag1->ParentChild(d1, H1);
	dag1->ParentChild(H1, O1);
	AssayNode *c1 = d1;

	// DAG 2
	DAG *dag2 = cfg->AddNewDAG("dag2");
	D1 = dag2->AddDispenseNode("blood", 10, "D1_2");
	H1 = dag2->AddHeatNode(.2, "H1_2");
	d1 = dag2->AddDetectNode(1, .2, "d1_2");
	O1 = dag2->AddOutputNode("waste", "O1_2");
	dag2->ParentChild(D1, d1);
	dag2->ParentChild(d1, H1);
	dag2->ParentChild(H1, O1);
	AssayNode *c2 = d1;

	// DAG 3
	DAG *dag3 = cfg->AddNewDAG("dag3");
	D1 = dag3->AddDispenseNode("blood", 10, "D1_3");
	H1 = dag3->AddHeatNode(.3, "H1_3");
	O1 = dag3->AddOutputNode("waste", "O1_3");
	dag3->ParentChild(D1, H1);
	dag3->ParentChild(H1, O1);


	ConditionalGroup *cg = cfg->AddNewCG();
	Expression *inner1 = new Expression(c1, OP_LT, 15);
	Expression *inner2 = new Expression(c2, OP_LT, 10);
	cg->addNewCondition(inner1, dag3);
	cg = cfg->AddNewCG();
	cg->addNewCondition(inner2, dag3);


	Expression *outer = new Expression(OP_OR);
	Expression *inner11 = new Expression(c1, OP_GT, 15);
	Expression *inner22 = new Expression(c2, OP_LT, 10);
	outer->addOperand(inner11);
	outer->addOperand(inner22);
	cg = cfg->AddNewCG();
	cg->addNewCondition(outer, dag3);

	cfg->ConstructAndValidateCFG();
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
	cout << "Conditional CFG Generated" << endl;
	return cfg;
}

///////////////////////////////////////////////////////////////////////////
// This tests the fault tolerant split.  It has one FTSplit.  It demonstrates
// how a DAG with one FTSplit can be converted into a CFG with several
// DAGs, which transfer droplets between each other to ensure that the split
// occurs properly.
///////////////////////////////////////////////////////////////////////////
CFG * Demo::Create_FaultTolerant_Split_Demo1_CFG()
{
	double mult = .1; // TimeStep multiplier
	double secPerTS = .05; //seconds per timestep

	CFG *cfg = new CFG();
	cfg->setName("Create_FaultTolerant_Split_Demo1_CFG");

	// DAG 1
	DAG *dag = cfg->AddNewDAG("pcr");
	AssayNode *D1 = dag->AddDispenseNode("sample", 10, "sample");
	AssayNode *D2 = dag->AddDispenseNode("reagent", 10, "reagent");

	AssayNode *M1 = dag->AddMixNode(2, 1*mult, "M1");
	AssayNode *M2 = dag->AddMixNode(2, 1*mult, "M2");

	AssayNode *H1 = dag->AddHeatNode(1*mult, "H1");
	AssayNode *H2 = dag->AddHeatNode(1*mult, "H2");
	AssayNode *H3 = dag->AddHeatNode(1*mult, "H3");

	AssayNode *S1 = dag->AddSplitNode(false, 2, 1*mult, "FTS1");
	AssayNode *FTS1 = dag->AddSplitNode(true, 2, 1*mult, "FTS2");


	AssayNode *O1 = dag->AddOutputNode("output", "O1");
	AssayNode *O2 = dag->AddOutputNode("output", "O2");

	dag->ParentChild(D1, M1);
	dag->ParentChild(D2, M1);
	dag->ParentChild(M1, S1);
	dag->ParentChild(S1, H1);
	dag->ParentChild(H1, FTS1);
	dag->ParentChild(FTS1, O1);
	dag->ParentChild(FTS1, M2);
	dag->ParentChild(S1, H2);
	dag->ParentChild(H2, H3);
	dag->ParentChild(H3, M2);
	dag->ParentChild(M2, O2);

	cfg->convertFTSplits();
	cfg->ConstructAndValidateCFG();
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
	cout << "Fault-tolerant Split Test 1 CFG Generated" << endl;
	return cfg;
}

///////////////////////////////////////////////////////////////////////////
// Similar to FAULT_TOLERANT_SPLIT_TEST_1()....reaction starts out similarly
// to a PCR mix.
///////////////////////////////////////////////////////////////////////////
CFG * Demo::Create_FaultTolerant_Split_Demo2_CFG()
{
	double mult = .1; // TimeStep multiplier
	double secPerTS = .05; //seconds per timestep

	CFG *cfg = new CFG();
	cfg->setName("Create_FaultTolerant_Split_Demo2_CFG");

	// DAG 1
	DAG *dag = cfg->AddNewDAG("pcr");
	AssayNode *D1 = dag->AddDispenseNode("tris-hcl", 10, "D_tris-hcl");
	AssayNode *D2 = dag->AddDispenseNode("kcl", 10, "D_kcl");
	AssayNode *D3 = dag->AddDispenseNode("bovine", 10, "D_bovine");
	AssayNode *D4 = dag->AddDispenseNode("gelatin", 10, "D_gelatin");
	AssayNode *D5 = dag->AddDispenseNode("primer", 10, "D_primer");
	AssayNode *D6 = dag->AddDispenseNode("beosynucleotide", 10, "D_beosynucleotide");
	AssayNode *D7 = dag->AddDispenseNode("amplitag", 10, "D_amplitag");
	AssayNode *D8 = dag->AddDispenseNode("lambda", 10, "D_lambda");
	AssayNode *M1 = dag->AddMixNode(2, 1*mult, "M1");
	AssayNode *M2 = dag->AddMixNode(2, 1*mult, "M2");
	AssayNode *M3 = dag->AddMixNode(2, 1*mult, "M3");
	AssayNode *M4 = dag->AddMixNode(2, 1*mult, "M4");
	AssayNode *M5 = dag->AddMixNode(2, 1*mult, "M5");
	AssayNode *M6 = dag->AddMixNode(2, 1*mult, "M6");
	AssayNode *M7 = dag->AddMixNode(2, 1*mult, "M7");
	AssayNode *FTS1 = dag->AddSplitNode(true, 2, 1*mult, "FTS1");
	AssayNode *M8 = dag->AddMixNode(2, 1*mult, "M8");
	AssayNode *O1 = dag->AddOutputNode("output", "O_output");

	dag->ParentChild(D1, M1);
	dag->ParentChild(D2, M1);
	dag->ParentChild(D3, M2);
	dag->ParentChild(D4, M2);
	dag->ParentChild(D5, M4);
	dag->ParentChild(D6, M4);
	dag->ParentChild(D7, M5);
	dag->ParentChild(D8, M5);
	dag->ParentChild(M1, M3);
	dag->ParentChild(M2, M3);
	dag->ParentChild(M4, M6);
	dag->ParentChild(M5, M6);
	dag->ParentChild(M3, M7);
	dag->ParentChild(M6, M7);
	dag->ParentChild(M7, FTS1);
	dag->ParentChild(FTS1, M8);
	dag->ParentChild(FTS1, M8);
	dag->ParentChild(M8, O1);

	cfg->convertFTSplits();
	cfg->ConstructAndValidateCFG();
	cfg->OutputGraphFile(cfg->getName(), true, true, true);
	cout << "Fault-tolerant Split Test 2 CFG Generated" << endl;
	return cfg;
}
