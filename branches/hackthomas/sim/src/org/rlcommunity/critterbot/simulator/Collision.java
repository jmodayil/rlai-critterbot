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
  * Collision
  *
  * This class encapsulates collision information, in particular where
  *  the two colliding objects meet in space.
  *
  * @author Marc G. Bellemare
  */

public class Collision
{
    public Collision(Collision pCol) {
        this.normal = new Vector2D(pCol.normal);
        this.point = new Vector2D(pCol.point);
        this.magnitude = pCol.magnitude;
        this.alpha = pCol.alpha;
        this.beta = pCol.beta;
    }

    public Collision() {
    }
    
    /** The normal to the collision, pointing away from the first object */
    public Vector2D normal;
    /** The point of collision */
    public Vector2D point;
    /** The magnitude of the force generated by this collision */
    public double magnitude;
    /** The point of collision, in polygon-coordinates, for each polygon
     *   involved */
    public double alpha,  beta;

    /** Creates the reverse of this collision (so that the Collision information
     *    is shown from the other object's "viewpoint"). In particular,
     *    alpha becomes beta and vice-versa, and the normal's direction is
     *    reversed.
     *
     * @param pCol The original Collision information.
     * @return The reversed collision information.
     */
    public static Collision reverse(Collision pCol) {
      Collision revCol = new Collision();
      revCol.point = pCol.point;
      revCol.magnitude = pCol.magnitude;
      revCol.alpha = pCol.beta;
      revCol.beta = pCol.alpha;
      revCol.normal = pCol.normal.reverse();

      return revCol;
    }
}
