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
    /** The reward scalar contained in this drop */
    public double reward;

    /** Returns the size, in ints, of this drop
     *
     * @return Size, in ints, of this drop.
     */
    public int getSize() {
        return (Double.SIZE) / 8;
    }

    /** Writes this drop to an output stream
     *
     * @param pOut The output stream that this drop should be written to.
     *
     * @throws java.io.IOException
     */
    public void writeData(InterfaceOutputStream pOut) throws IOException {
        pOut.writeDouble(reward);
    }

    /**
     * Reads in a drop from a given input stream.
     *
     * @param pIn The input stream from which we want to read the drop.
     *
     * @throws java.io.IOException
     */
    public void readData(InterfaceInputStream pIn) throws IOException {
        reward = pIn.readDouble();
    }

}
