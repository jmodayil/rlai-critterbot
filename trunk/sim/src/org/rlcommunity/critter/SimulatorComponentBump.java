package org.rlcommunity.critter;

/**
 * SimulatorComponentBump
 *
 * This component deals with bump sensors, and possibly in the future any
 *  response to contact between two objects.
 *
 * @author Marc G. Bellamre
 */

import java.util.LinkedList;
import java.util.List;

public class SimulatorComponentBump implements SimulatorComponent {

    public static final String NAME = "bump";

    public static final double BUMP_DECAY = 0.9;

    public SimulatorComponentBump() 
    {
    }

    /** Computes what bump sensors should receive given the current state,
      *  and set the result in the next state.
      *
      * @param pCurrent The current state of the system (must not be modified)
      * @param pNext  The next state of the system (where results are stored)
      * @param delta  The number of milliseconds elapsed between pCurrent
      *   and pNext.
      */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) 
    {
      List<SimulatorObject> objects = 
        pCurrent.getObjects(ObjectStateBumpSensor.NAME);

      for (SimulatorObject o : objects)
      {
        SimulatorObject nextObj = pNext.getObject(o);

        ObjectStateBumpSensor bs = (ObjectStateBumpSensor)
          o.getState(ObjectStateBumpSensor.NAME);
        ObjectStateBumpSensor nextbs = (ObjectStateBumpSensor)
          nextObj.getState(ObjectStateBumpSensor.NAME);

        // Decay existing forces
        List<Force> forces = bs.getForces();

        // Assume that any force already in nextbs is NEW, so we should keep
        //  it (i.e. we do NOT call nextbs.clearForces() here)
        // MGB:
        //  We should most likely NOT decay forces, because this will
        //  result in duplicate measurements. In other words, the bump sensor
        //  would measure 'twice the force' if it occured twice as long.
        //  We probably want something smoother, maybe a time-dependent
        //  sensor, some sort of decaying accumulator?
        for (Force f : forces)
        {
          Vector2D newVec = 
            new Vector2D(f.vec.x * BUMP_DECAY, f.vec.y * BUMP_DECAY);

          // @@@ Yuck, fixme
          // For now, remove forces when they get small
          if (newVec.length() < 0.01)
            continue;

          Force newForce = new Force(newVec, f.source);
          nextbs.addForce(newForce);
        }
      }
    }
}


