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

import org.rlcommunity.critter.svg.Loader;

public class SimulatorEngine {
	protected SimulatorState aState, aNextState;
	protected LinkedList<SimulatorComponent> aComponents;

	private long last_time;

	protected SimulatorVizEvents vizHandler;

	public SimulatorEngine() {
		aState = new SimulatorState();
		aComponents = new LinkedList<SimulatorComponent>();

		vizHandler = new SimulatorVizEvents();

		// Construct the simulator state by adding objects to it
		// MGB: I moved all of this code to debugCreateStuff because it was
		// getting fairly bulky
		debugCreateStuff();

		// Clone the current state; we will use it to do state-state transitions
		aNextState = (SimulatorState) aState.clone();
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
	 */
	public void step() {
		// Determine how much time has elapsed
		// @@@ Note: the step() function per se should not be called unless
		// we DO want to take a time step (so the clocking mechanisms should be
		// somewhere else, e.g. in a run()-like function)
		int ms = debugGetElapsedTime();

		if (ms <= 0)
			return;

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

	/** Temporary methods (used mainly until a proper replacement is written */
	private int nextObjectId = 1;

	public void debugCreateStuff() {
		Polygon shape;
		debugCreateWall();
		debugCreateRobot();
		// Add an hexagonal obstacle
		SimulatorObject hex = new SimulatorObject("Hex", nextObjectId++);

		// Create the hex polygon
		Polygon hexShape = new Polygon();
		hexShape.addPoint(0, 0);
		hexShape.addPoint(-8, -6);
		hexShape.addPoint(-8, -16);
		hexShape.addPoint(0, -22);
		hexShape.addPoint(8, -16);
		hexShape.addPoint(8, -6);
		hexShape.translate(new Vector2D(0, 11));
		System.out.println("Hex");
		hexShape.doneAddPoints();

		/*
		 * hexShape.addPoint(0,0); hexShape.addPoint(40,0);
		 * hexShape.addPoint(40,40); hexShape.addPoint(0, 40);
		 * hexShape.addPoint(0, 35); hexShape.addPoint(35,35);
		 * hexShape.addPoint(35, 5); hexShape.addPoint(0, 5);
		 */

		hex.setShape(hexShape);

		// Important - set position after setting shape
		hex.setPosition(new Vector2D(100, 100));

		// Add dynamics to this object
		hex.addState(new ObjectStateDynamics(0.5, 2));

		aState.addObject(hex);

		SimulatorObject lightSource = new SimulatorObject("light",
				nextObjectId++);

		shape = new Polygon();

		shape.addPoint(0.0, 0.0);
		shape.addPoint(3.0, 3.0);
		shape.addPoint(6.0, 0.0);
		System.out.println("Shape");
		shape.doneAddPoints();
		lightSource.setShape(shape);
		lightSource.setPosition(new Vector2D(50.0, 50.0));
		lightSource.setSVG("lightsource");

		ObjectStateLightSource specificLightSource = new ObjectStateLightSource();
		specificLightSource.setIntensity(10000.0);
		lightSource.addState(specificLightSource);

		aState.addObject(lightSource);

		Loader svgLoader = new Loader(aState, nextObjectId);
		svgLoader.loadStaticObject("book", new Vector2D(345, 267), 5);
		svgLoader.loadStaticObject("table", new Vector2D(100, 220), 0.5);
		svgLoader.loadStaticObject("chair", new Vector2D(320, 400), 3.64);
		svgLoader.loadStaticObject("chair", new Vector2D(262, 500), 3.64);

		nextObjectId = svgLoader.objectId();

	}

	private void debugCreateWall() {
		Wall w = new Wall("Wall", nextObjectId++);
		w.setSVG("wall").resetNativeTranslation();

		w.addPoint(20, 20);
		w.addPoint(20, 480);
		w.addPoint(480, 480);
		w.addPoint(480, 20);
		w.addPoint(20, 20);

		// Make a polygon for the wall as well
		Polygon wallShape = new Polygon();
		// Exterior
		wallShape.addPoint(0, 0);
		wallShape.addPoint(0, 500);
		wallShape.addPoint(500, 500);
		wallShape.addPoint(500, 0);
		wallShape.addPoint(0, 0);
		// Interior (notice! the interior must be given in counter-clockwise
		// order)
		wallShape.addPoint(20, 20);
		wallShape.addPoint(480, 20);
		wallShape.addPoint(480, 480);
		wallShape.addPoint(20, 480);
		wallShape.addPoint(20, 20);
		System.out.println("Wall");
		wallShape.doneAddPoints();

		// Note that this polygon self-intersects at the duplicated edge
		// (0,0)-(20,20)
		// This polygon is also evil because everything falls within its
		// bounding
		// box
		w.setShape(wallShape);

		// Make the wall react to dynamics
		ObjectStateDynamics wallDyn = new ObjectStateDynamics(10000, 10000);
		wallDyn.setMaxSpeed(0);
		w.addState(wallDyn);
		aState.addObject(w);
	}

	public void debugCreateRobot() {
		SimulatorAgent sa = new SimulatorAgent("Motor Gabor", nextObjectId++);

		Polygon agentShape = new Polygon();
		agentShape.addPoint(-0, 20);
		agentShape.addPoint(-7.5, 18.5);
		agentShape.addPoint(-14, 14);
		agentShape.addPoint(-18.5, 7.5);
		agentShape.addPoint(-20, 0);
		agentShape.addPoint(-18.5, -6.5);
		agentShape.addPoint(-16.5, -16);
		agentShape.addPoint(-13, -26);
		agentShape.addPoint(-8, -35.5);
		agentShape.addPoint(-1, -47);
		agentShape.addPoint(0, -48);
		agentShape.addPoint(-2, -40.5);
		agentShape.addPoint(-4, -32.5);
		agentShape.addPoint(-4.5, -20);
		agentShape.addPoint(-3, -20);
		agentShape.addPoint(3.5, -16);
		agentShape.addPoint(9, -16);
		agentShape.addPoint(15.5, -12.5);
		agentShape.addPoint(19, -6);
		agentShape.addPoint(20, 0);
		agentShape.addPoint(18.5, 7.5);
		agentShape.addPoint(14, 14);
		agentShape.addPoint(7.5, 18.5);
		System.out.println("Agent");
		agentShape.rotate(-Math.PI / 2, new Vector2D(0, 0));

		agentShape.doneAddPoints();

		sa.setShape(agentShape);

		// sa.setSVG("robot");

		// Give the agent a 'physics' state, with mass 4 and mom. of inertia 2
		ObjectStateDynamics osd = new ObjectStateDynamics(4, 2);
		osd.setCoefficientFrictionStatic(0.1);
		sa.addState(osd);

		// This agent is a Critterbot!
		sa.addState(new ObjectStateCritterbotInterface());

		// Give the agent an omnidirectional drive
		sa.addState(new ObjectStateOmnidrive());
		sa.addState(new ObjectStateBumpSensor());

		aState.addObject(sa);

		// Create an external light sensor
		SimulatorObject sensor = new SimulatorObject("LightSensor1",
				nextObjectId++);
		/*
		 * shape = new Polygon();
		 * 
		 * shape.addPoint(0.0, -1.0); shape.addPoint(20.0, 0.0);
		 * shape.addPoint(0.0, 1.0); shape.doneAddPoints();
		 * 
		 * sensor.setShape(shape);
		 */
		// These three light sensors have no shape!
		sensor.setPosition(new Vector2D(20.001, 0));
		sensor.addState(new ObjectStateLightSensor());

		sa.addChild(sensor);

		// Create two more light sensors
		sensor = sensor.makeCopy("LightSensor2", nextObjectId++);
		sensor.setPosition(new Vector2D(0, -20.001));
		sa.addChild(sensor);

		sensor = sensor.makeCopy("LightSensor3", nextObjectId++);
		sensor.setPosition(new Vector2D(0, 20.001));
		sa.addChild(sensor);

		sa.setPosition(new Vector2D(250, 250));

		// Now the IR distance sensors
		sensor = new SimulatorObject("IRSensor1", nextObjectId++);
		sensor.addState(new ObjectStateIRDistanceSensor(100));

		sensor.setPosition(new Vector2D(0.001, 0));
		sensor.setDirection(0);

		sa.addChild(sensor);

		sensor = new SimulatorObject("IRSensor2", nextObjectId++);
		sensor.addState(new ObjectStateIRDistanceSensor(100));

		sensor.setPosition(new Vector2D(0.001, 0));
		sensor.setDirection(Math.PI / 2);

		sa.addChild(sensor);
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
