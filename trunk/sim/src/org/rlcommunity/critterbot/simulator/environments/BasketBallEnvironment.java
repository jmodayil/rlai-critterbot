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

package org.rlcommunity.critterbot.simulator.environments;
import java.util.LinkedList;
import java.util.List;

import org.rlcommunity.critterbot.simulator.SimulatorObject;
import org.rlcommunity.critterbot.simulator.Vector2D;
/**
 *
 * @author awhite
 */
public class BasketBallEnvironment implements EnvironmentDescription{

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
                CommonObjects.generateBasket("Box", id),
                new Vector2D(3.0, 2.5), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateBall("Ball_1", id, 0.05),
                new Vector2D(Math.random() * 4 + 1, Math.random() * 3 + 1), 0.0, id);      

        return objects;
    }

    public boolean usesSVG() {
        return false;
    }

}
