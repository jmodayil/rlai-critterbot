/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critter.environments;

import java.util.LinkedList;
import java.util.List;
import org.rlcommunity.critter.SimulatorObject;

/**
 * Describes a completely empty environment.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class EmptyEnvironment implements EnvironmentDescription {

    public List<SimulatorObject> generateObjects() {
        return new LinkedList<SimulatorObject>();
    }

}
