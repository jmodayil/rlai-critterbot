import java.awt.Color;
import java.awt.Graphics;

/**
  * SimulatorAgent
  *
  * This class defines agent-specific methods and values. This includes
  *  sensors (which should be specific classes, or at least bundled into a
  *  Sensor class), actions and such.
  *
  * @author Marc G. Bellemare
  */
public class SimulatorAgent extends SimulatorObject
{
  /** Three values representing the current robot command */
  protected double aCmdX, aCmdY, aCmdTheta;
  
  /** Creates a new instance of a SimulatorAgent
    */
  public SimulatorAgent(String pLabel, int pId)
  {
    super(pLabel, pId);
    aCmdX = aCmdY = aCmdTheta;
  }
  
  public void drawAgent(Graphics g) {
	  Color tempC = g.getColor();
	  g.setColor(Color.black);
	  g.drawOval((int)aPos.x - 10, (int)aPos.y - 10, 20, 20);
	  g.setColor(Color.red);
	  g.drawLine((int)aPos.x, (int)aPos.y, (int)(aPos.x + 10 * Math.sin(aDir)), (int)(aPos.y + 10 * Math.cos(aDir)));
	  g.setColor(Color.lightGray);
	  g.drawString(aLabel, (int)aPos.x + 10, (int)aPos.y + 20);
	  g.setColor(tempC);
  }
}
