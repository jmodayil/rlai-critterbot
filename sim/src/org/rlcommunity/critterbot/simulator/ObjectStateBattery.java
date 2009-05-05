/* Copyright 2009 Adam White
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

package org.rlcommunity.critterbot.simulator;

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
        private boolean aIsBeingCharged = false;

        public static final String NAME = SimulatorComponentBattery.NAME;        
    
    public ObjectStateBattery(int pChargeCapacity, int pMaxChargeRate, int pIdleDepletionRate){
        aChargeCapacity = pChargeCapacity;
        aMaxChargeRate = pMaxChargeRate;
        aIdleDepletionRate = pIdleDepletionRate;
        aCurrentCharge = pChargeCapacity;
        aIsBeingCharged = false;
        aFullyCharged = false;
        
    }

    private ObjectStateBattery(ObjectStateBattery battery2Copy){
        this.aFullyCharged = battery2Copy.aFullyCharged;
        this.aCurrentCharge = battery2Copy.aCurrentCharge;
        this.aChargeCapacity = battery2Copy.aChargeCapacity;   
        this.aMaxChargeRate = battery2Copy.aMaxChargeRate;
        this.aIdleDepletionRate = battery2Copy.aIdleDepletionRate;
        this.aIsBeingCharged = battery2Copy.aIsBeingCharged;
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

     public boolean isBeingCharged(){
        return aIsBeingCharged;
    }  
    
    public void setCurrentCharge(int pCurrentCharge){
        aCurrentCharge = pCurrentCharge;
    }

    public void setFullyCharged(boolean pFullyCharged){
        aFullyCharged = pFullyCharged;
    }
    
    public void setIsCharging(boolean pIsCharging)
    {
        aIsBeingCharged = pIsCharging;
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
