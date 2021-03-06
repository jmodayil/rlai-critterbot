/* Copyright 2009 Marc G. Bellemare
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

package org.rlcommunity.critterbot.simulator;

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
        accelValue = accelValue.rotate(-sensor.getDirection());

        double noise = accelData.getError();

        // Add some Gaussian noise to the accelerometer sample
        accelValue.plusEquals(accelValue.times(aRandom.nextGaussian()*noise));
        double zAccel = ObjectStateDynamics.GRAVITY * (1.0 + aRandom.nextGaussian()*noise);

        nextAccelData.setSensorValue(accelValue);
        nextAccelData.setZSensorValue(zAccel);
        // Store the current velocity for the next time step
        nextAccelData.setVelocitySample(curVel);
      }
    }
}


