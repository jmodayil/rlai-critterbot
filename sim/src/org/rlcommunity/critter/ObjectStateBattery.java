/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter;

/**
 *
 * @author awhite
 */
public class ObjectStateBattery implements ObjectState {


        
        private boolean aFullyCharged = false;
        private double aCurrentCharge = 0.0;
        private double aChargeCapacity = 0.0;

        public static final String NAME = SimulatorComponentBattery.NAME;        
    
    public ObjectStateBattery(double pFullCapacity){
        aChargeCapacity = pFullCapacity;
        
    }

    private ObjectStateBattery(ObjectStateBattery battery2Copy){
        this.aFullyCharged = battery2Copy.aFullyCharged;
        this.aCurrentCharge = battery2Copy.aCurrentCharge;
        this.aChargeCapacity = battery2Copy.aChargeCapacity;        
    }
    
    public double getCurrentCharge(){
        return aCurrentCharge;
    }

    public double getChargeCapacity(){
        return aChargeCapacity;
    }
   
    public boolean isCharged(){
        return aFullyCharged;
    }
   
    
    public void setCurrentCharge(double pCurrentCharge){
        aCurrentCharge = pCurrentCharge;
    }

    public void setFullyCharged(boolean pFullyCharged){
        aFullyCharged = pFullyCharged;
    }
   
    public Object clone()
    {
        return new ObjectStateBattery(this);
    }
    
    public String getName() {
        return NAME;
    }

    public void draw(SimulatorGraphics g, SimulatorObject parent) {
        //nothing to draw
    }

    public void clearTransient() {
        aFullyCharged = false;
        aCurrentCharge = 0.0;
    }

}
