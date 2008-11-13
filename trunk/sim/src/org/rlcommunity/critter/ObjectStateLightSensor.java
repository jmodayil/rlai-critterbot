package org.rlcommunity.critter;

/**
  * ObjectStateLightSensor
  *
  * State component for an object which senses light.
  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;

public class ObjectStateLightSensor implements ObjectState
{
  public static final String NAME = SimulatorComponentLight.NAME + "sensor";

  public double aData;

  public ObjectStateLightSensor()
  {
    aData = 0;
  }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateLightSensor newSensor = new ObjectStateLightSensor();
    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateLightSensor sensor = (ObjectStateLightSensor)os;
  
    this.aData = sensor.aData;
  }
  
  protected void setLightSensorValue (double intensity)
  {
      aData = intensity;
  }
  
  protected double getLightSensorValue ()
  {
   return aData;   
      
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
}

