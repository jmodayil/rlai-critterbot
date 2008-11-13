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
        SimulatorObject sensor = pNext.getObjects(ObjectStateLightSensor.NAME).getFirst();
        ObjectStateLightSensor lightSensor = (ObjectStateLightSensor)sensor.getState(ObjectStateLightSensor.NAME);
        ObjectStateLightSource lightSource = (ObjectStateLightSource)source.getState(ObjectStateLightSource.NAME);

               
        
        Vector2D srcPosition = source.getPosition();
        Vector2D sensorPosition = sensor.getPosition();
        
        Set<Polygon> polys = new HashSet<Polygon>();
        
        for (SimulatorObject o : pCurrent.getObjects())
        {
            if (o.corresponds(sensor) || o.corresponds(source)) continue;
            Polygon shape = o.getShape();
            
            polys.add(shape);
        }
        if (Scene.isVisible(sensorPosition, srcPosition, polys))
        {
            
            Point2D.Double srcPoint = new Point2D.Double(srcPosition.x, srcPosition.y);
            Point2D.Double sensorPoint = new Point2D.Double(sensorPosition.x, sensorPosition.y);
            
            double lightDistance = srcPoint.distance(sensorPoint);
            double intensity = lightSource.getIntensity();  
            lightSensor.setLightSensorValue(intensity/(Math.pow(lightDistance,2.0)));
           // System.out.println("distance is "+ lightDistance + "initial intensity is" + intensity);
           // System.out.println("computed light " + intensity/(lightDistance*lightDistance));
            
            System.out.printf("light sensor reading = %f\n",lightSensor.getLightSensorValue());
            
        }
    }
}

