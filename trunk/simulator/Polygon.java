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
    bx = by = bw = bh = 0;
  }

  public int size()
  {
    return points.size();
  }

  /** Add a point to this polygon.
    */
  public void addPoint(double x, double y)
  {
    points.add(new Vector2D(x,y));
    if (x < bx) bx = x;
    else if (x - bx > bw) bw = x - bx;

    if (y < by) by = y;
    else if (y - by > bh) bh = y - by;

    // Not quite efficient
    aXPoints = aYPoints = null;
  }

  /** Translate this polygon with the given (dx,dy) vector
    * 
    * @param delta The vector to translate by
    */
  public void translate(Vector2D delta)
  {
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
    * Returns whether this polygon intersects the given polygon. The
    *  relationship is, of course, symmetric. Note that no efficiency
    *  guarantee is given.
    *
    * NOTE! In its current form, this algorithm is broken. 
    *
    * @param poly The polygon to be tested against.
    * @return Whether the two polygons intersect.
    */

  public boolean intersects(Polygon poly)
  {
    // Compare the bounding boxes first, and hope that they don't intersect
    double l = poly.bx;
    double r = poly.bx + poly.bw;
    double t = poly.by;
    double b = poly.by + poly.bh;
    
    double br = bx + bw;
    double bb = by + bh;

    // If none of our corners is inside the other guy's bounding box, we're
    //  safe
    if (! ((l < bx && bx < r) || (l < br && br < r) ||
           (t < by && by < b) || (t < bb && bb < b)) &&
        ! ((bx < l && l < br) || (bx < r && r < br) ||
           (by < t && t < bb) || (by < b && b < bb)) )
      return false;

    // Make sure neither polygon is fully enclosed in the other one
    //  If A is fully in B, then all of A's points must lie inside B
    //  Otherwise we will have to go through the O(n^2) case, ick
    if (poly.contains(this.points.getFirst())) return true;
    else if (this.contains(poly.points.getFirst())) return true;

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
        if (denom == 0) continue;

        double num1 = (pb2.x - pb1.x) * (pa1.y - pb1.y) -
                       (pb2.y - pb1.y) * (pa1.x - pb1.x); 
        double num2 = (pa2.x - pa1.x) * (pa1.y - pb1.y) -
                       (pa2.y - pa1.y) * (pa1.x - pb1.x); 

        double alpha = num1 / denom;
        double beta = num2 / denom;

        if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1)
          return true;

        pb1 = pb2;
      }

      pa1 = pa2;
    }

    return false;
  }

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

      if (poly.intersects(oth))
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
