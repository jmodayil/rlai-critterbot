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
import org.rlcommunity.critter.environments.EnvironmentDescription;
import org.rlcommunity.critter.environments.FunEnvironment;

public class SimulatorMain {

    static private int discoServerPort = 2324;
    static private boolean useGui = true;
    static private boolean useKeyboard = true;
    static private double timeScale = 1.0;

    private static SimulatorEngine createSimulatorEngine(DropInterface dropInterface, 
            EnvironmentDescription environmentDescription) {
        System.out.println("Creating simulator engine...");
        final SimulatorEngine engine = new SimulatorEngine(environmentDescription);
        engine.addComponent(new SimulatorComponentDynamics());
        engine.addComponent(new SimulatorComponentLight());
        engine.addComponent(new SimulatorComponentBumpSensor());
        engine.addComponent(new SimulatorComponentOmnidrive());
        engine.addComponent(new SimulatorComponentAccelerometer());
        engine.addComponent(new SimulatorComponentGyroscope());
        engine.addComponent(new SimulatorComponentCritterbotInterface(dropInterface));
        engine.addComponent(new SimulatorComponentIRDistance());
        return engine;
    }

    static private DiscoInterfaceServer launchDisco() {
        System.out.println("Starting Disco server on port " + discoServerPort);
        // Create a drop server to send and receive robot (subjective) data
        DiscoInterfaceServer discoServer = new DiscoInterfaceServer(discoServerPort);
        discoServer.start();
        return discoServer;
    }

    static private KeyboardClient createKeyboardClient(DropInterface dropInterface) {
        KeyboardClient theKeyboardClient = new KeyboardClient();
        if (useKeyboard) {
            dropInterface.addClient(theKeyboardClient);
        }
        return theKeyboardClient;
    }

    static private DropInterface createCentralDropInterface(DiscoInterfaceServer discoServer) {
        DropInterface dropInterface = new DropInterface();
        dropInterface.addClient(discoServer);
        return dropInterface;
    }

    private static void mainLoop(final SimulatorEngine engine, int millisPerStep) {
        SimulatorScheduler scheduler = new SimulatorScheduler(engine, millisPerStep, timeScale);
        scheduler.run();
    }

    private static void parseArgs(String[] args) {
        if (args.length >= 1) {
            discoServerPort = Integer.parseInt(args[0]);
        }
        if (args.length >= 2) {
            if (args[1].equals("-ng")) {
                useGui = false;
                useKeyboard = false;
            }
            else if (args[1].equals("-nk")) {
                useKeyboard = false;
            }

        }
        System.out.println("Using GUI: " + useGui);
        if (args.length >= 3) {
            timeScale = Double.parseDouble(args[2]);
        }
    }

    private static void runGUI(final SimulatorEngine engine, final KeyboardClient keyboardClient) {
        javax.swing.SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                new SimulatorViz(engine, keyboardClient, engine.vizHandler);
            }
        });
    }

    public static void main(String[] args) {
        parseArgs(args);

        DiscoInterfaceServer discoServer = launchDisco();
        DropInterface dropInterface = createCentralDropInterface(discoServer);
        final KeyboardClient keyboardClient = createKeyboardClient(dropInterface);

        int millisPerStep = 10;
        final SimulatorEngine engine = createSimulatorEngine(dropInterface,
                new FunEnvironment());

        if (useGui) {
            runGUI(engine, keyboardClient);
        }
        mainLoop(engine, millisPerStep);
    }
}
