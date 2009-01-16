package org.rlcommunity.critter;

/**
  * ObjectStateAccelerometer 
  *
  * State component for the accelerometer. Currently it stores acceleration
  *   as a Vector2D and a double, this should be refactored asap into a
  *   Vector3D, even if the said Vector3D contains no methods.
  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;
import java.awt.Color;

import java.util.LinkedList;
import java.util.List;

public class ObjectStateAccelerometer implements ObjectState
{
  public static final String NAME = SimulatorComponentAccelerometer.NAME; 

  /** The acceleration being sensed by this accelerometer, in XY space,
    *  in m/s^2 */
  protected Vector2D aAccel;

  /** The previous timestep's velocity */
  protected Vector2D aVelSample;

  /** The Z acceleration sensed by this accelerometer. In 2D, this is
    *  always gravity */
  protected double aZAccel = ObjectStateDynamics.GRAVITY; 

  public ObjectStateAccelerometer()
  {
    clearTransient();
  }

  /** Returns the current acceleration data
    *
    * @return The current acceleration data of this sensor
    */
  public Vector2D getSensorValue()
  {
    return aAccel;
  }

  /** Sets the new acceleration data
    *
    * @return The new acceleration data
    */
  public void setSensorValue(Vector2D pAccel)
  {
    aAccel = new Vector2D(pAccel); 
  }

  public double getZSensorValue()
  {
    return aZAccel;
  }

  /** Returns the current velocity sample
    *
    * @return The current velocity sample
    */
  public Vector2D getVelocitySample()
  {
    return aVelSample;
  }

  /** Set the current velocity sample used to determine acceleration
    *
    * @param pVel The new velocity sample
    */
  public void setVelocitySample(Vector2D pVel)
  {
    aVelSample = new Vector2D(pVel); 
  }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateAccelerometer newSensor = new ObjectStateAccelerometer();
    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateAccelerometer sensor = (ObjectStateAccelerometer)os;

    this.aAccel = new Vector2D(sensor.aAccel);
    this.aZAccel = sensor.aZAccel;
    this.aVelSample = new Vector2D(sensor.aVelSample);
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
    // Assume the default acceleration vector is 0,0
    aAccel = new Vector2D(0.0, 0.0); 
    aVelSample = new Vector2D(0.0, 0.0);
  }
}


