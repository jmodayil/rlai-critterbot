package org.rlcommunity.critter.Drops;

/**
  * DropInterface
  *
  * This class defines a generic interface which handles drops from
  *  and to various sources (clients).
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;
import java.util.List;

import org.rlcommunity.critter.Clients.*;

public class DropInterface
{
  protected LinkedList<DropClient> aClients;
  
  public DropInterface()
  {
    aClients = new LinkedList<DropClient>();
  }

  /**
    * Send a given Drop out to all DropClients, which may be Disco interfaces,
    *  keyboard clients, RL agents, etc.
    */
  public void sendDrop(SimulatorDrop pDrop)
  {
    // Simply call each client handler's send method
    for (DropClient ch : aClients)
      ch.send(pDrop);
  }

  /**
    * Returns a (possibly empty) list of drops that were received from all
    *  connected clients since the last call to receiveDrops.
    */
  public List<SimulatorDrop> receiveDrops()
  {
    LinkedList<SimulatorDrop> drops = new LinkedList<SimulatorDrop>();
    // 1. Collect  drops into list from client handlers
    // 2. Clear the client handlers' lists

    for (DropClient ch : aClients)
    {
      // Add all drops returned by the client to our list
      List<SimulatorDrop> clientDrops = ch.receive();

      for (SimulatorDrop drop : clientDrops)
        drops.add(drop);
    }

    return drops;
  }

  public void addClient(DropClient newClient){
      aClients.add(newClient);
  }
}

