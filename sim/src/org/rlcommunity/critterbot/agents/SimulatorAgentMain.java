package org.rlcommunity.critterbot.agents;

import org.rlcommunity.critterbot.javadrops.clients.DiscoInterfaceServer;
import org.rlcommunity.critterbot.javadrops.clients.DropClient;

public class SimulatorAgentMain {

  public static final boolean aUseSimulator = true;
  public static final int aDiscoPort = 2325;
  
  public static void main(String[] args) {
    // Determine whether to receive agent drops from Disco or
    DropClient dropSource;

    if (aUseSimulator) {
      // Create the simulator
      SimulatorProcess simProcess = new SimulatorProcess();

      // Create a wrapper to asynchronously communicate between the simulator
      //  and the agent
      DropClientAsynchronousWrapper wrapper = new DropClientAsynchronousWrapper();
      // The agent gets one client
      dropSource = wrapper.getFirstClient();

      // The simulator gets the other
      simProcess.getDropInterface().addClient(wrapper.getSecondClient());

      // Start the simulator
      new Thread(simProcess).start();
    }
    else
      dropSource = new DiscoInterfaceServer(aDiscoPort);

    // Create and start the agent
    AgentProcess agentProcess = new AgentProcess(dropSource);

    // Start the agent
    new Thread(agentProcess).start();

    while (true) {
    }
  }
}
