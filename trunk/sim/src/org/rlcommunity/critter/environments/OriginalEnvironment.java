/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critter.environments;

import java.util.LinkedList;
import java.util.List;
import org.rlcommunity.critter.SimulatorObject;
import org.rlcommunity.critter.Vector2D;

/**
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class OriginalEnvironment implements EnvironmentDescription {

    public List<SimulatorObject> generateObjects() {
        LinkedList<SimulatorObject> objects = new LinkedList<SimulatorObject>();

        int id = 0;

        // Add a Critterbot
        id = CommonObjects.addObject(objects,
                CommonObjects.generateCritterbot("Crittebot", id),
                new Vector2D(250,250), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateWall("Wall", id), id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("Light Source", id),
                new Vector2D(50, 50), 0.0, id);

        return objects;
    }


}
