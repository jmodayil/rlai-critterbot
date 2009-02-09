/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.rlcommunity.critter;

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


        try {
            SimulatorObject oldBattery = pCurrent.getObjects(ObjectStateBattery.NAME).getFirst();
            ObjectStateBattery oldRobotBattery = (ObjectStateBattery) oldBattery.getState(ObjectStateBattery.NAME);
            Vector2D robotPosition = oldBattery.getParent().getPosition();


            SimulatorObject newBattery = pNext.getObjects(ObjectStateBattery.NAME).getFirst();
            ObjectStateBattery newRobotBattery = (ObjectStateBattery) newBattery.getState(ObjectStateBattery.NAME);

            SimulatorObject charger = pCurrent.getObjects(ObjectStateBatteryCharger.NAME).getFirst();
            ObjectStateBatteryCharger batteryCharger = (ObjectStateBatteryCharger) charger.getState(ObjectStateBatteryCharger.NAME);
            Vector2D chargerPosition = charger.getPosition();

            newRobotBattery.setFullyCharged(false);

            double dist = robotPosition.distance(chargerPosition);
            double currentCharge = oldRobotBattery.getCurrentCharge();
            double newCharge = currentCharge;
            double capacity = oldRobotBattery.getChargeCapacity();

            if (dist < batteryCharger.getRange()) {
                //System.out.println("charging");
                double chargerRate = batteryCharger.getChargeRate();
                double batteryChargeRate = newRobotBattery.getMaxChargeRate();
                double rate;
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
                newCharge -= oldRobotBattery.getIdleDepletionRate();
                if (newCharge < 0) {
                    newCharge = 0; //BATTERY is Dead
                }
            }

            newRobotBattery.setCurrentCharge(newCharge);
        } catch (NoSuchElementException e) {
            //no charger and/or battery
        }

    }
}
