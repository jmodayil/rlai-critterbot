package org.rlcommunity.critter;

/**
  * ObjectStateGyroscope 
  *
  * State component for the gyroscope.
  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;
import java.awt.Color;

import java.util.LinkedList;
import java.util.List;

public class ObjectStateGyroscope implements ObjectState
{
  public static final String NAME = SimulatorComponentGyroscope.NAME; 

  /** The angular velocity measured by the gyroscope */
  protected double aVelocity; 

  public ObjectStateGyroscope()
  {
    clearTransient();
  }

  /** Returns the current angular velocity measured by the gyroscope
    *
    * @return The current measured angular velocity 
    */
  public double getSensorValue()
  {
    return aVelocity;
  }

  /** Sets the new angular velocity measurement 
    *
    * @return The new angular velocity measured by the gyroscope 
    */
  public void setSensorValue(double pAngularVel)
  {
    aVelocity = pAngularVel;
  }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateGyroscope newSensor = new ObjectStateGyroscope();
    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateGyroscope sensor = (ObjectStateGyroscope)os;

    this.aVelocity = sensor.aVelocity;
  }


  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(Graphics g, SimulatorObject parent)
  {
  }
  
  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient()
  {
    aVelocity = 0;
  }
}


