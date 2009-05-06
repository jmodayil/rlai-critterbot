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

package org.rlcommunity.critterbot.javadrops.drops;

import java.io.IOException;

import org.rlcommunity.critterbot.javadrops.InterfaceInputStream;
import org.rlcommunity.critterbot.javadrops.InterfaceOutputStream;

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
    public void readData(InterfaceInputStream pIn,int pDropSize) throws IOException {
        assert (pDropSize == getSize());
        reward = pIn.readDouble();
    }

}
