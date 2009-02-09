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

  protected int aData; //in candela
  protected double aSensorWidth; //in Meters
  protected int aNumPixels;     
  protected double aSensorDepth; //in Meters 

  public ObjectStateLightSensor()
  {
      this(0,0.0,0.0);
  }

    public ObjectStateLightSensor(int pNumPixels, double pSensorDepth, 
            double pSensorWidth) {
    aData = 0;
    aSensorWidth = pSensorWidth;
    aNumPixels = pNumPixels;
    aSensorDepth = pSensorDepth;
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
  
  protected void setLightSensorValue (int intensity)
  {
      aData = intensity;
  }
  
  protected int getLightSensorValue ()
  {
   return aData;   
      
  }
  
  protected double getSensorWidth ()
  {
   return aSensorWidth;   
      
  }
  
  protected int getNumPixels ()
  {
   return aNumPixels;   
      
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
  public void draw(SimulatorGraphics g, SimulatorObject parent)
  {
  }

  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient()
  {
    aData = 0;
  }
}

