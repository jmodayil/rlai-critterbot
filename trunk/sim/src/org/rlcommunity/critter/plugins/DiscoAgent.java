/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critter.plugins;

import java.util.Arrays;
import java.util.List;
import org.rlcommunity.critter.Clients.DiscoInterfaceServer;
import org.rlcommunity.critter.Drops.CritterControlDrop;
import org.rlcommunity.critter.Drops.CritterRewardDrop;
import org.rlcommunity.critter.Drops.CritterStateDrop;
import org.rlcommunity.critter.Drops.DropInterface;
import org.rlcommunity.critter.Drops.SimulatorDrop;

/**
 * This class defines a sample agent connected to the simulator via Disco.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class DiscoAgent {

    /** The last observation received by the agent, as a real-valued vector */
    protected double[] aObservation;

    /** Whether the agent has an un-processed observation */
    protected boolean aHasNewObservation;
    
    public DiscoAgent() {
    }

    /** Method that determines what action the agent should do next and 
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
            actionDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;
            actionDrop.x_vel = 20;
            actionDrop.y_vel = 0;
            actionDrop.theta_vel = 10;

            return actionDrop;
        }
        else
            return null;

    }

    /** Receive a drop from the server, assumed to be an observation.
     *   In our case we parse the drop depending on its type (it could be a reward
     *   or state information/an observation).
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
    }

    /** Method to parse the state drop. Here, we construct an observation vector
     *   out of the drop.
     *
     * @param pDrop The received CritterStateDrop.
     */
    public void parseStateDrop(CritterStateDrop pDrop) {
        // Construct some real-valued vector from the state drop (here, we only
        //  care about the light sensors)
        aObservation = new double[pDrop.light.length];

        // Fill in the values
        for (int i = 0; i < pDrop.light.length; i++)
            aObservation[i] = pDrop.light[i];

        // @todo this might be Java 1.6
        System.out.println ("Observation: "+Arrays.toString(aObservation));
    }

    public void parseRewardDrop(CritterRewardDrop pDrop) {
        
    }







    /** A main method for running the agent */

    public static final int discoAgentPort = 2325;
    
    public static void main(String[] args) {
        // Create a TCP server to talk with Disco
        DiscoInterfaceServer discoServer = new DiscoInterfaceServer(discoAgentPort);
        // Start its thread
        discoServer.start();

        // Create the central drop interface
        DropInterface dropInterface = new DropInterface();

        // Add the disco server to the central drop interface
        dropInterface.addClient(discoServer);

        // Create a new agent
        DiscoAgent agent = new DiscoAgent();
        
        while (true) {
            // Receive drops from the simulator
            List<SimulatorDrop> drops = dropInterface.receiveDrops();

            // Pass them on to the agent (this may be an empty list)
            for (SimulatorDrop d : drops) {
                agent.observeDrop(d);
            }

            // Request an action from the agent (this may be null)
            SimulatorDrop actionDrop = agent.act();
            if (actionDrop != null)
                dropInterface.sendDrop(actionDrop);

            // Sleep for a little while to give the processor a break
            try {
                Thread.sleep(10);
            } catch (InterruptedException ex) {
            }
        }
    }
}
