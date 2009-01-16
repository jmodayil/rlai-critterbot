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

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import org.rlcommunity.critter.environments.EnvironmentDescription;
import org.rlcommunity.critter.svg.Loader;

public class SimulatorEngine {
	protected SimulatorState aState, aNextState;
	protected LinkedList<SimulatorComponent> aComponents;

	private long last_time;

	protected SimulatorVizEvents vizHandler;

    protected EnvironmentDescription aEnvDescription;
    
	public SimulatorEngine(EnvironmentDescription pDescription) {
		aComponents = new LinkedList<SimulatorComponent>();
		vizHandler = new SimulatorVizEvents();
        aEnvDescription = pDescription;

        initState();
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
	 * Returns a list of objects influenced by the given component
	 * 
	 * @param pComponent
	 *            The identifier of the component of interest
	 * @return A list of objects as per SimulatorState.getObjects(pComponent)
	 */
	public List<SimulatorObject> getObjects(String pComponent) {
		return aState.getObjects(pComponent);
	}

	/**
	 * Returns the current state of the simulator.
	 */
	public SimulatorState getState() {
		return aState;
	}

	public void addComponent(SimulatorComponent pComponent) {
		aComponents.add(pComponent);
	}

    public void initState() {
        // @todo move somewhere else
        Loader.initSVGUniverse();

        List<SimulatorObject> objects = aEnvDescription.generateObjects();
        aState = new SimulatorState(objects);

        // @todo Don't load the SVG stuff for now, because it is not properly scaled
        debugLoadSVG();
        
		// Clone the current state; we will use it to do state-state transitions
		aNextState = (SimulatorState) aState.clone();
    }
    
	public void step() {
		// Determine how much time has elapsed
		// @todo Note: the step() function per se should not be called unless
		// we DO want to take a time step (so the clocking mechanisms should be
		// somewhere else, e.g. in a run()-like function)
		int ms = debugGetElapsedTime();

		if (ms <= 0)
			return;

    step(ms);
    // Hack.  Replace this once a better keyboard handler is introduced.
    if(vizHandler.debug == 1) {
      System.out.println("Toggling debug draw.");
      vizHandler.debug = 0;
      List<SimulatorObject> sensorObjs =
        aNextState.getObjects(ObjectStateIRDistanceSensor.NAME);

      for(SimulatorObject obj : sensorObjs) {
        SimulatorObject nextObj = aNextState.getObject(obj);
        if(nextObj == null)
          continue;

        ObjectStateIRDistanceSensor sensor = (ObjectStateIRDistanceSensor)
          nextObj.getState(ObjectStateIRDistanceSensor.NAME);
        if(sensor.debugDrawRays == true)
          sensor.debugDrawRays = false;
        else
          sensor.debugDrawRays = true;
      }
      sensorObjs =
        aState.getObjects(ObjectStateIRDistanceSensor.NAME);

      for(SimulatorObject obj : sensorObjs) {
        SimulatorObject nextObj = aState.getObject(obj);
        if(nextObj == null)
          continue;

        ObjectStateIRDistanceSensor sensor = (ObjectStateIRDistanceSensor)
          nextObj.getState(ObjectStateIRDistanceSensor.NAME);
        if(sensor.debugDrawRays == true)
          sensor.debugDrawRays = false;
        else
          sensor.debugDrawRays = true;
      }
    } 
  }

	/**
	 * Takes one 'step' in the simulation. The main part of the step() method is
	 * to invoke each SimulatorComponent in turn, to do its job. The components
	 * are handed the current state, the next state and the time elapsed in
	 * between (in ms). Following this, the next state becomes the current
	 * state.
	 * 
	 * On top of that, the step() method is currently providing some hacked
	 * together functionality which will be progressively removed. All of the
	 * debug* functions must be taken out sooner or later. For now, the step()
	 * method also handles:
	 * 
	 * - Arrow-controlled driving of the robot - Silly physics involving crude
	 * collision detection and force fields - A clock synchronization mechanism
   *
   * @param ms Time elapsed, in milliseconds, since the last step
	 */
	public void step(int ms) 
  {
		// Re-initialize the next state to be filled with data
		aNextState.clear();

		/**
		 * Begin new (real) simulator code - everything above has to be moved
		 * (more or less)
		 */
		// Apply each component in turn (order matters!)
		for (SimulatorComponent comp : aComponents) {
			comp.apply(aState, aNextState, ms);
		}

		SimulatorState tmpState = aState;

		// Replace the current state by the new state
		aState = aNextState;
		// Be clever and reuse what was the current state as our next 'new
		// state'
		aNextState = tmpState;
	}

	public void debugLoadSVG() {
        // @todo needs to be moved somewhere else
        int nextObjectId = aState.getObjects().size();
        
		/* Loader svgLoader = new Loader(aState, nextObjectId);
		//svgLoader.loadStaticObject("book", new Vector2D(345, 377), 0);
		svgLoader.loadStaticObject("table", new Vector2D(160, 100), 0.5);
		svgLoader.loadStaticObject("chair", new Vector2D(250, 310), -2.6);
		svgLoader.loadStaticObject("bookcase", new Vector2D(476, 200), Math.PI/2);
        
		nextObjectId = svgLoader.objectId();
         */
	}

	public int debugGetElapsedTime() {
		// This needs to be turned into a proper clocking mechanism
		long time = System.currentTimeMillis();
		if (last_time == 0) {
			last_time = time;
			return 0;
		}
		int ms = (int) (time - last_time);

		/** Don't run too fast */
		if (ms < 10)
			return 0;
		else {
			last_time = time;
			return ms;
		}
	}
}
