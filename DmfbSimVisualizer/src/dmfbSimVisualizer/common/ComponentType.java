package dmfbSimVisualizer.common;

import dmfbSimVisualizer.parsers.HardwareParser;

///////////////////////////////////////////////////////////////////////////////
// Types of popular shift registers as found on mouser.com. The Fairchild
// 74VHC595MTC 8-bit Shift Register is the main one used here. The first three
// types are all derived from this, and they vary in the type of their
// packages. (SOIC = Lead Small Outline Integrated Circuit, SOP = Lead Small 
// Outline Package, TSSOP = Lead Thin Shrink Small Outline Package)
// The dimensions for these types are found at: 
// http://www.mouser.com/ds/2/149/74VHC595-101446.pdf
//
// Types of popular microcontrollers are found on mouser.com. The ATmega2560
// Microcontroller is the main one used here. Similar to the Shift Registers,
// the ones that are currently implemented are variations in the package type
// their dimensions are found at:
// http://www.atmel.com/devices/atmega2560.aspx
//
// HOW TO ADD A COMPONENT TYPE:
///////////////////////////////////////////////////////////////////////////////
public enum ComponentType {

	// SPECIFICATIONS IN HARDWARECONSTANTS FILE

	// Shift Registers
	FAIRCHILD_SOIC,	// Lead Small Outline Integrated Circuit
	FAIRCHILD_SOP,	// Lead Small Outline Package
	FAIRCHILD_TSSOP,	// Lead Thin Shrink Small Outline Package

	// Microcontrollers
	ATMEGA_TQFP,	// Thin Profile Plastic Quad Flat Package
	ATMEGA_CBGA,	// Chip Array BGA Package
	MICROCHIP_SS,	// Microchip 579-PIC16F18855-I/SP with a Lead Plastic Shrink Small Outline package type (abbreviated SS)
	MICROCHIP_ML,	// Microchip 579-PIC16F18855-I/SP with a Lead Plastic Quad Flat, No Lead Package type
	
	// Via
	VIA,
	
	// Broad component types (only used for shifting PCB sections)
	MICROCONTROLLER,
	SHIFTREGISTER,
	ARRAY;

	public static ComponentType getShiftRegisterWithID(int x)
	{
		ComponentType type = null;

		switch(x) {
		case 0:
			type = FAIRCHILD_SOIC;
			break;
		case 1:
			type = FAIRCHILD_SOP;
			break;
		case 2:
			type = FAIRCHILD_TSSOP;
		}

		return type;
	}

	public static ComponentType getMicrocontrollerWithID(int x)
	{
		ComponentType type = null;

		switch(x) {
		case 0:
			type = ATMEGA_TQFP;
			break;
		case 1:
			type = ATMEGA_CBGA;
			break;
		case 2:
			type = MICROCHIP_SS;
			break;
		case 3:
			type = MICROCHIP_ML;
		}

		return type;
	}

	// Gets component width in pixels
	public static int getComponentWidth(ComponentType type)
	{
		int width = -1;
		
		// Calculate conversion factor from mm to pixels
		double conversionFactor = 17 * DmfbDrawer.changeInPitch / (HardwareParser.getElectronMicrons() / 1000);

		// Find width by type
		switch(type)
		{
			case FAIRCHILD_SOIC:
				width = (int) (HardwareConstants.SOIC_BODY_WIDTH);
				break;
			case FAIRCHILD_SOP:
				width = (int) (HardwareConstants.SOP_BODY_WIDTH);
				break;
			case FAIRCHILD_TSSOP:
				width = (int) (HardwareConstants.TSSOP_BODY_WIDTH);
				break;
			case ATMEGA_CBGA:
				width = (int) (HardwareConstants.CBGA_BODY_WIDTH);
				break;
			case ATMEGA_TQFP:
				width = (int) (HardwareConstants.TQFP_BODY_WIDTH);
				break;
			case MICROCHIP_ML:
				width = (int) (HardwareConstants.ML_BODY_WIDTH);
				break;
			case MICROCHIP_SS:
				width = (int) (HardwareConstants.SS_BODY_WIDTH);
		}
		
		return width;
	}

	// Gets component height in pixels
	public static int getComponentHeight(ComponentType type)
	{
		int height = -1;
		
		// Calculate conversion factor from mm to pixels
		double conversionFactor = 17 * DmfbDrawer.changeInPitch / (HardwareParser.getElectronMicrons() / 1000);

		// Find width by type
		switch(type)
		{
			case FAIRCHILD_SOIC:
				height = (int) (HardwareConstants.SOIC_BODY_HEIGHT);
				break;
			case FAIRCHILD_SOP:
				height = (int) (HardwareConstants.SOP_BODY_HEIGHT);
				break;
			case FAIRCHILD_TSSOP:
				height = (int) (HardwareConstants.TSSOP_BODY_HEIGHT );
				break;
			case ATMEGA_CBGA:
				height = (int) (HardwareConstants.CBGA_BODY_HEIGHT);
				break;
			case ATMEGA_TQFP:
				height = (int) (HardwareConstants.TQFP_BODY_HEIGHT);
				break;
			case MICROCHIP_ML:
				height = (int) (HardwareConstants.ML_BODY_HEIGHT);
				break;
			case MICROCHIP_SS:
				height = (int) (HardwareConstants.SS_BODY_HEIGHT);
		}
		
		return height;
	}
}
