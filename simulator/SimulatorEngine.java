/**
  * SimulatorEngine
  *
  * Defines the core of the simulator engine. This class should call the
  *  simulator components in turn, keep track of existing objects and agents, 
  *  etc.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;

public class SimulatorEngine
{
  protected LinkedList<SimulatorAgent> aAgentList;
  protected SimulatorState aState;

  public SimulatorEngine()
  {
    aAgentList = new LinkedList<SimulatorAgent>();
    aState = new SimulatorState();

    // Construct the simulator state by adding objects to it
  }

  /** Returns a list of existing agents */
  public LinkedList<SimulatorAgent> getAgentList() { return aAgentList; }

  /**
    * Returns the current state of the simulator. 
    */
  public SimulatorState getState()
  {
    return aState;
  }
  
  /** Takes one 'step' in the simulation: update positions, velocities, etc
    */
  public void step()
  {
  }
}
