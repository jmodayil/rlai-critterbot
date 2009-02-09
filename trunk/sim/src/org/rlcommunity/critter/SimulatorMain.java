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
import java.util.Random;
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
    static private String dumpFilePath = null;

    private static SimulatorEngine createSimulatorEngine(DropInterface dropInterface, 
            EnvironmentDescription environmentDescription) {
        System.out.println("Creating simulator engine...");

        Random randomizer = new Random();
        
        final SimulatorEngine engine = new SimulatorEngine(environmentDescription, randomizer);
        engine.addComponent(new SimulatorComponentDynamics(randomizer));
        engine.addComponent(new SimulatorComponentLight(randomizer));
        engine.addComponent(new SimulatorComponentBumpSensor(randomizer));
        engine.addComponent(new SimulatorComponentOmnidrive(randomizer));
        engine.addComponent(new SimulatorComponentAccelerometer(randomizer));
        engine.addComponent(new SimulatorComponentGyroscope(randomizer));
        engine.addComponent(new SimulatorComponentCritterbotInterface(dropInterface));
        engine.addComponent(new SimulatorComponentIRDistance(randomizer));
        engine.addComponent(new SimulatorComponentBattery());
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
        if (dumpFilePath != null)
            dropInterface.addClient(new DumpClient(dumpFilePath));
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
            else if (flag.startsWith("-d")) {
                dumpFilePath = args[idx];
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
        System.out.println ("  -d [dumpfile]      Data are dumped in [dumpfile]");
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
