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

package org.rlcommunity.critter.environments;

import java.util.LinkedList;
import java.util.List;
import org.rlcommunity.critter.SimulatorObject;
import org.rlcommunity.critter.Vector2D;

/**
 * The original environment (with a robot, a wall, a hex and a light source)
 * 
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class SimpleEnvironment implements EnvironmentDescription {

    public List<SimulatorObject> generateObjects() {
        LinkedList<SimulatorObject> objects = new LinkedList<SimulatorObject>();

        int id = 0;
        
        // Add a Critterbot
        id = CommonObjects.addObject(objects,
                CommonObjects.generateCritterbot("Crittebot", id),
                new Vector2D(2.50,2.50), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateWall("Wall", id),
                new Vector2D(0.0, 0.0), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("Light Source1", id,600),
                new Vector2D(0.5, 0.5), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateBatteryCharger("batteryCharger", id),
                new Vector2D(3.0, 0.25), 0.0, id);

        int numBars = 1;
        for (int i = 0; i < numBars; i++) {
          id = CommonObjects.addObjectRandomPosition(objects,
                CommonObjects.generateBar("Bar"+i, id),
                new double[][] {{0.5, 4.5}, {0.5, 4.5}, {-Math.PI, Math.PI}}, id);
        }

        int numSmallBalls = 2;

        for (int i = 0; i < numSmallBalls; i++) {
          id = CommonObjects.addObjectRandomPosition(objects,
                CommonObjects.generateBall("SmallBall_"+i, id, 0.05),
                new double[][] {{0.5, 4.5}, {0.5, 4.5}, {0, 0}}, id);
        }

        int numMediumBalls = 1;

        for (int i = 0; i < numMediumBalls; i++) {
          id = CommonObjects.addObjectRandomPosition(objects,
                CommonObjects.generateBall("MediumBall"+i, id, 0.1),
                new double[][] {{0.5, 4.5}, {0.5, 4.5}, {0, 0}}, id);
        }

        int numBigBalls = 1;

        for (int i = 0; i < numBigBalls; i++) {
          id = CommonObjects.addObjectRandomPosition(objects,
                CommonObjects.generateBall("BigBall_"+i, id, 0.2),
                new double[][] {{0.5, 4.5}, {0.5, 4.5}, {0, 0}}, id);
        }

        return objects;
    }

    public boolean usesSVG() {
        return false;
    }


}
