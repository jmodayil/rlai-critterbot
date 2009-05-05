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

package org.rlcommunity.critterbot.simulator.environments;

import org.rlcommunity.critterbot.simulator.*;

import java.util.List;

/**
 * A class that encapsulates simulator environment information. Will be replaced
 *  or enhanced by/with a SVG loader at a later stage.
 * 
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public interface EnvironmentDescription {
    /**
     * Generates a list of all the objects that are part of this environment,
     *  with their properties set. As a general rule of thumb, the simulator 
     *  should be able to take the returned list and use it (without cloning)
     *  as its SimulatorState's list of objects.
     * 
     * @return A list of objects that comprised in this environment.
     */
    public List<SimulatorObject> generateObjects();

    /** Returns whether the environment uses SVG drawing. If false, the
     *   environment is assumed to be drawn using PNG's and polygons.
     *
     * @return Whether the environment uses SVG drawing.
     */
    public boolean usesSVG();
}
