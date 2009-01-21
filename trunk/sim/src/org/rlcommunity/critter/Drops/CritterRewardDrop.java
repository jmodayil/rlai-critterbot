/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critter.Drops;

import java.io.IOException;
import org.rlcommunity.critter.InterfaceInputStream;
import org.rlcommunity.critter.InterfaceOutputStream;

/**
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class CritterRewardDrop implements SimulatorDrop {

    public int getSize() {
        return 1;
    }

    public void writeData(InterfaceOutputStream pOut) throws IOException {
    }

    public void readData(InterfaceInputStream pIn) throws IOException {
    }

}
