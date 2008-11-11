package org.rlcommunity.critter;

/**
 * SimulatorComponentLight
 *
 * This component deals with light-emitting objects and light sensors,
 *  pairing the two into a happy match.
 *
 * @author Marc G. Bellamre
 * @author Anna Koop
 */
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
    }
}

