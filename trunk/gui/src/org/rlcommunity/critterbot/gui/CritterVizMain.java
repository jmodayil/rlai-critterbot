/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critterbot.gui;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.List;

import org.rlcommunity.critterbot.javadrops.clients.DiscoInterfaceClient;
import org.rlcommunity.critterbot.javadrops.drops.CritterControlDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterRewardDrop;
import org.rlcommunity.critterbot.javadrops.drops.CritterStateDrop;
import org.rlcommunity.critterbot.javadrops.drops.DropInterface;
import org.rlcommunity.critterbot.javadrops.drops.SimulatorDrop;
/**
 *
 * @author critterbot
 */
public class CritterVizMain {
    public static final int critterVizPort = 2330;
    public static final String critterVizHost = "localhost";
    public static final int dropQueueSize = 32;
    
    public CritterStateDrop currentState;
    
    private CritterViz gui;
    
    public CritterVizMain() {
        
        currentState = new CritterStateDrop();
        // Create a TCP server to talk with Disco
        InetAddress netAddr = null;
        try {
        	netAddr = InetAddress.getByName(critterVizHost);
        }
        catch (UnknownHostException e) {
        	System.err.println("Unknown hostname.");
        	System.exit(1);
        }
        DiscoInterfaceClient discoClient = new DiscoInterfaceClient(
        		netAddr, critterVizPort, dropQueueSize);
        // Start its thread
        discoClient.start();

        // Create the central drop interface
        DropInterface dropInterface = new DropInterface();

        // Add the disco server to the central drop interface
        dropInterface.addClient(discoClient);

        // Create a new agent
        //CritterVizMain me = new CritterVizMain();
        runGUI(currentState,dropInterface);
        //me.runGUI(me.currentState,dropInterface);
        
        while (true) {
            // Receive drops from the simulator
            List<SimulatorDrop> drops = dropInterface.receiveDrops();

            // Pass them on to the agent (this may be an empty list)
            for (SimulatorDrop d : drops) {
                observeDrop(d);
            }
          
            // Sleep for a little while to give the processor a break
            try {
                Thread.sleep(10);
            } catch (InterruptedException ex) {
            }
        }
    }
    
    private void runGUI(final CritterStateDrop state, final DropInterface di) {
        javax.swing.SwingUtilities.invokeLater( 
                new Runnable() {
                    public void run() {
                        gui = new CritterViz(di);
                        gui.setTitle("Critterbot GUI");
                        gui.setVisible(true);
                    }
                }
        );
        /*javax.swing.SwingUtilities.invokeLater( 
                new Runnable() {
                    public void run() {
                        new CritterVizLogTag(di);
                    }
                }
        );*/
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
            final CritterStateDrop stateDrop = (CritterStateDrop) pDrop;
            javax.swing.SwingUtilities.invokeLater(
                    new Runnable() {
                        public void run() {
                            gui.updateDisplay(stateDrop);
                        }
            } );
        }
        else if (pDrop instanceof CritterRewardDrop) {
        }
        // An unfortunate side effect of the current Disco set up is that we
        //  receive our own ControlDrop's
        else if (pDrop instanceof CritterControlDrop) {
        }
    }
    
    public static void main(String[] args) {
        new CritterVizMain();
    }
}
