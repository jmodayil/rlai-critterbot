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
  * Ray
  *
  * Defines a single ray as a point (source) and a normalized vector 
  * (direction).
  */

public class Ray
{
  public Vector2D src;
  public Vector2D dir;

  /** Creates a new ray composed of the given source and direction vector.
    *  This constructor does not make copies of the vectors given to it.
    *
    * @param _src The source of the ray
    * @param _dir The direction of the ray (preferably normalized)
    */
  public Ray(Vector2D _src, Vector2D _dir)
  {
    src = _src; 
    dir = _dir; 
  }

  /** Returns the point on the ray defined as src + alpha * dir. Negative
    *  alpha values are allowed.
    *
    * @param alpha The parametrized distance of the required point from the
    *   source
    * @return A point on the ray
    */
  public Vector2D getPoint(double alpha)
  {
    return new Vector2D(src.x + alpha * dir.x, src.y + alpha * dir.y);
  }

}
