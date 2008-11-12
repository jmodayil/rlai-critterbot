package org.rlcommunity.critter;

/**
  *  Force
  *
  * A simple class encapsulating what represents a force.
  *
  * @author Marc G. Bellemare
  */

public class Force
{
  /** The force vector */
  public Vector2D vec;
  /** The point of contact (possibly null, if a motor force) where this
    *  force was transferred. */
  public Vector2D source;

  /** Create a new force object with no source.
    *
    * @param x The x copmonent of the force
    * @param y The y component of the force
    */
  public Force (double x, double y)
  {
    this(new Vector2D(x,y),null);
  }

  /** Create a new force object with no source.
    *
    * @param v The vector representing the force
    */
  public Force (Vector2D v)
  {
    this(v,null);
  }
 
  /** Create a new force object. The source argument is used to construct
    *  bump sensor information and may potentially have other uses (such
    *  as generating torque based on a vector force). Note that the vectors
    *  are NOT cloned.
    *
    * @param v The vector representing the force
    * @param src The point of contact where the force was transferred
    */
  public Force (Vector2D v, Vector2D src) 
  {
    vec = v;
    source = src;
  }

}
