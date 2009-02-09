/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter.environments;
import java.util.LinkedList;
import java.util.List;
import org.rlcommunity.critter.SimulatorObject;
import org.rlcommunity.critter.Vector2D;
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
