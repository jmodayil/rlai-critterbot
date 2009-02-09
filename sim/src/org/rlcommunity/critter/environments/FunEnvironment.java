/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
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
public class FunEnvironment implements EnvironmentDescription {

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
                CommonObjects.generateHex("Hex", id),
                new Vector2D(1.0, 1.0), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateBar("Bar", id),
                new Vector2D(3.0, 2.5), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateBall("Ball_1", id, 0.05),
                new Vector2D(Math.random() * 4 + 1, Math.random() * 3 + 1), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateBall("Ball_2", id, 0.1),
                new Vector2D(Math.random() * 4 + 1, Math.random() * 3 + 1), 0.0, id);

        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("Light Source1", id,600),
                new Vector2D(0.5, 0.5), 0.0, id); //something funnie going on with positions
 
        id = CommonObjects.addObject(objects,
                CommonObjects.generateBatteryCharger("batteryCharger", id),
                new Vector2D(3.0, 0.25), 0.0, id);       
        

        return objects;
    }

    public boolean usesSVG() {
        return false;
    }


}
