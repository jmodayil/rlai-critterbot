/**
  * ComponentState
  *
  * Defines a SimulatorObject's state for a particular component layer.
  *  For example, the PhysicsState contains velocities, mass, forces, etc.
  *  This class should be extended once for each component layer.
  *  While the SimulatorComponent provides the means of updating the data,
  *  the ComponentState is a property of each object which requires it and
  *  contains the data. The SimulatorComponent is to the ComponentState as
  *  the SimulatorEngine is the to SimulatorObject. Somewhat.
  *
  *
  * @author Marc G. Bellemare
  */

public abstract class ComponentState
{
  protected String aName;

  protected void ComponentState(String pName)
  {
    aName = pName;
  }

  public String getName()
  {
    return aName;
  }
}
