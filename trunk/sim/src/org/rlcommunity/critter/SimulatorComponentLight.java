package org.rlcommunity.critter;

/**
 * SimulatorComponentLight
 *
 * This component deals with light-emitting objects and light sensors,
 *  pairing the two into a happy match.
 *
 * @author Marc G. Bellamre
 */

import java.awt.geom.Point2D;
import java.util.*;

public class SimulatorComponentLight implements SimulatorComponent {

    public static final String NAME = "light";

    public SimulatorComponentLight() {
    }

    /** Computes what light sensors should receive given the current state,
      *  and set the result in the next state.
      *
      * @param pCurrent The current state of the system (must not be modified)
      * @param pNext  The next state of the system (where results are stored)
      * @param delta  The number of milliseconds elapsed between pCurrent
      *   and pNext.
      */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) 
    {
        SimulatorObject source = pCurrent.getObjects(ObjectStateLightSource.NAME).getFirst();
        SimulatorObject sensor;// = pNext.getObjects(ObjectStateLightSensor.NAME).getFirst();
        int numSensors = pNext.getObjects(ObjectStateLightSensor.NAME).size();

        // MGB Create a Scene will all polygons
        Scene scene = new Scene(pCurrent);

        for (int sensorJ=0;sensorJ<numSensors;sensorJ++)
        {
              sensor = pNext.getObjects(ObjectStateLightSensor.NAME).get(sensorJ);
                
              ObjectStateLightSensor lightSensor = (ObjectStateLightSensor)sensor.getState(ObjectStateLightSensor.NAME);
              ObjectStateLightSource lightSource = (ObjectStateLightSource)source.getState(ObjectStateLightSource.NAME);

            Vector2D srcPosition = source.getPosition();
        Vector2D sensorPosition = sensor.getPosition();
        
        Set<Polygon> polys = new HashSet<Polygon>();
        
        // MGB: remove the sensor and source polygons - might want to use
        //   Scene.removeSubtree and sensor.getRoot
        scene.removeObject(sensor);
        scene.removeObject(source);

        if (scene.isVisible(sensorPosition, srcPosition))
        {
            // Maybe use Vector2D's and take the length() / add a distance()
            //  method to Vector2D? I.e. to avoid using both Point2D and
            //  Vector2D
            Point2D.Double srcPoint = 
              new Point2D.Double(srcPosition.x, srcPosition.y);
            Point2D.Double sensorPoint = 
              new Point2D.Double(sensorPosition.x, sensorPosition.y);
            
            double lightDistance = srcPoint.distance(sensorPoint);

            double intensity = lightSource.getIntensity();
            lightSensor.setLightSensorValue(intensity/(Math.pow(lightDistance,2.0)));
        }
        else
            lightSensor.setLightSensorValue(0.0);
    
        scene.addObject(sensor);
        scene.addObject(source);
//                           System.out.printf("light sensor reading %s = %f\n",
//            sensor.getLabel(), lightSensor.getLightSensorValue());        
        }
//        System.out.println("-----------");
    }

}

