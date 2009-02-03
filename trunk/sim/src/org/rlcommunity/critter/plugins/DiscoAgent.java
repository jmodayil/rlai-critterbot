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
    /** If debugPrintObservations = true, the observation vector used by the
     *   agent will be printed out. */
    protected static final boolean debugPrintObservations = true;
    /** If debugPrintActions = true, the action returned by the agent will
     *   be printed out. */
    protected static final boolean debugPrintActions      = true;

    protected static final int numObservations =
            CritterStateDrop.BUMP_SIZE +
            CritterStateDrop.IR_DISTANCE_SIZE +
            CritterStateDrop.LIGHT_SIZE;
    
    /** The last observation received by the agent, as a real-valued vector */
    protected double[] aObservation;

    /** Whether the agent has an un-processed observation */
    protected boolean aHasNewObservation;

    protected double aReward;
    
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
            actionDrop.led_mode = CritterControlDrop.LedMode.THING1;
            
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

        // @todo this might be Java 1.6
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
