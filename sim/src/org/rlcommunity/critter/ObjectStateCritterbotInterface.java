package org.rlcommunity.critter;

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
    aStateDropFrequency = 500;
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

  /** Returns whether a new state drop should be sent. When the function 
    *  returns true, the timer (aLastStateUpdate) is also reset to 
    *  the leftover time (e.g. the time elapsed since the last update, minus
    *  the update frequency).
    *
    * In the event that needsStateUpdate() has not been called for more than
    *  one update period, aLastStateUpdate may still be greater than 
    *  aStateUpdateFrequency, such that calling needsStateUpdate() immediately
    *  after will again return true.
    *
    * @return Whether a new state drop should be sent
    */
  public boolean needsStateUpdate() 
  {
    // @@@ this scheme breaks the convention that the current state is 
    //  not modified
    if (aLastStateUpdate >= aStateDropFrequency)
    {
      aLastStateUpdate -= aStateDropFrequency; 
      return true;
    }
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
  public void draw(Graphics g, SimulatorObject parent)
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
