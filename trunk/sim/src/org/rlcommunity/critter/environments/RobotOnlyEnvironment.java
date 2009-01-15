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
                new Vector2D(250,250), 0.0, id);

        return objects;
    }


}
