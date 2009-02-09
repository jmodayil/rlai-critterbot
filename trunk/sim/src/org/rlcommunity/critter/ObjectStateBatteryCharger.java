/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter;

import java.awt.Color;

/**
 *
 * @author awhite
 */
public class ObjectStateBatteryCharger implements ObjectState {

      public static final String NAME = SimulatorComponentBattery.NAME + "charger";
      private double aRange = 0.0; //in Meters
      private int aChargeRate = 0; //in Ampere
      
      public ObjectStateBatteryCharger(double pRange, int pChargeRate)
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
    public int getChargeRate()
    {
        return aChargeRate;
    }
    

    public void draw(SimulatorGraphics g, SimulatorObject parent) {
        Vector2D pos = parent.getPosition();
        g.setColor(Color.cyan);
      g.drawOval((double)pos.x, (double)pos.y, aRange*.75, aRange*.75);
      g.drawString("Charger", (double)pos.x, (double)pos.y);
        

    }

    public void clearTransient() {
    }

      
}
