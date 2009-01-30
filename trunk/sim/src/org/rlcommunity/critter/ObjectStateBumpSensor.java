package org.rlcommunity.critter;

/**
  * ObjectStateBumpSensor 
  *
  * State component for bump sensors. The way this is implemented is by
  *  providing an object with this property (ObjectStateBumpSensor). The 
  *  whole polygon then becomes bump-sensitive.

  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;
import java.awt.Color;

import java.util.LinkedList;
import java.util.List;

public class ObjectStateBumpSensor implements ObjectState
{
    public static final boolean drawBumpSensors = true;
    
    public static final double BUMP_SENSOR_DRAW_SCALE = 10.0;
    public static final String NAME = SimulatorComponentBumpSensor.NAME;

    /** Class encapsulating relevant bump sensor data */
    public class BumpSensorData {
        // The magnitude of the sensed force
        public double magnitude;
        // The location, in Polygon coordinates, of the sensed force
        public double alpha;
        // The location, as a point, of the sensed force
        public Vector2D point;

        public BumpSensorData (double pMag, double pAlpha, Vector2D pPoint) {
            magnitude = pMag;
            alpha = pAlpha;
            point = pPoint;
        }
    }

    /** A list of forces sensed by this bump sensor */
    protected LinkedList<BumpSensorData> aData = new LinkedList<BumpSensorData>();

  public ObjectStateBumpSensor()
  {
  }

  /** Clears the list of sensed forces */
  public void clearData() { aData.clear(); }

  // @todo - document
  public void addCollision(Collision pCol) {
      BumpSensorData newData = 
              new BumpSensorData(pCol.magnitude, pCol.alpha, pCol.point);
      aData.add(newData);
  }

  public void addData(double pMagnitude, double pAlpha, Vector2D pPoint) {
      aData.add(new BumpSensorData(pMagnitude, pAlpha, pPoint));
  }
  
  /** Returns the list of forces being sensed
    *
    * @return A list of forces, normal to the polygon, being applied.
    *   Magnitude matters.
    */
  public List<BumpSensorData> getData() { return aData; }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateBumpSensor newSensor = new ObjectStateBumpSensor();
    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateBumpSensor sensor = (ObjectStateBumpSensor)os;
 
    // Assume that whatever forces were sensed at the last state do not
    //  carry over (let the BumpSensor component take care of this)
  }


  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(SimulatorGraphics g, SimulatorObject parent)
  {
      // Draw a little circle where each bobo is
      // The assumption is that drawing is synchronized with the main thread,
      //  so that this will not cause a ConcurrentModificationException
      if (drawBumpSensors) {
        Color tempC = g.getColor();
        g.setColor(Color.red);

        for (BumpSensorData f : aData) {
            double rad = (f.magnitude * 2) / BUMP_SENSOR_DRAW_SCALE;
            // Find out the current point of the sensor
            Vector2D curPoint;
            if (parent != null) curPoint = parent.aShape.getPoint(f.alpha);
            else curPoint = f.point;
            
            g.drawOval(curPoint.x-rad/2, curPoint.y-rad/2, rad, rad);
        }
        g.setColor(tempC);
      }
  }
  
  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient()
  {
    clearData();
  }

  /** Returns the ObjectStateBumpSensor for this object, or null if it does not
   *   exist.
   *
   * @param pObject The object being queried.
   * @return The query's ObjectStateBumpSensor property, if it has one.
   */
  public static ObjectStateBumpSensor retrieve(SimulatorObject pObject) {
      ObjectState os = pObject.getState(ObjectStateBumpSensor.NAME);

      if (os == null) return null;
      else return (ObjectStateBumpSensor)os;
  }
}


