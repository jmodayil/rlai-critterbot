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
  // Terribly broken... remove this asap
  protected CritterControlDrop aCommand;

  protected SimulatorState aState;
  protected LinkedList<SimulatorComponent> aComponents;

  private long last_time;
  
  protected SimulatorVizEvents vizHandler;

  public SimulatorEngine()
  {
    aState = new SimulatorState();
    aComponents = new LinkedList<SimulatorComponent>();

    vizHandler = new SimulatorVizEvents();

    aCommand = new CritterControlDrop();

    // Construct the simulator state by adding objects to it
    Wall w = new Wall("Wall", 1);
    w.addPoint(20,20);
    w.addPoint(20,480);
    w.addPoint(480,480);
    w.addPoint(480,20);
    w.addPoint(20,20);
   
    aState.addObject(w);

    SimulatorAgent sa = new SimulatorAgent("Anna Banana", 2);

    Polygon agentShape = new Polygon();
    agentShape.addPoint (-10,-10);
    agentShape.addPoint (10,-10);
    agentShape.addPoint (10,10);
    agentShape.addPoint(-10,10);

    sa.setShape(agentShape);

    sa.setPosition(new Vector2D(250,250));
    sa.setMass(4);
    sa.setMoment(2);
    // Give the agent a 'physics' state
    sa.addState(new ObjectStatePhysics());

    aState.addObject(sa);
    
    // Add an hexagonal obstacle
    SimulatorObject hex = new SimulatorObject("Hex", 3);

    // This obstacle can be moved around
    hex.addState (new ObjectStatePhysics());

    // Create the hex polygon
    Polygon hexShape = new Polygon();
    hexShape.addPoint(0,0);
    hexShape.addPoint(-4,-3);
    hexShape.addPoint(-4,-8);
    hexShape.addPoint(0,-11);
    hexShape.addPoint(4,-8);
    hexShape.addPoint(4,-3);

    hex.setShape(hexShape);

    // Important - set position after setting shape
    hex.setPosition(new Vector2D(100,100));
    hex.setMass(1);
    hex.setMoment(2);
   
    aState.addObject(hex);
  }

  /** Returns a list of existing agents */
  public List<SimulatorAgent> getAgentList() { 
    return Collections.unmodifiableList(aState.getAgents()); 
  }

  /** Returns a list of existing objects */
  public List<SimulatorObject> getObjectList() { 
    return Collections.unmodifiableList(aState.getObjects()); 
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

  // @@@ remove me
  public void setCommand(CritterControlDrop pDrop)
  {
    aCommand = pDrop;
  }

  /** Takes one 'step' in the simulation: update positions, velocities, etc
    */
  public void step()
  {
    // @@@ remove this!!! 
	  long time = System.currentTimeMillis();
	  if(last_time == 0) {
		  last_time = time;
		  return;
	  }
	  int ms = (int)(time - last_time); 
	  
	  /** Don't run too fast */
	  if(ms < 10)
		  return;
	  last_time = time;
	  
	  SimulatorAgent test = aState.getAgents().getFirst();
	  
	  // @@@ this needs to be removed
    // This is the motion calculation.
	  double forceX, forceY, torque;
	  
	  forceX = vizHandler.up * 8 * Math.sin(test.aDir);
	  forceY = vizHandler.up * 8 * Math.cos(test.aDir);
	  torque = (vizHandler.right * -4  + vizHandler.left * 4);

    // Add RobotControlDrop's forces! Also, not quite right - the command
    //  is a velocity, not a force
    forceX += (aCommand.x_vel / 10.0) * 4;
    forceY += (aCommand.y_vel / 10.0) * 4;
    torque += (aCommand.theta_vel / 10.0) * (-2);

    // Modify the agent's physics state by the external forces
    ObjectStatePhysics phys = 
      (ObjectStatePhysics)test.getState(SimulatorComponentPhysics.NAME);
    phys.addForce(new Force(forceX, forceY));
    phys.setTorque(torque);

    // Ha ha ha.
    if (test.aPos.y >= 500)
      phys.addForce(new Force(0, -2000));
    else if (test.aPos.y < 0)
      phys.addForce(new Force(0, 2000));
    if (test.aPos.x >= 500)
      phys.addForce(new Force(-2000, 0));
    else if (test.aPos.x < 0)
      phys.addForce(new Force(2000, 0));

    // Take this out, Anna
    SimulatorObject hex = aState.getObject(3);
    if (hex.getShape().intersects(test.getShape()))
    {
      System.err.println("Bang!");
      Vector2D fv = hex.getPosition().minus(test.getPosition());
      Vector2D vel = phys.getVelocity();
      fv.x = fv.x * vel.length() / 5;
      fv.y = fv.y * vel.length() / 5;

      ObjectStatePhysics hexPhys =
        (ObjectStatePhysics)hex.getState(SimulatorComponentPhysics.NAME);
      hexPhys.addForce(new Force(fv,test.getPosition()));
    }

    /** Begin new (real) simulator code - everything above has to be moved
      *  (more or less) */
    // Make a copy of the state, which will later become the new state
    SimulatorState newState = (SimulatorState)aState.clone();

    // Apply each component in turn (order matters!)
    for (SimulatorComponent comp : aComponents)
    {
      comp.apply(aState, newState, ms);
    }

    // Replace the current state by the new state
    aState = newState;
  }
}
