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

package org.rlcommunity.critterbot.javadrops.clients;

import java.util.Arrays;
import java.util.List;

import org.rlcommunity.critterbot.javadrops.drops.CritterControlDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterRewardDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterStateDrop;
import org.rlcommunity.critterbot.javadrops.drops.DropInterface;
import org.rlcommunity.critterbot.javadrops.drops.SimulatorDrop;

/**
 * This class defines a sample reward generator connected to the Simulator and
 *   the Agent via Disco.
 *
 * It receives CritterStateDrop and outputs CritterRewardDrop's.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class DiscoRewardGenerator {
    /** If debugPrintActions = true, the reward generated will be printed out. */
    protected static final boolean debugPrintActions      = true;

    /** The next reward to be sent out */
    protected double aReward;
    
    /** Whether the reward generator has un-processed state data */
    protected boolean aHasNewState;
    
    public DiscoRewardGenerator() {
    }

    /** Method that returns a RewardDrop encapsulating the next reward.
     *
     * @return A drop containing reward information.
     */
    public SimulatorDrop act() {
        // Only send a new drop if we have received a new observation
        if (aHasNewState) {
            aHasNewState = false;

            // Create a new RewardDrop, set its reward value and return it
            CritterRewardDrop rewardDrop = new CritterRewardDrop();
            rewardDrop.reward = aReward;

            if (debugPrintActions)
                System.out.println ("R: "+rewardDrop.reward);

            return rewardDrop;
        }
        else
            return null;

    }

    /** Receive a drop from the server.
     * 
     * @param pDrop The received drop.
     */
    public void observeDrop(SimulatorDrop pDrop) {
        // Determine what to do with the drop based on the drop type
        if (pDrop instanceof CritterStateDrop) {
            parseStateDrop((CritterStateDrop)pDrop);
        }
        else if (pDrop instanceof CritterRewardDrop) {
            // @todo Most likely this is our own reward drop
        }
        else if (pDrop instanceof CritterControlDrop) {
            // This could be use to add motor costs to the reward function
        }
    }

    /** Method to parse the state drop. Here, we generate the next reward based
     *    on the drop data.
     *
     * @param pDrop The received CritterStateDrop.
     */
    public void parseStateDrop(CritterStateDrop pDrop) {
        assert (pDrop.light.length > 0);

        // Give a reward based on the light intensity, with no reward if too
        //  far away, and up to 1.0
        aReward = Math.max(0.0, Math.min((pDrop.light[0] - 100) / 100, 1.0));

        // Notify the RewardDrop generating method to make a new reward drop
        aHasNewState = true;
    }








    /** A main method for running the reward generator. Copied from DiscoAgent.java. */

    public static final int discoRewardGeneratorPort = 2326;
    
    public static void main(String[] args) {
        // Create a TCP server to talk with Disco
        DiscoInterfaceServer discoServer = new DiscoInterfaceServer(discoRewardGeneratorPort);
        // Start its thread
        discoServer.start();

        // Create the central drop interface
        DropInterface dropInterface = new DropInterface();

        // Add the disco server to the central drop interface
        dropInterface.addClient(discoServer);

        // Create a new reward generator
        DiscoRewardGenerator generator = new DiscoRewardGenerator();
        
        while (true) {
            // Receive drops from the simulator
            List<SimulatorDrop> drops = dropInterface.receiveDrops();

            // Pass them on (this may be an empty list)
            for (SimulatorDrop d : drops) {
                generator.observeDrop(d);
            }

            // Request a new reward drop from the generator, which may be null
            SimulatorDrop rewardDrop = generator.act();
            if (rewardDrop != null) {
                dropInterface.sendDrop(rewardDrop);
            }
            
            // Sleep for a little while to give the processor a break
            try {
                Thread.sleep(10);
            } catch (InterruptedException ex) {
            }
        }
    }
}
