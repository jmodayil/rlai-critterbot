/**
  * SimulatorMain
  *
  * Defines the main function for running the simulator.
  * Most likely to be unorganized until we figure out a proper structure for
  *  everything.
  *
  * @author Marc G. Bellemare
  */

import java.util.List;

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

    System.out.println ("Creating simulator engine...");
    final SimulatorEngine engine = new SimulatorEngine();

    // Get the first agent from the engine
    //SimulatorAgent agent = engine.getAgentList().getFirst();

    System.out.println ("Starting servers on ports "+objPort+" and "+subjPort);
    // Create a drop server to send and receive robot (subjective) data
    DropServer robotServ = new DropServer(subjPort);
    robotServ.start();

    javax.swing.SwingUtilities.invokeLater(new Runnable() {
        public void run() {
        	new SimulatorViz(engine);
        }
    });

    
   
    int stateThrottle = 0;

    while (true)
    {
      engine.step();
      List<SimulatorDrop> drops = robotServ.receiveDrops();

      // Receive some actions (@@@ needs to be written properly)
      for (SimulatorDrop drop : drops)
      {
        CritterControlDrop command = (CritterControlDrop)drop;
        engine.setCommand(command);
      }

      // Send the system state
      // @@@ call makeDropFromState or such
      CritterStateDrop stateDrop = new CritterStateDrop();
      List<SimulatorAgent> agentList = engine.getAgentList();
      SimulatorAgent varun = null;
      if (agentList.size() > 0)
        varun = agentList.get(0);

      // @@@ NOTE TO SELF: need to synchronize with the agent list
      if (varun != null)
      {
        // Wrong (this isn't acceleration!), but this is only for debugging
        //  purposes
        stateDrop.accel.x = (int)(varun.aVel.x*100);
        stateDrop.accel.y = (int)(varun.aVel.y*100);

        if (++stateThrottle >= 100)
        {
          robotServ.sendDrop(stateDrop);
          stateThrottle = 0;
        }
      }
      else throw new RuntimeException("No agent!");

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
