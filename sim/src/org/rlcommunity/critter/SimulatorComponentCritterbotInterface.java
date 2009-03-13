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

package org.rlcommunity.critter;

/**
 * SimulatorComponentCritterbotInterface
 *
 * A (very long-named) class that converts drops received from Disco
 *  (via a DropServer) into simulator data, and vice-versa.
 *
 * @author Marc G. Bellemare
 */
import org.rlcommunity.critter.Drops.*;

import java.util.List;

public class SimulatorComponentCritterbotInterface implements SimulatorComponent {

    private class MotorCommand {

        public double motor100 = 0;
        public double motor220 = 0;
        public double motor340 = 0;

        public void setCommandValue(Vector2D velocity, Double angleVelocity) {
            double xvel = velocity.x;
            double yvel = velocity.y;
            double tvel = angleVelocity;

            motor100 = xvel * XYT2MS[0][0] + yvel * XYT2MS[0][1] + tvel * XYT2MS[0][2];
            motor220 = xvel * XYT2MS[1][0] + yvel * XYT2MS[1][1] + tvel * XYT2MS[1][2];
            motor340 = xvel * XYT2MS[2][0] + yvel * XYT2MS[2][1] + tvel * XYT2MS[2][2];

        }

        @Override
        public String toString() {
            return String.format("100: %s; 220: %s; 340: %s",
                    motor100, motor220, motor340);
        }
    }
    /** Two matrices defining the transformations from XYT space to
     *   wheel space and vice-versa.
     */
    private static final double[][] XYT2MS = {{-0.98, -0.17, 1.07},
        {0.64, -0.76, 1.07},
        {0.34, 0.93, 1.07}};
    private static final double[][] MS2XYT = {{-0.65, 0.42, 0.22},
        {-0.11, -0.51, 0.62},
        {0.31, 0.31, 0.31}};
    
    private static final double motorCommandFactor = 10;
    private static final double motorSpeedFactor = 12;
    private static final double motorIntensity = 5;

    public static final String NAME = "critterbot_interface";

    // Scales for the different drop values
    /** Accelerometer data is in g / 1024 */
    public static final double ACCEL_SCALE =
            1024.0 / ObjectStateDynamics.GRAVITY;
    public static final int ACCEL_MIN = -2048;
    public static final int ACCEL_MAX = 2048;

    public static final double XY_VELOCITY_SCALE = 100.0;
    public static final double ANG_VELOCITY_SCALE = 9.0;
    // All of these need to be made proper
    public static final double GYRO_SCALE = 1024.0 / (Math.PI * 2);
    public static final double LIGHT_SCALE = 1.0;
    public static final double BATTERY_SCALE = 1.0;
    public static final double IRDIST_SCALE = 255.0;
    public static final double BUMP_SENSOR_SCALE = 100.0;
    public static final double BUMP_SENSOR_MAX = 255.0;

    protected DropInterface aDropInterface;
    private final int numBatteries = 3;

    private final MotorCommand aCommand = new MotorCommand();

    public SimulatorComponentCritterbotInterface(DropInterface pInterface) {
        aDropInterface = pInterface;
    }

    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) {
        if (aDropInterface != null) {
            // Read in drops
            List<SimulatorDrop> drops = aDropInterface.receiveDrops();
            List<SimulatorObject> critters =
                    pCurrent.getObjects(SimulatorComponentCritterbotInterface.NAME);

            for (SimulatorObject obj : critters) {
                ObjectStateCritterbotInterface iface = (ObjectStateCritterbotInterface) obj.getState(ObjectStateCritterbotInterface.NAME);

                SimulatorObject nextObj = pNext.getObject(obj);
                ObjectStateCritterbotInterface nextIface =
                        (ObjectStateCritterbotInterface) nextObj.getState(ObjectStateCritterbotInterface.NAME);

                for (SimulatorDrop drop : drops) {
                    // @todo icky
                    if (drop instanceof CritterControlDrop) {
                        CritterControlDrop command = (CritterControlDrop) drop;

                        // Set the NEXT object's Omnidrive data
                        if (command != null) {
                            setFromDrop(nextObj, command);
                        }
                    }
                }

                // Increment the interface timer
                nextIface.setLastStateUpdate(iface.getLastStateUpdate() + delta);

                // Send the system state, if necessary
                if (nextIface.needsStateUpdate()) {
                    aDropInterface.sendDrop(makeStateDrop(pCurrent, obj));
                    nextIface.setLastStateUpdate(nextIface.getLastStateUpdate() -
                            iface.getStateDropFrequency());
                }
            }

        }
    }

    /** Copies over the relevant data from the given drop. Should probably
     *  be moved somewhere else, e.g. into a separate object which transforms
     *  drops into states.
     *
     *  IMPORTANT: If this gets removed, clearTime() should be called after
     *   setting the drive values.
     *
     * @param pDrop The drop containing the data of interest
     */
    public void setFromDrop(SimulatorObject pObj, CritterControlDrop pDrop) {
        // Assume, for now, that this Critterbot has a Omnidrive; do nothing
        //  otherwise
        if (pObj.getState(SimulatorComponentOmnidrive.NAME) == null) {
            return;
        }

        if (pDrop.motor_mode == null) {
            return;
        }

        ObjectStateOmnidrive driveData = (ObjectStateOmnidrive) pObj.getState(SimulatorComponentOmnidrive.NAME);

        // Clear the number of steps since the last command
        driveData.clearTime();

        Vector2D velocity = null;
        Double angleVelocity = null;

        // Based on the motor mode, set velocities appropriately
        switch (pDrop.motor_mode) {
            case XYTHETA_SPACE:
                // The drop x,y velocity has units in cm/s
                velocity = new Vector2D(
                        pDrop.x_vel / XY_VELOCITY_SCALE,
                        pDrop.y_vel / XY_VELOCITY_SCALE);
                // Units for the drop's angular velocity are in 1/(18PI) of a circle
                //  per second, which is 1/9th of a radian per second
                angleVelocity = pDrop.theta_vel / ANG_VELOCITY_SCALE;
                break;
            case WHEEL_SPACE:
                double m100 = pDrop.m100_vel;
                double m220 = pDrop.m220_vel;
                double m340 = pDrop.m340_vel;

                double xvel = m100 * MS2XYT[0][0] + m220 * MS2XYT[0][1] + m340 * MS2XYT[0][2];
                double yvel = m100 * MS2XYT[1][0] + m220 * MS2XYT[1][1] + m340 * MS2XYT[1][2];
                double tvel = m100 * MS2XYT[2][0] + m220 * MS2XYT[2][1] + m340 * MS2XYT[2][2];

                velocity = new Vector2D(xvel / XY_VELOCITY_SCALE, yvel / XY_VELOCITY_SCALE);
                angleVelocity = tvel / ANG_VELOCITY_SCALE;
                break;
            default:
                System.err.println("Unimplemented motor mode: " + pDrop.motor_mode);
                break;
        }
        driveData.setVelocity(velocity);
        driveData.setAngVelocity(angleVelocity);
        aCommand.setCommandValue(velocity, angleVelocity);
    }

    private int computeIntensity(double command, double actual) {
        double intensity = Math.abs(command) + Math.abs(command - actual);
        return (int) (intensity * motorIntensity);
    }

    public CritterStateDrop makeStateDrop(SimulatorState pState, SimulatorObject pObject) {

        CritterStateDrop stateDrop = new CritterStateDrop();

        // Some bogus percentage
        stateDrop.cycle_time = 50;

        // Set the command value
        stateDrop.motor100.command = (int) (aCommand.motor100 * motorCommandFactor);
        stateDrop.motor220.command = (int) (aCommand.motor220 * motorCommandFactor);
        stateDrop.motor340.command = (int) (aCommand.motor340 * motorCommandFactor);

        // Compute fake intensity
        ObjectStateDynamics driveData = (ObjectStateDynamics) pObject.getState(SimulatorComponentDynamics.NAME);
        MotorCommand fakeCommand = new MotorCommand();
        fakeCommand.setCommandValue(driveData.getVelocity(), driveData.getAngVelocity());

        stateDrop.motor100.velocity = (int) (aCommand.motor100 * motorSpeedFactor);
        stateDrop.motor220.velocity = (int) (aCommand.motor220 * motorSpeedFactor);
        stateDrop.motor340.velocity = (int) (aCommand.motor340 * motorSpeedFactor);

        stateDrop.motor100.current = computeIntensity(aCommand.motor100, fakeCommand.motor100);
        stateDrop.motor220.current = computeIntensity(aCommand.motor220, fakeCommand.motor220);
        stateDrop.motor340.current = computeIntensity(aCommand.motor340, fakeCommand.motor340);

        // Get all of this object's light sensors
        List<SimulatorObject> sensors =
                pObject.getChildren(ObjectStateLightSensor.NAME);

        // Fill in the light data array
        int idx = 0;
        for (SimulatorObject sen : sensors) {
            ObjectStateLightSensor sData = (ObjectStateLightSensor) sen.getState(ObjectStateLightSensor.NAME);
            stateDrop.light[idx] = (int) (sData.getLightSensorValue() * LIGHT_SCALE);
            // Don't add more light data than we have space
            if (++idx == stateDrop.light.length) {
                break;
            }
        }

        sensors = pObject.getChildren(ObjectStateBattery.NAME);
        idx = 0;
        for (SimulatorObject sen : sensors) {        
            ObjectStateBattery bData = (ObjectStateBattery)sen.getState(ObjectStateBattery.NAME);
            // assume the batteries are discharging together for now...
            stateDrop.batv40 = (int) (bData.getCurrentCharge() * BATTERY_SCALE);
            stateDrop.batv160 = (int) (bData.getCurrentCharge() * BATTERY_SCALE);
            stateDrop.batv280 = (int) (bData.getCurrentCharge() * BATTERY_SCALE);

            // @todo should be SHORE when charging
            stateDrop.power_source = CritterStateDrop.PowerSource.BAT40;

            idx += 3;
            // Don't add more light data than we have space
            if (++idx >= numBatteries) {
                break;
            }            
        }
        
        sensors = pObject.getChildren(ObjectStateIRDistanceSensor.NAME);

        idx = 0;
        for (SimulatorObject sen : sensors) {
            ObjectStateIRDistanceSensor sData = (ObjectStateIRDistanceSensor) sen.getState(ObjectStateIRDistanceSensor.NAME);
            // The IR sensors are special: they produce a MAX value when the
            //  distance read is 0, and a MIN value at max range
            double dist = sData.getSensorValue();
            double range = sData.getRange();

            stateDrop.ir_distance[idx] = (int) ((range - dist) * IRDIST_SCALE / range);

            // Don't add more light data than we have space
            if (++idx == stateDrop.ir_distance.length) {
                break;
            }
        }

        // Add accelerometer data
        ObjectState os = pObject.getState(ObjectStateAccelerometer.NAME);

        if (os != null) {
            ObjectStateAccelerometer sData = (ObjectStateAccelerometer) os;
            Vector2D xyAccel = sData.getSensorValue();
            double zAccel = sData.getZSensorValue();

            stateDrop.accel.x = Math.max(ACCEL_MIN, 
                    Math.min((int) (xyAccel.x * ACCEL_SCALE), ACCEL_MAX));
            stateDrop.accel.y = Math.max(ACCEL_MIN,
                    Math.min((int) (xyAccel.y * ACCEL_SCALE), ACCEL_MAX));
            stateDrop.accel.z = Math.max(ACCEL_MIN,
                    Math.min((int) (zAccel * ACCEL_SCALE), ACCEL_MAX));
        }

        os = pObject.getState(ObjectStateGyroscope.NAME);

        if (os != null) {
            ObjectStateGyroscope sData = (ObjectStateGyroscope) os;
            double angVel = sData.getSensorValue();

            stateDrop.rotation = (int) (angVel * GYRO_SCALE);
        }

        ObjectStateBumpSensor bumpState = ObjectStateBumpSensor.retrieve(pObject);
        // Assume stateDrop.bump is initialized to 0's
        int numEdges = pObject.getShape().getPoints().size();
        int numSensors = stateDrop.bump.length;

        for (ObjectStateBumpSensor.BumpSensorData data : bumpState.getData()) {
            double sensor = (data.alpha / numEdges) * numSensors;
            // Interpolate between the two closest bump sensors
            int lowSensor = (int) Math.floor(sensor);
            int highSensor = (int) Math.ceil(sensor);

            // If lowSensor >= numSensors, something is wrong
            assert (lowSensor < numSensors);

            if (highSensor >= numSensors) {
                highSensor -= numSensors;
            }

            double alpha = sensor - lowSensor;
            double val = Math.min(data.magnitude * BUMP_SENSOR_SCALE, BUMP_SENSOR_MAX);

            stateDrop.bump[lowSensor] = (int) ((1 - alpha) * val);
            stateDrop.bump[highSensor] = (int) (alpha * val);
        }

        return stateDrop;
    }
}
