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
 * Source: Microcontroller.java													*
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

public class Microcontroller {
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Instance Variables of Shift Register
	//////////////////////////////////////////////////////////////////////////////////////
	private ComponentType type;
	private int xPos;
	private int yPos;
	private int buildType;
	private int bodyWidth;
	private int bodyHeight;
	private int pinHeight;
	private int pinWidth;
	private int pinBuffer;
	private int edgeRadius;
	private int numPinsPerSide;
	private boolean useRem;
	private LinkedList<HardwareConstants.PinType> pinTypes = new LinkedList<HardwareConstants.PinType>();

	// Electrode pitch of cells in DMFB (milimeters) (every other value is divided by this number to scale image)
	private static int cellDimPixels = 17;	// The ACTUAL amount of pixels taken for cell dim
	private static double cellDimMM = 0;

	//////////////////////////////////////////////////////////////////////////////////////
	// Constructors
	//////////////////////////////////////////////////////////////////////////////////////
	public Microcontroller(double electrodePitch, boolean useRemainder)
	{
		useRem = useRemainder;
		cellDimMM = electrodePitch / DmfbDrawer.changeInPitch;

		// Default type for Microcontroller is ATmega2560 with Thin Profile Plastic Quad Flat Package type (TQFP)
		type = ComponentType.ATMEGA_TQFP;
		buildType = HardwareConstants.TQFP_BUILD_TYPE;
		bodyWidth = (int) (cellDimPixels * HardwareConstants.TQFP_BODY_WIDTH / cellDimMM);
		bodyHeight = (int) (cellDimPixels * HardwareConstants.TQFP_BODY_HEIGHT / cellDimMM);
		pinWidth = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_WIDTH / cellDimMM);
		pinHeight = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_HEIGHT / cellDimMM);
		pinBuffer = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_BUFFER / cellDimMM);
		edgeRadius = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_BUFFER / cellDimMM);

		numPinsPerSide = HardwareConstants.TQFP_NUM_PINS;
		
		xPos = (int) (350 * DmfbDrawer.changeInPitch);
		yPos = (int) (50 * DmfbDrawer.changeInPitch);
	}

	public Microcontroller(ComponentCoordinate coord, double electrodePitch, boolean useRemainder)
	{
		useRem = useRemainder;
		cellDimMM = electrodePitch / DmfbDrawer.changeInPitch;

		// Instantiate type-dependent variables
		switch(coord.getType())
		{
		case ATMEGA_TQFP:
			type = ComponentType.ATMEGA_TQFP;
			buildType = HardwareConstants.TQFP_BUILD_TYPE;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.TQFP_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.TQFP_BODY_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_WIDTH / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_HEIGHT / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.TQFP_PIN_BUFFER / cellDimMM);

			numPinsPerSide = HardwareConstants.TQFP_NUM_PINS;
			
			// Define the pin types
			for(int i = 0; i < numPinsPerSide * 4; i++)
			{
				if(i == 20 || i == 51 || i == 63)
					pinTypes.add(PinType.MC_VCC);
				else if(i == 21 || i == 52 || i == 62)
					pinTypes.add(PinType.MC_GROUND);
				else if(i == 22 || i == 23)
					pinTypes.add(PinType.MC_TXAL);
				else if(i == 19)
					pinTypes.add(PinType.MC_RESET);
				else if(i == 61)
					pinTypes.add(PinType.MC_OTHER);
				else
					pinTypes.add(PinType.MC_DIGITAL_IO);
			}
			break;
		case ATMEGA_CBGA:
			type = ComponentType.ATMEGA_CBGA;
			buildType = HardwareConstants.CBGA_BUILD_TYPE;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.CBGA_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.CBGA_BODY_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.CBGA_PIN_WIDTH / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.CBGA_PIN_HEIGHT / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.CBGA_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.CBGA_PIN_BUFFER / cellDimMM);

			numPinsPerSide = HardwareConstants.CBGA_NUM_PINS;
			
			// Define the pin types
			for(int i = 0; i < numPinsPerSide * 2; i++)
			{
				pinTypes.add(PinType.MC_DIGITAL_IO);
			}
			break;
		case MICROCHIP_SS:
			type = ComponentType.MICROCHIP_SS;
			buildType = HardwareConstants.SS_BUILD_TYPE;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.SS_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.SS_BODY_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.SS_PIN_WIDTH / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.SS_PIN_HEIGHT / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.SS_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.SS_PIN_BUFFER / cellDimMM);

			numPinsPerSide = HardwareConstants.SS_NUM_PINS;
			
			// Define the pin types
			for(int i = 0; i < numPinsPerSide * 4; i++)
			{
				pinTypes.add(PinType.MC_DIGITAL_IO);
			}
			break;
		case MICROCHIP_ML:
			type = ComponentType.MICROCHIP_ML;
			buildType = HardwareConstants.ML_BUILD_TYPE;
			bodyWidth = (int) (cellDimPixels * HardwareConstants.ML_BODY_WIDTH / cellDimMM);
			bodyHeight = (int) (cellDimPixels * HardwareConstants.ML_BODY_HEIGHT / cellDimMM);
			pinWidth = (int) (cellDimPixels * HardwareConstants.ML_PIN_WIDTH / cellDimMM);
			pinHeight = (int) (cellDimPixels * HardwareConstants.ML_PIN_HEIGHT / cellDimMM);
			pinBuffer = (int) (cellDimPixels * HardwareConstants.ML_PIN_BUFFER / cellDimMM);
			edgeRadius = (int) (cellDimPixels * HardwareConstants.ML_PIN_BUFFER / cellDimMM);

			numPinsPerSide = HardwareConstants.ML_NUM_PINS;
			
			// Define the pin types
			for(int i = 0; i < numPinsPerSide * 4; i++)
			{
				pinTypes.add(PinType.MC_DIGITAL_IO);
			}
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
		switch(buildType)
		{
		case 0:
			// Place the rectangles for each top and bottom side pin
			int topPinNum = numPinsPerSide * 4 - 1;
			int botPinNum = numPinsPerSide;
			for(int i = 0; i < numPinsPerSide; i++)
			{
				PinType topPinType = pinTypes.get(topPinNum);
				PinType botPinType = pinTypes.get(botPinNum);
				
				// Increment pin numbers
				topPinNum--;
				botPinNum++;
				
				int xCoord = CalculateMCPinX(i);

				int topYCoord = yPos - pinHeight;
				int botYCoord = yPos + bodyHeight;

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

		case 1:
			// Place the rectangles for each left and right side pin
			int rightPinNum = numPinsPerSide * 3 - 1;
			int leftPinNum = 0;
			for(int i = 0; i < numPinsPerSide; i++)
			{
				PinType leftPinType = pinTypes.get(leftPinNum);
				PinType rightPinType = pinTypes.get(rightPinNum);
				
				// Increment pin numbers
				rightPinNum--;
				leftPinNum++;
				
				int yCoord = CalculateMCPinY(i);

				int leftXCoord = xPos - pinHeight;
				int rightXCoord = xPos + bodyWidth;

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
			
			// Draw body
			g2d.fillRoundRect(xPos, yPos, bodyWidth, bodyHeight, edgeRadius, edgeRadius);
			
			g2d.setColor(Color.WHITE);
			
			// Draw pin 1 marker
			g2d.fillOval(xPos + pinBuffer, yPos + pinBuffer, pinWidth, pinWidth);
			
			g2d.setColor(Color.BLACK);
				break;
			case 2:
				g2d.setColor(Color.BLACK);

				// Draw body
				g2d.fillRoundRect(xPos, yPos, bodyWidth, bodyHeight, edgeRadius, edgeRadius);
				
				g2d.setColor(Color.WHITE);
				
				// Place the circles inside of the body
				for(int i = 0; i < numPinsPerSide; i++)
				{	
					for(int j = 0; j < numPinsPerSide; j++)
					{
						g2d.fillOval(xPos + pinBuffer - (pinWidth / 2) + (i *(bodyWidth - (pinBuffer * 2)) / (numPinsPerSide - 1)), yPos + pinBuffer - (pinHeight / 2) + (j *(bodyWidth - (pinBuffer * 2)) / (numPinsPerSide - 1)), pinWidth, pinHeight);
					}
				}
				break;
			case 3:
				g2d.setColor(Color.BLACK);
				// Draw body
				g2d.fillRoundRect(xPos, yPos, bodyWidth, bodyHeight, edgeRadius, edgeRadius);
				
				// Place the rectangles for each left and right side pin
				for(int i = 0; i < numPinsPerSide; i++)
				{
					int yCoord = CalculateMCPinY(i);

					int leftXCoord = xPos;
					int rightXCoord = xPos + bodyWidth - pinHeight;

					g2d.setColor(Color.LIGHT_GRAY);

					// Draw the pins (height and width are misleading... height is horizontal, width is vertical)
					g2d.fillRect(leftXCoord, yCoord, pinHeight, pinWidth);  // +1 accounts for the border
					g2d.fillOval(leftXCoord + (pinHeight / 2), yCoord, pinHeight, pinWidth);
					g2d.fillRect(rightXCoord, yCoord, pinHeight, pinWidth);  // -1 accounts for the border
					g2d.fillOval(rightXCoord - (pinHeight / 2), yCoord, pinHeight, pinWidth);
				}
				
				// Place the rectangles for each top and bottom side pin
				for(int i = 0; i < numPinsPerSide; i++)
				{
					int xCoord = CalculateMCPinX(i);

					int topYCoord = yPos - pinHeight;
					int botYCoord = yPos + bodyHeight;

					g2d.setColor(Color.LIGHT_GRAY);

					// Draw the pins (height and width are misleading... height is horizontal, width is vertical)
					g2d.fillRect(xCoord, topYCoord + pinHeight, pinWidth, pinHeight);  // +1 accounts for the border
					g2d.fillOval(xCoord, topYCoord + (pinHeight / 2) + pinHeight, pinWidth, pinHeight);
					g2d.fillRect(xCoord, botYCoord - pinHeight, pinWidth, pinHeight);  // -1 accounts for the border
					g2d.fillOval(xCoord, botYCoord - (pinHeight / 2) - pinHeight, pinWidth, pinHeight);
				}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Calculates the y value for the shift register pin given index
	// ranging from 0 - (numPins-1)
	//////////////////////////////////////////////////////////////////////////////////////
	private int CalculateMCPinY(int index)
	{	
		int y = yPos + pinBuffer;

		// Calculate the size of the blank spaces between pins
		int pinBlank = (bodyHeight - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);
		
		y += (pinWidth * (index)) + (pinBlank * index);

		return y;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Calculates the x value for the shift register pin given index
	// ranging from 0 - (numPins-1) (used for the top and bottom)
	//////////////////////////////////////////////////////////////////////////////////////
	private int CalculateMCPinX(int index)
	{	
		int x = xPos + pinBuffer;

		// Calculate the size of the blank spaces between pins
		int pinBlank = (bodyWidth - (2 * pinBuffer + numPinsPerSide * pinWidth)) / (numPinsPerSide-1);
		
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
}
