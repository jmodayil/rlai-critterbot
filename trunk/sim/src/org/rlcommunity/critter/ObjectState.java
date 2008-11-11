package org.rlcommunity.critter;

/**
  * ObjectState
  *
  * Defines a generic object state (in general tied to a particular component).
  *  This allows the actual state of the object to be factorized into 
  *  the relevant parts only, and also allows new state information to be
  *  added without having to restructure everything or even touch the existing
  *  parts of the object state.
  *
  *  @author Marc G. Bellemare
  */

import java.awt.Graphics;

public interface ObjectState
{
  /** Returns a unique identifier for this type of state. */
  public String getName();

  public Object clone();


  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(Graphics g, SimulatorObject parent);
}
