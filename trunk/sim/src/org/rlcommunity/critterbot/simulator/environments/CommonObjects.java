/* Copyright 2009 Marc G. Bellemare
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

package org.rlcommunity.critterbot.simulator.environments;

import java.util.LinkedList;
import java.util.List;

import org.rlcommunity.critterbot.simulator.ObjectStateAccelerometer;
import org.rlcommunity.critterbot.simulator.ObjectStateBattery;
import org.rlcommunity.critterbot.simulator.ObjectStateBatteryCharger;
import org.rlcommunity.critterbot.simulator.ObjectStateBumpSensor;
import org.rlcommunity.critterbot.simulator.ObjectStateCritterbotInterface;
import org.rlcommunity.critterbot.simulator.ObjectStateDynamics;
import org.rlcommunity.critterbot.simulator.ObjectStateGyroscope;
import org.rlcommunity.critterbot.simulator.ObjectStateIRDistanceSensor;
import org.rlcommunity.critterbot.simulator.ObjectStateLightSensor;
import org.rlcommunity.critterbot.simulator.ObjectStateLightSource;
import org.rlcommunity.critterbot.simulator.ObjectStateOmnidrive;
import org.rlcommunity.critterbot.simulator.Polygon;
import org.rlcommunity.critterbot.simulator.SimulatorAgent;
import org.rlcommunity.critterbot.simulator.SimulatorObject;
import org.rlcommunity.critterbot.simulator.Vector2D;
import org.rlcommunity.critterbot.simulator.Wall;
import org.rlcommunity.critterbot.simulator.svg.Loader;

/**
 * This class defines a set of common objects that are used across environments.
 *   It should be made strictly obsolete when an environment loader is added to
 *   the system.
 * 
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class CommonObjects {

    /** Robot length in meters */
    public static final double NOMINAL_ROBOT_LENGTH = 0.68;
    public static final double ROBOT_LENGTH = 0.41;
    public static final double CM_PER_METER = 100;

    /** Adds a generated object to the given list. This method also takes care of
     *   setting the position and the direction of the object. Returns the ID that
     *   the next object should receive.
     * 
     * @param pList The list to which the object should be added
     * @param pNewObject The object to be added.
     * @param pId The first available object ID.
     * 
     * @return The next available object ID, after the given object has been added.
     */
    public static int addObject(List<SimulatorObject> pList,
            SimulatorObject pNewObject, int pId) {
        pList.add(pNewObject);

        return pId + pNewObject.getChildren().size() + 1;
    }

    private static final int xIndex = 0;
    private static final int yIndex = 1;
    private static final int dIndex = 2;
    /** Adds a generated object to the given list. This method also finds a
     *   random position and direction within the given ranges for the object. 
     *   Returns the ID that the next object should receive.
     *
     * @param pList The list to which the object should be added
     * @param pNewObject The object to be added.
     * @param pRanges Three pairs of <min, max> values defining the allowed
     *    position and direction. The pairs respectively correspond to the x
     *    coordinate, the y coordinate and the direction of the object.
     * @param pId The first available object ID.
     *
     * @return The next available object ID, after the given object has been added.
     */
    public static int addObjectRandomPosition(List<SimulatorObject> pList,
            SimulatorObject pNewObject, double[][] pRanges, int pId) {

        boolean done = false;
        double xRange = pRanges[xIndex][1] - pRanges[xIndex][0];
        double yRange = pRanges[yIndex][1] - pRanges[yIndex][0];
        double dRange = pRanges[dIndex][1] - pRanges[dIndex][0];

        int retries = 100;
        
        // Create a list with all the objects in the new object's tree...
        List<SimulatorObject> allOurObjects = pNewObject.getChildren();
        allOurObjects.add(pNewObject);

        // Keep only those objects which have a Polygon
        List<SimulatorObject> ourObjects = new LinkedList<SimulatorObject>();
        for (SimulatorObject o : allOurObjects) {
          if (o.getShape() != null)
            ourObjects.add(o);
        }

        // Also create a list of all other objects that have a polygon
        List<SimulatorObject> otherObjects = new LinkedList<SimulatorObject>();
        for (SimulatorObject root : pList) {
          // Get this object's tree
          List<SimulatorObject> rootTree = root.getChildren();
          rootTree.add(root);
          
          // Add any of these which have a polygon to our master list
          for (SimulatorObject o : rootTree) {
            Polygon shape = o.getShape();
            if (shape != null)
              otherObjects.add(o);
          }
        }
        
        while (!done) {
          retries--;
          if (retries <= 0) {
            throw new IllegalArgumentException("Could not add object at random position: "
                    +pNewObject.getLabel());
          }
          
          Vector2D pos = new Vector2D(Math.random() * xRange + pRanges[xIndex][0],
                  Math.random() * yRange + pRanges[yIndex][0]);
          double dir = Math.random() * dRange + pRanges[dIndex][0];

          // Set the object to a random valid position
          pNewObject.setPosition(pos);
          pNewObject.setDirection(dir);

          boolean invalidPosition = false;

          // Determine whether any of the existing objects intersect with us
          //  collidesWith() already traverses the tree, so we use the root
          //   objects here
          for (SimulatorObject o : pList) {
            if (pNewObject.collidesWith(o) != null) {
              invalidPosition = true;
              break;
            }
          }

          // Test for a polygon containing the other, which can occur
          //  independently of intersection
          for (SimulatorObject o1 : otherObjects) {
            for (SimulatorObject o2 : ourObjects) {
              // We know both o1 and o2 have polygons, by construction
              Polygon poly1 = o1.getShape();
              Polygon poly2 = o2.getShape();

              if (poly1.contains(poly2) || poly2.contains(poly1)) {
                invalidPosition = true;
                break;
              }
            }
          }

          done = !invalidPosition;
        }

        // Add the object to the list
        pList.add(pNewObject);

        return pId + pNewObject.getChildren().size() + 1;
    }

    /** Adds a generated object to the given list. This method also takes care of
     *   setting the position and the direction of the object. Returns the ID that
     *   the next object should receive.
     *
     * @param pList The list to which the object should be added
     * @param pNewObject The object to be added.
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

        double robotLength = ROBOT_LENGTH;
        double robotScale = ROBOT_LENGTH / NOMINAL_ROBOT_LENGTH;

        // Rough robot polygon, in cm
        double[][] agentShapePoints = new double[][]{
            new double[]{-0, 20},
            new double[]{-7.5, 18.5},
            new double[]{-14, 14},
            new double[]{-18.5, 7.5},
            new double[]{-20, 0},
            new double[]{-18.5, -6.5},
            new double[]{-16.5, -16},
            new double[]{-13, -26},
            new double[]{-8, -35.5},
            new double[]{-1, -47},
            new double[]{0, -48},
            new double[]{-2, -40.5},
            new double[]{-4, -32.5},
            new double[]{-4.5, -20},
            new double[]{-3, -20},
            new double[]{2.5, -16},
            new double[]{9, -16},
            new double[]{15.5, -12.5},
            new double[]{19, -6},
            new double[]{20, 0},
            new double[]{18.5, 7.5},
            new double[]{14, 14},
            new double[]{7.5, 18.5},};

        Polygon agentShape = new Polygon();

        for (double[] pt : agentShapePoints) {
            // Convert the points to meters
            agentShape.addPoint(robotScale * pt[0] / CM_PER_METER,
                    robotScale * pt[1] / CM_PER_METER);
        }

        agentShape.rotate(-Math.PI / 2, new Vector2D(0, 0));

        agentShape.doneAddPoints();

        sa.setShape(agentShape);

        // sa.setSVG("robot");

        // Give the agent a 'physics' state, with mass 4 and mom. of inertia 0.64
        //  0.64 taken from m r^2, assuming a radius of 0.4 (which is not quite right)
        ObjectStateDynamics osd = new ObjectStateDynamics(4, 0.64);
        osd.setCoefficientFrictionStatic(0.1);
        sa.addState(osd);

        // This agent is a Critterbot!
        sa.addState(new ObjectStateCritterbotInterface());

        // Give the agent an omnidirectional drive
        sa.addState(new ObjectStateOmnidrive());
        sa.addState(new ObjectStateBumpSensor());

        sa.addState(new ObjectStateAccelerometer());
        sa.addState(new ObjectStateGyroscope());


        SimulatorObject lightSensor = new SimulatorObject("LightSensor1",
        pId++);
        // These three light sensors have no shape!
        lightSensor.setPosition(new Vector2D(0.198*robotScale, 0));
        lightSensor.setDirection(0);
        //  lightSensor.setLocalDirection(0.0);
        ObjectStateLightSensor specificLightSensor =
        new ObjectStateLightSensor(11,0.005, 0.01); //odd number of pixels ONLY
        
        //new ObjectStateLightSensor(5, 0.005, 0.1); //get real numbers from mike
        //->->pixels, depth(m), width(m)
        lightSensor.addState(specificLightSensor);

        sa.addChild(lightSensor);


        // Create three more light sensors
        lightSensor = lightSensor.makeCopy("LightSensor2", pId++);
        lightSensor.setPosition(new Vector2D(0, -0.198*robotScale));
        lightSensor.setLocalDirection(-Math.PI / 2.0);
        sa.addChild(lightSensor);

        lightSensor = lightSensor.makeCopy("LightSensor3", pId++);
        lightSensor.setPosition(new Vector2D(0, 0.198*robotScale));
        lightSensor.setLocalDirection(Math.PI / 2.0);
        sa.addChild(lightSensor);

        lightSensor = lightSensor.makeCopy("LightSensor4", pId++);
        lightSensor.setPosition(new Vector2D(-0.198*robotScale, 0.0));
        lightSensor.setLocalDirection(Math.PI);
          sa.addChild(lightSensor); 

        SimulatorObject battery = new SimulatorObject("battery",
        pId++);
        battery.setPosition(new Vector2D(0.198*robotScale, 0));
        battery.setDirection(0);
        ObjectStateBattery specificBattery = new ObjectStateBattery(10000,5,1);
        //->->capacity(Coulomb), chargeRate(Ampere), idleDepletionRate(Ampere)
        battery.addState(specificBattery);

        sa.addChild(battery);

        

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
                    new Vector2D(irDistancePos[i][0] * robotScale / CM_PER_METER,
                    irDistancePos[i][1] * robotScale / CM_PER_METER));
            sensor.setDirection(irDistancePos[i][2]);
            sa.addChild(sensor);
        }

        return sa;
    }

    public static double defaultWallWidth = 5.0;
    public static double defaultWallHeight = 5.0;
    public static double defaultWallThickness = 0.2;

    public static SimulatorObject generateWall(String pName, int pId) {
      return generateWall(pName, pId, defaultWallWidth, defaultWallHeight,
              defaultWallThickness);
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
    public static SimulatorObject generateWall(String pName, int pId, double
            pWidth, double pHeight, double pThickness) {
        Wall w = new Wall(pName, pId++);
        // @todo w.setSVG("wall").resetNativeTranslation();

        w.addPoint(pThickness, pThickness);
        w.addPoint(pThickness, pHeight - pThickness);
        w.addPoint(pWidth - pThickness, pHeight - pThickness);
        w.addPoint(pWidth - pThickness, pThickness);
        w.addPoint(pThickness, pThickness);

        double[][][] partPoints = new double[][][] {
            { // North side
                { pThickness, 0.0 },
                { pWidth, 0.0 },
                { pWidth, pThickness },
                { pThickness, pThickness },
            },
            { // East side
                { pWidth, pThickness },
                { pWidth, pHeight },
                { pWidth - pThickness, pHeight },
                { pWidth - pThickness, pThickness },
            },
            { // South side
                { pWidth - pThickness, pHeight },
                { 0.0, pHeight },
                { 0.0, pHeight - pThickness },
                { pWidth - pThickness, pHeight - pThickness },
            },
            { // West side
                { 0.0, pHeight - pThickness },
                { 0.0, 0.0 },
                { pThickness, 0.0 },
                { pThickness, pHeight - pThickness },
            }
        };

        // We break down this wall in four parts
        int numParts = partPoints.length;

        for (int p = 0; p < numParts; p++) {
            SimulatorObject wallPart = new SimulatorObject(pName+p, pId++);
            // Make a polygon for each part
            Polygon wallShape = new Polygon(partPoints[p]);
            wallPart.setShape(wallShape);

            // Make the wall react to dynamics
            ObjectStateDynamics wallDyn = new ObjectStateDynamics(10000, 10000);
            wallDyn.setMaxSpeed(0);
            wallPart.addState(wallDyn);

            w.addChild(wallPart);
        }

        // The parent wall has no shape
        w.setShape(null);

        return w;
    }

        public static SimulatorObject generateBasket(String pName, int pId) {
        // @todo w.setSVG("wall").resetNativeTranslation();
           SimulatorObject basket = new SimulatorObject(pName, pId);

        // Create the hex polygon
        Polygon basketShape = new Polygon();
        basketShape.addPoint(0, 0);
        basketShape.addPoint(0, 1.0);
        basketShape.addPoint(1.0, 1.0);
        basketShape.addPoint(1.0, 0.0);
        basketShape.addPoint(0.9, 0.0);
        basketShape.addPoint(0.9, 0.9);
        basketShape.addPoint(0.1, 0.9);
        basketShape.addPoint(0.1, 0.0);        
        
        basketShape.doneAddPoints();

        basket.setShape(basketShape);
        
            ObjectStateDynamics baskeyDyn = new ObjectStateDynamics(10000, 10000);
            baskeyDyn.setMaxSpeed(0);
            basket.addState(baskeyDyn);



        return basket;
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
        // Create the object
        SimulatorObject hex = new SimulatorObject(pName, pId);

        // Create the hex polygon
        Polygon hexShape = new Polygon();
        hexShape.addPoint(0, 0);
        hexShape.addPoint(-0.08, -0.06);
        hexShape.addPoint(-0.08, -0.16);
        hexShape.addPoint(0, -0.22);
        hexShape.addPoint(0.08, -0.16);
        hexShape.addPoint(0.08, -0.06);
        hexShape.translate(new Vector2D(0, 0.11));
        hexShape.doneAddPoints();

        hex.setShape(hexShape);

        // Add dynamics to this object
        hex.addState(new ObjectStateDynamics(0.5, 0.5));

        return hex;
    }

        /** Creates some form of bar to be kicked around.
     *
     * @param pName The name of the object.
     * @param pId The id of the object.
     *
     * @return A bar-shaped object.
     */
    public static SimulatorObject generateBar(String pName, int pId) {
        SimulatorObject bar = new SimulatorObject(pName, pId);

        // @todo needs to be implemented
        // Create the hex polygon
        Polygon barShape = new Polygon();
        barShape.addPoint(-0.25, -0.02);
        barShape.addPoint(-0.25, 0.02);
        barShape.addPoint(0.25, 0.02);
        barShape.addPoint(0.25, -0.02);

        barShape.doneAddPoints();

        bar.setShape(barShape);

        // Add dynamics to this object
        bar.addState(new ObjectStateDynamics(1, 0.25));

        return bar;
    }
    
    public static SimulatorObject generateBall(String pName, int pId, double pRadius) {
        // Create the object
        SimulatorObject ball = new SimulatorObject(pName, pId);

        // Create its shape
        Polygon shape = new Polygon();

        int numPoints = 16;

        for (int i = 0; i < numPoints; i++) {
            double angle = i * 2 * Math.PI / numPoints;
            Vector2D pt = new Vector2D(Math.cos(angle) * pRadius, Math.sin(angle) * pRadius);
            pt.zeroize();

            shape.addPoint(pt.x, pt.y);
        }

        shape.translate(new Vector2D(pRadius, pRadius));
        shape.doneAddPoints();

        ball.setShape(shape);

        // Add dynamics to this object
        ball.addState(new ObjectStateDynamics(0.5, 0.5));

        return ball;
    }

    public static SimulatorObject generateBox(String pName, int pId, 
            double pWidth, double pHeight, double pDensity) {
        // Create the object
        SimulatorObject box = new SimulatorObject(pName, pId);

        // Create its shape
        Polygon shape = new Polygon();

        // Make the object position the center of the rectangle
        double orgX = -pWidth/2;
        double orgY = -pHeight/2;

        shape.addPoint(0, 0);
        shape.addPoint(0, pHeight);
        shape.addPoint(pWidth, pHeight);
        shape.addPoint(pWidth, 0);

        shape.translate(new Vector2D(orgX, orgY));
        shape.doneAddPoints();

        box.setShape(shape);

        double mass = pWidth * pHeight * pDensity;
        // From wikipedia (List_of_moments_of_inertia)
        double momInertia = mass * (pHeight*pHeight + pWidth*pWidth) / 12;
        // Add dynamics to this object
        box.addState(new ObjectStateDynamics(mass, momInertia));

        return box;
    }

    /** Generates a light source.
     *
     * @param pName The name of the light source.
     * @param pId The ID associated with the light source.
     * 
     * @return A new SimulatorObject with light source properties.
     */
    public static SimulatorObject generateLightSource(String pName, int pId, int intensity) {
        SimulatorObject lightSource = new SimulatorObject(pName, pId);

//        double pRadius = 0.1;
//        Polygon shape = new Polygon();
//
//        int numPoints = 16;
//
//        for (int i = 0; i < numPoints; i++) {
//            double angle = i * 2 * Math.PI / numPoints;
//            Vector2D pt = new Vector2D(Math.cos(angle) * pRadius, Math.sin(angle) * pRadius);
//            pt.zeroize();
//
//            shape.addPoint(pt.x, pt.y);
//        }
//
//        shape.translate(new Vector2D(pRadius, pRadius));
//        shape.doneAddPoints();

        lightSource.setShape(null);

        ObjectStateLightSource specificLightSource = new ObjectStateLightSource();
        specificLightSource.setIntensity(intensity); //talk to mike
        lightSource.addState(specificLightSource);
                        
        return lightSource;
    }
    
    public static SimulatorObject generateBatteryCharger(String pName, int pId) {
        SimulatorObject batteryCharger = new SimulatorObject(pName, pId);

        batteryCharger.setShape(null);

        // @todo lightSource.setSVG("lightsource");

        ObjectStateBatteryCharger specificCharger = new ObjectStateBatteryCharger(0.5,50); 
        //->->range(m),charge rate (Ampere)
        batteryCharger.addState(specificCharger);

        return batteryCharger;
    }    

    public static int generateSVGObjects(List<SimulatorObject> objects, int id) {
        Loader svgLoader = new Loader(id);
        objects.add(
                svgLoader.loadStaticObject("book", new Vector2D(345, 377), 0));
        objects.add(
                svgLoader.loadStaticObject("table", new Vector2D(160, 100), 0.5));
        objects.add(
                svgLoader.loadStaticObject("chair", new Vector2D(250, 310), -2.6));
        objects.add(
                svgLoader.loadStaticObject("bookcase", new Vector2D(476, 200), Math.PI / 2));

        return svgLoader.objectId();

    }
}
