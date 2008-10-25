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
  // @@@ Move all of these but position into a Physics 'state'
  protected Point2D.Double aPos;
  protected double aDir;
  protected Vector2D aVel;
  protected double aRot;
  protected double mass;
  protected double momI;
  
  protected String aLabel;

  /** A unique identifier which is used to keep track of an object from state
    *  to state
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
  
  public int getId()
  {
    return aId;
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
 
 
  /** Makes a copy of this object. For cloning purposes, the Object id 
    *  remains identical. This method's purpose is to be used for copying
    *  objects between states.
    */
  public Object clone()
  {
    SimulatorObject newObj = new SimulatorObject(this.aLabel, this.aId);

    newObj.copyFrom(this);
    return newObj; 
  }

  /** Copies the data from 'original' onto this object. This is used for
    *  cloning purposes.
    */
  public void copyFrom(SimulatorObject org)
  {
    this.aPos = (Point2D.Double) org.aPos.clone();
  
    /* @@@ Copy the other attributes of the object, the actuator and sensor 
     * list */
  
    // @@@ remove these attributes, move to physics
    this.aDir = org.aDir;
    this.aVel = new Vector2D(org.aVel.x, org.aVel.y);
    this.aRot = org.aRot;
    this.mass = org.mass;
    this.momI = org.momI;
  }
}
