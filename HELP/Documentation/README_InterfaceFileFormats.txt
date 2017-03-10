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

/////////////////////////////////////////////////////////////////////////////
// GENERAL NOTES: 
// Capitalization should be irrelevant
// All parameters must be inclosed within the "( and ") parenthesis
// All parameters must not separated by a "," comma
// There can be NO additional "(", ")", or "," symbols in the line
// Empty lines and lines that begin with "//" (comments) ARE allowed in all formats
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Architecture file
/////////////////////////////////////////////////////////////////////////////
Dim (numCellsX, numCellsY)
External (operationType = {HEAT, DETECT}, leftX, topY, rightX, bottomY)
Input (ioSide = {North, South, East, West}, posXorY, ioLength (s), fluidName) // Assumes not a wash droplet
Input (ioSide = {North, South, East, West}, posXorY, ioLength (s), fluidName, isWash = {TRUE, FALSE})
Output (ioSide = {North, South, East, West}, posXorY, ioLength (s), sinkName)
Freq (cyclesPerSec)
Timestep (length (s))

PinMap (topLeft,...,topRight,...,botLeft,...,botRight)
SpecialPins(pinCount, pin1, pin2,...)
ResourceCount(c1, c2, c3,...)
ResourceLocation(resType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, tiledNum)

/////////////////////////////////////////////////////////////////////////////
// Control Flow Graph (.cfg) File (order is IMPORTANT)
// NOTE: Can have multiple DAGs, should all be together.
// NOTE: The general pattern is to have a COND(ition), followed by 0+ EXP(ressions),
// followed by 0+ TDs(transfer droplets). This general COND-EXP-TD can repeate itself,
// but the EXPs and TDs should be directly under the COND they belong to. 
/////////////////////////////////////////////////////////////////////////////
NAME (cfgName)
DAG (dagName)	// Can be multiple
NUMCGS (numConditionalGroups)
COND (conditionalGroupId, numDependentDAGs, firstDependentDAG, .... , lastDependentDAG, numBranchIfTrueDAGs, firstBranchIfTrueDAG, .... , lastBranchIfTrueDAG, expressionId)
EXP (expId, operandType [TRUE, FALSE], operationType [UNCOND], unconditionalParentDagName)
EXP (expId, operandType [ONE_SENSOR], operationType [GT, GoE, LT, LoE, EQUAL], op1SensorDagName, op1SensorNodeId, op2StaticVal)
EXP (expId, operandType [TWO_SENSORS], operationType [GT, GoE, LT, LoE, EQUAL], op1SensorDagName, op1SensorNodeId, op2SensorDagName, op2SensorNodeId,)
EXP (expId, operandType [RUN_COUNT], operationType [GT, GoE, LT, LoE, EQUAL], repeatableDagName, runCount)
EXP (expId, operandType [SUB_EXP], operationType [AND, OR], subExp1Id, subExp2Id)
EXP (expId, operandType [EXP], operationType [NOT], subExpId)
TD (transOutDag, transOutNodeId, transInDag, transInNodeId)


/////////////////////////////////////////////////////////////////////////////
// 0_DAG_to_SCHED File Format (order is unimportant)
/////////////////////////////////////////////////////////////////////////////
DagName (name)
Node (id, DISPENSE, fluidName, volume, name) 
Node (id, MIX, numDropsBefore, time (s), name)
Node (id, DILUTE, numDropsBefore, time (s), name)
Node (id, SPLIT, numDropsAfter, time (s), name)
Node (id, HEAT, time (s), name)
Node (id, COOL, time (s), name)
Node (id, DETECT, numDropsIo, time (s), name)
Node (id, OUTPUT, sinkName, name)
Node (id, STORAGE, name)
Node (id, TRANSFER_IN, name)
Node (id, TRANSFER_OUT, name)
Node (id, GENERAL, name)
Edge (parentId, childId)

///////////////////////////////////////////////////////////////////////////
// 1_SCHED_to_PLACE File Format (order unimportant except Dim should be before PinMapType)
// This is essentially a concatenation of the input DAG and ARCH files
// with the resourceType, start and stop time-steps added to the nodes
// Resource types are either a General, Detect, Heat or DetectHeat module.
///////////////////////////////////////////////////////////////////////////
Dim (numCellsX, numCellsY)
External (operationType = {HEAT, DETECT}, leftX, topY, rightX, bottomY)
Input (ioSide = {North, South, East, West}, posXorY, ioLength (s), pinNo, fluidName, isWash = {TRUE, FALSE})
Output (ioSide = {North, South, East, West}, posXorY, ioLength (s), pinNo, sinkName)
Freq (cyclesPerSec)
Timestep (length (s))
SchedType (schedulerType (string/key from library))
PinMapType (pinMapperType (string/key from library))
MaxStorageDropsPerMod (dropsPerModule)
PinMap (topLeft,...,topRight,...,botLeft,...,botRight)
SpecialPins(pinCount, pin1, pin2,...)
ResourceCount(c1, c2, c3,...)
ResourceLocation(resType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, tiledNum)

DagName (name)
Node (id, DISPENSE, fluidName, volume, name, startTS, endTS) 
Node (id, MIX, numDropsBefore, time (s), name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, DILUTE, numDropsBefore, time (s), name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, SPLIT, numDropsAfter, time (s), name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, HEAT, time (s), name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, COOL, time (s), name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, DETECT, numDropsIo, time (s), name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, OUTPUT, sinkName, name, startTS, endTS)
Node (id, STORAGE, name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, TRANSFER_IN, name, startTS, endTS)
Node (id, TRANSFER_OUT, name, startTS, endTS)
Node (id, GENERAL, name, startTS, endTS, resType = {enum # for ResourceType})
Node (id, STORAGE_HOLDER, startTS, endTS, resType = {enum # for ResourceType})
Edge (parentId, childId)

/////////////////////////////////////////////////////////////////////////////
// 2_PLACE_to_ROUTE File Format (ORDER IS IMPORTANT; Reconfigurable Modules
// must be output before the DAG nodes, Dim should be before PinMapType).
// IO ports now have IDs
/////////////////////////////////////////////////////////////////////////////
Dim (numCellsX, numCellsY)
External (operationType = {HEAT, DETECT}, leftX, topY, rightX, bottomY)
Input (id, ioSide = {North, South, East, West}, posXorY, ioLength (s), pinNo, fluidName, containsWashFluid = {TRUE, FALSE})
Output (id, ioSide = {North, South, East, West}, posXorY, ioLength (s), pinNo, portName, containsWashFluid = {TRUE, FALSE})
Freq (cyclesPerSec)
Timestep (length (s))
SchedType (schedulerType (string/key from library))
PinMapType (pinMapperType (string/key from library))
PlacerType (placerType (string/key from library))
HCellsBetweenModIr (numCells)
VCellsBetweenModIr (numCells)
PinMap (topLeft,...,topRight,...,botLeft,...,botRight)
SpecialPins(pinCount, pin1, pin2,...)
ResourceCount(c1, c2, c3,...)
ResourceLocation(resType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, tiledNum)

Reconfig (id, operationType = {MIX, DILUTE, SPLIT, HEAT, DETECT, STORAGE_HOLDER}, resourceType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, startTS, endTS, tileNum)
DagName (name)
Node (id, DISPENSE, fluidName, volume, name, startTS, endTS, ioPortId) 
Node (id, MIX, numDropsBefore, time (s), name, startTS, endTS, reconfigModId)
Node (id, DILUTE, numDropsBefore, time (s), name, startTS, endTS, reconfigModId)
Node (id, SPLIT, numDropsAfter, time (s), name, startTS, endTS, reconfigModId)
Node (id, HEAT, time (s), name, startTS, endTS, reconfigModId)
Node (id, COOL, time (s), name, startTS, endTS, reconfigModId)
Node (id, DETECT, numDropsIo, time (s), name, startTS, endTS, reconfigModId)
Node (id, OUTPUT, sinkName, name, startTS, endTS, ioPortId)
Node (id, STORAGE, startTS, endTS, reconfigModId)
Node (id, TRANSFER_IN, name, startTS, endTS)
Node (id, TRANSFER_OUT, name, startTS, endTS)
Node (id, GENERAL, name, startTS, endTS, reconfigModId)
Edge (parentId, childId)


/////////////////////////////////////////////////////////////////////////////
// 3_ROUTE_to_SIM File Format (file fed to the java image drawer)
// Commit Cycles can start anywhere (doesn't have to be 200, 0, etc.)
// Different droplet tags draw different colors:
// "--INTERFERENCE WAIT" => Draws Yellow Droplet
// "--PROCESSING" => Draws Light Gray Droplet
// "--RETIRING" => Draws Cyan Droplet
// "--OUTPUTTING" => Draws Orange Droplet
// "--PROCESS WAIT" => Draws RedDroplet
/////////////////////////////////////////////////////////////////////////////
=======================Initialization=======================
Dim (numCellsX, numCellsY)
Input (id, ioSide = {top, bottom, right, left}, posXorY, fluidName, containsWashFluid = {TRUE, FALSE})
Output (id, ioSide = {top, bottom, right, left}, posXorY, fluidName, containsWashFluid = {TRUE, FALSE})
PinMapType (pinMapperType (string/key from library))
ExternalDetector(id, leftX, topY, rightX, bottomY)
ExternalHeater(id, leftX, topY, rightX, bottomY)
ReconfigMixer(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigDiluter(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigSplitter(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigHeater(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigDetector(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigStorage(id, leftX, topY, rightX, bottomY, startTS, endTS)
PinMap (topLeft,...,topRight,...,botLeft,...,botRight)
SpecialPins(pinCount, pin1, pin2,...)
ResourceCount(c1, c2, c3,...)
ResourceLocation(resType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, tiledNum)
ROUTETO (tsAboutToBegin, startCycle, endCycle)
TSRANGE (timeStep, startCycle, endCycle)
=======================Init Done=======================
=======================Commit Cycle 200=======================
C (cellStatus, dropletId, x, y) // C = Cell status
Dirty (elecNum1, occupyingDropId1, elecNum2, occupyingDropId2...elecNumN, occupyingDropIdN) // xCoord = elecNum % numCellsX; yCoord = elecNum / numCellsX
ActivePins (activePin1, activePin2,...,activePinX)
Number of electrodes to activate = 1
=======================Commit Cycle 201=======================
C (cellStatus, dropletId, x, y) // C = Cell status
Dirty (elecNum1, occupyingDropId1, elecNum2, occupyingDropId2...elecNumN, occupyingDropIdN)
ActivePins (activePin1, activePin2,...,activePinX)
Number of electrodes to activate = 1
=======================Commit Cycle 202=======================
C (cellStatus, dropletId, x, y) // C = Cell status
Dirty (elecNum1, occupyingDropId1, elecNum2, occupyingDropId2...elecNumN, occupyingDropIdN)
ActivePins (activePin1, activePin2,...,activePinX)
Number of electrodes to activate = 1
=======================Commit Cycle 203=======================
C (cellStatus, dropletId, x, y) // C = Cell status
Dirty (elecNum1, occupyingDropId1, elecNum2, occupyingDropId2...elecNumN, occupyingDropIdN)
ActivePins (activePin1, activePin2,...,activePinX)
Number of electrodes to activate = 1

.

.

.


/////////////////////////////////////////////////////////////////////////////
// 3_COMPACT_ROUTE_to_SIM File Format (file fed to the java image drawer)
// Commit Cycles can start anywhere (doesn't have to be 200, 0, etc.)
// Different droplet tags draw different colors:
// "--INTERFERENCE WAIT" => Draws Yellow Droplet
// "--PROCESSING" => Draws Light Gray Droplet
// "--RETIRING" => Draws Cyan Droplet
// "--OUTPUTTING" => Draws Orange Droplet
// "--PROCESS WAIT" => Draws RedDroplet
/////////////////////////////////////////////////////////////////////////////
=======================Initialization=======================
Dim (numCellsX, numCellsY)
Input (id, ioSide = {top, bottom, right, left}, posXorY, fluidName, containsWashFluid = {TRUE, FALSE})
Output (id, ioSide = {top, bottom, right, left}, posXorY, fluidName, containsWashFluid = {TRUE, FALSE})
ExternalDetector(id, leftX, topY, rightX, bottomY)
ExternalHeater(id, leftX, topY, rightX, bottomY)
ReconfigMixer(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigDiluter(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigSplitter(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigHeater(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigDetector(id, leftX, topY, rightX, bottomY, startTS, endTS)
ReconfigStorage(id, leftX, topY, rightX, bottomY, startTS, endTS)
PinMap (topLeft,...,topRight,...,botLeft,...,botRight)
SpecialPins(pinCount, pin1, pin2,...)
ResourceCount(c1, c2, c3,...)
ResourceLocation(resType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, tiledNum)
ModuleDeltaType (type = {enum # for ModuleDeltaType})
=======================Init Done=======================
=======================Routing to TimeStep 0=======================
Number of electrodes to activate = 0
=======================Routing to TimeStep 1=======================
Number of electrodes to activate = 0
=======================Routing to TimeStep 2=======================
Droplet 1
(4, 0)
(5, 0)
(5, 1)
(5, 2)
(5, 3)
(5, 4)
End Route
Droplet 2
(2, 0)
(3, 0)
(4, 0)
(5, 0)
(5, 1)
(5, 2)
(5, 3)
(5, 4)
End Route
Number of electrodes to activate = 14

.

.

.

/////////////////////////////////////////////////////////////////////////////
// 4_HARDWARE_DESCRIPTION File Format (file used to describe architecture)
// Gives physical hardware description, including DMFB dimensions, I/O,
// external resources (heaters/detectors), pin-mapping, and wire-routing
// (if computed).
/////////////////////////////////////////////////////////////////////////////
Dim (numCellsX, numCellsY)
External (operationType = {HEAT, DETECT}, leftX, topY, rightX, bottomY)
Input (id, ioSide = {North, South, East, West}, posXorY, ioLength (s), pinNo, fluidName, containsWashFluid = {TRUE, FALSE})
Output (id, ioSide = {North, South, East, West}, posXorY, ioLength (s), pinNo, portName, containsWashFluid = {TRUE, FALSE})
Freq (cyclesPerSec)
Timestep (length (s))
PinMap (topLeft,...,topRight,...,botLeft,...,botRight)
SpecialPins(pinCount, pin1, pin2,...)
ResourceCount(c1, c2, c3,...)
ResourceLocation(resType = {enum # for ResourceType}, leftX, topY, rightX, bottomY, tiledNum)

NumVTracks (numVertTracksPerElec)
NumHTracks (numHorizTracksPerElec)
WireGridDim (numCellsX, numCellY)
RelLine (pinNum, layerNum, wireGridSourceX, wireGridSourceY, wireGridDestX, wireGridDestY)

Via (x, y, beginLayer, endLayer)
ElectrodeMicrons (pitchInMicrometers)
ShiftRegister (type, xCoord, yCoord)
Microcontroller (type, xCoord, yCoord)
ArrayPosition(x, y)
ArrayOffset(x, y)
PCB(firstHalfWidth, secondHalfWidth, height, imgBeginX, imgBeginY)
