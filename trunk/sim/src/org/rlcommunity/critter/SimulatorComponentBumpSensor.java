package org.rlcommunity.critter;

/**
 * SimulatorComponentBumpSensor
 *
 * This component deals with bump sensors, and possibly in the future any
 *  response to contact between two objects.
 *
 * @author Marc G. Bellemare
 */

import java.util.LinkedList;
import java.util.List;

public class SimulatorComponentBumpSensor implements SimulatorComponent {

    public static final String NAME = "bump_sensor";

    public static final double BUMP_DECAY = 0.9;

    public SimulatorComponentBumpSensor()
    {
    }

    /** Computes what bump sensors should receive given the current state,
      *  and set the result in the next state.
      *
      * @param pCurrent The current state of the system (must not be modified)
      * @param pNext  The next state of the system (where results are stored)
      * @param delta  The number of milliseconds elapsed between pCurrent
      *   and pNext.
      */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) 
    {
      List<SimulatorObject> objects = 
        pCurrent.getObjects(ObjectStateBumpSensor.NAME);

      for (SimulatorObject obj : objects)
      {
        SimulatorObject nextObj = pNext.getObject(obj);

        ObjectStateBumpSensor bs = ObjectStateBumpSensor.retrieve(obj);
        ObjectStateBumpSensor nextbs = ObjectStateBumpSensor.retrieve(nextObj);
        ObjectStateDynamics dynData = ObjectStateDynamics.retrieve(obj);

        // If this object is not subject to collisions, assume its bump sensor
        //  will not pick up anything
        if (dynData == null) continue;
        
        // Decay existing forces and add them to the next state
        List<Force> forces = bs.getForces();

        for (Force f : forces) {
          Vector2D newVec = 
            new Vector2D(f.vec.x * BUMP_DECAY, f.vec.y * BUMP_DECAY);

          // @todo Yuck, fixme
          // For now, remove forces when they get small
          if (newVec.length() < 0.01)
            continue;

          Force newForce = new Force(newVec, f.source);
          nextbs.addForce(newForce);
        }

        // Add new collisions
        for (Collision c : dynData.getCollisions()) {
            Force newForce = new Force(c.normal, c.point);
            nextbs.addForce(newForce);
        }
      }
    }
}


