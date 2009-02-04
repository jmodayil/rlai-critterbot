package org.rlcommunity.critter;

/**
 * SimulatorComponentBumpSensor
 *
 * This component deals with bump sensors, and possibly in the future any
 *  response to contact between two objects.
 *
 * @author Marc G. Bellemare
 */

import java.util.List;
import java.util.Random;

public class SimulatorComponentBumpSensor implements SimulatorComponent {

    public static final String NAME = "bump_sensor";

    public static final double BUMP_DECAY = 0.95;

    protected final Random aRandom;
    
    public SimulatorComponentBumpSensor() {
        this(new Random());
        System.err.println ("Deprecated: using local Random object.");
    }

    public SimulatorComponentBumpSensor(Random pRandom) {
        aRandom = pRandom;
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
        List<ObjectStateBumpSensor.BumpSensorData> forces = bs.getData();

        for (ObjectStateBumpSensor.BumpSensorData f : forces) {
            double newMagnitude = f.magnitude * BUMP_DECAY;
            if (newMagnitude < 0.01)
                continue;

            nextbs.addData(newMagnitude, f.alpha, f.point);
        }

        // Add new collisions
        for (Collision c : dynData.getCollisions()) {
            nextbs.addCollision(c);
        }
      }
    }
}


