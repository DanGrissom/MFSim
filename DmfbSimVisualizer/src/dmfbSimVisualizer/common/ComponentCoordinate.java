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
 * Source: ComponentCoordinate.java												*
 * Original Code Author(s): Jordan Ishii										*
 * Original Completion/Release Date:											*
 *																				*
 * Details: Provides a coordinate location for hardware components.          	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
package dmfbSimVisualizer.common;

public class ComponentCoordinate {
	private ComponentType type;
	private int x;
	private int y;
	private int rotateDeg;
	
	// Vias ONLY
	private int beginLayer;
	private int endLayer;
	
	public ComponentCoordinate(ComponentType srType, String xCoord, String yCoord, String rotate, String beginLayerNum, String endLayerNum) {
		type = srType;
		x = Integer.parseInt(xCoord);
		y = Integer.parseInt(yCoord);
		beginLayer = Integer.parseInt(beginLayerNum);
		endLayer = Integer.parseInt(endLayerNum);
		
		rotateDeg = Integer.parseInt(rotate);
	}
	
	public ComponentType getType() { return type; }
	public int getX() { return x; }
	public int getY() { return y; }
	public int getBeginLayer() { return beginLayer; }
	public int getEndLayer() { return endLayer; }
	public int getRotateDeg() { return rotateDeg; }
	
	public void setType(ComponentType srType) { type = srType; }
	public void setX(int xCoord) { x = xCoord; }
	public void setY(int yCoord) { y = yCoord; }
	public void setBeginLayer(int beginLayerNum) { beginLayer = beginLayerNum; }
	public void setEndLayer(int endLayerNum) { endLayer = endLayerNum; }
	public void setIsRotated(int rotate) { rotateDeg = rotate; }
}
