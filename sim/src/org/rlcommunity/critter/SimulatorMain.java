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

    /** Arguments parsed from the command line */
    static private int discoServerPort = 2324;
    static private boolean useGui = true;
    static private boolean useKeyboard = true;
    static private boolean doPrintHelp = false;
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
        int idx = 0;
        while (idx < args.length) {
            String flag = args[idx];
            idx++;

            if (flag.equals("-p")) {
                discoServerPort = Integer.parseInt(args[idx]);
                idx++;
            }
            else if (flag.equals("-ng")) {
                useGui = false;
                useKeyboard = false;
            }
            else if (flag.equals("-nk")) {
                useKeyboard = false;
            }
            else if (flag.equals("-s")) {
                timeScale = Double.parseDouble(args[idx]);
                idx++;
            }
            else if (flag.equals("-h") || flag.equals("--help")) {
                doPrintHelp = true;
            }
        }
    }

    private static void runGUI(final SimulatorEngine engine, final KeyboardClient keyboardClient) {
        javax.swing.SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                new SimulatorViz(engine, keyboardClient, engine.vizHandler);
            }
        });
    }

    public static void printHelp() {
        System.out.println ("Usage: ");
        System.out.println ("java ...SimulatorMain [options]");
        System.out.println ("\n");
        System.out.println ("Options are:");
        System.out.println ("  -p [port]          Set the Disco server port, default=2324");
        System.out.println ("  -ng                Disable the GUI");
        System.out.println ("  -nk                Disable the keyboard robot controller");
        System.out.println ("  -s [scale]         Set the simulator time scale, default=1.0");
    }

    public static void main(String[] args) {
        parseArgs(args);

        if (doPrintHelp) {
            printHelp();
            return;
        }
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
