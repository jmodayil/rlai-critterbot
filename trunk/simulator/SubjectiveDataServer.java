/**
  * SubjectiveDataServer
  *
  * This class handles the communication between the simulator and the
  *  DisCo wrapper corresponding to a particular agent. It receives 
  *  robot commands from the wrapper and sends back subjective information,
  *  such as sensor readings.
  *
  * @author Marc G. Bellemare
  */
public class SubjectiveDataServer
{
  protected SimulatorAgent aAgent;

  /**
    * Constructs a new SubjectiveDataServer which listens on the given
    *  port and handles a particular agent.
    *
    * @param pAgent The agent that is to be controlled.
    * @param pPort The port to listen to.
    */
  public SubjectiveDataServer(SimulatorAgent pAgent, int pPort)
  {
    aAgent = pAgent;
  }

  /**
    * Sends the subjective data available to the server's agent to the DisCo
    *  wrapper. The SimulatorAgent object should contain the latest sensor
    *  readings and other bits of information.
    */
  public void sendUpdate()
  {
  }

  /**
    * Receive data from the DisCo wrapper and pass it on to the agent.
    *  This is most likely going to be robot commands.
    */
  public void receiveData()
  {
    // @@@ set the agent's action if new data is received
  }
}
