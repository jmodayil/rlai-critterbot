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
        private int aCurrentCharge = 0; //in Coulomb
        private int aChargeCapacity = 0; //in Coulomb
        private int aMaxChargeRate = 0; //in Ampere
        private int aIdleDepletionRate = 0; //in Ampere

        public static final String NAME = SimulatorComponentBattery.NAME;        
    
    public ObjectStateBattery(int pChargeCapacity, int pMaxChargeRate, int pIdleDepletionRate){
        aChargeCapacity = pChargeCapacity;
        aMaxChargeRate = pMaxChargeRate;
        aIdleDepletionRate = pIdleDepletionRate;
        aCurrentCharge = pChargeCapacity;
        
    }

    private ObjectStateBattery(ObjectStateBattery battery2Copy){
        this.aFullyCharged = battery2Copy.aFullyCharged;
        this.aCurrentCharge = battery2Copy.aCurrentCharge;
        this.aChargeCapacity = battery2Copy.aChargeCapacity;   
        this.aMaxChargeRate = battery2Copy.aMaxChargeRate;
        this.aIdleDepletionRate = battery2Copy.aIdleDepletionRate;
    }
    
    public int getCurrentCharge(){
        return aCurrentCharge;
    }

    public int getMaxChargeRate(){
        return aMaxChargeRate;
    }    

    public int getIdleDepletionRate(){
        return aIdleDepletionRate;
    }
    public int getChargeCapacity(){
        return aChargeCapacity;
    }
   
    public boolean isCharged(){
        return aFullyCharged;
    }
   
    
    public void setCurrentCharge(int pCurrentCharge){
        aCurrentCharge = pCurrentCharge;
    }

    public void setFullyCharged(boolean pFullyCharged){
        aFullyCharged = pFullyCharged;
    }
    public boolean isBatteryDepleted()
    {
        return (aCurrentCharge <= 0 );
    }
     public void requestCurrent(double pCurrent)
     {
         aCurrentCharge -= pCurrent;
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
        //aCurrentCharge = 0.0;
    }

}
