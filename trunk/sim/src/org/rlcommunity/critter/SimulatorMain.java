package org.rlcommunity.critter;

/**
 * SimulatorMain
 *
 * Defines the main function for running the simulator.
 * Most likely to be unorganized until we figure out a proper structure for
 *  everything.
 *
 * @author Marc G. Bellemare
 */
import org.rlcommunity.critter.Clients.*;
import org.rlcommunity.critter.Drops.*;
import org.rlcommunity.critter.environments.OriginalEnvironment;
import org.rlcommunity.critter.environments.RobotOnlyEnvironment;

public class SimulatorMain {

    public static void main(String[] args) {
        int discoServerPort;
        boolean useGui;
        int millisPerStep = 10;

        // Read in some arguments
        if (args.length < 1) {
            discoServerPort = 2324;
        } else {
            discoServerPort = Integer.parseInt(args[0]);
        }

        if (args.length >= 2 && args[1].equals("-ng")) {
            useGui = false;
        } else {
            useGui = true;
        }

        final KeyboardClient theKeyboardClient = new KeyboardClient();

        System.out.println("Starting Disco server on port " + discoServerPort);
        // Create a drop server to send and receive robot (subjective) data
        DiscoInterfaceServer discoServer = new DiscoInterfaceServer(discoServerPort);
        discoServer.start();

        // Create the central drop interface
        DropInterface dropInterface = new DropInterface();

        System.out.println("Using GUI: " + useGui);

        dropInterface.addClient(discoServer);
        if (useGui) {
            dropInterface.addClient(theKeyboardClient);
        }

        System.out.println("Creating simulator engine...");
        final SimulatorEngine engine = new SimulatorEngine(new OriginalEnvironment());
        engine.addComponent(new SimulatorComponentDynamics());
        engine.addComponent(new SimulatorComponentLight());
        engine.addComponent(new SimulatorComponentBump());
        engine.addComponent(new SimulatorComponentOmnidrive());
        engine.addComponent(new SimulatorComponentAccelerometer());
        engine.addComponent(new SimulatorComponentGyroscope());
        engine.addComponent(
                new SimulatorComponentCritterbotInterface(dropInterface));
        engine.addComponent(new SimulatorComponentIRDistance());

        if (useGui) {
            javax.swing.SwingUtilities.invokeLater(new Runnable() {

                public void run() {
                    new SimulatorViz(engine, theKeyboardClient, engine.vizHandler);
                }
            });
        }


        while (true) {
            engine.step(millisPerStep);

            try {
                Thread.sleep(millisPerStep);
            } catch (InterruptedException e) {
            }
        }
    }
}
