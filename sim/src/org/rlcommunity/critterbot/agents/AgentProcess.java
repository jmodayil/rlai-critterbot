package org.rlcommunity.critterbot.agents;

/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */
import java.util.Arrays;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.rlcommunity.critterbot.javadrops.clients.DropClient;
import org.rlcommunity.critterbot.javadrops.drops.CritterControlDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterStateDrop;
import org.rlcommunity.critterbot.javadrops.drops.DropInterface;
import org.rlcommunity.critterbot.javadrops.drops.SimulatorDrop;

/**
 * This class defines a sample agent connected to the simulator via Disco.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class AgentProcess implements Runnable {

  /** If debugPrintObservations = true, the observation vector used by the
   *   agent will be printed out. */
  protected static final boolean debugPrintObservations = false;
  /** If debugPrintActions = true, the action returned by the agent will
   *   be printed out. */
  protected static final boolean debugPrintActions = false;
  protected DropInterface aDropInterface;
  /** Define how many observations this agent is interested in */
  protected static final int numObservations =
      CritterStateDrop.BUMP_SIZE +
      CritterStateDrop.IR_DISTANCE_SIZE +
      CritterStateDrop.LIGHT_SIZE;
  /** The last observation received by the agent, as a real-valued vector */
  protected double[] aObservation;
  /** Whether the agent has an un-processed observation */
  protected boolean aHasNewObservation;
  /** For this example, possible actions will be -1, and 1
   *  corresponding to drive backward and drive forward
   */
  protected int action = 1;

  /**
   * Create a new Agent.
   */
  public AgentProcess(DropClient pClient) {
    aDropInterface = new DropInterface();
    aDropInterface.addClient(pClient);
    
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


      /*****************************************************
       * AGENT CODE GOES HERE
       *****************************************************/

      /* We create a very simple agent that changes direction
       * every time it bumps something
       */
      // Set the motor mode (if using WHEEL_SPACE, the variables to
      //  be set should be m*_vel, not x,y,theta_vel)
      actionDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;

      // Set the cartesean velocities (XYTHETA_SPACE)
      actionDrop.x_vel = 100 * action;
      actionDrop.y_vel = 0;
      actionDrop.theta_vel = 0;

      /*****************************************************
       * END AGENT CODE
       *****************************************************/

      // For completeness, set the LED mode, although it has no effect
      actionDrop.led_mode = CritterControlDrop.LedMode.NONE;

      if (debugPrintActions) {
        System.out.println(actionDrop.toString());
      }

      // Return the created action
      return actionDrop;
    }
    else {
      return null;
    }

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
      parseStateDrop((CritterStateDrop) pDrop);
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
    //  care about the bump, light, and distance sensors)
    aObservation = new double[numObservations];

    // Fill in the values
    int idx = 0;
    boolean hasBump = false;

    for (int i = 0; i < CritterStateDrop.BUMP_SIZE; i++) {
      aObservation[idx++] = pDrop.bump[i];
      if (pDrop.bump[i] != 0) {
        hasBump = true;
      }
    }
    //for (int i = 0; i < CritterStateDrop.LIGHT_SIZE; i++)
    //    aObservation[idx++] = pDrop.light[i];
    for (int i = 0; i < CritterStateDrop.IR_DISTANCE_SIZE; i++) {
      aObservation[idx++] = pDrop.ir_distance[i];
    }

    if (debugPrintObservations) {
      System.out.println("Observation: " + Arrays.toString(aObservation));
    }

    aHasNewObservation = true;

    if (hasBump) {
      action = -action;
    }
  }

  public void run() {
    while (true) {
      List<SimulatorDrop> received = aDropInterface.receiveDrops();

      // Receive drops
      for (SimulatorDrop drop : received) {
        observeDrop(drop);
      }

      // Send our control drop
      aDropInterface.sendDrop(act());
      
      try {
        Thread.sleep(10);
      } catch (InterruptedException ex) {
        Logger.getLogger(AgentProcess.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
  }
}
