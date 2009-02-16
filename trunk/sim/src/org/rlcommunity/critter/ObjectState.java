/* Copyright 2009 Marc G. Bellemare
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

package org.rlcommunity.critter;

/**
  * ObjectState
  *
  * Defines a generic object state (in general tied to a particular component).
  *  This allows the actual state of the object to be factorized into 
  *  the relevant parts only, and also allows new state information to be
  *  added without having to restructure everything or even touch the existing
  *  parts of the object state.
  *
  *  @author Marc G. Bellemare
  */

import java.awt.Graphics;

public interface ObjectState
{
  /** Returns a unique identifier for this type of state. */
  public String getName();

  public Object clone();


  /** (Potentially) draw something about the state; may be null. This
    *  most likely should be moved out of here when we have time.
    *
    * @param g The canvas to draw on
    * @param parent The owner of this state
    */
  public void draw(SimulatorGraphics g, SimulatorObject parent);

  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    */
  public void clearTransient();
}
