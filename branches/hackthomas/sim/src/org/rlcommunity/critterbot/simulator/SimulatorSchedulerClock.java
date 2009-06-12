/*
 * (c) 2009 Marc G. Bellemare.
 */

package org.rlcommunity.critterbot.simulator;

/** The clock part of the SimulatorScheduler. Keeps track of how fast the
 *   system is running compared to how fast it should run.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class SimulatorSchedulerClock {
    public final int NANOSECONDS_IN_MILLISECOND = 1000000;

    protected long aStartNanos;
    protected double tau;
    protected double averageTime;
    protected long count;
    protected boolean isBehindFlag;
    
    public SimulatorSchedulerClock() {
      tau = 0;
    }

    public boolean isBehind() { return isBehindFlag; }

    /** Tells the clock to start counting how long a given operation takes.
     * 
     */
    public void startClock() {
      startClock(System.nanoTime());
    }
    
    public void startClock(long pNanos) {
      aStartNanos = pNanos;
    }

    /** Tells the clock to stop ticking.
     *
     * @param pDelta The desired number of milliseconds for the clocked
     *  operation.
     * @return The actual number of milliseconds we should sleep.
     */
    public long endClock(double pDelta) {
      return endClock(pDelta, System.nanoTime());
    }

    public long endClock(double pDelta, long pNanos) {
      long computationTime = pNanos - aStartNanos;

      averageTime += computationTime;
      count++;
      tau += pDelta - (1.0 * computationTime) / NANOSECONDS_IN_MILLISECOND;

      long fullMillis;

      // If we are lagging behind do not stop
      if (tau < 0) {
        isBehindFlag = true;
        fullMillis = 0;
      }
      else {
        isBehindFlag = false;
        fullMillis = (long) Math.floor(tau);
      }

      tau -= fullMillis;

      return fullMillis;
    }

    public double getAverageTime() {
      return averageTime / (NANOSECONDS_IN_MILLISECOND * count);
    }

    public void resetAverageTime() { averageTime = 0; count = 0; }
}
