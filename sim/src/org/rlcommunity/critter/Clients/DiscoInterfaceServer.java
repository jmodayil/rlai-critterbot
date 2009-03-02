/* Copyright 2009 Marc G. Bellemare
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

package org.rlcommunity.critter.Clients;

/**
  * DiscoInterfaceServer
  *
  * This class handles the communication between the simulator and the
  *  DisCo wrapper corresponding to a particular agent. It receives 
  *  robot commands from the wrapper and sends back subjective information,
  *  such as sensor readings.
  *
  *  A new DiscoInterfaceServer should first be created, then started using
  *   Thread.start().
  *
  * @author Marc G. Bellemare
  */
import java.net.ServerSocket;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;

import java.io.IOException;

import org.rlcommunity.critter.*;
import org.rlcommunity.critter.Drops.*;
/**
 * Drop server listens for connections of sockets (say from Disco)
 * Added by Brian
 */

public class DiscoInterfaceServer extends Thread implements DropClient 
{
  protected ServerSocket aSocket;

  protected LinkedList<DiscoInterfaceClientHandler> aClients;

  /** The maximum number of drops in each client queue */
  protected int aMaxQueuedDrops;

  public static final int defaultMaxQueuedDrops = 20;

  /**
    * Constructs a new Server which listens on the given port, sending
    *  and receiving drops via TCP/IP.
    *
    * @param pPort The port to listen to.
    */
  public DiscoInterfaceServer(int pPort) {
    this(pPort, defaultMaxQueuedDrops);
  }

  /**
    * Constructs a new Server which listens on the given port, sending
    *  and receiving drops via TCP/IP.
    *
    * @param pPort The port to listen to.
    */
  public DiscoInterfaceServer(int pPort, int pMaxQueuedDrops) {
    try
    {
      aSocket = new ServerSocket(pPort);
    }
    catch (IOException e)
    {
      System.err.println ("Failed to create DropServer socket, "+
        e.getMessage());
    }
    
    aClients = new LinkedList<DiscoInterfaceClientHandler>();

    aMaxQueuedDrops = pMaxQueuedDrops;
  }

  /**
    * Send a given Drop out to all connected clients.
    */
  public void send(SimulatorDrop pDrop)
  {
    cleanupClients();

    // Simply call each client handler's send method
    for (DiscoInterfaceClientHandler ch : aClients)
      ch.send(pDrop);
  }

  /**
    * Returns a (possibly empty) list of drops that were received from all
    *  connected clients since the last call to receiveDrops.
    */
  public List<SimulatorDrop> receive()
  {
    cleanupClients();

    LinkedList<SimulatorDrop> drops = new LinkedList<SimulatorDrop>();
    // 1. Collect  drops into list from client handlers
    // 2. Clear the client handlers' lists

    for (DiscoInterfaceClientHandler ch : aClients)
    {
      SimulatorDrop drop;
      drop = ch.receive();

      while (drop != null)
      {
        drops.add(drop);
        drop = ch.receive();
      }
    }

    return drops;
  }

  /** Method to get rid of sockets that have been closed */
  public synchronized void cleanupClients()
  {
    LinkedList<DiscoInterfaceClientHandler> deaders = new
      LinkedList<DiscoInterfaceClientHandler>();

    // Find which clients are closed 
    for (DiscoInterfaceClientHandler ch : aClients)
    {
      if (ch.isClosed())
        deaders.add(ch);
    }

    // Remove them from the list
    for (DiscoInterfaceClientHandler ch : deaders)
      aClients.remove(ch);
  }

  /** From the Thread class */
    @Override
  public void run()
  {
    while (true)
    {
      try
      {
        cleanupClients();

        // Listen for a new connection (blocks here)
        Socket clientSocket = aSocket.accept();
        System.out.println ("New client!");
        DiscoInterfaceClientHandler ch = 
          new DiscoInterfaceClientHandler(clientSocket, aMaxQueuedDrops);
        ch.start();
        aClients.add(ch);
      }
      catch (IOException e)
      {
        throw new RuntimeException(e.getMessage());
      }
    }
  }
}
