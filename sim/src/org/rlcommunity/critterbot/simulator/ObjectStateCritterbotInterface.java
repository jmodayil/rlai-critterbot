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

package org.rlcommunity.critterbot.simulator;

/**
  * ObjectStateCritterbotInterface
  *
  * State component for a robot that takes commands from a CritterControlDrop
  *  and can be polled for data to be put in a CritterStateDrop
  */

import java.awt.Graphics;

public class ObjectStateCritterbotInterface implements ObjectState
{
  public static final String NAME = SimulatorComponentCritterbotInterface.NAME; 

  /** How often (in milliseconds) should we send a state drop */
  protected int aStateDropFrequency;
  protected int aLastStateUpdate;

  public ObjectStateCritterbotInterface()
  {
    aStateDropFrequency = 50;
    aLastStateUpdate = 0;
  }

  /** Sets how frequently, in milliseconds, we should send a state drop
    *
    * @param pTime State drop frequency, in milliseconds
    */
  public void setStateDropFrequency(int pTime) { aStateDropFrequency = pTime; }

  /** Returns how frequently we sent state drops
    *
    * @return The state drop frequency in milliseconds
    */
  public int getStateDropFrequency() { return aStateDropFrequency; }

  /** Sets the amount of time elapsed since the last time a state drop was
    *  emitted
    *
    * @param pTime The amount of time, in milliseconds, since the last
    *  state drop
    */
  public void setLastStateUpdate(int pTime) { aLastStateUpdate = pTime; }

  /** Returns the amount of time elapsed since the last state drop was emitted
    *
    * @return The amount of time in milliseconds since the last state drop
    */
  public int getLastStateUpdate() { return aLastStateUpdate; }

  /** Returns whether a new state drop should be sent. Note that the interface
    *  may somehow be behind (e.g. more than one state drop should be sent at this
    *  time). This method does not provide the means of testing for this,
    *  in order to avoid updating the data structure when needsStateUpdate is
    *  called.
    *
    * @return Whether a new state drop should be sent. Returns true until
    *   setLastStateUpdate is called with a time less than the frequency.
    */
  public boolean needsStateUpdate() 
  {
    if (aLastStateUpdate >= aStateDropFrequency)
      return true;
    else
      return false;
  }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateCritterbotInterface newIface = new 
      ObjectStateCritterbotInterface();

    newIface.copyFrom(this);

    return newIface;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateCritterbotInterface src = (ObjectStateCritterbotInterface)os;

    this.aStateDropFrequency = src.aStateDropFrequency;
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
  }
}
