package org.rlcommunity.critter;

/**
  * ObjectStateIRDistanceSensor
  *
  * State component for an infrared distance sensor
  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;

public class ObjectStateIRDistanceSensor implements ObjectState
{
  public static final String NAME = SimulatorComponentIRDistance.NAME;

  protected double aData;
  protected double aRange;

  public ObjectStateIRDistanceSensor(double pRange)
  {
    aData = 0;
    aRange = pRange;
  }

  /** Sets the current sensor reading for this sensor
    *
    * @param pValue The ir distance sensor reading
    */
  public void setSensorValue(double pValue) { aData = pValue; }
 
  /** Returns the current sensor reading
    * @return The current sensor reading
    */
  public double getSensorValue() { return aData; }

  /** Sets the maximum range of this distance sensor
    * @param pRange The new maximum range (in meters) of the sensor
    */
  public void setRange(double pRange) { aRange = pRange; }

  /** Returns the maximum range of this distance sensor
    * @return The maximum range of the sensor, in meters
    */
  public double getRange() { return aRange; }



  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateIRDistanceSensor newSensor = 
      new ObjectStateIRDistanceSensor(aRange);

    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateIRDistanceSensor sensor = (ObjectStateIRDistanceSensor)os;
  
    // Redundant code, but here for sanity purposes 
    this.aRange = sensor.aRange;
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
  public void clear()
  {
    aData = 0;
  }
}

