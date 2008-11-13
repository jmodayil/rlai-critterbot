package org.rlcommunity.critter;

/**
  * Scene
  *
  * Defines static methods for dealing with Polygons, Rays and such:
  *  computing the nearest intersection, for example, or whether a point
  *  'sees' another.
  *
  * @author Marc G. Bellemare
  */

import java.awt.geom.Line2D;
import java.util.Set;


public class Scene
{
  /**
    * Traces the given ray and finds the first intersection (if any) with
    *  the given set of polygons.
    *
    * @param pRay The ray to be traced
    * @param pPolySet The set of polygons of interest
    *
    * @return The point of intersection (we probably want to change this)
    */
  public static Vector2D traceRay(Ray pRay, Set<Polygon> pPolySet)
  {
    double minAlpha = Double.POSITIVE_INFINITY;
    
    for (Polygon poly : pPolySet)
    {
      double alpha = poly.intersect(pRay);
      // Do we actually have an intersection?
      if (alpha > 0)
      {
        if (alpha < minAlpha) minAlpha = alpha;
      }
    }

    return pRay.getPoint(minAlpha);
  }

  /**
    * Returns p1 is visible from p2. This relation should be symmetric, but
    *  in practice might not.
    *
    * @param p1 The first point in the comparison
    * @param p2 The second point in the comparison
    */
  public static boolean isVisible(Vector2D p1, Vector2D p2, 
    Set<Polygon> pPolySet)
  {
    // Avoid the ugly case when p1 == p2 and the 'vec' vector would be 0
    if (p1.equals(p2)) return true;
    
    // Create a ray from p1 to p2 and trace it
    Vector2D vec = p2.minus(p1);
    vec.normalize();

    Ray r = new Ray(p1, vec);
    // Find the first intersection of the ray with the polygon set
    Vector2D isect = traceRay(r, pPolySet);

    // In the lucky case where we simply don't intersect anything, we're done!
    if (isect == null) return true;

    // Now, p2 is visible from p1 provided the intersect occurs after p2
    double iAlpha, p2Alpha;
    if (vec.x != 0)
    {
      iAlpha = (isect.x - p1.x) / vec.x;
      p2Alpha = (p2.x - p1.x) / vec.x;
    }
    else
    {
      iAlpha = (isect.y - p1.y) / vec.y;
      p2Alpha = (p2.y - p1.y) / vec.y;
    }
    
    System.out.println(p2Alpha +" "+iAlpha);

    // By how we constructed the ray, we know p2Alpha > 0; the ray tracer
    //  finds intersections s.t. iAlpha > 0 so we know p2 is visible if 
    //  the first intersection occurs after
    return (p2Alpha < iAlpha);
  }
}
