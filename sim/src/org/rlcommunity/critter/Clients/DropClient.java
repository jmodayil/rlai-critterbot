/*
 * Copyright 2008 Brian Tanner
 * http://bt-recordbook.googlecode.com/
 * brian@tannerpages.com
 * http://brian.tannerpages.com
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package org.rlcommunity.critter.Clients;

import org.rlcommunity.critter.*;
import java.util.List;

/**
 *
 * @author Brian Tanner, Marc G. Bellemare
 */
public interface DropClient 
{
    /** Returns a list of all new drops that were produced by this client
      *  since the last call to receive().
      *
      * @return A list of unprocessed drops 
      */
    public List<SimulatorDrop> receive();

    /** Passes a single drop to the client, e.g. a CritterStateDrop.
      *
      * @param pData The drop to be passed to the client
      */
    public void send(SimulatorDrop pData);
}
