package org.rlcommunity.critter;

/**
  * ObjectStateIRDistanceSensor
  *
  * State component for an infrared distance sensor
  *
  * @author Marc G. Bellemare
  */

import java.awt.Graphics;
import java.awt.Color;

public class ObjectStateIRDistanceSensor implements ObjectState
{
  public static final String NAME = SimulatorComponentIRDistance.NAME;
  public boolean debugDrawRays = false;

  protected double aData;
  protected double aRange;

  public ObjectStateIRDistanceSensor(double pRange)
  {
    aData = 0;
    aRange = pRange;
  }

  /** Sets the current sensor reading for this sensor
    *
    * @param pValue The ir distance sensor reading
    */
  public void setSensorValue(double pValue) { aData = pValue; }
 
  /** Returns the current sensor reading
    * @return The current sensor reading
    */
  public double getSensorValue() { return aData; }

  /** Sets the maximum range of this distance sensor
    * @param pRange The new maximum range (in meters) of the sensor
    */
  public void setRange(double pRange) { aRange = pRange; }

  /** Returns the maximum range of this distance sensor
    * @return The maximum range of the sensor, in meters
    */
  public double getRange() { return aRange; }



  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateIRDistanceSensor newSensor = 
      new ObjectStateIRDistanceSensor(aRange);

    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateIRDistanceSensor sensor = (ObjectStateIRDistanceSensor)os;
  
    // Redundant code, but here for sanity purposes 
    this.aRange = sensor.aRange;
  }

  /** only for debugging */
  public RayIntersection intersection;

  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(Graphics g, SimulatorObject parent)
  {
    if (intersection != null)
    {
      Vector2D pt = intersection.point;

	    Color tempC = g.getColor();
	    /*g.setColor(Color.red);

      int rad = 5; 
	    g.drawOval((int)pt.x-rad, (int)pt.y-rad, 2*rad, 2*rad); 
     
      Vector2D outPoint = intersection.normal.times(2*rad).plus(pt);
		  g.drawLine((int)pt.x, (int)pt.y, 
                 (int)outPoint.x, (int)outPoint.y);
     */ 
      if (debugDrawRays)
      {
        Vector2D pos = parent.getPosition();
        Double dir = parent.getDirection();

        int rad = 3;
        if( aData >= aRange ) {
          g.setColor(new Color(230, 220, 220));
          g.drawLine ((int)pos.x, (int)pos.y,
            (int)(pos.x + aRange * Math.cos(dir)),
            (int)(pos.y + aRange * Math.sin(dir)));
          g.setColor(Color.orange);
          g.drawOval((int)(pos.x + aRange * Math.cos(dir)) - rad,
            (int)(pos.y + aRange * Math.sin(dir)) - rad,
            2 * rad, 2 * rad);
        }
        else {
          g.setColor(new Color(230, 220, 220));
          g.drawLine ((int)pos.x, (int)pos.y, (int)pt.x, (int)pt.y);
          g.setColor(Color.orange);
          g.drawOval((int)pt.x-rad, (int)pt.y-rad, 2*rad, 2*rad); 
          
        }
        //g.drawLine ((int)pos.x, (int)pos.y, 
        //  (int)pt.x, (int)pt.y);
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
    aData = 0;
  }
}

