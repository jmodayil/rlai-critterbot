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

    private static final double cos100 = Math.cos(Math.toRadians(100));
    private static final double sin100 = Math.sin(Math.toRadians(100));
    private static final double cos220 = Math.cos(Math.toRadians(220));
    private static final double sin220 = Math.sin(Math.toRadians(220));
    private static final double cos340 = Math.cos(Math.toRadians(340));
    private static final double sin340 = Math.sin(Math.toRadians(340));
    
    public static final String NAME = "critterbot_interface";

    // Scales for the different drop values
    /** Accelerometer data is in g / 1024 */
    public static final double ACCEL_SCALE =
            1024.0 / ObjectStateDynamics.GRAVITY;
    public static final double XY_VELOCITY_SCALE = 100.0;
    public static final double ANG_VELOCITY_SCALE = 9.0;
    // All of these need to be made proper
    public static final double GYRO_SCALE = 1024.0 / (Math.PI * 2);
    public static final double LIGHT_SCALE = 100.0;
    public static final double IRDIST_SCALE = 255.0;

    protected DropInterface aDropInterface;

    public static final double BUMP_SENSOR_SCALE  = 1000.0;

    public SimulatorComponentCritterbotInterface(DropInterface pInterface) {
        aDropInterface = pInterface;
    }

    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) {
        if (aDropInterface != null) {
            List<SimulatorDrop> drops = aDropInterface.receiveDrops();
            List<SimulatorObject> critters =
                    pCurrent.getObjects(this.NAME);

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
                        if (command != null)
                            setFromDrop(nextObj, command);
                    }
                }

                // Send the system state, if necessary
                while (iface.needsStateUpdate()) {
                    aDropInterface.sendDrop(makeStateDrop(obj));
                }

                // Increment the interface timer
                nextIface.setLastStateUpdate(iface.getLastStateUpdate() + delta);

            }

        }
    }

    /* Fast approximation of the robot move */
    public Vector2D computeVelocity(int m100Vel, int m220Vel, int m340Vel) {
        Vector2D velocity = new Vector2D();
        velocity.x += m100Vel * cos100;
        velocity.y += m100Vel * sin100;
        velocity.x += m220Vel * cos220;
        velocity.y += m220Vel * sin220;
        velocity.x += m340Vel * cos340;
        velocity.y += m340Vel * sin340;
        velocity.normalize();
        return velocity;
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

        // Based on the motor mode, set velocities appropriately
        switch (pDrop.motor_mode) {
            case XYTHETA_SPACE:
                // The drop x,y velocity has units in cm/s
                driveData.setVelocity(new Vector2D(
                        pDrop.x_vel / XY_VELOCITY_SCALE,
                        pDrop.y_vel / XY_VELOCITY_SCALE));
                // Units for the drop's angular velocity are in 1/(18PI) of a circle
                //  per second, which is 1/9th of a radian per second
                // @todo 9 -> constant
                driveData.setAngVelocity(pDrop.theta_vel / ANG_VELOCITY_SCALE);
                break;
            case WHEEL_SPACE:
                Vector2D aVel = computeVelocity(pDrop.m100_vel,
                        pDrop.m220_vel,
                        pDrop.m340_vel);
                int engineSum = pDrop.m100_vel + pDrop.m220_vel + pDrop.m340_vel;
                double speedFactor = 1.0;
                aVel.x *= engineSum * speedFactor;
                aVel.y *= engineSum * speedFactor;
                driveData.setVelocity(aVel);
                double aAngVel = engineSum * 2;
                driveData.setAngVelocity(aAngVel);
                break;
            default:
                System.err.println("Unimplemented motor mode: " + pDrop.motor_mode);
                break;
        }
    }

    public CritterStateDrop makeStateDrop(SimulatorObject pObject) {
        CritterStateDrop stateDrop = new CritterStateDrop();

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

            stateDrop.accel.x = (int) (xyAccel.x * ACCEL_SCALE);
            stateDrop.accel.y = (int) (xyAccel.y * ACCEL_SCALE);
            stateDrop.accel.z = (int) (zAccel * ACCEL_SCALE);
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

            double alpha = sensor - lowSensor;
            stateDrop.bump[lowSensor] = (int) ((1 - alpha) * data.magnitude * BUMP_SENSOR_SCALE);
            stateDrop.bump[highSensor] = (int) ((1 - alpha) * data.magnitude * BUMP_SENSOR_SCALE);
        }

        return stateDrop;
    }
}
