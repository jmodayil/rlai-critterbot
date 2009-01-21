package org.rlcommunity.critter.Clients;

/**
  * DiscoInterfaceClientHandler
  *
  * This class handles client handling for the Disco Interface TCP/IP server.
  */

import org.rlcommunity.critter.*;
import org.rlcommunity.critter.Drops.*;

import java.net.Socket;

import java.util.LinkedList;

import java.io.IOException;


public class DiscoInterfaceClientHandler extends Thread
{
  public final int MAX_CLASSNAME_LENGTH = 1024;

  protected Socket aClient;

  /** List of queue'd elements waiting to be parsed by our server */
  protected final LinkedList<SimulatorDrop> aInQueue;

  protected InterfaceInputStream aIn;
  protected InterfaceOutputStream aOut;

  protected boolean aClosed = false;

  /** Creates a new client handler corresponding to the given Socket */
  public DiscoInterfaceClientHandler(Socket pClient) {
    aClient = pClient;
    try
    {
      aIn = new InterfaceInputStream(aClient.getInputStream());
      aOut = new InterfaceOutputStream(aClient.getOutputStream());
    }
    catch (IOException e)
    {
      System.err.println ("Failed to create input/output streams."); 
    }

    aInQueue = new LinkedList<SimulatorDrop>();
  }

  /** Main code for this Thread */
    @Override
  public void run()
  {
    while (!aClosed)
    {
      // Block and wait for new data
      // Read in a new drop (first its class name)
      try 
      {
        int nameLength = aIn.readInt();

        // If we don't test for this, we can kill the heap 
        if (nameLength > MAX_CLASSNAME_LENGTH)
        {
          throw new RuntimeException ("Garbage data");
        }

        String className = aIn.readString(nameLength);

        // Create a new Drop
        try
        {
          SimulatorDrop newDrop = DropFactory.create(className);
          // Read in the drop!
          newDrop.readData(aIn);
        
          // Add the drop to the queue
          synchronized(aInQueue)
          {
            aInQueue.add(newDrop);
          }
        }
        catch (ClassNotFoundException e)
        {
          System.err.println ("Invalid drop name: "+className);
          // @todo the data from then on is going to be garbage
        }
        // Catch other ugly exceptions here, and throw them as Runtime
        catch (Exception e)
        {
          throw new RuntimeException(e);
        }
      }
      catch (IOException e)
      {
        System.err.println ("IOException in DiscoInterfaceClientHandler.run - aborting.");
        System.err.println ("Type: "+e.toString());
        // End this thread and the associated socket asap; we use close()
        //  so that our owner (the server) knows to delete us
        this.close(); 
      }
    }

    // We are closed, end the socket
    try
    {
      aClient.close();
    }
    // Really, there is nothing to do if we fail to close the socket
    catch (Exception ee)
    {
    }
  }

  /** Send out a drop */ 
  public void send(SimulatorDrop pData)
  {
    synchronized(aOut) 
    {
      // Write the Drop's data to the output stream
      try
      {
        String className = pData.getClass().getSimpleName();
        aOut.writeString(className);
    
        // Get the drop to write itself to the output stream
        pData.writeData(aOut);
      }
      catch (IOException e)
      {
        System.err.println ("IOException in DiscoInterfaceClientHandler.send :: quitting");
        throw new RuntimeException(e);
      }
    }
  }

  public SimulatorDrop receive()
  {
    // Pop a drop if there is one
    synchronized (aInQueue)
    {
      if (aInQueue.isEmpty()) return null;
      else return aInQueue.remove();
    }
  }

  /** 'Lazy' close of the socket. This will cause the socket to be closed 
    *  by the client thread.
    */
  private synchronized void close()
  {
    aClosed = true;
  }

  public boolean isClosed() { return aClosed; }
}
