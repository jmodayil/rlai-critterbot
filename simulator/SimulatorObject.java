import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Point2D;

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
  /** Some physical properties of the object - position, velocity, mass, moment of intertia */
  protected Point2D.Double aPos;
  protected double aDir;
  protected Vector2D aVel;
  protected double aRot;
  protected double mass;
  protected double momI;
  
  protected String aLabel;

  /** Is this useful?  If it is, shouldn't it be automatically assigned by the constructor rather than
   *  passed as an argument?
   */
  protected int aId;

  /** Creates a new object with a given label (e.g., "wall") and identifier.
    *
    * @param pLabel The object label
    * @param pId    The object's unique identifier
    */
  public SimulatorObject(String pLabel, int pId)
  {
    aLabel = new String(pLabel);
    aId = pId;
    aPos = new Point2D.Double(0,0);
    aDir = 0;
    aVel = new Vector2D(0,0);
    aRot = 0;
    mass = 0;
    momI = 0;
  }
  
  private void loadObject() {
	  // Open files, lookup plabel, etc???
  }
  
  /**
   * 
   * 
   * @param obj
   * @param g
   */
  public void drawObj(Graphics g){
	  
  }
  
  public void setPosition(Point2D.Double newPos) {
	  aPos.x = newPos.x;
	  aPos.y = newPos.y;
  }

  void setVelocity(Vector2D newVel) {
	  aVel.x = newVel.x;
	  aVel.y = newVel.y;
  }
	
  void setMoment(double newMoment) {
	  momI = newMoment;
  }
	
  void setMass(double newMass) {
	  mass = newMass;
  }
	
  Point2D.Double getPosition() {
	  return aPos;
  }

  Vector2D getVelocity() {
	  return aVel;
  }

  double getMass() {
	  return mass;
  }

  double getMoment() {
	  return momI;
  }
  
}
