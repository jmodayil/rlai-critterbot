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
import org.rlcommunity.critter.environments.EmptyEnvironment;
import org.rlcommunity.critter.environments.OriginalEnvironment;
import org.rlcommunity.critter.environments.RobotOnlyEnvironment;

public class SimulatorMain
{
  public static void main(String[] args)
  {
    int objPort, subjPort;

    // Read in some arguments
    if (args.length < 1)
      objPort = 2323;
    else
      objPort = Integer.parseInt(args[0]);
    
    if (args.length < 2)
      subjPort = 2324;
    else
      subjPort = Integer.parseInt(args[1]);
    
    final KeyboardClient theKeyboardClient=new KeyboardClient();

    System.out.println ("Starting Disco server on port "+subjPort);
    // Create a drop server to send and receive robot (subjective) data
    DiscoInterfaceServer discoServer = new DiscoInterfaceServer(subjPort);
    discoServer.start();

    // Create the central drop interface
    DropInterface dropInterface = new DropInterface();

    dropInterface.addClient(discoServer);
    dropInterface.addClient(theKeyboardClient);

    System.out.println ("Creating simulator engine...");
    final SimulatorEngine engine = new SimulatorEngine(new RobotOnlyEnvironment());
    engine.addComponent(new SimulatorComponentDynamics());
    engine.addComponent(new SimulatorComponentLight());
    engine.addComponent(new SimulatorComponentBump());
    engine.addComponent(new SimulatorComponentOmnidrive());
    engine.addComponent(new SimulatorComponentAccelerometer());
    engine.addComponent(new SimulatorComponentGyroscope());
    engine.addComponent(
      new SimulatorComponentCritterbotInterface(dropInterface));
    engine.addComponent(new SimulatorComponentIRDistance());

    javax.swing.SwingUtilities.invokeLater(new Runnable() {
        public void run() {
        	new SimulatorViz(engine,theKeyboardClient,engine.vizHandler);
        }
    });

    
    while (true)
    {
      engine.step();

      try {
		Thread.sleep(9);
      } catch (InterruptedException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
      }
      //objServer.sendUpdate(engine.getState());
      //subjServer.sendUpdate();
      //subjServer.receiveData();
    }
  }
}
