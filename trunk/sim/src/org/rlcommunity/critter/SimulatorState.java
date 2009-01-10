package org.rlcommunity.critter;

/**
  * SimulatorState
  *
  * This class defines what the objective state of the simulator is, 
  *  including where objects are located, etc.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;
import java.util.List;

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
    // @todo this needs to be optimized, most likely by having getObjects()
    //  return the same list if the object structure is not modified
    List<SimulatorObject> allObjects = getObjects();

    for (SimulatorObject o : allObjects)
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

  /** Returns a list of all objects current in the simulator, including
    *  sub-objects (e.g. sensors or parts).
    *
    * @return A list of all existing objects
    */
  public LinkedList<SimulatorObject> getObjects()
  {
    LinkedList<SimulatorObject> objs = new LinkedList<SimulatorObject>();

    // To preserve the postorder traversal, we'll add the head objects
    //  after
    for (SimulatorObject o : aObjList)
    {
      List<SimulatorObject> itsChildren = o.getChildren();
      objs.addAll(itsChildren);
    }

    objs.addAll(aObjList);

    // @todo there is, of course, optimization to be done here; in particular,
    //  we shouldn't be re-computing the full object list  everytime
    return objs;
  }

  /** Returns all objects which are affected by the given component.
    * 
    * @param pComponent The identifier of the component of interest
    * @return A list of objects o such that o.getState(pComponent) != null
    */
  public LinkedList<SimulatorObject> getObjects(String pComponent)
  {
    LinkedList<SimulatorObject> objs = new LinkedList<SimulatorObject>();

    // First, in postorder traversal, add all of the children that contain
    //  the required ObjectState
    for (SimulatorObject o : aObjList)
    {
      List<SimulatorObject> itsChildren = o.getChildren(pComponent);

      objs.addAll(itsChildren);
    }

    // Now add the top-level objects that also contain that state
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
  
  /** This method clears this SimulatorState so that it can be filled with
    *  data from a new time step. It does not remove objects from the list,
    *  but will remove any transient attributes and/or reset them to their
    *  default values.
    *
    */
  public void clear()
  {
    // Simply clearTransient the attributes for all objects
    for (SimulatorObject o : aObjList)
    {
      o.clearTransient();
    }
  }
}

