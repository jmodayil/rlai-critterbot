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

import java.awt.Color;
import java.awt.Graphics;

public class Polygon
{
  /** The polygon's bounding box */
  protected double bx,by,bw,bh;

  protected LinkedList<Vector2D> points;

  protected int[] aXPoints, aYPoints;

  public Polygon()
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

  public Vector2D intersects(Polygon poly)
  {
    if(poly == null)
	    return null;

    // Compare the bounding boxes first, and hope that they don't intersect
    double l = poly.bx;
    double r = poly.bx + poly.bw;
    double t = poly.by;
    double b = poly.by + poly.bh;
    
    double br = bx + bw;
    double bb = by + bh;

    // @@@ this can be simplified to - assume no intersection if bb's
    //  don't intersect
    if (! ((l < bx && bx < r) || (l < br && br < r) ||
           (t < by && by < b) || (t < bb && bb < b)) &&
        ! ((bx < l && l < br) || (bx < r && r < br) ||
           (by < t && t < bb) || (by < b && b < bb)) )
      return null;

    // Assume fully enclosed polygons do NOT intersect

    // Use brute force if the bounding box test fails, for lack of 
    //  programming time (all-edges comparison)
    Vector2D pa1 = this.points.getLast();
    for (Vector2D pa2 : this.points)
    {
      Vector2D pb1 = poly.points.getLast();
      for (Vector2D pb2 : poly.points)
      {
        // Test whether the line segment pa1-pa2 intersects pb1-pb2
        double denom = (pb2.y - pb1.y) * (pa2.x - pa1.x) -
                       (pb2.x - pb1.x) * (pa2.y - pa1.y);
        
        // Assume general case and go on with your life 
        if (denom == 0)
        {
          pb1 = pb2;
          continue;
        }

        double num1 = (pb2.x - pb1.x) * (pa1.y - pb1.y) -
                       (pb2.y - pb1.y) * (pa1.x - pb1.x); 
        double num2 = (pa2.x - pa1.x) * (pa1.y - pb1.y) -
                       (pa2.y - pa1.y) * (pa1.x - pb1.x); 

        double alpha = num1 / denom;
        double beta = num2 / denom;

        if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1)
        {
          return new Vector2D(pa1.x + alpha * (pa2.x - pa1.x),
                              pa1.y + alpha * (pa2.y - pa1.y));
        }

        pb1 = pb2;
      }

      pa1 = pa2;
    }

    return null;
  }

  /**
    * Determine whether the given ray interesects this polygon. Only 
    *  positive intersections are considered, e.g. the ray intersects the
    *  polygon iff there exists an alpha > 0 s.t. ray.src + alpha * ray.dir
    *  is also a point on the polygon.
    *
    * @param r The ray of interest
    * @return The strictly positive alpha such that ray.getPoint(alpha) is
    *   on the polygon. Returns < 0 if there is no intersection.
    */
  public double intersect(Ray r)
  {
    // For now, do an all edges comparison with the polygon
    //  I feel like there is a much easier way to do this...
    Vector2D r1 = r.src;
    Vector2D r2 = r.src.plus(r.dir);

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
        // Return the 'alpha' corresponding to the ray (here, called beta)
        return beta;

      p1 = p2;
    }

    // No valid interesection, return -1 (we could also return 0, but this
    //  is a bad idea).
    return -1;
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
    
    // Draw the bounding box (@@@ only for debugging, take me out)
    /* int[] bbx = new int[5];
    int[] bby = new int[5];

		bbx[0] = bbx[1] = bbx[4] = (int)Math.round(bx);
    bbx[2] = bbx[3] = (int)Math.round(bx + bw);
    bby[0] = bby[3] = bby[4] = (int)Math.round(by);
    bby[1] = bby[2] = (int)Math.round(by + bh);

    g.setColor(Color.cyan);
		g.drawPolyline(bbx, bby, bbx.length);
		g.setColor(tempC); */
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
