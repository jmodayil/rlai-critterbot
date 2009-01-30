/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critter;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Defines a scheduler that runs the SimulatorEngine at a fixed rate, determined
 *  by a base rate and a time scale. When the time scale is equal to 1.0, the
 *  simulator runs in real-time. No guarantee is made if the computation time
 *  is greater than the required rate.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class SimulatorScheduler {
    /** The scale at which the simulator engine is run */
    protected volatile double aScale;
    /** The engine that we run */
    protected SimulatorEngine aEngine;
    /** The base step rate, in milliseconds */
    protected long aStepLength;

    /** A flag telling us that we should stop running. */
    protected volatile boolean aTerminated = false;

    public final int NANOSECONDS_IN_MILLISECOND = 1000000;
    
    /** Creates a new scheduler for the given engine. This scheduler will run
     *   the engine on the given time step at a frequency pStepLength / pScale,
     *   in milliseconds.
     *
     * @param pEngine The engine to be run.
     * @param pStepLength The length, in milliseconds, of a real-time time step.
     * @param pScale The initial time scale at which the scheduler runs.
     */
    public SimulatorScheduler(SimulatorEngine pEngine, long pStepLength, double pScale) {
        aEngine = pEngine;
        aStepLength = pStepLength;
        aScale = pScale;
    }

    public void run() {
        // Tau is the residual time
        double tau = 0;

        double averageTime = 0;
        int count = 0;
        int reportInterval = 1000;

        int lagCount = 0;
        
        while (!aTerminated) {
            // Delta is the actual length of time , in ms, between two runs
            //  Because the time scale may change over time we recompute delta
            double delta = aStepLength / aScale;

            // @todo look at the time before and after
            // @todo take care of the case when aScale << 1
            // Run the simulator for a bit
            long startTime = System.nanoTime();
            aEngine.step((int)aStepLength);
            long computationTime = System.nanoTime() - startTime;

            averageTime += computationTime;
            count++;

            if (count >= reportInterval) {
                count -= reportInterval;
                System.err.println ("Average time: "+averageTime/reportInterval);
                averageTime = 0;
            }

            tau += delta - computationTime / NANOSECONDS_IN_MILLISECOND;
            
            long fullMillis;
            // If we are lagging behind do not stop
            if (tau < 0) {
                lagCount++;
                if (lagCount % 100 == 0) {
                    System.err.println ("Simulator is running behind...");
                }
                
                fullMillis = 0;
            }
            else fullMillis = (long)Math.floor(tau);
            
            tau -= fullMillis;
            if (fullMillis > 0) {
                try {
                    Thread.sleep(fullMillis);
                } catch (InterruptedException ex) {
                    // @todo handle the interrupted case
                }
            }
        }

        aTerminated = false;
    }

    /** Set the time scale at which things run. A value greater than 1 means the
     *   simulator is run faster than real-time; a value smaller than 1 means
     *   the opposite.
     *
     * @param pScale The time scale at which the simulator is run.
     */
    public void setScale(double pScale) {
        aScale = pScale;
    }

    /** Returns the time scale at which the simulator is run.
     *
     * @return The time scale of the scheduler.
     */
    public double getScale() {
        return aScale;
    }

    /** Flags the scheduler as needing to stop running.
     */
    public void terminate() {
        aTerminated = true;
    }
}