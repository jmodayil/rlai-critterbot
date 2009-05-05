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

  /** Create a new force object as a clone of the given force.
    *
    * @param f The force to be cloned.
    */
  public Force (Force f)
  {
    this(new Vector2D(f.vec),f.source == null? null : new Vector2D(f.source));
  }

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

  public Vector2D getUnit() {
    Vector2D v = new Vector2D(vec);
    v.normalize();
    return v;
  }

    double magnitude() {
        return vec.length();
    }

}
