package org.rlcommunity.critterbot.agents;

import java.util.LinkedList;
import java.util.List;
import org.rlcommunity.critterbot.javadrops.clients.DropClient;
import org.rlcommunity.critterbot.javadrops.drops.SimulatorDrop;

/** This defines a DropClient which encapsulates a DropClient within it. 
  *  The drops are buffered for asynchronous usage.
  *  @author Marc G. Bellemare
  */
public class DropClientAsynchronousWrapper {
    private static int idx = 0;
  public class WrapperClient implements DropClient {
    protected final List<SimulatorDrop> aSendQueue;
    protected final List<SimulatorDrop> aReceiveQueue;

    int label;

    public WrapperClient(List<SimulatorDrop> pSendQueue, List<SimulatorDrop> pRecvQueue) {
      aSendQueue = pSendQueue;
      aReceiveQueue = pRecvQueue;

      label = idx;
      idx++;
    }
    
    public List<SimulatorDrop> receive() {
      LinkedList<SimulatorDrop> returnList = new LinkedList<SimulatorDrop>();

      // Flush the receive buffer
      synchronized(aReceiveQueue) {
        for (SimulatorDrop drop : aReceiveQueue) {
          returnList.add(drop);
        }

        aReceiveQueue.clear();
      }

      return returnList;
    }

    public void send(SimulatorDrop pData) {
      // Add to our send queue
      synchronized(aSendQueue) {
        aSendQueue.add(pData);
      }
    }
  }

  protected WrapperClient aFirstClient;
  protected WrapperClient aSecondClient;

  public DropClientAsynchronousWrapper() {
    LinkedList<SimulatorDrop> buffer1 = new LinkedList<SimulatorDrop>();
    LinkedList<SimulatorDrop> buffer2 = new LinkedList<SimulatorDrop>();

    // Create the two drop clients so that they feed into eachother
    aFirstClient = new WrapperClient(buffer1, buffer2);
    aSecondClient = new WrapperClient(buffer2, buffer1);
  }

  public WrapperClient getFirstClient() { return aFirstClient; }
  public WrapperClient getSecondClient() { return aSecondClient; }
}
