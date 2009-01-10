package org.rlcommunity.critter;

/**
 * SimulatorComponentIRDistance
 *
 * This component computes the sensor readings for IR distance sensors 
 *
 * @author Marc G. Bellemare
 */

import java.awt.geom.Point2D;
import java.util.*;

public class SimulatorComponentIRDistance implements SimulatorComponent {

    public static final String NAME = "ir_distance";

    public SimulatorComponentIRDistance() 
    {
    }

    /** Computes what IR distance sensors should receive given the current 
      *  state, and set the result in the next state.
      *
      * @param pCurrent The current state of the system (must not be modified)
      * @param pNext  The next state of the system (where results are stored)
      * @param delta  The number of milliseconds elapsed between pCurrent
      *   and pNext.
      */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) 
    {
      // create a Scene corresponding to the current state
      Scene scene = new Scene(pCurrent);

      // Find out all the IR distance sensors
      List<SimulatorObject> sensorObjs = 
        pCurrent.getObjects(ObjectStateIRDistanceSensor.NAME);

      for (SimulatorObject obj : sensorObjs)
      {
        SimulatorObject nextObj = pNext.getObject(obj);

        if (nextObj == null) continue;

        // Get the next time step's object's sensor property
        ObjectStateIRDistanceSensor sensor = (ObjectStateIRDistanceSensor)
          nextObj.getState(ObjectStateIRDistanceSensor.NAME);

        assert(sensor != null);

        SimulatorObject root = obj.getRoot();

        // Remove this sensor's root's subtree (not necessarily a valid thing)
        scene.removeSubtree(root);

        Vector2D objPos = obj.getPosition();

        // Cast a ray
        Ray r = new Ray(objPos,
          Vector2D.unitVector(obj.getDirection())); 

        // Find the find intersection of the ray in the world
        RayIntersection intersection = scene.traceRay(r);

        // @todo remove me, for debugging purposes
        sensor.intersection = intersection;

        double range = sensor.getRange(); 
        double distance;

        // Find out how far the intersection is
        if (intersection == null)
          distance = range; 
        else
        {
          // Note - the distance is valid iff the ray uses a unit vector!
          distance = intersection.rayAlpha; 
          // The distance is capped by the range
          if (distance >= range) distance = range;
        }

        sensor.setSensorValue(distance);
      }

    }
}

