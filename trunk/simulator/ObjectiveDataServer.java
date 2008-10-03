/**
  * ObjectiveDataServer
  *
  * This class takes care of communicating the 'objective' view of the 
  * world to the DisCo component. It should accept a TCP/IP connection
  * and send the world state at regular intervals.
  *
  * @author Marc G. Bellemare
  */

public class ObjectiveDataServer
{
  /** Constructs a new ObjectiveDataServer that listens on the given port.
    *
    * @param pPort Port on which the server should listen.
    */
  public ObjectiveDataServer(int pPort)
  {
  }

  /** Sends an update to the server's clients informing them of the state 
    * of the world.
    * 
    * @param pState The current simulator state.
    */
  public void sendUpdate(SimulatorState pState)
  {
  }
}

