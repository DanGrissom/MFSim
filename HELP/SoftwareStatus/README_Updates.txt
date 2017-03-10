/*------------------------------------------------------------------------------*
 *                       (c)2014, All Rights Reserved.     			*
 *       ___           ___           ___     					*
 *      /__/\         /  /\         /  /\    					*
 *      \  \:\       /  /:/        /  /::\   					*
 *       \  \:\     /  /:/        /  /:/\:\  					*
 *   ___  \  \:\   /  /:/  ___   /  /:/~/:/        				*
 *  /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/___     UCR DMFB Synthesis Framework  *
 *  \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:::::/     www.microfluidics.cs.ucr.edu	*
 *   \  \:\  /:/   \  \:\  /:/   \  \::/~~~~ 					*
 *    \  \:\/:/     \  \:\/:/     \  \:\     					*
 *     \  \::/       \  \::/       \  \:\    					*
 *      \__\/         \__\/         \__\/    					*
 *-----------------------------------------------------------------------------*/

***********************************************************************************************
*                                        Revisions List                                       *
***********************************************************************************************
March 9, 2017 (v4.0)
General (applicable to multiple projects)
-----------------------------------------

MFSimStatic Changes (C++)
-------------------------
- Added various instance variables to dmfbarch class that deal with full pcb layout
- Added synthesis methods to fully route wires between array and IC's
- Added new timer to synthesis that times area routing process
- Added 4 hardcoded flags in synthesis file to dictate which wires to route for ease of debugging
- Added classes to encapsulate each of the IC types
- Updated file out to print out the PCB layout to Hardware file
- Added 3 larger array hardware types accessible through main.cc in wireRoutingCase method
- Added new structs and enums in their respective resource files used in pcb layout calculation


MFSimStaticGUI Changes (Java)
-----------------------------


DmfbSimVisualizer Changes (Java)
--------------------------------
- Added new image parts that show IC's and wires connecting them to array within the DmfbDrawer class
- Updated hardware file parser to reflect the changes that were made to make the above possible
- Added classes to encapsulate each of the IC types
- Added hardware constants class that must be changed in order to add new IC types (the specific IC class must also be changed to link to constants)

-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------

Sep X, 2016 (v4.0)
General (applicable to multiple projects)
-----------------------------------------


MFSimStatic Changes (C++)
-------------------------
- Added numerous BioCoder classes to project
- Added a number of new conditional assays, mostly in biocoder_test.h/cc, but also in demo.h/cc
- Added system.h/cc, compiled_cfg.h/cc and compiled_dag.h/cc to aid in compiling and saving
  CFGs and DAGs for dynamic execution of statically-compiled assays with control flow
- Made some slight modifications to the Synthesis class to accommodate dynamic execution
- Added TRANSFER_IN/OUT functionality to list_scheduler.h/cc and scheduler.h/cc to accomodate
  control flow
- Added TRANSFER_IN/OUT functionality to grissom_left_edge_binder.h/cc and binder.h/cc to accomodate
  control flow
- Updated analyze.cc AnalyzePlacement() method to undo the status change it makes
- Updated file_out.cc/h and file_in.cc/h to accommodate TransferIn, TransferOut and Cool assay nodes/operations
- Added TRANSFER_IN/OUT functionality to roy_maze_router.cc, post_subprob_compact_router.cc and router.cc
- Added TRANSFER_IN/OUT functionality to analyze.cc, specifically in the Analyze::AnalyzeRoutes() and
  AnalyzeDropletConcentrationAndIO() methods
- Added two new conditional benchmarks (Create_Conditional_PCR_Droplet_Replacement_CFG()
  and Create_Conditional_Probabilistic_PCR_CFG(), thanks to Chris Curtis)
- Added a new CFG output which shows the execution path through a particular CFG run, clearly showing
  the conditions that were evaluated and the routing vs. functional DAGs (CFG nodes)
- Added File I/O for control-flow graphs (CFGs) in file_out.cc/h and file_in.cc/h
- Added control-flow support (in BioCoder and in MFSimSSA simulator) for run-count such that a CFG
  transition can branch based on how many times a DAG (i.e., CFG node) has been executed


MFSimStaticGUI Changes (Java)
-----------------------------


DmfbSimVisualizer Changes (Java)
--------------------------------
- Added a new option for "All Graphs" which outputs all of the .dot files in the selected directory
- Made various internal changes to accommodate TransferIn, TransferOut and Cool assay nodes/operations

-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------

Jul 2, 2014 (v3.1)
General (applicable to multiple projects)
-----------------------------------------
- Organized DmfbArchs and Assays folders
- Added versions of B1, B3 and B4 benchmarks with dilution nodes
- Added pin-mappings for Chakrabarty DAC 2008 and 2012 pin-constrained designs

MFSimStatic Changes (C++)
-------------------------
- Modified the wire-routing FileOut structure to work on grid
- Added a wire-router based on pathfinder and a diagonal wire routing model
- Added a 2nd wire-router based on Yeh's wire routing
- Fixed a small bug in PostSubproblemCompactionRouter which now allows for proper execution and
  processing of nodes (e.g. dispense) with 0 length (they could be skipped over previously)
- Fixed small but critical bug in Left-Edge binder which caused improper bindings (was causing
  duplicate STORAGE_HOLDER nodes to be created)
- Fixed small but critical bug in path-binder which created duplicate modules for nodes after
  storage nodes in some instances
- Made a fix to the FileIn readers to account for skipping of node id's
- Fixed a large number (several thousand) of warnings (still a fair number that exist)
	- Mostly in for loops where int compared to unsigned int from vector.size()
	- Claim function changed to take string instead of char * for constant strings
- Created a GetLine() function in the FileIn class which fixes some Linux parsing errors that
  don't show up in windows
- Added routing-based-synthesis method
- Added formal support in framework and interface for wash droplets
	- I/Os can now have "wash fluid"
	- Droplets can now be "wash droplets"
	- Interface files have been changed/modified to support these changes
- Chaned the flow of the Synthesis:synthesizeDesign() function (for EntireFlow) to be able to
  handle flows that may not need to call all synthesis steps (e.g., routing-based synthesis)
- Added a dynamic programming compaction method in router.h/cc (thanks to Michael Albertson)

MFSimStaticGUI Changes (Java)
-----------------------------
- Added functionality for MFSimStaticGUI.jar to read 3 levels of these folders in the scheduling tab
- Made several fixes for Linux compatability (Thanks Oliver Kesz�cze)
	- Runtime.getRuntime().exec() now tries to start the executable from the current working directory
	- The getLocalBinaries() method checked that the filename ends with ".exe", which is not the case
	  if built using gcc/linux, thus requiring an executable w/ '.exe' extension is now optional
- Added support for washing/cleaning
	- Wash droplets now supported
	- Drawing dirty cells
	- Special I/Os for wash fluids
	- Removed Droplet class and replaced with CellStatus class which can represent droplets and cells (dirty)
- Changed color scheme of droplets and I/Os
- Added additional status updates below the progress bar during pre-draw stages
- Speed optimizations in parsing for drawing/parsing cycles
- Added "XY Grid Labels" to draw options (shows XY indices)

DmfbSimVisualizer Changes (Java)
--------------------------------
- Changed the wire-routing visualizer to draw based on a grid (accompanying wire-router included)
- Wire-routing visualizer draws image for each layer and pin, as well as all wires in one image
- Added function for centering text and fixed all mis-aligned text strings so now centered both
  vertically and horizontally
- Fixed scaling of some lines and text so the images look better irregardless of size
- Added a feature to automatically gray-out electrodes that are not assigned a pin number
  (helps easily indicate that these electrodes are not used)
- Added a "Browse..." button for selecting an input file
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
Oct 23, 2013 (v3.0)
- Added several analysis tools that analyzes the operations of the DMFB and validity of the implemented synthesis methods
	- Route analyzer validates that that the router did not break any interference rules (also, is split/merge-aware)
	- Droplet concentration analyzer ensures that everything input was also output and provides a detailed concentration
	  breakdown of all the droplets being output from the DMFB
	- Placement analyzer ensures that no modules are placed in the same place at the same time or directly adjacent to
	  one another; also generates warnings if modules are direcly blocking I/O ports
- Added DMFB dilution operation (this is the equivalent to a mix folowed by an instantaneous split)
	- Not yet supported with the FPPC setup
- Added a range feature to the visualizer which lets you output a specific range of cycles or time-steps
	- When done on the scheduled/placed DAG, will color only the nodes in the specified range
- Recolored DAG nodes for Unscheduled, Scheduled and Placed DAG
- Added new folders to file structure of code to clean things up; moved around some files and functions
	- Split Util.h into 4 files: Util/til.h, Util/file_in.h, Util/file_out.h, Util/analyze.h
- Added an internal implementation based on Cho's high-performance router
- Made some changes to compactRoutesWithMidStalls() to fix some small compaction errors regarding outputting droplets
- Removed processFixPlaceFullTS() function as it was not used and cannot complete a full simulation
- Added README_KnownBugs.txt list of known bugs and potential intermediate workarounds
- Made small fixes across framework for stability 
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
May 30, 2013 (v2.1)
- Added a simple tutorial for use with the binaries to demonstrate the impact of changing various parameters
- Added field-programmable pin-constrained code (Scheduler, Placer and Router), as described/published in DAC 2013
- Added implementation of KAMER (Keep All Maximum Empty Rectangles) placer, which does free placements
- Added new processing engine which performs operations in free-placed modules of various sizes (does not perform
  intra-module syncing like the fixed processing engine)
- Added history of synthesis methods to interface files for compatibility checking
- Added compatibility checking class so developers can add their own compatibility checks to help end users avoid
  known bad configurations with particular sets of parameters
- Used synthesis history to remove superfluous parameters from the top-level synthesis functions
- Used synthesis history to call dependency-elimination code (in post_subprob_compact_router.cc) which only gets
  called when free-placers were uses
- Made small fixes across framework for stability 
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
Apr 2, 2013 (v2.0)
- Added support for pin-constrained devices and basic wire routing in simulator and visualization suite.
- Restructured synthesis flow to eliminate similar code being called in scheduler/placer/router
- Added ResourceAllocationType which determines how many (and where, if doing fixed placement) resources are allocated
- Made small fixes across framework for stability 
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
Nov 15, 2012 (v1.1)
- Made some fixes to the three schedulers that use list scheduling (Genetic Scheduler, Ricketts GA Scheduler and
  Force-Direct LS) so that they would properly pass the base List-Scheduler the number of storage droplets - schedulers no longer die upon running
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
Nov 2, 2012 (v1.0)
- Initial Upload
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------