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

  protected double aData;
  protected double aSensorWidth;
  protected int aNumPixels;
  protected double aSensorDepth; 

  public ObjectStateLightSensor()
  {
    aData = 0;
    aSensorWidth = 0;
    aNumPixels = 0;
    aSensorDepth = 0;
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
    this.aSensorWidth = sensor.aSensorWidth;
    this.aNumPixels = sensor.aNumPixels;
    this.aSensorDepth = sensor.aSensorDepth;
    
  }
  
  protected void setLightSensorValue (double intensity)
  {
      aData = intensity;
  }
  
  protected double getLightSensorValue ()
  {
   return aData;   
      
  }
  protected void setSensorWidth (double width)
  {
      aSensorWidth = width;
  }
  
  protected double getSensorWidth ()
  {
   return aSensorWidth;   
      
  }
    protected void setNumPixels (int pixels)
  {
      aNumPixels = pixels;
      assert(pixels%2.0 != 0);
  }
  
  protected int getNumPixels ()
  {
   return aNumPixels;   
      
  }
  protected void setSensorDepth (double depth)
  {
      aSensorDepth = depth;
  }
  
  protected double getSensorDepth ()
  {
   return aSensorDepth;   
      
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
    aSensorWidth = 0;
    aNumPixels = 0;
    aSensorDepth = 0;
  }
}

