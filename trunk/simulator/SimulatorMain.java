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

    
    
    while (true)
    {
      engine.step();
      List<SimulatorDrop> drops = robotServ.receiveData();

      for (SimulatorDrop drop : drops)
      {
        CritterControlDrop command = (CritterControlDrop)drop;
        engine.setCommand(command);
      }
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
