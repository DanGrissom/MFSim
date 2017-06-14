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
/*------------------------Class/Implementation Details--------------------------*
 * Source: ShiftRegister.java													*
 * Original Code Author(s): Jordan Ishii										*
 * Original Completion/Release Date:											*
 *																				*
 * Details: Provides a basic function to output an error via a pop-up dialog.	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
package dmfbSimVisualizer.common;

import java.awt.Color;
import java.awt.Graphics2D;
import java.util.LinkedList;

import dmfbSimVisualizer.common.HardwareConstants.PinType;

public class ShiftRegister {

	/*
	 * Number  of Shift Registers = Math.ceil((numPins-28)/8) if num is > 32
	 */

	//////////////////////////////////////////////////////////////////////////////////////
	// Instance Variables of Shift Register
	//////////////////////////////////////////////////////////////////////////////////////
	private ComponentType type;
	private int xPos;
	private int yPos;
	private int bodyWidth;
	private int bodyHeight;
	private int pinHeight;
	private int pinWidth;
	private int pinBuffer;
	private int edgeRadius;
	private int numPinsPerSide;
	private double conversionFactor;
	private int bodyWidthMM;
	private int rotateDeg;
	private LinkedList<HardwareConstants.PinType> pinTypes = new LinkedList<HardwareConstants.PinType>();

	// Electrode pitch of cells in DMFB (milimeters) (every other value is divided by this number to scale image)
	private int cellDimPixels = 17;	// The ACTUAL amount of pixels taken for cell dim
	private static double cellDimMM = 0;

	//////////////////////////////////////////////////////////////////////////////////////
	// Constructors
	//////////////////////////////////////////////////////////////////////////////////////
	public ShiftRegister(double electrodePitch)
	{
		cellDimMM = electrodePitch / DmfbDrawer.changeInPitch;

		conversionFactor = cellDimPixels / cellDimMM;

		rotateDeg = 0;

		// Default type for Shift Register is Fairchild 74VHC595MTC with Lead Thin Shrink Small Outline Package
		type = ComponentType.FAIRCHILD_TSSOP;
		bodyWidth = (int) (cellDimPixels * HardwareConstants.TSSOP_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * HardwareConstants.TSSOP_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_BUFFER / cellDimMM);
		bodyWidthMM = (int) (HardwareConstants.TSSOP_BODY_WIDTH + HardwareConstants.TSSOP_PIN_HEIGHT * 2);

		numPinsPerSide = HardwareConstants.TSSOP_NUM_PINS;

		xPos = (int) (350 * DmfbDrawer.changeInPitch);
		yPos = (int) (50 * DmfbDrawer.changeInPitch);
		
		for(int i = 0; i < HardwareConstants.TSSOP_PIN_TYPES.length; i++)
			pinTypes.add(HardwareConstants.TSSOP_PIN_TYPES[i]);
	}

	public ShiftRegister(ComponentCoordinate coord, double electrodePitch)
	{
		cellDimMM = electrodePitch / DmfbDrawer.changeInPitch;

		conversionFactor = cellDimPixels / cellDimMM;

		rotateDeg = coord.getRotateDeg();

		// Instantiate type-dependent variables
		switch(coord.getType())
		{
		case FAIRCHILD_TSSOP:
			type = ComponentType.FAIRCHILD_TSSOP;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.TSSOP_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.TSSOP_BODY_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_WIDTH / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_HEIGHT / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.TSSOP_PIN_BUFFER / cellDimMM);
			bodyWidthMM = (int) (HardwareConstants.TSSOP_BODY_WIDTH + HardwareConstants.TSSOP_PIN_HEIGHT * 2);

			numPinsPerSide = HardwareConstants.TSSOP_NUM_PINS;
			
			for(int i = 0; i < HardwareConstants.TSSOP_PIN_TYPES.length; i++)
				pinTypes.add(HardwareConstants.TSSOP_PIN_TYPES[i]);
			break;
		case FAIRCHILD_SOP:
			type = ComponentType.FAIRCHILD_SOP;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.SOP_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.SOP_BODY_HEIGHT / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.SOP_PIN_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.SOP_PIN_WIDTH / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.SOP_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.SOP_PIN_BUFFER / cellDimMM);
			bodyWidthMM = (int) (HardwareConstants.SOP_BODY_WIDTH + HardwareConstants.SOP_PIN_HEIGHT * 2);

			numPinsPerSide = HardwareConstants.SOP_NUM_PINS;
			
			for(int i = 0; i < HardwareConstants.SOP_PIN_TYPES.length; i++)
				pinTypes.add(HardwareConstants.SOP_PIN_TYPES[i]);
			break;
		case FAIRCHILD_SOIC:
			type = ComponentType.FAIRCHILD_SOIC;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.SOIC_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.SOIC_BODY_HEIGHT / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.SOIC_PIN_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.SOIC_PIN_WIDTH / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.SOIC_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.SOIC_PIN_BUFFER / cellDimMM);
			bodyWidthMM = (int) (HardwareConstants.SOIC_BODY_WIDTH + HardwareConstants.SOIC_PIN_HEIGHT * 2);

			numPinsPerSide = HardwareConstants.SOIC_NUM_PINS;
			
			for(int i = 0; i < HardwareConstants.SOIC_PIN_TYPES.length; i++)
				pinTypes.add(HardwareConstants.SOIC_PIN_TYPES[i]);
		}


		// Set position
		xPos = (int) (coord.getX() * DmfbDrawer.changeInPitch);
		yPos = (int) (coord.getY() * DmfbDrawer.changeInPitch);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Draws the given Shift Register type based on the values calculated in constructor
	//////////////////////////////////////////////////////////////////////////////////////
	public void Draw(Graphics2D g2d)
	{
		if(rotateDeg == 0 || rotateDeg == 180)	// NOTE: Dimensions found in Java no longer fully apply if rotated
		{
			// Place the rectangles for each pin
			int leftPinNum = 0;
			int rightPinNum = 0;
			if(rotateDeg == 0)
			{
				leftPinNum = 0;
				rightPinNum = numPinsPerSide;
			}
			else if(rotateDeg == 180)
			{
				leftPinNum = numPinsPerSide * 2 - 1;
				rightPinNum = numPinsPerSide - 1;
			}
				
			for(int i = 0; i < numPinsPerSide; i++)
			{
				PinType leftPinType = pinTypes.get(leftPinNum);
				PinType rightPinType = pinTypes.get(rightPinNum);
				
				// Increment pin numbers
				if(rotateDeg == 0)
				{
					leftPinNum++;
					rightPinNum++;
				}
				else if(rotateDeg == 180)
				{
					rightPinNum--;
					leftPinNum--;
				}

				
				int yCoord = CalculateSRPinY(i);

				int leftXCoord = xPos;
				int rightXCoord = xPos + bodyWidth + pinHeight;
				
				g2d.setColor(Color.LIGHT_GRAY);

				// Draw the pins (height and width are misleading... height is horizontal, width is vertical)
				g2d.fillRect(leftXCoord + 1, yCoord, pinHeight, pinWidth);  // +1 accounts for the border
				g2d.setColor(Color.BLACK);
				g2d.drawRect(leftXCoord + 1, yCoord, pinHeight, pinWidth);
				g2d.setColor(Color.LIGHT_GRAY);
				g2d.fillRect(rightXCoord - 1, yCoord, pinHeight, pinWidth);  // -1 accounts for the border
				g2d.setColor(Color.BLACK);
				g2d.drawRect(rightXCoord - 1, yCoord, pinHeight, pinWidth);
				
				g2d.setColor(Color.BLACK);
				
				DmfbDrawer.DrawRotatedCenteredString(HardwareConstants.getDescByPinType(leftPinType), leftXCoord - DmfbDrawer.getArrayOffsetX() + pinHeight / 2, yCoord - DmfbDrawer.getArrayOffsetY() + pinWidth / 2, pinHeight, pinWidth, false, g2d);
				DmfbDrawer.DrawRotatedCenteredString(HardwareConstants.getDescByPinType(rightPinType), rightXCoord - DmfbDrawer.getArrayOffsetX() + pinHeight / 2, yCoord - DmfbDrawer.getArrayOffsetY() + pinWidth / 2, pinHeight, pinWidth, false, g2d);
			}

			g2d.setColor(Color.BLACK);

			// Draw body
			g2d.fillRoundRect(xPos + pinHeight, yPos, bodyWidth, bodyHeight, edgeRadius, edgeRadius);
		}
		else
		{
			// Place the rectangles for each pin
			int topPinNum = 0;
			int botPinNum = 0;
			if(rotateDeg == 90)
			{
				topPinNum = numPinsPerSide - 1;
				botPinNum = numPinsPerSide * 2 - 1;
			}
			else if(rotateDeg == 270)
			{
				topPinNum = numPinsPerSide;
				botPinNum = 0;
			}
			
			for(int i = 0; i < numPinsPerSide; i++)
			{
				PinType topPinType = pinTypes.get(topPinNum);
				PinType botPinType = pinTypes.get(botPinNum);
				
				// Increment pin numbers
				if(rotateDeg == 90)
				{
					topPinNum--;
					botPinNum--;
				}
				else if(rotateDeg == 270)
				{
					topPinNum++;
					botPinNum++;
				}
				
				int xCoord = CalculateSRPinX(i);

				int topYCoord = yPos;
				int botYCoord = yPos + bodyWidth + pinHeight;

				g2d.setColor(Color.LIGHT_GRAY);
			
				// Draw the pins (height and width are misleading... height is horizontal, width is vertical)
				g2d.fillRect(xCoord, topYCoord + 1, pinWidth, pinHeight);  // +1 accounts for the border
				g2d.setColor(Color.BLACK);
				g2d.drawRect(xCoord, topYCoord + 1, pinWidth, pinHeight);
				g2d.setColor(Color.LIGHT_GRAY);
				g2d.fillRect(xCoord, botYCoord - 1, pinWidth, pinHeight);  // -1 accounts for the border
				g2d.setColor(Color.BLACK);
				g2d.drawRect(xCoord, botYCoord - 1, pinWidth, pinHeight);
				
				g2d.setColor(Color.BLACK);
				
				DmfbDrawer.DrawRotatedCenteredString(HardwareConstants.getDescByPinType(topPinType), xCoord - DmfbDrawer.getArrayOffsetX() + pinHeight / 2, topYCoord - DmfbDrawer.getArrayOffsetY() + pinWidth / 2 + 2, pinHeight, pinWidth, true, g2d);
				DmfbDrawer.DrawRotatedCenteredString(HardwareConstants.getDescByPinType(botPinType), xCoord - DmfbDrawer.getArrayOffsetX() + pinHeight / 2, botYCoord - DmfbDrawer.getArrayOffsetY() + pinWidth / 2 + 2, pinHeight, pinWidth, true, g2d);
			}

			g2d.setColor(Color.BLACK);

			// Draw body (NOTE: (xPos, yPos) signifies top left of body
			g2d.fillRoundRect(xPos, yPos + pinHeight, bodyHeight, bodyWidth, edgeRadius, edgeRadius);
		}
		
		// Draw pin 1 marker
		g2d.setColor(Color.WHITE);
		if(rotateDeg == 0)
			g2d.fillOval(xPos + pinHeight + pinBuffer, yPos + pinBuffer, pinWidth, pinWidth);
		else if(rotateDeg == 90)
			g2d.fillOval(xPos + bodyHeight - pinBuffer - pinWidth, yPos + pinHeight + pinBuffer, pinWidth, pinWidth);
		else if(rotateDeg == 180)
			g2d.fillOval(xPos + pinHeight + bodyWidth - pinWidth - pinBuffer, yPos + bodyHeight - pinBuffer - pinWidth, pinWidth, pinWidth);
		else
			g2d.fillOval(xPos + pinBuffer, yPos + pinHeight + bodyWidth - pinBuffer - pinWidth, pinWidth, pinWidth);
		g2d.setColor(Color.BLACK);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Calculates the y value for the shift register pin given index
	// ranging from 0 - (numPins-1)
	//////////////////////////////////////////////////////////////////////////////////////
	private int CalculateSRPinY(int index)
	{	
		int y = yPos + pinBuffer;

		// Calculate the size of the blank spaces between pins
		int pinBlank = (bodyHeight - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);

		y += (pinWidth * (index)) + (pinBlank * index);


		return y;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Calculates the x value for the rotated shift register pin given index
	// ranging from 0 - (numPins-1)
	//////////////////////////////////////////////////////////////////////////////////////
	private int CalculateSRPinX(int index)
	{	
		int x = xPos + pinBuffer;

		// Calculate the size of the blank spaces between pins
		int pinBlank = (bodyHeight - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);

		x += (pinWidth * (index)) + (pinBlank * index);


		return x;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Moves the component to a different location
	//////////////////////////////////////////////////////////////////////////////////////
	public void ChangeLocation(int x, int y)
	{
		xPos = x;
		yPos = y;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Resizes all Shift Register Variables to account for a change in dimensions
	//////////////////////////////////////////////////////////////////////////////////////
	public static void Resize(int cellSize)
	{
		cellDimMM = cellSize;
	}

	public int testGetPinX()
	{
		int x = 0;

		x += xPos + 1 + bodyWidth + pinHeight * 2;

		return x;
	}

	public int testGetPinY()
	{
		int y = 0;

		y += yPos + pinBuffer + pinWidth / 2;

		return y;
	}
}







/*// Factor in lack of decimal precision in Java AWT by adding to space between pins
if(i % 2 == 0 && ((srBodyHeight/(numPinsPerSide*2-1)) * (numPinsPerSide*2-1)) < srBodyHeight)
{
	// Add part of remainder
	y += (srBodyHeight % (srBodyHeight/(numPinsPerSide*2-1)) * (numPinsPerSide*2-1)) / (numPinsPerSide/2-1);

	y += srBodyHeight / (numPinsPerSide * 2 - 1);
}
else
{*/