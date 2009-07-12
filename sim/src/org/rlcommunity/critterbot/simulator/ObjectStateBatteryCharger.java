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
        double radius = aRange;

      g.drawOval((double)pos.x-radius, (double)pos.y-radius, radius*2, radius*2);
      g.drawString("Charger", (double)pos.x-radius, (double)pos.y-radius);
        

    }

    public void clearTransient() {
    }

      
}
