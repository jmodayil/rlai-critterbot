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

public interface ObjectState
{
  /** Returns a unique identifier for this type of state. */
  public String getName();

  public Object clone();
}
