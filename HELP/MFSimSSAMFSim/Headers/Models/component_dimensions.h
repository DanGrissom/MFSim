/*
 * component_dimensions.h
 *
 *  Created on: Sep 5, 2016
 *      Author: jishii14
 */

#ifndef HEADERS_MODELS_COMPONENT_DIMENSIONS_H_
#define HEADERS_MODELS_COMPONENT_DIMENSIONS_H_

class ComponentDimensions
{
public:
	// Variables for each type of component

	// SHIFT REGISTER CONSTANTS:
	// Dimensions for Fairchild 74VHC595MTC 8-bit Shift Register with Lead Small Outline Integrated Circuit package type
	static const int SOIC_NUM_PINS = 8;	// Number of pins on one side of Shift Register
	static const int SOIC_NUM_OUTPUTS = 8;
	static constexpr double SOIC_BODY_WIDTH = 3.9;
	static constexpr double SOIC_BODY_HEIGHT = 9.9;
	static constexpr double SOIC_PIN_HEIGHT = 1;	// Usually larger than height
	static constexpr double SOIC_PIN_WIDTH = .5;
	static constexpr double SOIC_EDGE_RADIUS = 0;	// The height and width of the rounded corners
	static constexpr double SOIC_PIN_BUFFER = .3;	// The buffer outside each side of pins (often same as edge radius)
	// List of types of pins on Shift Register.  These progress on the left side from top to bottom then the right bottom to top (from device specifications)
	static constexpr PinType SOIC_PIN_TYPES[16] = {PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT,
		PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT,
		PinType::IO_GROUND, PinType::IO_SERIAL_OUTPUT, PinType::IO_RESET_INPUT, PinType::IO_SR_CLOCK_INPUT,
		PinType::IO_STORAGE_CLOCK_INPUT, PinType::IO_3STATE_INPUT, PinType::IO_SERIAL_INPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_VOLTAGE_EMITTER};
	static const int SOIC_PIN_TYPES_SIZE = 16;
	// PIN TYPE LISTS MUST BE DECLARED WITHIN SHIFT REGISTER CLASS BECAUSE OF SYNTACTICAL ISSUES

	// Dimensions for Fairchild 74VHC595MTC 8-bit Shift Register with Lead Small Outline Package package type
	static const int SOP_NUM_PINS = 4;
	static const int SOP_NUM_OUTPUTS = 8;
	static constexpr double SOP_BODY_WIDTH = 5.3;
	static constexpr double SOP_BODY_HEIGHT = 10.2;
	static constexpr double SOP_PIN_HEIGHT = 2.13;
	static constexpr double SOP_PIN_WIDTH = .6;
	static constexpr double SOP_EDGE_RADIUS = .47;
	static constexpr double SOP_PIN_BUFFER = .47;
	// List of types of pins on Shift Register.  These progress on the left side from top to bottom then the right bottom to top (from device specifications)
	static constexpr PinType SOP_PIN_TYPES[8] = {PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT,
		PinType::IO_GROUND, PinType::IO_SERIAL_OUTPUT, PinType::IO_SR_CLOCK_INPUT,
		PinType::IO_SERIAL_INPUT, PinType::IO_VOLTAGE_EMITTER};
	static const int SOP_PIN_TYPES_SIZE = 8;

	// Dimensions for Fairchild 74VHC595MTC 8-bit Shift Register with Lead Lead Thin Shrink Small Outline Package package type
	static const int TSSOP_NUM_PINS = 8;
	static const int TSSOP_NUM_OUTPUTS = 8;
	static constexpr double TSSOP_BODY_WIDTH = 4.4;
	static constexpr double TSSOP_BODY_HEIGHT = 5;
	static constexpr double TSSOP_PIN_HEIGHT = 1;
	static constexpr double TSSOP_PIN_WIDTH = .3;
	static constexpr double TSSOP_EDGE_RADIUS = .11;
	static constexpr double TSSOP_PIN_BUFFER = .11;
	// List of types of pins on Shift Register.  These progress on the left side from top to bottom then the right bottom to top (from device specifications)
	static constexpr PinType TSSOP_PIN_TYPES[16] = {PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT,
		PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_PARALLEL_OUTPUT,
		PinType::IO_GROUND, PinType::IO_SERIAL_OUTPUT, PinType::IO_RESET_INPUT, PinType::IO_SR_CLOCK_INPUT,
		PinType::IO_STORAGE_CLOCK_INPUT, PinType::IO_3STATE_INPUT, PinType::IO_SERIAL_INPUT, PinType::IO_PARALLEL_OUTPUT, PinType::IO_VOLTAGE_EMITTER};
	static const int TSSOP_PIN_TYPES_SIZE = 16;

	// MICROCONTROLLERS:
	// Dimensions for ATmega2560 with Thin Profile Plastic Quad Flat Package type (TQFP)
	// Pins on both sides
	static const int TQFP_NUM_PINS = 16;
	static constexpr double TQFP_BODY_WIDTH = 14;
	static constexpr double TQFP_BODY_HEIGHT = 14;
	static constexpr double TQFP_PIN_HEIGHT = 1;
	static constexpr double TQFP_PIN_WIDTH = .375;
	static constexpr double TQFP_EDGE_RADIUS = .75;
	static constexpr double TQFP_PIN_BUFFER = .75;
	static const int TQFP_BUILD_TYPE = 0;	// 0 = leads all four sides, 1 = leads on only left and right, 2 = circles, 3 = leads inside body

	// Dimensions for ATmega2560 with Chip Array BGA Package type (CBGA)
	// Circles for pins
	static const int CBGA_NUM_PINS = 10;
	static constexpr double CBGA_BODY_WIDTH = 9;
	static constexpr double CBGA_BODY_HEIGHT = 9;
	static constexpr double CBGA_PIN_HEIGHT = .2;
	static constexpr double CBGA_PIN_WIDTH = .2;
	static constexpr double CBGA_EDGE_RADIUS = .9;
	static constexpr double CBGA_PIN_BUFFER = .9;
	static const int CBGA_BUILD_TYPE = 2;

	// Dimensions for with Lead Plastic Shrink Small Outline type (abbreviated SS)
	// Pins on only one side
	static const int SS_NUM_PINS = 14;
	static constexpr double SS_BODY_WIDTH = 5.3;
	static constexpr double SS_BODY_HEIGHT = 10.2;
	static constexpr double SS_PIN_HEIGHT = .75;
	static constexpr double SS_PIN_WIDTH = .3;
	static constexpr double SS_EDGE_RADIUS = .73;
	static constexpr double SS_PIN_BUFFER = .73;
	static const int SS_BUILD_TYPE = 1;

	// Dimensions for with Lead Plastic Quad Flat, No Lead Package type (abbreviated ML)
	// Pins on no sides
	static const int ML_NUM_PINS = 7;
	static constexpr double ML_BODY_WIDTH = 6;
	static constexpr double ML_BODY_HEIGHT = 6;
	static constexpr double ML_PIN_HEIGHT = .55;
	static constexpr double ML_PIN_WIDTH = .3;
	static constexpr double ML_EDGE_RADIUS = .9;
	static constexpr double ML_PIN_BUFFER = .9;
	static const int ML_BUILD_TYPE = 3;

	// PCB

	// PCB BUFFERS BETWEEN SR AND BETWEEN MICROCONTROLLER NO LONGER A STATIC VARIABLE, NOW JUST A LOCAL VARIABLE WITHIN SYNTHESIS
	static constexpr double PCB_BUFFER_SR = 2.5;
	static constexpr double PCB_BUFFER_DMFB = 6;	// Amount of milimeters between end of PCB and DMFB

	static constexpr double PCB_BUFFER_PCB = 2.5;	// Amount of milimeters between end of PCB and Shift Registers
	static constexpr double PCB_BUFFER_ARRAY_TO_SR = 5;	// The minimum amount between the array and the Shift Registers
	static constexpr double PCB_BUFFER_MC = 4;	//6  // Amount of milimeters between end of PCB and Microcontroller

	// Dimensions for VIA
	static constexpr double VIA_CONTACT_WIDTH = 0.2794;	// Full diameter
	static constexpr double VIA_WIDTH = 0.2032;	// Only diameter of hole
	static constexpr double VIA_TRACE_SPACING = 0;	// Space between wires
};


#endif /* HEADERS_MODELS_COMPONENT_DIMENSIONS_H_ */
