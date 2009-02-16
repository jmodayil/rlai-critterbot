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
  * RayIntersection
  *
  * This class encapsulates intersection information between a Ray and
  *  a Polygon, most likely generated by a Scene object.
  *
  * @author Marc G. Bellemare
  */

public class RayIntersection
{
  public Polygon polygon;
  public Ray ray;
 
  /** The normal to the intersection on the polygon */ 
  public Vector2D normal;

  /** The point of intersection, in global coordinates */
  public Vector2D point; 

  /** The point of intersection, in Polygon coordinates */ 
  public double polyAlpha;

  /** The point of intersection, in ray coordinates (r = alpha * v + p) */ 
  public double rayAlpha;
}
