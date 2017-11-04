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
 * Name: Demo (Demo)															*
 *																				*
 * Details: This class contains several functions of popular benchmarks that	*
 * can be run in a hard-coded environment (i.e., from main()). PLEASE NOTE that	*
 * these methods are only meant to act as a DEMO and do not demonstrate all		*
 * capabilities of the system. ALSO, please note that not all combinations of	*
 * schedulers, placers, routers, benchmarks, architecture files, etc. are		*
 * compatible with one another; please keep this in mind as you try your own 	*
 * combination of settings and encounter unknown errors. We make no guarantee	*
 * of every combination and remind you that this is not a commercial product,	*
 * but a free, opnen-source research tool.
 *-----------------------------------------------------------------------------*/

#ifndef _DEMO_H
#define _DEMO_H

#include "../Resources/enums.h"
#include "../Models/cfg.h"
using namespace std;
#include <iostream>
#include <stdlib.h>
#include <sstream>

class Demo
{
    public:
        // Constructors
		Demo();
        virtual ~Demo();

        // General
        static void FieldProgrammablePinConstrainedDMFB(bool isParallel, bool performWash, bool useFPPCWireRouter, CommonBenchmarkType benchmark);
        static void RoutingBasedSynthesis(bool performWash, CommonBenchmarkType benchmark);
        static void ProgrammableDMFB(bool isParallel, bool useVirtualTopology, PinMapType pmType, bool performWireRouting, CommonBenchmarkType benchmark);
        static void IndividuallyAddressableDMFB(bool isParallel, bool useVirtualTopology, bool performWireRouting, CommonBenchmarkType benchmark);
        static void CliquePartitionedDMFB(bool isParallel, bool useVirtualTopology, bool performWireRouting, CommonBenchmarkType benchmark);
        static void WireRoutingCase(int orthogonalCapacity, int testNum);
        static void PinMapWireRouteFlow(PinMapType pmt, WireRouteType wrt, int oCap, int testNum);
        static void CopyArchFile(string fileName);

        // Conditional Demos
        static CFG * Create_Conditional_Demo_CFG();
        static CFG * Create_FaultTolerant_Split_Demo1_CFG();
        static CFG * Create_FaultTolerant_Split_Demo2_CFG();

        static void Create_Compile_Execute_Hardcoded_CFG(int testNum);
        static void Create_Compile_Execute_Biocoder_CFG(int testNum);
};

#endif /* _DEMO_H */
