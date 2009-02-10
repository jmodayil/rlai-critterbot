/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.rlcommunity.critter;

import java.util.LinkedList;
import java.util.List;
import java.util.NoSuchElementException;

/**
 *
 * @author awhite
 */
public class SimulatorComponentBattery implements SimulatorComponent {

    public static final String NAME = "battery";

    public SimulatorComponentBattery() {
    }

    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) {

        LinkedList<SimulatorObject> oldBatterys = pCurrent.getObjects(ObjectStateBattery.NAME);
        LinkedList<SimulatorObject> newBatterys = pNext.getObjects(ObjectStateBattery.NAME);        
        int numBatterys = newBatterys.size();
        
        LinkedList<SimulatorObject> chargers = pCurrent.getObjects(ObjectStateBatteryCharger.NAME);
        int numChargers = chargers.size();
        
        SimulatorObject oldBattery, newBattery, charger;
        ObjectStateBatteryCharger batteryCharger;
        ObjectStateBattery newRobotBattery, oldRobotBattery;
        Vector2D chargerPosition, robotPosition;
                
        for(int iBattery=0;iBattery <numBatterys;iBattery++)
        {
            oldBattery = oldBatterys.get(iBattery);
            oldRobotBattery = (ObjectStateBattery) oldBattery.getState(ObjectStateBattery.NAME);
            robotPosition = oldBattery.getParent().getPosition();


            newBattery = newBatterys.get(iBattery);
            newRobotBattery = (ObjectStateBattery) newBattery.getState(ObjectStateBattery.NAME);
            
            newRobotBattery.setFullyCharged(false);
            
            int currentCharge = oldRobotBattery.getCurrentCharge();
            int newCharge = currentCharge;
            int capacity = oldRobotBattery.getChargeCapacity();
            boolean deplete = true;
            
            for(int jCharger=0;jCharger<numChargers;jCharger++)
            {

             charger = chargers.get(jCharger);
             batteryCharger = (ObjectStateBatteryCharger) charger.getState(ObjectStateBatteryCharger.NAME);
             chargerPosition = charger.getPosition();

                double dist = robotPosition.distance(chargerPosition);


              //  System.out.println("level = "+ currentCharge);
            
                if (dist < batteryCharger.getRange()) 
                {
               // System.out.println("charging");
                    int chargerRate = batteryCharger.getChargeRate();
                    int batteryChargeRate = newRobotBattery.getMaxChargeRate();
                    int rate;
                
                    if(chargerRate > batteryChargeRate)
                        rate = batteryChargeRate; //damage battery??
                    else
                        rate = chargerRate;
                
                    newCharge += rate;
                
                    if (newCharge >= capacity) {
                        newCharge = capacity;
                        newRobotBattery.setFullyCharged(true);
                    }
                } else //battery is depleted 
                {
                    deplete = deplete && true;
                
                }
            }
            if(deplete){                           
                    newCharge -= oldRobotBattery.getIdleDepletionRate();
                    if (newCharge < 0) 
                        newCharge = 0; //BATTERY is Dead   
            }
            
            newRobotBattery.setCurrentCharge(newCharge);

        }
    }
}
