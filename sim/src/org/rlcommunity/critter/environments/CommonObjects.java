package org.rlcommunity.critter.environments;

import java.util.List;
import org.rlcommunity.critter.ObjectStateAccelerometer;
import org.rlcommunity.critter.ObjectStateBumpSensor;
import org.rlcommunity.critter.ObjectStateCritterbotInterface;
import org.rlcommunity.critter.ObjectStateDynamics;
import org.rlcommunity.critter.ObjectStateGyroscope;
import org.rlcommunity.critter.ObjectStateIRDistanceSensor;
import org.rlcommunity.critter.ObjectStateLightSensor;
import org.rlcommunity.critter.ObjectStateLightSource;
import org.rlcommunity.critter.ObjectStateOmnidrive;
import org.rlcommunity.critter.Polygon;
import org.rlcommunity.critter.SimulatorAgent;
import org.rlcommunity.critter.SimulatorObject;
import org.rlcommunity.critter.Vector2D;
import org.rlcommunity.critter.Wall;

/**
 * This class defines a set of common objects that are used across environments.
 *   It should be made strictly obsolete when an environment loader is added to
 *   the system.
 * 
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class CommonObjects {

    /** Adds a generated object to the given list. This method also takes care of
     *   setting the position and the direction of the object. Returns the ID that
     *   the next object should receive.
     * 
     * @param pList The list to which the object should be added
     * @param newObject The object to be added.
     * @param pId The first available object ID.
     * 
     * @return The next available object ID, after the given object has been added.
     */
    public static int addObject(List<SimulatorObject> pList,
            SimulatorObject pNewObject, int pId) {
        pList.add(pNewObject);

        return pId + pNewObject.getChildren().size() + 1;
    }

    /** Adds a generated object to the given list. This method also takes care of
     *   setting the position and the direction of the object. Returns the ID that
     *   the next object should receive.
     *
     * @param pList The list to which the object should be added
     * @param newObject The object to be added.
     * @param pPosition The new position of the object.
     * @param pDirection The new direction of the object.
     * @param pId The first available object ID.
     *
     * @return The next available object ID, after the given object has been added.
     */
    public static int addObject(List<SimulatorObject> pList,
            SimulatorObject pNewObject, Vector2D pPosition, double pDirection, int pId) {
        pNewObject.setPosition(pPosition);
        pNewObject.setDirection(pDirection);

        return CommonObjects.addObject(pList, pNewObject, pId);
    }

   /** Generates (and returns) a standard issue Critterbot. The user may wish
     *   to add additional sensors to the robot. The list of properties added
     *   to the Critterbot by default:
     *
     *  - Dynamics (ObjectStateDynamics)
     *  - Critterbot Interface (ObjectStateCritterbotInterface)
     *  - Omnidrive (ObjectStateOmnidrive)
     *  - Bump sensors (ObjectStateBumpSensor)
     *  - Accelerometer (ObjectStateAccelerometer)
     *  - Gyroscope (ObjectStateGyroscope)
     *  - Four light sensors (separate objects with a ObjectStateLightSensor)
     *  - Ten IR distance sensors (separate objects with a ObjectStateIRDistanceSensor)
     *
     *  More than one object is created within this method. As such, the object
     *   ID should be increment by the size of the robot tree, not by 1.
     *
     * @param pName The name / label of the created Critterbot.
     * @param pId   The ID corresponding to this Critterbot.
     * 
     * @return A basic Critterbot.
     */
    public static SimulatorObject generateCritterbot(String pName, int pId) {
        SimulatorAgent sa = new SimulatorAgent("Critterbot", pId++);

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
        agentShape.addPoint(2.5, -16);
        agentShape.addPoint(9, -16);
        agentShape.addPoint(15.5, -12.5);
        agentShape.addPoint(19, -6);
        agentShape.addPoint(20, 0);
        agentShape.addPoint(18.5, 7.5);
        agentShape.addPoint(14, 14);
        agentShape.addPoint(7.5, 18.5);
        agentShape.rotate(-Math.PI / 2, new Vector2D(0, 0));
        double robotLength = 68; // Rough length estimate

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

        sa.addState(new ObjectStateAccelerometer());
        sa.addState(new ObjectStateGyroscope());

        // Create an external light sensor
        SimulatorObject lightSensor = new SimulatorObject("LightSensor1",
                pId++);
        // These three light sensors have no shape!
        lightSensor.setPosition(new Vector2D(19.8, 0));
        lightSensor.setDirection(0);
        //  lightSensor.setLocalDirection(0.0);
        ObjectStateLightSensor specificLightSensor =
                new ObjectStateLightSensor(5, 1.0, 5.0);
        lightSensor.addState(specificLightSensor);

        sa.addChild(lightSensor);


        // Create three more light sensors
        lightSensor = lightSensor.makeCopy("LightSensor2", pId++);
        lightSensor.setPosition(new Vector2D(0, -19.8));
        lightSensor.setLocalDirection(-Math.PI / 2.0);
        sa.addChild(lightSensor);

        lightSensor = lightSensor.makeCopy("LightSensor3", pId++);
        lightSensor.setPosition(new Vector2D(0, 19.8));
        lightSensor.setLocalDirection(-Math.PI / 2.0);
        sa.addChild(lightSensor);

        lightSensor = lightSensor.makeCopy("LightSensor4", pId++);
        lightSensor.setPosition(new Vector2D(-19.8, 0.0));
        lightSensor.setLocalDirection(-Math.PI);
        sa.addChild(lightSensor);

        // Now the IR distance sensors
        double irRange = 3 * robotLength;

        // Make a dummy object that we will not actually add
        SimulatorObject baseIrSensor =
                new SimulatorObject("IRSensor-base", -1);
        baseIrSensor.addState(new ObjectStateIRDistanceSensor(irRange));

        double pi = Math.PI;

        // Oops, why are these all hardcoded? - use robot length at least!
        // The sensors are slightly inside the robot so that they fall within the
        //  robot polygon
        double[][] irDistancePos = new double[][]{
            // Sensor 0 forward
            {19.8, 0.0, 0.0},
            // Sensor 6 45 cw
            {14.0, -14.0, -pi / 4},
            // Sensor 5 90 cw
            {0.0, -19.8, -pi / 2},
            // Sensor 4 135 cw
            {-14.0, -14.0, -3 * pi / 4},
            // Sensor 3 135 ccw
            {-14.0, 14.0, 3 * pi / 4},
            // Sensor 2 90 ccw
            {0.0, 19.8, pi / 2},
            // Sensor 1 45 ccw
            {14.0, 14.0, pi / 4},
            // Sensor 9 - a bit further back on the tail than 7 (quantify this!)
            //  faces 105 cw. This value may be wrong.
            {-26.0, 5.5, -pi / 2},
            // Sensor 8 - faces directly back, next to 3, arranged to miss the tail
            //  (which is at (-48,0) here)
            {-16, -5.0, -pi},
            // Sensor 7 - midway on the tail, facing out. Pos. may be wrong
            {-32.0, 9.0, pi / 2 + pi / 8},};

        for (int i = 0; i < irDistancePos.length; i++) {
            SimulatorObject sensor =
                    baseIrSensor.makeCopy("IRSensor" + i, pId++);
            sensor.setPosition(
                    new Vector2D(irDistancePos[i][0], irDistancePos[i][1]));
            sensor.setDirection(irDistancePos[i][2]);
            sa.addChild(sensor);
        }

        return sa;
    }

    /** Generates an instance of the wall enclosing the environment.
     *
     * Note: This wall object may be composed of many objects. As such, the
     *  next ID should be increased by the size of the object tree, not by 1.
     * 
     * @param pName The name of the wall to be created.
     * @param pId The id of the wall.
     *
     * @return A new wall instance.
     */
    public static SimulatorObject generateWall(String pName, int pId) {
		Wall w = new Wall(pName, pId);
		// @todo w.setSVG("wall").resetNativeTranslation();

		w.addPoint(20, 20);
		w.addPoint(20, 480);
		w.addPoint(480, 480);
		w.addPoint(480, 20);
		w.addPoint(20, 20);

		// Make a polygon for the wall as well
		Polygon wallShape = new Polygon();
        // @todo - build the wall from four convex objects
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
		wallShape.doneAddPoints();

		// Note that this polygon self-intersects at the duplicated edge
		// (0,0)-(20,20)
		// This polygon is also evil because everything falls within its
		// bounding box
		w.setShape(wallShape);

		// Make the wall react to dynamics
		ObjectStateDynamics wallDyn = new ObjectStateDynamics(10000, 10000);
		wallDyn.setMaxSpeed(0);
		w.addState(wallDyn);
        
        return w;
    }

    /** Creates a hexagon-shaped object. The object has physical properties
     *   (e.g. ObjectStateDynamics) so that it can be moved around.
     *
     * @param pName The name of the object.
     * @param pId The id of the object.
     *
     * @return A hexagon-shaped object.
     */
    public static SimulatorObject generateHex(String pName, int pId) {
        		// Add an hexagonal obstacle
		SimulatorObject hex = new SimulatorObject(pName, pId);

		// Create the hex polygon
		Polygon hexShape = new Polygon();
		hexShape.addPoint(0, 0);
		hexShape.addPoint(-8, -6);
		hexShape.addPoint(-8, -16);
		hexShape.addPoint(0, -22);
		hexShape.addPoint(8, -16);
		hexShape.addPoint(8, -6);
		hexShape.translate(new Vector2D(0, 11));
		hexShape.doneAddPoints();

		hex.setShape(hexShape);

		// Add dynamics to this object
		hex.addState(new ObjectStateDynamics(0.5, 2));

        return hex;
    }

    /** Generates a light source.
     *
     * @param pName The name of the light source.
     * @param pId The ID associated with the light source.
     * 
     * @return A new SimulatorObject with light source properties.
     */
    public static SimulatorObject generateLightSource(String pName, int pId) {
		SimulatorObject lightSource = new SimulatorObject(pName, pId);

		lightSource.setShape(null);

		// @todo lightSource.setSVG("lightsource");

		ObjectStateLightSource specificLightSource = new ObjectStateLightSource();
		specificLightSource.setIntensity(10000.0);
		lightSource.addState(specificLightSource);

		return lightSource;
    }
}
