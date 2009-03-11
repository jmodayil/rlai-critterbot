/* Copyright 2009 Adam White
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
  * ObjectStateLightSensor
  *
  * State component for an object which senses light.
  *
  * @author Adam White, Marc G. Bellemare
  */

import java.awt.Color;
import java.util.LinkedList;

public class ObjectStateLightSensor implements ObjectState
{
  public static final boolean debugDrawIntersections = false;
  
  public static final String NAME = SimulatorComponentLight.NAME + "sensor";

  protected int aData; //in candela
  protected double aSensorWidth; //in Meters
  protected int aNumPixels;     
  protected double aSensorDepth; //in Meters 

  protected double aError;

  public static final double defaultError = 0.05;
  
  public ObjectStateLightSensor()
  {
      this(0,0.0,0.0);
  }

  public ObjectStateLightSensor(int pNumPixels, double pSensorDepth,
          double pSensorWidth) {
    this(pNumPixels, pSensorDepth, pSensorWidth, defaultError);
  }

  public ObjectStateLightSensor(int pNumPixels, double pSensorDepth,
                                double pSensorWidth, double pError) {
    aData = 0;
    aSensorWidth = pSensorWidth;
    aNumPixels = pNumPixels;
    aSensorDepth = pSensorDepth;
    aError = pError;
  }

    public double getError() { return aError; }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateLightSensor newSensor = new ObjectStateLightSensor();
    newSensor.copyFrom(this);

    return newSensor;
  }

  protected void copyFrom(ObjectState os)
  {
      
    ObjectStateLightSensor sensor = (ObjectStateLightSensor)os;
  
    this.aData = sensor.aData;
    this.aSensorWidth = sensor.aSensorWidth;
    this.aNumPixels = sensor.aNumPixels;
    this.aSensorDepth = sensor.aSensorDepth;
    
  }
  
  protected void setLightSensorValue (int intensity)
  {
      aData = intensity;
  }
  
  protected int getLightSensorValue ()
  {
   return aData;   
      
  }
  
  protected double getSensorWidth ()
  {
   return aSensorWidth;   
      
  }
  
  protected int getNumPixels ()
  {
   return aNumPixels;   
      
  }
  
  protected double getSensorDepth ()
  {
   return aSensorDepth;   
      
  }

  /** For debugging purposes only */
  protected LinkedList<RayIntersection> intersections = new LinkedList<RayIntersection>();

  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(SimulatorGraphics g, SimulatorObject parent) {
    if (debugDrawIntersections) {
      for (RayIntersection isect : intersections) {
        g.setColor(new Color(127, 0, 127));
        g.drawOval(isect.point.x - 0.025, isect.point.y - 0.025, 0.05, 0.05);
      }
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

