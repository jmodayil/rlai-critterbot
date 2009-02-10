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
public class LightBatteryEnvironment  implements EnvironmentDescription{

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
                CommonObjects.generateLightSource("LightSource1", id,400),
                new Vector2D(2.5, 4.2), 0.0, id); //something funnie going on with positions   
        
        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("LightSource2", id, 75),
                new Vector2D(0.5, 0.5), 0.0, id); //something funnie going on with positions           
 
        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("LightSource3", id, 75),
                new Vector2D(4.5, 0.5), 0.0, id); //something funnie going on with positions  
        
        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("LightSource4", id, 75),
                new Vector2D(4.5, 4.5), 0.0, id); //something funnie going on with positions  

        id = CommonObjects.addObject(objects,
                CommonObjects.generateLightSource("LightSource5", id, 75),
                new Vector2D(0.5, 4.5), 0.0, id); //something funnie going on with positions          
                 
        id = CommonObjects.addObject(objects,
                CommonObjects.generateBatteryCharger("batteryCharger", id),
                new Vector2D(2.5, 4.4), 0.0, id);       
        

        return objects;
    }

    public boolean usesSVG() {
        return false;
    }    
    
}
