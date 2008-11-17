package org.rlcommunity.critter;

/**
  * SimulatorObject
  *
  * Defines the basic properties and methods relevant to any simulator object,
  *  be it a wall, a tennis ball or a critterbot.
  *
  * @author Marc G. Bellemare
  * @author Mike Sokolsky
  * @author Anna Koop
  */

import java.util.LinkedList;
import java.util.List;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeMap;

/**
 * 
 * 
 * @author anna
 */public class SimulatorObject
{
  /** Some properties of the object - position, velocity */
  protected Vector2D aPos;
  protected double aDir;

  /** A polygon describing the shape of the object; may be null if the
    *  object is an invisible source */
  protected Polygon aShape;

  /** List of state components for this object */
  //protected LinkedList<ObjectState> aStates;
  protected Map<String, ObjectState> aStates=new TreeMap<String, ObjectState>();
  

  protected String aLabel;

  /** A unique identifier which is used to keep track of an object from state
    *  to state
    */
  protected int aId;

  /** Creates a new object with a given label (e.g., "wall") and identifier.
    *
    * @param pLabel The object label
    * @param pId    The object's unique identifier
    */
  public SimulatorObject(String pLabel, int pId)
  {
    aLabel = new String(pLabel);
    aId = pId;
    aPos = new Vector2D(0,0);
    aDir = 0;

  }

  /** Returns collision information, as a Collision class, if this object
    *  is colliding with the given object. The returned Collision will be
    *  null if no collision actually happened. The meaning of the Collision 
    *  information is somewhat undefined in the case where the two objects'
    *  polygons intersect at more than two points.
    *
    * @param compObj The object to be compared with
    * @return Null if there is no collision between the two objects, or a
    *  Collision object containing collision information otherwise.
    */
  public Collision collidesWith(SimulatorObject compObj)
  {
    Polygon compShape = compObj.getShape();
    
    if (aShape == null || compShape == null)
      return null;
    
    // Get the first two intersections
    List<Polygon.Intersection> isects = aShape.getIntersections(compShape, 2);

    // No intersection, no collision
    if (isects.size() == 0) 
      return null;
    else if (isects.size() == 1)
    {
      // @@@ the case when we have one intersection is incorrect, as in this
      //  case it's not clear what the normal is
      Collision col = new Collision();
      Polygon.Intersection i1 = isects.get(0);
      double alpha = i1.alpha; 
      col.point = aShape.getPoint(alpha);
      col.normal = aShape.getNormal(alpha);
      col.alpha = i1.alpha;
      col.beta = i1.beta;

      return col;
    }
    else
    {
      Collision col = new Collision();
      // Look at the two intersections that we have
      Polygon.Intersection i1 = isects.get(0);
      Polygon.Intersection i2 = isects.get(1);

      Vector2D p1 = aShape.getPoint(i1.alpha);
      Vector2D p2 = aShape.getPoint(i2.alpha);

      // Keep the polygon coordinates of the collision point
      //  This is an interpolation between the two points. Could be
      //   very wrong, e.g. pacman-style shapes
      /*
      This is wrong - need to view alpha's as between (-n/2, n/2]
      col.alpha = (i1.alpha + i2.alpha) / 2; 
      col.beta = (i1.beta + i2.beta) / 2; 
      */
      // @@@ For now, we return a single intersection point - this is because
      //  The interpolation of the two points might not be a good point of
      //  contact at all
      col.alpha = i1.alpha;
      col.beta = i1.beta;

      // @@@ this is also wrong in general, e.g. if alpha1 = n-1 and alpha2 = 0,
      //  this results in alpha = (n-1)/2 (when it should be n - 1/2)
      col.point = aShape.getPoint(col.alpha);

      // The normal is the normal to p2-p1
      // @@@ this might be the wrong direction
      // @@@ normalize?
      col.normal = p2.minus(p1).rotate(Math.PI / 2);
      
      return col;
    }
  }

    /**
     * Returns true of this objects' shape intersects the parameter objects'
     *
     * @returns True if the passed object intersects this object
     * @obsolete
     **/
    public Vector2D intersects(SimulatorObject compObj) 
    {
	//@@todo find out why this was throwing a null
	// pointer exception before I initialzed aShape
	// in the constructor
      if (aShape == null || compObj.getShape() == null) 
        return null;
      else
      {
        Polygon.Intersection isect =
          aShape.intersects(compObj.getShape());
        if (isect == null)
          return null;
        else 
          return aShape.getPoint(isect.alpha);
      }
    }
    
    public boolean corresponds(SimulatorObject o) {
        
        if (this.aId == o.aId) return true;
        else return false;
    }


  /** Sets the shape of the object (which is shapeless by default).
    *
    * @param pShape The polygon representing the shape. It is not copied over.
    */
  public void setShape(Polygon pShape)
  {
    aShape = pShape;
  }

  public Polygon getShape()
  {
    return aShape;
  }

  /** Returns the partial state corresponding to a particular component,
    * identified by pLabel.
    *
    * @param pLabel The identifier used by the SimulatorComponent and 
    *   the corresponding ObjectState.
    * @return The corresponding ObjectState or null if this object does not
    *   store this state information.
    *
    */
  
  public ObjectState getState(String pLabel)
  {
      // We want to return null if the ObjectState does not exist
      return aStates.get(pLabel);
  }

  /** Add a new state component to the object; duplicates are not allowed!
    *
    * @param pState The new state component
    */
  public void addState(ObjectState pState)
  {
    // @@@ Automatically replaces duplicates.
    // @@@ MGB - we probably don't want an assert here, but it is fine for
    //  now; whether we should crash on duplicates is open to discussion 
    assert(!aStates.containsKey(pState.getName()));
    aStates.put(pState.getName(),pState);
  }

  private void loadObject() {
	  // Open files, lookup plabel, etc???
  }
  
  /**
   * Draw the object on the provided canvas 
   * 
   * @param g The Graphics object used to draw
   */
  public void draw(Graphics g)
  {
	  aShape.draw(g);

    // Draw each ObjectState in turn
    Set<Entry<String, ObjectState>> theEntrySet = aStates.entrySet();
    
      for (Entry<String, ObjectState> thisEntry : theEntrySet)
      {
        thisEntry.getValue().draw(g, this);
      }
  }
  
  /**
    * Return the unique identifier of this object. The identifier is the same
    *  across time steps.
    *
    * @return An integer uniquely identifying this object
    */
  public int getId()
  {
    return aId;
  }

  public String getLabel()
  {
    return aLabel;
  }

  public void setPosition(Vector2D newPos) {
    // If we have a shape, also translate it by the difference
    if (aShape != null)
      aShape.translate(newPos.minus(aPos));

	  aPos = (Vector2D) newPos.clone();
  }

  public void setDirection(double newDir) {
    // If we have a shape, also rotate it
    if (aShape != null)
      aShape.rotate(aDir - newDir, aPos);
  
    aDir = newDir;
  }

  public Vector2D getPosition() { return aPos; }

  public double getDirection() { return aDir; }

  /** Makes a copy of this object. For cloning purposes, the Object id 
    *  remains identical. This method's purpose is to be used for copying
    *  objects between states.
    */
  public Object clone()
  {
    SimulatorObject newObj = new SimulatorObject(this.aLabel, this.aId);

    newObj.copyFrom(this);
    return newObj; 
  }

    public String toString() {
	return aLabel;
    }

    /**
     * Compares the position and direction of the current object
     * to the argument's position and direction
     * @param Comparison SimulatorObject
     **/
    public boolean geometryEquals(SimulatorObject compObj) {
	return(aPos.equals(compObj.getPosition()) && aDir==compObj.getDirection());
    }

    /**
     * Sets the position and direction of the current object
     * to the argument's position and direction
     * @param Template SimulatorObject
     **/
    public void setGeometry(SimulatorObject compObj) {
	    this.setPosition(compObj.getPosition());
	    this.setDirection(compObj.getDirection());
    }

  /** Copies the data from 'original' onto this object. This is used for
    *  cloning purposes.
    */
  protected void copyFrom(SimulatorObject org)
  {
    this.aPos = (Vector2D) org.aPos.clone();
    this.aDir = org.aDir;

    // We must copy the shape around, because the polygons get modified
    //  by translations and rotations
    this.aShape = (Polygon)org.aShape.clone();

    /* @@@ Copy the other attributes of the object, the actuator and sensor 
     * list */
    Set<Entry<String, ObjectState>> theEntrySet=org.aStates.entrySet();
    
      for (Entry<String, ObjectState> thisEntry : theEntrySet) {
          this.addState((ObjectState)thisEntry.getValue().clone());
      }
   }

}
