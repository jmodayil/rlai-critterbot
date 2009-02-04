package org.rlcommunity.critter;

/**
 * SimulatorComponentAccelerometer
 *
 * This component deals with accelerometer sensors
 *
 * @author Marc G. Bellmare
 */

import java.util.List;
import java.util.Random;

public class SimulatorComponentAccelerometer implements SimulatorComponent {

    public static final String NAME = "accelerometer";
    public final Random aRandom;
    
    public SimulatorComponentAccelerometer() {
        this(new Random());
        System.err.println ("Deprecated: using local Random object.");
    }

    public SimulatorComponentAccelerometer(Random pRandom) {
        aRandom = pRandom;
    }

    /** Computes what accelerometers should receive given the current state,
      *  and set the result in the next state.
      *
      * @param pCurrent The current state of the system (must not be modified)
      * @param pNext  The next state of the system (where results are stored)
      * @param delta  The number of milliseconds elapsed between pCurrent
      *   and pNext.
      */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) 
    {
      List<SimulatorObject> sensors = 
        pCurrent.getObjects(ObjectStateAccelerometer.NAME);

      // For each accelerometer ...
      for (SimulatorObject sensor : sensors)
      {
        SimulatorObject nextSensor = pNext.getObject(sensor);
        if (nextSensor == null) continue;

        assert(nextSensor.getState(ObjectStateAccelerometer.NAME) != null);

        ObjectStateAccelerometer nextAccelData = (ObjectStateAccelerometer)
          nextSensor.getState(ObjectStateAccelerometer.NAME);

        ObjectStateAccelerometer accelData = (ObjectStateAccelerometer)
          sensor.getState(ObjectStateAccelerometer.NAME);

        ObjectState os = sensor.getState(SimulatorComponentDynamics.NAME);
        if (os == null) continue;

        ObjectStateDynamics dynData = (ObjectStateDynamics)os;

        Vector2D oldVel = accelData.getVelocitySample();
        Vector2D curVel = dynData.getVelocity();

        //  Compute a very rough estimate of the acceleration as a 
        //   difference in velocities
        Vector2D accelValue = curVel.minus(oldVel).times(1000.0 / delta);
        nextAccelData.setSensorValue(accelValue);
        // Store the current velocity for the next time step
        nextAccelData.setVelocitySample(curVel);
      }
    }
}


