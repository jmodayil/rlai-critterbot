package org.rlcommunity.critter;

/**
 * SimulatorComponentLight
 *
 * This component deals with light-emitting objects and light sensors,
 *  pairing the two into a happy match.
 *
 * @author Marc G. Bellamre
 */

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
            System.out.println("I see light");
        }
    }
}

