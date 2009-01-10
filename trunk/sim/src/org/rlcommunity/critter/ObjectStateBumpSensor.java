package org.rlcommunity.critter;

/**
  * ObjectStateBumpSensor 
  *
  * State component for bump sensors. The way this is implemented is by
  *  providing an object with this property (ObjectStateBumpSensor). The 
  *  whole polygon then becomes bump-sensitive.

  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;
import java.awt.Color;

import java.util.LinkedList;
import java.util.List;

public class ObjectStateBumpSensor implements ObjectState
{
  public static final String NAME = SimulatorComponentBump.NAME+"sensor";

  /** A list of forces being sensed by this bump sensor. The forces are
    *  normal to the underlying Polygon. */ 
  protected LinkedList<Force> aForces = new LinkedList<Force>();

  public ObjectStateBumpSensor()
  {
  }

  /** Clears the list of sensed forces */
  public void clearForces() { aForces.clear(); }

  /** Adds a force to the list of forces being sensed by the object
    *
    * @param f The new sensed force, assumed to be normal to the underlying
    *   Polygon.
    */
  public void addForce(Force f) { aForces.add(f); }

  /** Returns the list of forces being sensed
    *
    * @return A list of forces, normal to the polygon, being applied.
    *   Magnitude matters.
    */
  public List<Force> getForces() { return aForces; }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateBumpSensor newSensor = new ObjectStateBumpSensor();
    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateBumpSensor sensor = (ObjectStateBumpSensor)os;
 
    // Assume that whatever forces were sensed at the last state do not
    //  carry over (let the BumpSensor component take care of this)
  }


  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(Graphics g, SimulatorObject parent)
  {
    // Draw a little circle where each bobo is
    // @todo Evil! It wasn't synchronized and causing errors.
	  /*Color tempC = g.getColor();
	  g.setColor(Color.red);

    for (Force f : aForces)
    {
      int rad = (int)(f.vec.length() * 2);
	    g.drawOval((int)f.source.x, (int)f.source.y, rad, rad); 
    }
	  g.setColor(tempC); */
  }
  
  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient()
  {
    clearForces();
  }
}


