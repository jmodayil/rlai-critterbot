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
 * SimulatorComponentGyroscope
 *
 * This component deals with gyroscope sensors
 *
 * @author Marc G. Bellemare
 */

import java.util.List;
import java.util.Random;

public class SimulatorComponentGyroscope implements SimulatorComponent {

    /** That which we call a gyroscope */
    public static final String NAME = ObjectStateGyroscope.NAME;

    /** A random number generator to produce noise */
    protected Random aRandom;

    /** Creates a new gyroscope SimulatorComponent with an internal
     *   random number generator.
     *
     * @deprecated
     */
    public SimulatorComponentGyroscope() {
        this(new Random());
        System.err.println ("Deprecated: using local Random object.");
    }

    /** Creates a new gyroscope SimulatorComponent.
     *
     * @param pRandom A random number generator  (used for noise).
     */
    public SimulatorComponentGyroscope(Random pRandom) {
        aRandom = pRandom;
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
      // Request the list of all gyroscopes in the world, at time t
      List<SimulatorObject> sensors = 
        pCurrent.getObjects(ObjectStateGyroscope.NAME);

      // For each gyroscope ...
      for (SimulatorObject sensor : sensors)
      {
        // Find the object at time t+1 corresponding to the object that has the gyroscope property
        SimulatorObject nextSensor = pNext.getObject(sensor);
        if (nextSensor == null) continue;

        assert(nextSensor.getState(ObjectStateGyroscope.NAME) != null);

        // Obtain the gyroscope ObjectState for times t+1 and t
        ObjectStateGyroscope nextGyroData = (ObjectStateGyroscope)
          nextSensor.getState(ObjectStateGyroscope.NAME);

        ObjectStateGyroscope gyroData = (ObjectStateGyroscope)
          sensor.getState(ObjectStateGyroscope.NAME);

        // Also get the dynamics at time t; if there is no such thing,
        //  we cannot measure rotational velocity and will ignore this sensor
        ObjectState os = sensor.getState(SimulatorComponentDynamics.NAME);
        if (os == null) continue;

        ObjectStateDynamics dynData = (ObjectStateDynamics)os;

        // We set the value of the gyroscope at time t+1 to reflect the
        //  rotational velocity at time t, plus noise
        double vel = dynData.getAngVelocity();
        double noise = gyroData.getError();
        
        // add some gaussian noise
        vel += vel * aRandom.nextGaussian() * noise;
        nextGyroData.setSensorValue(vel);
      }
    }
}


