package dmfbSimVisualizer.common;

import java.awt.Color;

public class HardwareConstants {
	
	// Possible pin types for Microcontroller and Shift Register
	protected enum PinType { IO_SERIAL_INPUT, IO_SR_CLOCK_INPUT, IO_PARALLEL_OUTPUT, IO_SERIAL_OUTPUT, IO_RESET_INPUT, IO_STORAGE_CLOCK_INPUT, IO_3STATE_INPUT, IO_GROUND, IO_VOLTAGE_EMITTER, IO_OTHER, MC_DIGITAL_IO, MC_GROUND, MC_VCC, MC_ANALOG_PIN, MC_ANALOG_REF, MC_RESET, MC_TXAL, MC_OTHER };

	// Returns a short description of the pin to be put on the pin (TEN LETTER MAX ON DESCRIPTIONS)
	protected static String getDescByPinType(PinType pinType)
	{
		String desc = "";
		
		switch(pinType)
		{
		case IO_SERIAL_INPUT:
			desc = "Serial In";
			break;
		case IO_SR_CLOCK_INPUT:
			desc = "SR Clock";
			break;
		case IO_PARALLEL_OUTPUT:
			desc = "Output";
			break;
		case IO_SERIAL_OUTPUT:
			desc = "Serial O";
			break;
		case IO_RESET_INPUT:
			desc = "Reset In";
			break;
		case IO_STORAGE_CLOCK_INPUT:
			desc = "Sto Clock";
			break;
		case IO_3STATE_INPUT:
			desc = "3 State In";
			break;
		case IO_GROUND:
			desc = "Ground";
			break;
		case IO_VOLTAGE_EMITTER:
			desc = "Voltage";
			break;
		case IO_OTHER:
			desc = "Other";
			break;
		case MC_ANALOG_PIN:
			desc = "Analog Pin";
			break;
		case MC_ANALOG_REF:
			desc = "Analog Ref";
			break;
		case MC_DIGITAL_IO:
			desc = "Digital IO";
			break;
		case MC_GROUND:
			desc = "Ground";
			break;
		case MC_OTHER:
			desc = "Other";
			break;
		case MC_RESET:
			desc = "Reset";
			break;
		case MC_TXAL:
			desc = "TXAL";
			break;
		case MC_VCC:
			desc = "VCC";
		}
		
		return desc;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Microcontrollers
	//////////////////////////////////////////////////////////////////////////////////////

	// Number of pins in dmfb (taken from parsing of hardware file... decides how many shift registers)
	public static int numPins = 0;

	// Dimensions for ATmega2560 with Thin Profile Plastic Quad Flat Package type (TQFP)
	public static final int TQFP_NUM_PINS = 16;
	public static final int TQFP_NUM_OUTPUTS = 8;
	public static final double TQFP_BODY_WIDTH = 14;	// Actually 14, but 15 works better visually
	public static final double TQFP_BODY_HEIGHT = 14;
	public static final double TQFP_PIN_HEIGHT = 1;
	public static final double TQFP_PIN_WIDTH = .375;
	public static final double TQFP_EDGE_RADIUS = .75;
	public static final double TQFP_PIN_BUFFER = .75;
	public static final int TQFP_BUILD_TYPE = 0;	// 0 = leads all four sides, 1 = leads on only left and right, 2 = circles, 3 = leads inside body

	// Dimensions for ATmega2560 with Chip Array BGA Package type (CBGA)
	public static final int CBGA_NUM_PINS = 10;
	public static final int CBGA_NUM_OUTPUTS = 8;
	public static final double CBGA_BODY_WIDTH = 9;
	public static final double CBGA_BODY_HEIGHT = 9;
	public static final double CBGA_PIN_HEIGHT = .2;
	public static final double CBGA_PIN_WIDTH = .2;
	public static final double CBGA_EDGE_RADIUS = .9;
	public static final double CBGA_PIN_BUFFER = .9;
	public static final int CBGA_BUILD_TYPE = 2;

	// Dimensions for with Lead Plastic Shrink Small Outline type (abbreviated SS)
	public static final int SS_NUM_PINS = 14;
	public static final int SS_NUM_OUTPUTS = 8;
	public static final double SS_BODY_WIDTH = 5.3;
	public static final double SS_BODY_HEIGHT = 10.2;
	public static final double SS_PIN_HEIGHT = .75;
	public static final double SS_PIN_WIDTH = .3;
	public static final double SS_EDGE_RADIUS = .73;
	public static final double SS_PIN_BUFFER = .73;
	public static final int SS_BUILD_TYPE = 1;

	// Dimensions for with Lead Plastic Quad Flat, No Lead Package type (abbreviated ML)
	public static final int ML_NUM_PINS = 7;
	public static final int ML_NUM_OUTPUTS = 8;
	public static final double ML_BODY_WIDTH = 6;
	public static final double ML_BODY_HEIGHT = 6;
	public static final double ML_PIN_HEIGHT = .55;
	public static final double ML_PIN_WIDTH = .3;
	public static final double ML_EDGE_RADIUS = .9;
	public static final double ML_PIN_BUFFER = .9;
	public static final int ML_BUILD_TYPE = 3;

	//////////////////////////////////////////////////////////////////////////////////////
	// PCB
	//////////////////////////////////////////////////////////////////////////////////////

	// Only one type of PCB, specified by the research papers
	public static final double PCB_BUFFER_DMFB = 12.7;	// Amount of milimeters between end of PCB and DMFB
	public static final double PCB_BUFFER_SR = 2;	// Amount of milimeters between end of PCB and Shift Registers
	public static final double PCB_BUFFER_ARRAY_TO_SR = 4;	// The minimum amount between the array and the Shift Registers
	public static final double PCB_BUFFER_MC = 4;	// Amount of milimeters between end of PCB and Microcontroller

	// 12.7
	//////////////////////////////////////////////////////////////////////////////////////
	// Shift Registers
	//////////////////////////////////////////////////////////////////////////////////////

	// Dimensions for Fairchild 74VHC595MTC 8-bit Shift Register with Lead Small Outline Integrated Circuit package type
	public static final int SOIC_NUM_PINS = 8;	// Number of pins on one side of Shift Register
	public static final int SOIC_NUM_OUTPUTS = 8;
	public static final double SOIC_BODY_WIDTH = 3.9;
	public static final double SOIC_BODY_HEIGHT = 9.9;
	public static final double SOIC_PIN_HEIGHT = 1;	// Usually larger than height
	public static final double SOIC_PIN_WIDTH = .5;
	public static final double SOIC_EDGE_RADIUS = 0;	// The height and width of the rounded corners
	public static final double SOIC_PIN_BUFFER = .3;	// The buffer outside each side of pins (often same as edge radius)
	public static final PinType SOIC_PIN_TYPES[] = {PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT,
			PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT,
			PinType.IO_GROUND, PinType.IO_SERIAL_OUTPUT, PinType.IO_RESET_INPUT, PinType.IO_SR_CLOCK_INPUT,
			PinType.IO_STORAGE_CLOCK_INPUT, PinType.IO_3STATE_INPUT, PinType.IO_SERIAL_INPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_VOLTAGE_EMITTER};

	
	// Dimensions for Fairchild 74VHC595MTC 8-bit Shift Register with Lead Small Outline Package package type
	public static final int SOP_NUM_PINS = 4;
	public static final int SOP_NUM_OUTPUTS = 8;
	public static final double SOP_BODY_WIDTH = 5.3;
	public static final double SOP_BODY_HEIGHT = 10.2;
	public static final double SOP_PIN_HEIGHT = 2.13;
	public static final double SOP_PIN_WIDTH = .6;
	public static final double SOP_EDGE_RADIUS = .47;
	public static final double SOP_PIN_BUFFER = .47;
	// List of types of pins on Shift Register.  These progress on the left side from top to bottom then the right bottom to top (from device specifications)
	public static final PinType SOP_PIN_TYPES[] = {PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT,
		PinType.IO_GROUND, PinType.IO_SERIAL_OUTPUT, PinType.IO_SR_CLOCK_INPUT,
		PinType.IO_SERIAL_INPUT, PinType.IO_VOLTAGE_EMITTER};

	// Dimensions for Fairchild 74VHC595MTC 8-bit Shift Register with Lead Lead Thin Shrink Small Outline Package package type
	public static final int TSSOP_NUM_PINS = 8;
	public static final int TSSOP_NUM_OUTPUTS = 8;
	public static final double TSSOP_BODY_WIDTH = 4.4;
	public static final double TSSOP_BODY_HEIGHT = 5;
	public static final double TSSOP_PIN_HEIGHT = 1;
	public static final double TSSOP_PIN_WIDTH = .3;
	public static final double TSSOP_EDGE_RADIUS = .11;
	public static final double TSSOP_PIN_BUFFER = .11;
	// List of types of pins on Shift Register.  These progress on the left side from top to bottom then the right bottom to top (from device specifications) (ERROR RIGHT NOW... MUST BE INPUTTED WITH SECOND SIDE BACKWARDS TO MATCH REAL LIFE AND C++ SIMULATOR)
	public static final PinType TSSOP_PIN_TYPES[] = {PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT,
		PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT, PinType.IO_PARALLEL_OUTPUT,
		PinType.IO_GROUND, PinType.IO_VOLTAGE_EMITTER, PinType.IO_PARALLEL_OUTPUT, PinType.IO_SERIAL_INPUT,
		PinType.IO_3STATE_INPUT, PinType.IO_STORAGE_CLOCK_INPUT, PinType.IO_SR_CLOCK_INPUT, PinType.IO_RESET_INPUT, PinType.IO_SERIAL_OUTPUT};

	//////////////////////////////////////////////////////////////////////////////////////
	// VIA
	//////////////////////////////////////////////////////////////////////////////////////

	// Only one type of PCB, specified by the research papers
	public static final double VIA_CONTACT_WIDTH = 0.2794;	// Full diameter			(.6)
	public static final double VIA_WIDTH = 0.2032;	// Only diameter of hole			(.35)
	public static final double VIA_TRACE_SPACING = 0;	// Space between wires

	//////////////////////////////////////////////////////////////////////////////////////
	// WIRE COLORS
	//////////////////////////////////////////////////////////////////////////////////////

	public static final Color[] COLORS = {Color.RED, Color.BLUE, Color.GREEN, Color.ORANGE, Color.MAGENTA, Color.PINK, Color.CYAN, Color.YELLOW};
	public static int orthogonalCapacity;	// ceil((pitch-contactwidth-minspacebetweenwires)/(wireTraceWidth-MinSpaceBetweenWires))
}
