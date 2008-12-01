package org.rlcommunity.critter;

/**
  * Polygon
  *
  * This class is meant as a replacement for java.awt.Polygon, which does
  *  most of the job but not quite. The polygon is represented as a list
  *  of successive points, with the n^th point being connected to the first
  *  point.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

import java.awt.Color;
import java.awt.Graphics;

public class Polygon
{
  /* Used for approximate methods; epsilon is the tolerance at which two
   *   coordinates are considered the same.
   */
  public static final double EPSILON = 10E-6;

  /** Class encapsulating intersection information for efficient polygon
    *  processing. Alpha and beta represent the same point in two different
    *  polygons, in polygon-coordinates.
    */
  public class Intersection
  {
    public double alpha;
    public double beta;

    public Intersection(double a, double b)
    {
      alpha = a;
      beta = b;
    }
  }

  /** The polygon's bounding box */
  protected double bx,by,bw,bh;

  protected LinkedList<Vector2D> points;

  protected int[] aXPoints, aYPoints;

  public Polygon()
  {
    init();
  }

  public void init()
  {
    points = new LinkedList<Vector2D>();
    bx = by = Double.POSITIVE_INFINITY;
    bw = bh = 0;
  }

  public int size()
  {
    return points.size();
  }

  public LinkedList<Vector2D> getPoints()
  {
    return points; 
  }

  /** Add a point to this polygon.
    */
  public void addPoint(double x, double y)
  {
    Vector2D pt = new Vector2D(x,y);
    // Add the new point to our list and adjust the bounding box if necessary
    points.add(pt);
    adjustBoundingBox(pt, points.size() == 1);

    // Not quite efficient
    aXPoints = aYPoints = null;
  }

  /** 
    * This method should be called after all points have been added to the
    *  polygon.
    */
  public void doneAddPoints()
  {
    // If less than three points, things are bad
    if (points.size() < 3) return;

    // Find the clockwiseness of this Polygon
    Vector2D p1 = points.getLast();

    boolean isClockwise = false;
    boolean resolved = false;

    for (Vector2D p2 : points)
    {
      // Find out the handedness of the p2-p1 edge 
      Vector2D center = 
        new Vector2D(0.5 * p1.x + 0.5 * p2.x,
                     0.5 * p1.y + 0.5 * p2.y);
      Vector2D normal = p2.minus(p1).rotate(Math.PI / 2);
      normal.normalize();

      // Construct a point epsilon away from the edge along the normal,
      //  outside of the polygon
      Vector2D outPoint = normal.times(EPSILON).plus(center);
      // Also cconstruct a point epsilon inside of the polygon
      Vector2D inPoint = normal.times(-EPSILON).plus(center);

      boolean containsInPoint = this.contains(inPoint);
      boolean containsOutPoint = this.contains(outPoint);

      // There are three cases, one of which (when neither point is contained)
      //  is inconclusive
      
      // Case 1: The polygon is clockwise ordered
      if (containsInPoint && !containsOutPoint)
      {
        isClockwise = true;
        resolved = true;
        break;
      }
      // Case 2: the polygon is anticlockwise ordered
      else if (!containsInPoint && containsOutPoint)
      {
        isClockwise = false;
        resolved = true;
        break;
      }
      // Case 3: we picked an edge that is either surrounded by only inside,
      //  or outside the polygon (unpleasant but possible cases)
    }

    if (!resolved) // We still don't know whether this polygon is clockwise!
      // @@@ fixme; not the right type of exception
      throw new IllegalArgumentException("Really ugly polygon");

    // If the polygon is not clockwise, re-add the points backwards
    if (!isClockwise)
    {
      System.err.println ("Not clockwise, recreating polygon");
      LinkedList<Vector2D> oldPoints = points;

      // Re-initialize the polygon, as if we had no points
      init();
      ListIterator<Vector2D> it = oldPoints.listIterator();

      // Get the last element
      while (it.hasNext())
        it.next();
      
      // Add the points in reverse order
      while (it.hasPrevious())
      {
        Vector2D pt = it.previous();
        addPoint(pt.x, pt.y);
      }
    }
  }

  /** Translate this polygon with the given (dx,dy) vector
    * 
    * @param delta The vector to translate by
    */
  public void translate(Vector2D delta)
  {
    if (delta.x == 0 && delta.y == 0)
      return;

    // Adjust the bounding box
    bx += delta.x;
    by += delta.y;

    // adjust all points :( we could also optimize things by simply keeping
    //  track of the translation and adjusting only when necessary (e.g.
    //  when we need the points, rather than the bounding box)
    int idx = 0;
    for (Vector2D p : points)
    {
      p.x += delta.x;
      p.y += delta.y;

      if (aXPoints != null)
      {
        aXPoints[idx] = (int)Math.round(p.x);
        aYPoints[idx] = (int)Math.round(p.y);
        idx++;
      }
    }

    if (aXPoints != null && points.size() > 0)
    {
      Vector2D first = points.getFirst();
      aXPoints[idx] = (int)Math.round(first.x);
      aYPoints[idx] = (int)Math.round(first.y);
    }
  }

  /** Adjusts the bounding box to include a new point.
    *
    * @param pt The point to be included
    * @param firstPt Whether this is the first point used to create
    *   the bounding box
    */
  public void adjustBoundingBox(Vector2D pt, boolean firstPt)
  {
    if (pt.x < bx)
    {
      if (!firstPt)
        bw += bx - pt.x;
      bx = pt.x;
    }
    else if (pt.x - bx > bw) bw = pt.x - bx;

    if (pt.y < by)
    {
      if (!firstPt)
        bh += by - pt.y;
      by = pt.y; 
    }
    else if (pt.y - by > bh) bh = pt.y - by;
  }

  /** Rotates this polygon (counter-clockwise) by the given angle around
    *  the given center.
    * 
    * @param theta The angle to rotate by
    * @param center The center of rotation
    */
  public void rotate(double theta, Vector2D center)
  {
    if (theta == 0) return;

    double cost = Math.cos(theta);
    double sint = Math.sin(theta);

    double x, y;

    // Adjust the bounding box by finding the new maxes
    bx = by = Double.POSITIVE_INFINITY;
    bw = bh = 0;

    // Rotate all points
    int idx = 0;
    boolean firstPt = true;
    for (Vector2D p : points)
    {
      x = p.x - center.x;
      y = p.y - center.y;

      p.x = center.x + (x * cost - y * sint);
      p.y = center.y + (y * cost + x * sint);

      adjustBoundingBox(p, firstPt);
      firstPt = false;
      
      if (aXPoints != null)
      {
        aXPoints[idx] = (int)Math.round(p.x);
        aYPoints[idx] = (int)Math.round(p.y);
        idx++;
      }
    }

    if (aXPoints != null && points.size() > 0)
    {
      Vector2D first = points.getFirst();
      aXPoints[idx] = (int)Math.round(first.x);
      aYPoints[idx] = (int)Math.round(first.y);
    }
  }
  /**
    * Returns whether the given point is inside this polygon. Polygons
    *  with less than three edges (points) are assumed to contain no points.
    *
    * @param p The point to be tested
    * @return Whether point p is inside this polygon
    */
  public boolean contains(Vector2D p)
  {
    int numPoints = points.size();

    if (numPoints < 3) return false; // No cheating

    // Use the even-odd rule
    int count = 0;
    Vector2D cur = points.getLast();

    // Walk the list of edges
    for (Vector2D next : points)
    {
      // If the edge from the previous point (starting with the last point
      //  in the list) to np crosses the half-line (p.x,p.y)-(inf,p.y), 
      //  increment the count
      if ((next.y > p.y && cur.y < p.y) || (next.y < p.y && cur.y > p.y))
      {
        // Make sure that the intersection falls on the x > 0 half-line
        double alpha = (next.y - p.y) / (next.y - cur.y);
        double ix = next.x - p.x + alpha * (cur.x - next.x);
        if (ix > 0)
          count++;
      }

      cur = next;
    }

    return (count % 2 == 1);
  }

  /** 
    * Returns a point, if any, at which this polygon intersects the given 
    *  polygon. The 'relationship' is, of course, symmetric. Note that no 
    *  efficiency guarantee is given.
    *
    *
    * @param poly The polygon to be tested against.
    * @return The (first) point of intersection of the two polygons, or 
    *         null if none exist.
    */

  public Intersection intersects(Polygon pPoly)
  {
    List<Intersection> isect = getIntersections(pPoly, 1);

    if (isect.isEmpty()) return null;
    else return isect.get(0);
  }

  /** Returns up to num intersections found with the given polygon as a
    *  list of pairs <alpha, beta> where alpha is the point of intersection,
    *  in polygon coordinates, for this polygon and beta is the same point
    *  in the other polygons' coordinate system. 
    *
    * @param pPoly The polygon to test for intersections
    * @param num   The maximum number of intersections to return. If num is
    *   0, all valid intersections are returned
    * @return A list of up to num Intersection objects found between the 
    *  two polygons. The list is never null, but may be empty.
    */
  public List<Intersection> getIntersections (Polygon pPoly, int num)
  {
    LinkedList<Intersection> isects = new LinkedList<Intersection>();
    if(pPoly == null)
	    return isects;

    // Compare the bounding boxes first, and hope that they don't intersect
    // @@@ this can be rewritten as we now only care about intersections 
    //  at the edges, NOT whether the two polygons occupy the same space
    double l = pPoly.bx;
    double r = pPoly.bx + pPoly.bw;
    double t = pPoly.by;
    double b = pPoly.by + pPoly.bh;
    
    double br = bx + bw;
    double bb = by + bh;

    // @@@ this can be simplified to - assume no intersection if bb's
    //  don't intersect
    if (! ((l < bx && bx < r) || (l < br && br < r) ||
           (t < by && by < b) || (t < bb && bb < b)) &&
        ! ((bx < l && l < br) || (bx < r && r < br) ||
           (by < t && t < bb) || (by < b && b < bb)) )
      return isects;

    boolean done = false;

    // We keep track of the edge numbering to properly generate polygon
    //  coordinates
    int edge1 = 0;

    // Use brute force if the bounding box test fails, for lack of 
    //  programming time (all-edges comparison)
    Vector2D pa1 = this.points.getLast();
    for (Vector2D pa2 : this.points)
    {
      int edge2 = 0;

      Vector2D pb1 = pPoly.points.getLast();
      for (Vector2D pb2 : pPoly.points)
      {
        // Test whether the line segment pa1-pa2 intersects pb1-pb2
        double denom = (pb2.y - pb1.y) * (pa2.x - pa1.x) -
                       (pb2.x - pb1.x) * (pa2.y - pa1.y);
        
        // Assume general case and go on with your life 
        if (denom == 0)
        {
          edge2++;
          pb1 = pb2;
          continue;
        }

        double num1 = (pb2.x - pb1.x) * (pa1.y - pb1.y) -
                       (pb2.y - pb1.y) * (pa1.x - pb1.x); 
        double num2 = (pa2.x - pa1.x) * (pa1.y - pb1.y) -
                       (pa2.y - pa1.y) * (pa1.x - pb1.x); 

        double alpha = num1 / denom;
        double beta = num2 / denom;

        // Are these two line segments intersecting?
        if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1)
        {
          isects.add (new Intersection(alpha + edge1, beta + edge2));
          if (num > 0)
          {
            num--;
            // If we've reached the max. number of intersections that we
            //  need, return what we've found
            if (num == 0)
              return isects;
          }
        }

        edge2++;
        pb1 = pb2;
      }

      edge1++;
      pa1 = pa2;
    }

    // Return a possibly empty or smaller-than-requested list
    return isects;
  }


  /**
    * Determine whether the given ray interesects this polygon. Only 
    *  positive intersections are considered, e.g. the ray intersects the
    *  polygon iff there exists an alpha > 0 s.t. ray.src + alpha * ray.dir
    *  is also a point on the polygon.
    *
    * @param r The ray of interest
    * @return A RayIntersection containing information, or null if there is
    *   no intersection.
    */
  public RayIntersection intersect(Ray r)
  {
    // For now, do an all edges comparison with the polygon
    //  I feel like there is a much easier way to do this...
    Vector2D r1 = r.src;
    Vector2D r2 = r.src.plus(r.dir);

    int edgeIdx = 0;

    Vector2D p1 = points.getLast();
    for (Vector2D p2 : points)
    {
      // Test whether the line segment p1-p2 interescts r.src-(r.src+r.dir)
      double denom = (r2.y - r1.y) * (p2.x - p1.x) -
                     (r2.x - r1.x) * (p2.y - p1.y);
      
      // Parallel lines
      if (denom == 0)
      {
        p1 = p2;
        continue;
      }

      double num1 = (r2.x - r1.x) * (p1.y - r1.y) -
                     (r2.y - r1.y) * (p1.x - r1.x); 
      double num2 = (p2.x - p1.x) * (p1.y - r1.y) -
                     (p2.y - p1.y) * (p1.x - r1.x); 

      double alpha = num1 / denom;
      double beta = num2 / denom;

      // We only require that the ray be directed towards the polygon and
      //  that the ray interesct the line segment
      if (alpha > 0 && alpha < 1 && beta > 0)
      {
        // Return some information of interest
        RayIntersection isect = new RayIntersection();
        isect.polygon = this;
        isect.ray = r;

        isect.polyAlpha = edgeIdx + alpha;
        isect.rayAlpha = beta;
        
        // Some extra information - maybe it's a bad idea to compute it
        //  by default?
        isect.normal = getNormal(isect.polyAlpha);//this.computeNormal(p1,p2);
        isect.point = r.getPoint(beta);

        return isect;
      }

      p1 = p2;
      edgeIdx++;
    }

    // No valid interesection, return null
    return null;
  }


  /** Returns a point on the polygon in cartesian coordinates based on its 
    *   polygon coordinates. The polygon coordinate is a single real number
    *   in [0, n). The integer part represents which edge (out of n) is
    *   being referenced, and the decimal part represents where on the
    *   edge the point lies. If the decimal part is c, the integer part
    *   i, and the i^th edge is represented by the points p1 and p2, then
    *   this method returns (1 - c) p1 + c p2.
    *
    * @param alpha The polygon coordinate of the point of interest
    * @return The requested point, in cartesian coordinates
    */
  public Vector2D getPoint (double alpha)
  {
    if (alpha < 0) return null;

    int edgeNum = (int)alpha;
    if (edgeNum >= points.size()) return null;
   
    double c = alpha - edgeNum;

    // Find the edge of interest
    Vector2D p1 = points.getLast();
    for (Vector2D p2 : points)
    {
      if (edgeNum == 0)
      {
        // Interpolate to find the point of interest
        return new Vector2D ((1.0 - c) * p1.x + c * p2.x, 
                             (1.0 - c) * p1.y + c * p2.y);
      }

      edgeNum--;
      p1 = p2;
    }

    // Oops? Something went wrong
    //  @@@ possibly throw an exception?
    return null;
  }

  /** Returns the normal to the polygon at a given point, in polygon
    *  coordinates.
    *
    * @param alpha The polygon coordinate of the point of interest
    * @return The normal to the polygon at the point of interest; this is
    *   the vector perpendicular to the edge on which the point is 
    */
  public Vector2D getNormal (double alpha)
  {
    if (alpha < 0) return null;

    int edgeNum = (int)alpha;
    if (edgeNum >= points.size()) return null;
   
    double c = alpha - edgeNum;

    // Find the edge that we want 
    Vector2D p1 = points.getLast();
    for (Vector2D p2 : points)
    {
      if (edgeNum == 0)
      {
        // Return the perpendicular to this edge
        return computeNormal(p1, p2);
      }

      p1 = p2;
      edgeNum--;
    }

    // Oops? Something went wrong
    //  @@@ possibly throw an exception?
    return null;
  }
   
  /** Computes the normal to a given edge p1-p2. The edge should be given in
    *  clockwise order.
    * 
    * @param p1 The first point of the edge
    * @param p2 The second point of the edge
    * @return The normal to the edge, pointing outwards
    */
  public Vector2D computeNormal(Vector2D p1, Vector2D p2)
  {
    Vector2D edge = p2.minus(p1);
    // If the Polygon points are in clockwise order, then the normal
    //  is the edge rotated by pi/2. Otherwise, it's given by a 
    //  rotation of -pi/2. 

    Vector2D normal = edge.rotate(Math.PI / 2);
    normal.normalize();

    return normal;
  }


  /** SimulatorObject interface **/

  /** Create a deep copy of this polygon
    *
    * @return A copy of this polygon, with the guarantee that the original
    *    and the copy do not share any data
    */
  public Object clone()
  {
    Polygon poly = new Polygon();

    poly.copyFrom(this);

    return poly;
  }

  /** Method used by clone() to copy data from the original polygon to the
    *  new polygon
    *
    * @param org The polygon to copy data from
    */
  protected void copyFrom(Polygon org)
  {
    // We will add each point from the original polygon in
    //  Because we are passing the (x,y) coordinates, we are assured 
    //  that we will not be reusing the original Polygon's points
    for (Vector2D pt : org.points)
      this.addPoint(pt.x, pt.y);
    this.doneAddPoints();
  }

  /** Draw the polygon on the provided canvas
    * @param g The main Graphics object to draw with
    */
  public void draw(Graphics g) 
  {
    // If the point arrays are missing, re-create them
    if (aXPoints == null || aYPoints == null)
    {
      int num = points.size();
      aXPoints = new int[num+1];
      aYPoints = new int[num+1];
      
      int idx = 0;
      // Set the (x,y) coordinates of each point in turn
      for (Vector2D pt : points)
      {
        aXPoints[idx] = (int)Math.round(pt.x);
        aYPoints[idx] = (int)Math.round(pt.y);

        idx++;
      }
     
      // Re-iterate the first point to close the loop
      Vector2D first = points.getFirst();
      aXPoints[num] = (int)Math.round(first.x);
      aYPoints[num] = (int)Math.round(first.y);
    }

    Color tempC = g.getColor();
		g.setColor(Color.black);
		g.drawPolyline(aXPoints, aYPoints, aXPoints.length);
		g.setColor(tempC);
    
    // Draw the edge normals (@@@ only for debugging)
    drawEdgeNormals(g);
    // Draw the bounding box
    //drawBoundingBox(g);
  }

  public void drawBoundingBox (Graphics g)
  {
    int[] bbx = new int[5];
    int[] bby = new int[5];

		bbx[0] = bbx[1] = bbx[4] = (int)Math.round(bx);
    bbx[2] = bbx[3] = (int)Math.round(bx + bw);
    bby[0] = bby[3] = bby[4] = (int)Math.round(by);
    bby[1] = bby[2] = (int)Math.round(by + bh);

    Color tempC = g.getColor();
    g.setColor(Color.cyan);
		g.drawPolyline(bbx, bby, bbx.length);
		g.setColor(tempC);
  }

  public void drawEdgeNormals(Graphics g)
  {
    Vector2D p1 = points.getLast();

    Color tempC = g.getColor();
    g.setColor(Color.black);

    // Walk through all edges, find their normal, draw it starting at the
    //  center of the edge
    for (Vector2D p2 : points)
    {
      Vector2D center = 
        new Vector2D(0.5 * p1.x + 0.5 * p2.x,
                     0.5 * p1.y + 0.5 * p2.y);
      Vector2D normal = computeNormal(p1,p2);

      Vector2D outPoint = normal.times(5).plus(center);
		  g.drawLine((int)center.x, (int)center.y, 
                 (int)outPoint.x, (int)outPoint.y);

      p1 = p2;
    }
		g.setColor(tempC);
  }


  public String toStringList()
  {
    String s = "";

    for (Vector2D p : points)
      s += p.x + " " + p.y + "\n";  

    Vector2D first = points.getFirst();

    s += first.x + " " + first.y;

    return s;
  }

  /** A simple test routine
    * If you think my contains() algorithm is broken, use this by 
    *  replacing the given polygon by a polyon of your choice
    */
  public static void main(String[] args)
  {
    Polygon poly = new Polygon();
    poly.addPoint(-1, -1);
    poly.addPoint(0,2);
    poly.addPoint(1,-1);
    poly.addPoint(0,0);

    int maxN = 1000;

    if (args.length > 0)
      maxN = Integer.parseInt(args[0]);

    Polygon oth = new Polygon();
    oth.addPoint(0,0);
    oth.addPoint(0,0.5);
    oth.addPoint(0.5,0.5);
    oth.addPoint(0.5,0);

    //testContains(poly, maxN);
    testIntersects(poly, oth, maxN);
  }
  
  public static void testIntersects(Polygon poly, Polygon oth, int maxN)
  {
    for (int n = 0; n < maxN; n++)
    {
      // Translate by a random amount
      double x = Math.random()*poly.bw*2 + poly.bx - poly.bw*0.5;
      double y = Math.random()*poly.bh*2 + poly.by - poly.bh*0.5;
   
      oth.translate(new Vector2D(x,y));

      if (poly.intersects(oth) != null)
        System.out.println (x+" "+y);
      else
        System.err.println (x+" "+y);

      oth.translate(new Vector2D(-x,-y));
    }
  }


  public static void testContains(Polygon poly, int maxN)
  {
    for (int n = 0; n < maxN; n++)
    {
      double x = Math.random()*poly.bw*2 + poly.bx - poly.bw*0.5;
      double y = Math.random()*poly.bh*2 + poly.by - poly.bh*0.5;
    
      // To view this data you will need magic goggles
      // More specifically:
      //  java Polygon 10000 1>inside 2>outside
      //  gnuplot
      //  plot 'inside' with points
      //  replot 'outside' with points
      //  Ta-da
      if (poly.contains(new Vector2D(x,y)))
        System.out.println (x+" "+y);
      else
        System.err.println (x+" "+y);
    }
  }
}
