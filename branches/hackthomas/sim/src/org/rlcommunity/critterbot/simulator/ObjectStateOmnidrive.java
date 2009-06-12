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

package org.rlcommunity.critterbot.simulator;

/**
 * ObjectStateOmnidrive
 *
 * Defines what constitutes the state of an object with an omnidirectional
 *  drive.
 *
 * @author Marc G. Bellemare
 */
import org.rlcommunity.critterbot.javadrops.drops.CritterControlDrop;

import java.awt.Graphics;
import java.awt.Color;

public class ObjectStateOmnidrive implements ObjectState {

    /** Target velocity */
    protected Vector2D aVel;
    /** Target angular velocity */
    protected double aAngVel;
    /** For PID control, not quite what we want */
    protected Force aForce;
    protected int aTimeSinceCommand;
    /** 'Persistent' data */
    /** How much the PID should compensate for velocity errors; for now
     * a single value, most likely to be changed when other parts are in place
     */
    protected double aPIDCoefficient;
    /** The maximum force that this drive can generate */
    protected double aMaxForce;
    /** The relative error on thrust generated by this drive */
    protected double aThrustError;
    /** The relative error on torque generated by this drive */
    protected double aTorqueError;

    public static final double defaultPIDCoefficient = 0.5;
    public static final double defaultMaxForce = 10.0;
    public static final double defaultThrustError = 0.1;
    public static final double defaultTorqueError = 0.1;
    
    public ObjectStateOmnidrive() {
        this(defaultPIDCoefficient, defaultMaxForce,
                defaultThrustError, defaultTorqueError);
    }

    public ObjectStateOmnidrive(double pPIDCoeff, double pMaxForce,
            double pThrustError, double pTorqueError) {
        aVel = new Vector2D(0, 0);
        aForce = new Force(0, 0);
        aAngVel = 0;
        aTimeSinceCommand = 0;

        aPIDCoefficient = pPIDCoeff;
        aMaxForce = pMaxForce;
        aThrustError = pThrustError;
        aTorqueError = pTorqueError;
    }

    /** Returns the target velocity for this omni-drive
     *
     * @return Target velocity
     */
    public Vector2D getVelocity() {
        return aVel;
    }

    /** Returns the target angular velocity for this omni-drive
     *
     * @return Target angular velocity
     */
    public double getAngVelocity() {
        return aAngVel;
    }

    /** Sets the target velocity for this omni-drive
     * @param pVel New target velocity
     */
    public void setVelocity(Vector2D pVel) {
        assert (pVel != null);
        aVel = new Vector2D(pVel);
    }

    /** Sets the target angular velocity for this omni-drive
     * @param pAngVel New target angular velocity
     */
    public void setAngVelocity(double pAngVel) {
        aAngVel = pAngVel;
    }

    /** Returns the last force that the "PID loop" generated
     *
     * @return Force generated by the PID loop.
     */
    public Force getForce() {
        return aForce;
    }

    /** Sets the force generated by the PID loop.
     *
     * @param pForce The force generated by the PID loop.
     */
    public void setForce(Force pForce) {
        aForce = pForce;
    }

    /**
     * Returns the relative error on thrust. An error of 0.05 indicates a standard
     *   deviation of 0.05 * thrust.
     *
     * @return The relative error on thrust.
     */
    public double getThrustError() {
        return aThrustError;
    }

    /**
     * Returns the relative error on torque. An error of 0.05 indicates a standard
     *   deviation of 0.05 * torque.
     *
     * @return The relative error on torque.
     */
    public double getTorqueError() {
        return aTorqueError;
    }
    
    /**
     * Sets the time-since-command counter.
     *
     * @param pMillis The new number of ms since the last command was
     *  given.
     */
    public void setTime(int pMillis) {
        aTimeSinceCommand = pMillis;
    }

    /** Resets the time-since-command counter.
     */
    public void clearTime() {
        aTimeSinceCommand = 0;
    }

    public int getTime() {
        return aTimeSinceCommand;
    }

    /** Return the PID coefficient used to generate forces for this drive.
     *
     * @return The PID coefficient for this Omnidrive.
     */
    public double getPIDCoefficient() {
        return aPIDCoefficient;
    }

    /** Returns the maximum force that this drive can generate.
     *
     * @return Maximum force generated by this Omnidrive.
     */
    public double getMaxForce() {
        return aMaxForce;
    }

    /** ObjectState interface */
    public String getName() {
        return SimulatorComponentOmnidrive.NAME;
    }

    public Object clone() {
        ObjectStateOmnidrive newDrive = new ObjectStateOmnidrive();
        newDrive.copyFrom(this);

        return newDrive;
    }

    protected void copyFrom(ObjectStateOmnidrive org) {
        this.aVel = new Vector2D(org.aVel);
        this.aForce = new Force(org.aForce);
        this.aAngVel = org.aAngVel;

        this.aTimeSinceCommand = org.aTimeSinceCommand;
        this.aPIDCoefficient = org.aPIDCoefficient;
        this.aThrustError = org.aThrustError;
        this.aTorqueError = org.aTorqueError;
    }

    public void draw(SimulatorGraphics g, SimulatorObject parent) {
        // protected Vector2D aVel;
        Vector2D objPos = parent.getPosition();
        double objDir = parent.getDirection();

        if (aVel.x != 0) {
            Color tempC = g.getColor();
            g.setColor(Color.red);
            // We're only drawing the x component here!
            int endX = (int) (objPos.x + aVel.x * Math.cos(objDir));
            int endY = (int) (objPos.y + aVel.x * Math.sin(objDir));

            //g.drawLine((int)objPos.x, (int)objPos.y, endX, endY);
            g.setColor(tempC);
        }
    }

    /** Provides a mean of clearing whatever data this ObjectState contains
     *  and resetting it to the default values. Meant to be used when
     *  re-initializing a state.
     *
     * For the Omnidrive, we do not clearTransient the data per-se as we do want it
     *  to be persistent across time steps.
     */
    public void clearTransient() {
    }
}
