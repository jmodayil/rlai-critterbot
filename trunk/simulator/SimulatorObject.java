/**
  * SimulatorObject
  *
  * Defines the basic properties and methods relevant to any simulator object,
  *  be it a wall, a tennis ball or a critterbot.
  *
  * @author Marc G. Bellemare, Mike Sokolsky
  */

import java.util.LinkedList;

import java.awt.Graphics;
import java.awt.Graphics2D;

public class SimulatorObject
{
  /** Some physical properties of the object - position, velocity, mass, moment of intertia */
  // @@@ Move all of these but position into a Physics 'state'
  protected Vector2D aPos;
  protected double aDir;
  protected double mass;
  protected double momI;

  protected Polygon aShape;

  protected LinkedList<ObjectState> aStates;

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
    aPos = new Vector2D(0,0);
    aDir = 0;
    mass = 0;
    momI = 0;

    aStates = new LinkedList<ObjectState>();
  }

  /** Sets the shape of the object (which is shapeless by default).
    *
    * @param pShape The polygon representing the shape. It is not copied over.
    */
  public void setShape(Polygon pShape)
  {
    aShape = pShape;
  }

  public Polygon getShape()
  {
    return aShape;
  }

  /** Returns the partial state corresponding to a particular component,
    * identified by pLabel.
    *
    * @param pLabel The identifier used by the SimulatorComponent and 
    *   the corresponding ObjectState.
    * @return The corresponding ObjectState or null if this object does not
    *   store this state information.
    *
    */
  public ObjectState getState(String pLabel)
  {
    for (ObjectState s : aStates)
    {
      if (s.getName().equals(pLabel))
        return s;
    }

    return null;
  }

  public void addState(ObjectState pState)
  {
    // @@@ check for duplicates?
    aStates.add(pState);
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
  public void draw(Graphics g)
  {
	  aShape.draw(g);  
  }
  
  public int getId()
  {
    return aId;
  }

  public void setPosition(Vector2D newPos) {
    // If we have a shape, also translate it by the difference
    if (aShape != null)
      aShape.translate(newPos.minus(aPos));

	  aPos = (Vector2D) newPos.clone();
  }

  void setMoment(double newMoment) {
	  momI = newMoment;
  }
	
  void setMass(double newMass) {
	  mass = newMass;
  }
	
  Vector2D getPosition() {
	  return aPos;
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
  protected void copyFrom(SimulatorObject org)
  {
    this.aPos = (Vector2D) org.aPos.clone();
    this.aDir = org.aDir;
    this.mass = org.mass;
    this.momI = org.momI;

    // To avoid copying too much stuff around, let's assume objects don't
    //  change shapes from state to state
    this.aShape = org.aShape;

    /* @@@ Copy the other attributes of the object, the actuator and sensor 
     * list */
    for (ObjectState os : org.aStates)
      this.addState((ObjectState)os.clone());
  }
}
