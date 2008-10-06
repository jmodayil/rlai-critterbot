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
  protected LinkedList<SimulatorAgent> aAgentList;
  protected LinkedList<SimulatorObject> aObjList;
  protected SimulatorState aState;
  
  private long last_time;
  
  protected SimulatorVizEvents vizHandler;

  public SimulatorEngine()
  {
    aAgentList = new LinkedList<SimulatorAgent>();
    aObjList = new LinkedList<SimulatorObject>();
    aState = new SimulatorState();
    vizHandler = new SimulatorVizEvents();

    // Construct the simulator state by adding objects to it
    Wall w = new Wall("Um?", 1);
    w.addPoint(20,20);
    w.addPoint(20,480);
    w.addPoint(480,480);
    w.addPoint(480,20);
    w.addPoint(20,20);
    aObjList.add(w);
    
    SimulatorAgent sa = new SimulatorAgent("Varun", 2);
    sa.setPosition(new Point2D.Double(250,250));
    sa.setMass(4);
    sa.setMoment(2);
    aAgentList.add(sa);
    
  }

  /** Returns a list of existing agents */
  public List<SimulatorAgent> getAgentList() { return Collections.unmodifiableList(aAgentList); }

  public List<SimulatorObject> getObjList() { return Collections.unmodifiableList(aObjList); }
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
	  
	  SimulatorAgent test = aAgentList.getFirst();
	  
	  // This is the motion calculation.
	  double forceX, forceY, torque;
	  
	  forceX = vizHandler.up * 4 * Math.sin(test.aDir);
	  forceY = vizHandler.up * 4 * Math.cos(test.aDir);
	  torque = (vizHandler.right * -2  + vizHandler.left * 2);
	  
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
	  List<Line2D.Double> lines = ((Wall)aObjList.getFirst()).getLines();
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

	  
	  
  }
}
