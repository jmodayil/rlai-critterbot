/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter;

/**
 *
 * @author awhite
 */
public class ObjectStateBatteryCharger implements ObjectState {

      public static final String NAME = SimulatorComponentBattery.NAME + "charger";
      private double aRange = 0.0;
      private double aChargeRate = 0.0;
      
      public ObjectStateBatteryCharger(double pRange, double pChargeRate)
      {
          aRange = pRange;
          aChargeRate = pChargeRate;        
      }
      

    private ObjectStateBatteryCharger(ObjectStateBatteryCharger battery2Copy){
        this.aRange = battery2Copy.aRange;
        this.aChargeRate = battery2Copy.aChargeRate;
      
    }
     
    public Object clone()
    {
        return new ObjectStateBatteryCharger(this);
    }
    
    public String getName() {
        return NAME;
    }

    public double getRange()
    {
        return aRange;
    }
    public double getChargeRate()
    {
        return aChargeRate;
    }
    

    public void draw(SimulatorGraphics g, SimulatorObject parent) {
    }

    public void clearTransient() {
    }

      
}
