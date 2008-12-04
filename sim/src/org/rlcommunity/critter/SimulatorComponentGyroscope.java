package org.rlcommunity.critter;

/**
 * SimulatorComponentGyroscope
 *
 * This component deals with gyroscope sensors
 *
 * @author Marc G. Bellemare
 */

import java.util.LinkedList;
import java.util.List;

public class SimulatorComponentGyroscope implements SimulatorComponent {

    public static final String NAME = "gyroscope";

    public SimulatorComponentGyroscope() 
    {
    }

    /** Computes what gyroscopes should receive given the current state,
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
        pCurrent.getObjects(ObjectStateGyroscope.NAME);

      // For each accelerometer ...
      for (SimulatorObject sensor : sensors)
      {
        SimulatorObject nextSensor = pNext.getObject(sensor);
        if (nextSensor == null) continue;

        assert(nextSensor.getState(ObjectStateGyroscope.NAME) != null);

        ObjectStateGyroscope nextGyroData = (ObjectStateGyroscope)
          nextSensor.getState(ObjectStateGyroscope.NAME);

        ObjectStateGyroscope gyroData = (ObjectStateGyroscope)
          sensor.getState(ObjectStateGyroscope.NAME);

        ObjectState os = sensor.getState(SimulatorComponentDynamics.NAME);
        if (os == null) continue;

        ObjectStateDynamics dynData = (ObjectStateDynamics)os;

        double vel = dynData.getAngVelocity();

        nextGyroData.setSensorValue(vel);
      }
    }
}


