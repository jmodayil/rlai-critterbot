package org.rlcommunity.critter;

/**
  * SimulatorEngine
  *
  * Defines the core of the simulator engine. This class should call the
  *  simulator components in turn, keep track of existing objects and agents, 
  *  etc.
  *
  * @author Marc G. Bellemare
  */

import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

public class SimulatorEngine
{
  protected SimulatorState aState, aNextState;
  protected LinkedList<SimulatorComponent> aComponents;

  private long last_time;
  
  protected SimulatorVizEvents vizHandler;

  public SimulatorEngine()
  {
    aState = new SimulatorState();
    aComponents = new LinkedList<SimulatorComponent>();

    vizHandler = new SimulatorVizEvents();

    // Construct the simulator state by adding objects to it
    // MGB: I moved all of this code to debugCreateStuff because it was
    //  getting fairly bulky
    debugCreateStuff();

    // Clone the current state; we will use it to do state-state transitions
    aNextState = (SimulatorState)aState.clone();
  }

  /** Returns a list of existing agents */
  public List<SimulatorAgent> getAgentList() { 
    return Collections.unmodifiableList(aState.getAgents()); 
  }

  /** Returns a list of existing objects */
  public List<SimulatorObject> getObjectList() { 
    return Collections.unmodifiableList(aState.getObjects()); 
  }

  /** Returns a list of objects influenced by the given component
    *
    * @param pComponent The identifier of the component of interest
    * @return A list of objects as per SimulatorState.getObjects(pComponent)
    */
  public List<SimulatorObject> getObjects(String pComponent)
  {
    return aState.getObjects(pComponent);
  }

  /**
    * Returns the current state of the simulator. 
    */
  public SimulatorState getState()
  {
    return aState;
  }

  public void addComponent(SimulatorComponent pComponent)
  {
    aComponents.add(pComponent);
  }

  /** Takes one 'step' in the simulation.
    *  The main part of the step() method is to invoke each SimulatorComponent
    *  in turn, to do its job. The components are handed the current state,
    *  the next state and the time elapsed in between (in ms). Following this,
    *  the next state becomes the current state.
    *
    * On top of that, the step() method is currently providing some hacked
    *  together functionality which will be progressively removed. All of the
    *  debug* functions must be taken out sooner or later. For now, the
    *  step() method also handles:
    *
    *   - Arrow-controlled driving of the robot
    *   - Silly physics involving crude collision detection and force fields
    *   - A clock synchronization mechanism
    */
  public void step()
  {
    // Determine how much time has elapsed
    // @@@ Note: the step() function per se should not be called unless
    //  we DO want to take a time step (so the clocking mechanisms should be
    //  somewhere else, e.g. in a run()-like function)
    int ms = debugGetElapsedTime();

    if (ms <= 0) return;
    
    // Allow the agent to be manually controlled using the arrows
    debugManualControl();
    // Provide some silly physics to convince other people to work on the
    //  simulator
    debugSillyPhysics();

    /** Begin new (real) simulator code - everything above has to be moved
      *  (more or less) */
    // Apply each component in turn (order matters!)
    for (SimulatorComponent comp : aComponents)
    {
      comp.apply(aState, aNextState, ms);
    }

    SimulatorState tmpState = aState;

    // Replace the current state by the new state
    aState = aNextState;
    // Be clever and reuse what was the current state as our next 'new state'
    aNextState = tmpState;
  }




  /** Temporary methods (used mainly until a proper replacement is written */

  public void debugCreateStuff()
  {
    Wall w = new Wall("Wall", 1);
    w.addPoint(20,20);
    w.addPoint(20,480);
    w.addPoint(480,480);
    w.addPoint(480,20);
    w.addPoint(20,20);

    // Make a polygon for the wall as well
    Polygon wallShape = new Polygon();
    // Interior
    wallShape.addPoint(20,20);
    wallShape.addPoint(20,480);
    wallShape.addPoint(480,480);
    wallShape.addPoint(480,20);
    wallShape.addPoint(20,20);
    // Exterior
    wallShape.addPoint(0,0);
    wallShape.addPoint(0,500);
    wallShape.addPoint(500,500);
    wallShape.addPoint(500,0);
    wallShape.addPoint(0,0);
    // Note that this polygon self-intersects at the duplicated edge
    //  (0,0)-(20,20)
    // This polygon is also evil because everything falls within its bounding
    //  box
    w.setShape(wallShape);

    // Make the wall react to dynamics
    w.addState(new ObjectStateDynamics(10000,10000));
    aState.addObject(w);

    SimulatorAgent sa = new SimulatorAgent("Motor Gabor", 2);

    Polygon agentShape = new Polygon();
    agentShape.addPoint (-10,10);
    agentShape.addPoint (10,10);
    agentShape.addPoint (10,-40);
    agentShape.addPoint (6,-40);
    agentShape.addPoint (6,-10);
    agentShape.addPoint (-10,-10);

    sa.setShape(agentShape);

    sa.setPosition(new Vector2D(250,250));
    // Give the agent a 'physics' state, with mass 4 and mom. of inertia 2
    sa.addState(new ObjectStateDynamics(4,2));
    // Give the agent an omnidirectional drive
    sa.addState(new ObjectStateOmnidrive());

    aState.addObject(sa);
    
    // Add an hexagonal obstacle
    SimulatorObject hex = new SimulatorObject("Hex", 3);

    // Create the hex polygon
    Polygon hexShape = new Polygon();
    hexShape.addPoint(0,0);
    hexShape.addPoint(-8,-6);
    hexShape.addPoint(-8,-16);
    hexShape.addPoint(0,-22);
    hexShape.addPoint(8,-16);
    hexShape.addPoint(8,-6);
    hexShape.translate(new Vector2D(0, 11));

    /*hexShape.addPoint(0,0);
    hexShape.addPoint(40,0);
    hexShape.addPoint(40,40);
    hexShape.addPoint(0, 40);
    hexShape.addPoint(0, 35);
    hexShape.addPoint(35,35);
    hexShape.addPoint(35, 5);
    hexShape.addPoint(0, 5);*/


    hex.setShape(hexShape);

    // Important - set position after setting shape
    hex.setPosition(new Vector2D(100,100));

    // Add dynamics to this object
    hex.addState(new ObjectStateDynamics(0.5,2));
   
    aState.addObject(hex);
  }

  public int debugGetElapsedTime()
  {
    // This needs to be turned into a proper clocking mechanism
	  long time = System.currentTimeMillis();
	  if(last_time == 0) {
		  last_time = time;
		  return 0;
	  }
	  int ms = (int)(time - last_time); 
	  
	  /** Don't run too fast */
	  if(ms < 10)
		  return 0;
	  else
    {
      last_time = time;
      return ms;
    }
  }

  public void debugManualControl()
  {
//	  SimulatorAgent agent = aState.getAgents().getFirst();
//	  
//	  double forceX, forceY, torque;
//	 
//    // If any of the visualizer keys are pressed, we override the omnidrive
//    //  @@@ This needs to be moved somewhere else or ...
//    if (vizHandler.up > 0 || vizHandler.down > 0 ||
//        vizHandler.right > 0 || vizHandler.left > 0)
//    {
//	    forceX = (vizHandler.up * 8 - vizHandler.down * 8) * Math.sin(agent.aDir);
//	    forceY = (vizHandler.up * 8 - vizHandler.down * 8) * Math.cos(agent.aDir);
//	    torque = (vizHandler.right * -4  + vizHandler.left * 4);
//
//            
//      // Modify the agent's omni drive data 
//      ObjectStateOmnidrive driveData = 
//        (ObjectStateOmnidrive)agent.getState(SimulatorComponentOmnidrive.NAME);
//      driveData.setVelocity (new Vector2D(forceX, forceY));
//      driveData.setAngVelocity (torque);
//    }
  }

  public void debugSillyPhysics()
  {
    SimulatorAgent agent = aState.getAgents().getFirst();

    ObjectStateDynamics phys =
      (ObjectStateDynamics)agent.getState(SimulatorComponentDynamics.NAME);

    LinkedList<SimulatorObject> stuff = new LinkedList<SimulatorObject>();

    /*
    SimulatorObject hex = aState.getObject(3);
    
    stuff.add (agent);
    stuff.add (hex);

    // Ha ha ha.
    for (SimulatorObject o : stuff)
    {
      if (o.getState(SimulatorComponentDynamics.NAME) == null) continue;

      ObjectStateDynamics oPhys = (ObjectStateDynamics)
        o.getState(SimulatorComponentDynamics.NAME);
      
      if (o.aPos.y >= 500)
        oPhys.addForce(new Force(0, -1000));
      else if (o.aPos.y < 0)
        oPhys.addForce(new Force(0, 1000));
      if (o.aPos.x >= 500)
        oPhys.addForce(new Force(-1000, 0));
      else if (o.aPos.x < 0)
        oPhys.addForce(new Force(1000, 0));
    }
    // Anna: simple collision detection, take this out once your stuff is
    //  in place (or in fact when you start working on it)
    Vector2D iPoint = hex.getShape().intersects(agent.getShape());
    if (iPoint != null)
    {
      System.err.println("Intersection at ("+iPoint.x+","+iPoint.y+")");
      Vector2D fv = hex.getPosition().minus(iPoint);
      fv.x = fv.x * 5;
      fv.y = fv.y * 5; 

      ObjectStateDynamics hexPhys =
        (ObjectStateDynamics)hex.getState(SimulatorComponentDynamics.NAME);
      hexPhys.addForce(new Force(fv,agent.getPosition()));
    }
    */

  }
}




