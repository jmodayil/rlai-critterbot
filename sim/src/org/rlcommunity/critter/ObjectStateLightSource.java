package org.rlcommunity.critter;

/**
  * ObjectStateLightSource
  *
  * State component for a light-emitting object
  */

import java.awt.Color;
import java.awt.Graphics;

public class ObjectStateLightSource implements ObjectState
{
  public static final String NAME = SimulatorComponentLight.NAME + "source";

  // A grayscale light value (@todo this needs to be changed) 
  protected int aIntensity; //in Candela

  public ObjectStateLightSource()
  {
    aIntensity = 0;
  }

  public int getIntensity() { return aIntensity; }
  public void setIntensity(int pInt) { aIntensity = pInt; }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateLightSource newSource = new ObjectStateLightSource();
    newSource.copyFrom(this);

    return newSource;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateLightSource src = (ObjectStateLightSource)os;
  
    this.aIntensity = src.aIntensity;
  }



  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(SimulatorGraphics g, SimulatorObject parent)
  {
//        Vector2D pos = parent.getPosition();
//        g.setColor(Color.BLACK);
//        g.drawString("Light",pos.x, pos.y);  
      
  }

  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient()
  {
  }
}
