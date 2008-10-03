/**
  * SimulatorObject
  *
  * Defines the basic properties and methods relevant to any simulator object,
  *  be it a wall, a tennis ball or a critterbot.
  *
  * @author Marc G. Bellemare
  */
public class SimulatorObject
{
  /** Some physical properties of the object - position, velocity */
  protected double aPosX, aPosY;
  protected double aVelX, aVelY;

  protected String aLabel;

  protected int aId;

  /** Creates a new object with a given label (e.g., "wall") and identifier.
    *
    * @param pLabel The object label
    * @param pId    The object's unique identifier
    */
  public SimulatorObject(String pLabel, int pId)
  {
    aLabel = pLabel;
    aId = pId;
  }
}
