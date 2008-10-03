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
}
