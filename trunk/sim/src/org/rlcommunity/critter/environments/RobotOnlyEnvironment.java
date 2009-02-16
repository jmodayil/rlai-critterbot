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
 * This class defines an environment in which there is only a robot.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class RobotOnlyEnvironment implements EnvironmentDescription {

    public List<SimulatorObject> generateObjects() {
        LinkedList<SimulatorObject> objects = new LinkedList<SimulatorObject>();

        int id = 0;

        // Add a Critterbot
        id = CommonObjects.addObject(objects, 
                CommonObjects.generateCritterbot("Crittebot", id),
                new Vector2D(2.50,2.50), 0.0, id);

        return objects;
    }

    public boolean usesSVG() {
        return false;
    }


}
