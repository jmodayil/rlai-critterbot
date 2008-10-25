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
    sa.setPosition(new Point2D.Double(250,250));
    sa.setMass(4);
    sa.setMoment(2);
   
    aState.addObject(sa);
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
	  long ms = time - last_time;  
	  
	  /** Don't run too fast */
	  if(ms < 10)
		  return;
	  last_time = time;
	  
	  SimulatorAgent test = aState.getAgents().getFirst();
	  
	  // This is the motion calculation.
	  double forceX, forceY, torque;
	  
	  forceX = vizHandler.up * 4 * Math.sin(test.aDir);
	  forceY = vizHandler.up * 4 * Math.cos(test.aDir);
	  torque = (vizHandler.right * -2  + vizHandler.left * 2);

    // Add RobotControlDrop's forces! Also, not quite right - the command
    //  is a velocity, not a force
    forceX += (aCommand.x_vel / 10.0) * 4;
    forceY += (aCommand.y_vel / 10.0) * 4;
    torque += (aCommand.theta_vel / 10.0) * (-2);

	  // A very sad attempt at friction
	  forceX -= test.aVel.x * .1;
	  forceY -= test.aVel.y * .1;
	  torque -= test.aRot * .5;
	  
	  test.aRot += torque / test.momI;
	  test.aVel.x += forceX / test.mass;
	  test.aVel.y += forceY / test.mass;
	  
	  double newDir = test.aDir + test.aRot * ms / 1000;
	  double newX = test.aPos.x + test.aVel.x * ms / 1000;
	  double newY = test.aPos.y + test.aVel.y * ms / 1000;
	  
	  // Now test for collisions, again very sad.
	  List<Line2D.Double> lines = ((Wall)aState.getObject(1)).getLines();
	  Iterator<Line2D.Double> i = lines.iterator();
	  while(i.hasNext()) {
		  Line2D.Double line = i.next();
		  double dist = line.ptSegDist(newX, newY);
		  //System.out.println(dist);
		  if(10 >= dist) {
			  System.out.println("Collision!!!");
			  test.aVel.x = test.aVel.y = 0;
			  newX = test.aPos.x;
			  newY = test.aPos.y;
		  }
	  }
	  test.aDir = newDir;
	  test.aPos.x = newX;
	  test.aPos.y = newY;


    /** Begin new (real) simulator code - everything above has to be moved */
    // Make a copy of the state, which will later become the new state
    SimulatorState newState = (SimulatorState)aState.clone();

    // Apply each component in turn (order matters!)
    for (SimulatorComponent comp : aComponents)
    {
      comp.apply(aState, newState);
    }

    // Replace the current state by the new state
    aState = newState;
  }
}
