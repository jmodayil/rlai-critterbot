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
  * ObjectStateLightSource
  *
  * State component for a light-emitting object
  */

import java.awt.Color;
import java.awt.Graphics;

public class ObjectStateLightSource implements ObjectState
{
  public static final String NAME = SimulatorComponentLight.NAME + "source";

  protected int aIntensity; //in Candela

  public ObjectStateLightSource()
  {
    aIntensity = 0;
  }

  public int getIntensity() { return aIntensity; }
  public void setIntensity(int pInt) { aIntensity = pInt; }

  /** ObjectState interface */
  
  /** Returns a unique identifier for this type of state. */
  public String getName() { return NAME; }

  public Object clone()
  {
    ObjectStateLightSource newSource = new ObjectStateLightSource();
    newSource.copyFrom(this);

    return newSource;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateLightSource src = (ObjectStateLightSource)os;
  
    this.aIntensity = src.aIntensity;
  }



  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(SimulatorGraphics g, SimulatorObject parent)
  {
        Vector2D pos = parent.getPosition();
        g.setColor(Color.YELLOW);
        double radius = 0.15;

        g.drawOval(pos.x-radius/2, pos.y-radius/2, radius, radius);
        g.drawOval(pos.x+0.02-radius/2, pos.y+0.02-radius/2, 0.1, 0.1);
  }

  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient()
  {
  }
}
