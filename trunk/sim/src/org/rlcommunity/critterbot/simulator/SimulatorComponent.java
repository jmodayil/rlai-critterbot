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

package org.rlcommunity.critterbot.simulator;

/**
  * SimulatorComponent
  *
  * Defines a generic SimulatorEngine component.
  *
  * @author Marc G. Bellemare
  */


public interface SimulatorComponent
{
  /** Apply this component to the current state, storing the results in
    *  the next state. Note that the next state may (and is likely to) contain 
    *  information computed by other components, so components should play
    *  nice and only modify the variables that they correspond to.
    *
    *  @param pCurrent The current state (should not be modified)
    *  @param pNext    The next state, in which the results of applying the
    *       this component should be stored.
    *  @param delta    The amount of time (in ms) elapsed between 
    *       the current and next states.
    */
  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta);
}
