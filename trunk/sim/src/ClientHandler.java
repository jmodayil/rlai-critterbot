package org.rl.community.critter;

/**
  * ClientHandler
  *
  * This class handles client handling for generic DropServer's.
  */

import java.net.Socket;

import java.util.LinkedList;

import java.io.IOException;

import java.nio.ByteBuffer;

public class ClientHandler extends Thread
{
  public final int MAX_CLASSNAME_LENGTH = 1024;

  protected Socket aClient;
 
  /** List of queue'd elements waiting to be parsed by our server */
  protected LinkedList<SimulatorDrop> aInQueue;

  protected InterfaceInputStream aIn;
  protected InterfaceOutputStream aOut;

  /** Creates a new client handler corresponding to the given Socket */
  public ClientHandler(Socket pClient)
  {
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
  public void run()
  {
    // @@@ cleanup
    /*while (true)
    {
      try
      {
        int b = aIn.readFixedInt();
        System.out.print(b+" ");
        if (b == -125) break;
      }
      catch (Exception e)
      {
        return;
      }
    } */

    while (true)
    {
      // Block and wait for new data
      // Read in a new drop (first its class name)
      try 
      {
        int nameLength = aIn.readInt();
        System.out.println ("I've got some data... "+nameLength+" length");

        // If we don't test for this, we can kill the heap 
        if (nameLength > MAX_CLASSNAME_LENGTH)
        {
          throw new RuntimeException ("Garbage data");
        }

        String className = aIn.readString(nameLength); 

        System.err.println ("A class is "+className);
        // Create a new Drop
        try
        {
          Class<?> dropType  = Class.forName(className);
          // Create a new instance and cross your fingers - it has better be
          //  a subclass of SimulatorDrop
          SimulatorDrop newDrop = (SimulatorDrop) dropType.newInstance();
          // Read in the drop!
          newDrop.readData(aIn);
        
          System.err.println ("New drop: "+newDrop.toString());
       
          // Add the drop to the queue
          synchronized(aInQueue)
          {
            // @@@ Add...
            aInQueue.add(newDrop);
          }
        }
        catch (ClassNotFoundException e)
        {
          System.err.println ("Invalid drop name: "+className);
          // @@@ the data from then on is going to be garbage
        }
        // Catch other ugly exceptions here, and throw them as Runtime
        catch (Exception e)
        {
          throw new RuntimeException(e);
        }
      }
      catch (IOException e)
      {
        System.err.println ("IOException in ClientHandler.run - aborting.");
        // Close socket and run away
        try { aClient.close(); }
        catch (Exception ee) {}
        return;
      }
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
        String className = pData.getClass().getName();
        aOut.writeString(className);
    
        // Get the drop to write itself to the output stream
        pData.writeData(aOut);
      }
      catch (IOException e)
      {
        System.err.println ("IOException in ClientHandler.send");
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
}
