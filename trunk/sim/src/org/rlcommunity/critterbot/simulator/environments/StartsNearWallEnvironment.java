
package org.rlcommunity.critterbot.simulator.environments;

import java.util.LinkedList;
import java.util.List;

import org.rlcommunity.critterbot.simulator.SimulatorObject;
import org.rlcommunity.critterbot.simulator.Vector2D;

public class StartsNearWallEnvironment implements EnvironmentDescription {
	
    public List<SimulatorObject> generateObjects() {
    	
    	LinkedList<SimulatorObject> objects = new LinkedList<SimulatorObject>();

    	int id = 0;

    	// Add a Critterbot
    	id = CommonObjects.addObject(objects,
    			CommonObjects.generateCritterbot("Critterbot", id),
    			new Vector2D(2.5,4.45), (Math.PI*3)/2, id);

    	id = CommonObjects.addObject(objects,
    			CommonObjects.generateWall("Wall", id),
    			new Vector2D(0.0, 0.0), 0.0, id);

    	double chargerRadius = 0.25;

    	id = CommonObjects.addObject(objects,
    			CommonObjects.generateBatteryCharger("batteryCharger", id),
    			new Vector2D(5.0-chargerRadius, 5.0-chargerRadius), 0.0, id);

    	id = CommonObjects.addObject(objects,
    			CommonObjects.generateLightSource("Light Source", id,600),
    			new Vector2D(0.50, 2.50), 0.0, id);
    	return objects;
    }

    public boolean usesSVG() {
        return false;
    }
}
