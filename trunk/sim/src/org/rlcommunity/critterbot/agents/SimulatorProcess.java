/* Copyright 2009 Thomas Degris
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

package org.rlcommunity.critterbot.agents;

/**
 * SimulatorMain
 *
 * Defines the main function for running the simulator.
 * Most likely to be unorganized until we figure out a proper structure for
 *  everything.
 *
 * @author Marc G. Bellemare
 */
import org.rlcommunity.critterbot.simulator.*;
import java.util.Random;

import org.rlcommunity.critterbot.javadrops.clients.DumpClient;
import org.rlcommunity.critterbot.javadrops.drops.*;
import org.rlcommunity.critterbot.simulator.environments.EnvironmentDescription;
import org.rlcommunity.critterbot.simulator.environments.FunEnvironment;
import org.rlcommunity.critterbot.simulator.visualizer.CritterbotDataVisualizerClient;

public class SimulatorProcess implements Runnable {

    /** Arguments parsed from the command line */
    static private boolean useGui = true;
    static private boolean useVisualizer = true;
    static private double timeScale = 1.0;
    static private String dumpFilePath = null;

    private int millisPerStep = 10;

    protected SimulatorEngine aEngine;
    protected DropInterface aDropInterface;

    public SimulatorProcess() {
        aDropInterface = createCentralDropInterface();

        aEngine = createSimulatorEngine(aDropInterface,
                new FunEnvironment());
    }

    public DropInterface getDropInterface() { return aDropInterface; }

    private SimulatorEngine createSimulatorEngine(DropInterface dropInterface, 
            EnvironmentDescription environmentDescription) {
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

    private CritterbotDataVisualizerClient createVisualizer(DropInterface dropInterface) {
        CritterbotDataVisualizerClient client = new CritterbotDataVisualizerClient();
        dropInterface.addClient(client);
        return client;
    }

    private DropInterface createCentralDropInterface() {
        DropInterface dropInterface = new DropInterface();
        if (dumpFilePath != null)
            dropInterface.addClient(new DumpClient(dumpFilePath));

        return dropInterface;
    }

    private void mainLoop(final SimulatorEngine engine, int millisPerStep) {
        SimulatorScheduler scheduler = new SimulatorScheduler(engine, millisPerStep, timeScale);
        scheduler.run();
    }

    private void runGUI(final SimulatorEngine engine) {
        javax.swing.SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                new SimulatorViz(engine, null, null);
            }
        });
    }

    public void run() {
        if (useVisualizer) {
          createVisualizer(aDropInterface);
        }
        if (useGui) {
            runGUI(aEngine);
        }
        mainLoop(aEngine, millisPerStep);
    }
}
