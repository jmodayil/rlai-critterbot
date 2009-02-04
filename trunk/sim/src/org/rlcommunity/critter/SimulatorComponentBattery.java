/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter;

import java.util.List;

/**
 *
 * @author awhite
 */
public class SimulatorComponentBattery implements SimulatorComponent {

    
    public static final String NAME = "battery";

    public SimulatorComponentBattery ()
    {}
    
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) {
      List<SimulatorObject> sensors = 
        pCurrent.getObjects(ObjectStateGyroscope.NAME);
      
      //set fullycharged to zero
      
      //if dist between robot and charger is less than range
        //increase current charge
        //check if fully charged
      //else
            //deplete battery
    
    }
    
    

}
