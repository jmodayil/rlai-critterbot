/* Copyright 2009 Marc G. Bellemare
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

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
    * @param pAngularVel The new angular velocity measured by the gyroscope
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
  public void draw(SimulatorGraphics g, SimulatorObject parent)
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


