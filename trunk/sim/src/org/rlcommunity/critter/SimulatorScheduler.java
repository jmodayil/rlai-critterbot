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

/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critter;

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
    protected final SimulatorEngine aEngine;
    /** The base step rate, in milliseconds */
    protected long aStepLength;

    /** A flag telling us that we should stop running. */
    protected volatile boolean aTerminated = false;

    public final int NANOSECONDS_IN_MILLISECOND = 1000000;

    /** How often (in time steps) the average time should be reported, or 0 to
     * not report */
    public final int averageTimeReportInterval = 0;
    
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

        int lagCount = 0;

        double ltAverageTime = 0;
        
        while (!aTerminated) {
            // Delta is the actual length of time , in ms, between two runs
            //  Because the time scale may change over time we recompute delta
            double delta = aStepLength / aScale;

            // @todo look at the time before and after
            // @todo take care of the case when aScale << 1
            // Run the simulator for a bit
            long startTime = System.nanoTime();
            // Make sure no one else is modifying the engine's state before we
            //  step through
            synchronized(aEngine) {
                aEngine.step((int)aStepLength);
            }
            long computationTime = System.nanoTime() - startTime;

            averageTime += computationTime;
            count++;

            ltAverageTime = averageTime / count;
            
            if (averageTimeReportInterval > 0 &&
                    count >= averageTimeReportInterval) {
                count -= averageTimeReportInterval;
                System.err.println ("Average time: "+
                        averageTime/averageTimeReportInterval);
                averageTime = 0;
            }

            tau += delta - computationTime / NANOSECONDS_IN_MILLISECOND;
            
            long fullMillis;
            // If we are lagging behind do not stop
            if (tau < 0) {
                lagCount++;
                if (lagCount % 100 == 0) {

                  double estScale;
                  
                  if (count > 0)
                    estScale = aStepLength*NANOSECONDS_IN_MILLISECOND/(ltAverageTime);
                  else
                    estScale = 0.0;

                  // Make the time scale printable
                  estScale = Math.round(100*estScale) / 100.0;
                  
                  System.err.println ("Simulator is running behind... "+
                            "(actual time scale: "+estScale+")");
                }
                
                fullMillis = 0;
            }
            else {
              fullMillis = (long)Math.floor(tau);
              lagCount = 0;
            }
            
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
