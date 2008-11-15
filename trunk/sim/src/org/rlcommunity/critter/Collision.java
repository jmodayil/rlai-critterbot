package org.rlcommunity.critter;

/**
  * Collision
  *
  * This class encapsulates collision information, in particular where
  *  the two colliding objects meet in space.
  *
  * @author Marc G. Bellemare
  */

public class Collision
{
  /** The normal to the collision, pointing away from the first object */
  public Vector2D normal;

  /** The point of collision */
  public Vector2D point; 

  /** The point of collision, in polygon-coordinates, for each polygon
    *   involved */
  public double alpha, beta; 
}
