package org.rl.community.critter;

/**
  * SimulatorState
  *
  * This class defines what the objective state of the simulator is, 
  *  including where objects are located, etc.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;

public class SimulatorState
{
  // List of objects in this state
  protected LinkedList<SimulatorObject> aObjList;

  public SimulatorState()
  {
    aObjList = new LinkedList<SimulatorObject>();
  }

  public void addObject(SimulatorObject pObj)
  {
    aObjList.add(pObj);
  }

  private SimulatorObject getObject(int pId)
  {
    for (SimulatorObject o : aObjList)
    {
      if (o.getId() == pId)
        return o;
    }

    return null;
  }

  /** Returns a list of agents
    * Will be made obsolete in future versions, as all objects will be agents
    *
    */
  public LinkedList<SimulatorAgent> getAgents()
  {
    LinkedList<SimulatorAgent> list = new LinkedList<SimulatorAgent>();

    for (SimulatorObject obj : aObjList)
    {
      // Yuck, instanceof
      if (obj instanceof SimulatorAgent)
        list.add((SimulatorAgent)obj);
    }

    return list;
  }

  /** Returns a list of all objects current in the simulator.
    *
    * @return A list of all existing objects
    */
  public LinkedList<SimulatorObject> getObjects()
  {
    return aObjList;
  }

  /** Returns all objects which are affected by the given component.
    * 
    * @param pComponent The identifier of the component of interest
    * @return A list of objects o such that o.getState(pComponent) != null
    */
  public LinkedList<SimulatorObject> getObjects(String pComponent)
  {
    LinkedList<SimulatorObject> objs = new LinkedList<SimulatorObject>();

    for (SimulatorObject o : aObjList)
    {
      if (o.getState(pComponent) != null)
        objs.add(o);
    }

    return objs;
  }

  public Object clone()
  {
    SimulatorState newState = new SimulatorState();
    
    // Add a copy of each object to this new state
    for (SimulatorObject obj : aObjList)
    {
      newState.addObject((SimulatorObject)obj.clone());
    }

    return newState;
  }

    SimulatorObject getObject(SimulatorObject obj) {
       return this.getObject(obj.getId());
    }
}

