package org.rlcommunity.critter;

/**
  * Scene
  *
  * Defines a class for doing ray tracing and point visibility. It encapsulates
  *  a set of Polygons.
  *
  * Note to whomever may want to extend this class - do NOT make it keep
  *  track of a set of SimulatorObjects or return a set of SimulatorObjects
  *  without consulting MGB.
  *
  * @author Marc G. Bellemare
  */

import java.util.Set;
import java.util.HashSet;
import java.util.List;

public class Scene
{
  /** The set of objects contained in this Scene */
  protected HashSet<Polygon> aPolySet = new HashSet<Polygon>();

  /** Creates a new Scene containing all objects (their polygons) in the 
    *   given state.
    * Maybe this method should be somewhere else, e.g. in a SceneFactory?
    *
    * @param pState The state of interest
    */
  public Scene(SimulatorState pState)
  {
    // Get all the objects in the world
    List<SimulatorObject> objects = pState.getObjects();

    for (SimulatorObject o : objects)
    {
      // If this object has a shape, add it
      Polygon poly = o.getShape();
      if (poly != null)
        aPolySet.add(poly);
    }
  }


  /**
    * Traces the given ray and finds the first intersection (if any) with
    *  this scene's Polygon set. 
    *
    * @param pRay The ray to be traced
    *
    * @return The intersection
    */
  public RayIntersection traceRay(Ray pRay)
  {
    double minAlpha = Double.POSITIVE_INFINITY;
    RayIntersection minIsect = null;

    for (Polygon poly : aPolySet)
    {
      RayIntersection isect = poly.intersect(pRay);
      // Do we actually have an intersection?
      if (isect != null) 
      {
        if (isect.rayAlpha < minAlpha)
        {
          minIsect = isect;
          minAlpha = isect.rayAlpha;
        }
      }
    }

    if (minIsect == null) return null;
    else return minIsect;
  }

  /**
    * Returns p1 is visible from p2. This relation should be symmetric, but
    *  in practice might not.
    *
    * @param p1 The first point in the comparison
    * @param p2 The second point in the comparison
    */
  public boolean isVisible(Vector2D p1, Vector2D p2)
  {
    // Avoid the ugly case when p1 == p2 and the 'vec' vector would be 0
    if (p1.equals(p2)) return true;
    
    // Create a ray from p1 to p2 and trace it
    Vector2D vec = p2.minus(p1);
    vec.normalize();

    Ray r = new Ray(p1, vec);
    // Find the first intersection of the ray with the polygon set
    RayIntersection isect = traceRay(r);

    // In the lucky case where we simply don't intersect anything, we're done!
    if (isect == null) return true;

    // Now, p2 is visible from p1 provided the intersect occurs after p2
    double iAlpha, p2Alpha;
    iAlpha = isect.rayAlpha;
    
    if (vec.x != 0)
      p2Alpha = (p2.x - p1.x) / vec.x;
    else
      p2Alpha = (p2.y - p1.y) / vec.y;
    
    // By how we constructed the ray, we know p2Alpha > 0; the ray tracer
    //  finds intersections s.t. iAlpha > 0 so we know p2 is visible if 
    //  the first intersection occurs after
    return (p2Alpha < iAlpha);
  }

  /** Adds an object's polygon to this scene.
    *
    * @param pObject The object whose shape should be added
    */
  public void addObject(SimulatorObject pObject)
  {
    Polygon poly = pObject.getShape();
    
    if (poly != null)
      aPolySet.add(poly);
  }

  /** Removes an object's polygon (but not its children's!) from this scene. 
    *
    * @param pObject the object whose shape should be removed
    */
  public void removeObject(SimulatorObject pObject)
  {
    Polygon poly = pObject.getShape();
    
    if (poly != null)
      aPolySet.remove(poly);
  }
  
  /** Adds an object's whole subtree (itself and children) to this scene. 
    *
    * @param pObject the object whose subtree's polygons should be added. 
    */
  public void addSubtree(SimulatorObject pObject)
  {
    List<SimulatorObject> children = pObject.getChildren();
    Polygon poly = pObject.getShape();
    
    // Add the root's polygon
    if (poly != null)
      aPolySet.add(poly);

    // Add the children's polygons 
    for (SimulatorObject o : children)
    {
      poly = o.getShape();
      if (poly != null)
        aPolySet.add(poly);
    }
  }

  /** Removes an object's whole subtree (itself and children) from this scene. 
    *
    * @param pObject the object whose subtree's polygons should be removed. 
    */
  public void removeSubtree(SimulatorObject pObject)
  {
    List<SimulatorObject> children = pObject.getChildren();
    Polygon poly = pObject.getShape();
    
    if (poly != null)
      aPolySet.remove(poly);

    // Remove the children's polygons
    for (SimulatorObject o : children)
    {
      poly = o.getShape();
      if (poly != null)
        aPolySet.remove(poly);
    }
  }
}
