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

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;

import org.rlcommunity.critterbot.javadrops.drops.CritterControlDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterRewardDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterStateDrop;
import org.rlcommunity.critterbot.javadrops.drops.DropInterface;
import org.rlcommunity.critterbot.javadrops.drops.SimulatorDrop;

/**
 * This class defines a sample agent connected to the simulator via Disco.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class DiscoAgent {
    /** If debugPrintObservations = true, the observation vector used by the
     *   agent will be printed out. */
    protected static final boolean debugPrintObservations = true;
    /** If debugPrintActions = true, the action returned by the agent will
     *   be printed out. */
    protected static final boolean debugPrintActions      = false;

    protected static final int numObservations =
            CritterStateDrop.BUMP_SIZE +
            CritterStateDrop.IR_DISTANCE_SIZE +
            CritterStateDrop.LIGHT_SIZE;
	private static final int dropQueueSize = 32;
    
    /** The last observation received by the agent, as a real-valued vector */
    protected double[] aObservation;

    /** Whether the agent has an un-processed observation */
    protected boolean aHasNewObservation;

    protected double aReward;
	private DropInterface dropInterface;
    
	/** TCP information for connection to a disco server **/
	private static final int portNum = 2330;
	private static final String hostName = "localhost";
	
    /**
     * Create a new DiscoAgent.
     */
    public DiscoAgent() {
        aObservation = new double[numObservations];
    }

    /** Method that determines which action the agent should do next and
     *    encapsulate it into an appropriate drop.
     *
     * @return A drop containing control information.
     */
    public SimulatorDrop act() {
        // Only send a new drop if we have received a new observation
        if (aHasNewObservation) {
            aHasNewObservation = false;

            // Create a new ControlDrop to return
            CritterControlDrop actionDrop = new CritterControlDrop();

            // Set the action to be some fixed velocities
            // First, the motor mode (if using WHEEL_SPACE, the variables to
            //  be set should be m*_vel, not x,y,theta_vel
            actionDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;

            // Set a forward velocity of 20 and a rotational velocity of 10
            //  (these should be thought of as unitless quantities)
            actionDrop.x_vel = 20;
            actionDrop.y_vel = 0;
            actionDrop.theta_vel = 10;

            // For completeness, set the LED mode, although it has no effect
            actionDrop.led_mode = CritterControlDrop.LedMode.NONE;
            
            if (debugPrintActions) {
                System.out.println(actionDrop.toString());
            }

            // Return the created action
            return actionDrop;
        }
        else
            return null;

    }

    /** Receive a drop from the server.
     *  We parse the drop depending on its type (it could be a reward or state
     *   information/an observation).
     * 
     * @param pDrop The received drop.
     */
    public void observeDrop(SimulatorDrop pDrop) {
        // Determine what to do with the drop based on the drop type
        if (pDrop instanceof CritterStateDrop) {
            parseStateDrop((CritterStateDrop)pDrop);
        }
        else if (pDrop instanceof CritterRewardDrop) {
            parseRewardDrop((CritterRewardDrop)pDrop);
        }
        // An unfortunate side effect of the current Disco set up is that we
        //  receive our own ControlDrop's
        else if (pDrop instanceof CritterControlDrop) {

        }
    }

    /** Method to parse the state drop. Here, we construct an observation vector
     *   out of the drop.
     *
     * @param pDrop The received CritterStateDrop.
     */
    public void parseStateDrop(CritterStateDrop pDrop) {
        // Construct some real-valued vector from the state drop (here, we only
        //  care about the light sensors)
        aObservation = new double[numObservations];

        // Fill in the values
        int idx = 0;
        for (int i = 0; i < pDrop.bump.length; i++)
            aObservation[idx++] = pDrop.bump[i];
        for (int i = 0; i < pDrop.light.length; i++)
            aObservation[idx++] = pDrop.light[i];
        for (int i = 0; i < pDrop.ir_distance.length; i++)
            aObservation[idx++] = pDrop.ir_distance[i];

        if (debugPrintObservations)
            System.out.println ("Observation: "+Arrays.toString(aObservation));

        aHasNewObservation = true;
    }

    /** Parses a reward drop.
     *
     * @param pDrop A reward drop received from some reward source.
     */
    public void parseRewardDrop(CritterRewardDrop pDrop) {
        System.out.println ("Reward: "+pDrop.reward);
        aReward = pDrop.reward;
    }
    
    /** A main method for running the agent */
    
    public static void main(String[] args) {
        // Create a TCP server to talk with Disco
        //DiscoInterfaceServer discoServer = new DiscoInterfaceServer(discoAgentPort);
        // Start its thread
        DiscoInterfaceClient discoClient = null;
		try {
			discoClient = new DiscoInterfaceClient(
					InetAddress.getByName(hostName), portNum, 32);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			System.exit(0);
		}
    	discoClient.start();

        // Create the central drop interface
        DropInterface dropInterface = new DropInterface();

        // Add the disco server to the central drop interface
        dropInterface.addClient(discoClient);

        // Create a new agent
        CopyOfDiscoAgent agent = new CopyOfDiscoAgent();
        
        while (true) {
            // Receive drops from the simulator
            List<SimulatorDrop> drops = dropInterface.receiveDrops();

            // Pass them on to the agent (this may be an empty list)
            for (SimulatorDrop d : drops) {
                agent.observeDrop(d);
            }

            // Request an action from the agent (this may be null)
            SimulatorDrop actionDrop = agent.act();
            if (actionDrop != null) {
                dropInterface.sendDrop(actionDrop);
            }
            
            // Sleep for a little while to give the processor a break
            try {
                Thread.sleep(10);
            } catch (InterruptedException ex) {
            }
        }
    }
}
