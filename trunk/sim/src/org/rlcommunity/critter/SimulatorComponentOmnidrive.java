package org.rlcommunity.critter;

/**
  * SimulatorComponentOmnidrive
  *
  * Defines the omni-drive component of the simulator. This component is in
  *  charge of generating forces due to commands to an omni-drive. Objects
  *  equipped with ObjectStateOmnidrive are assumed to have an omnidirectional
  *  drive attached to them and can be controlled as such.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;
import java.util.Random;

public class SimulatorComponentOmnidrive implements SimulatorComponent
{
  public static final String NAME = "omnidrive";
  
  public static int aCommandlessTimeThreshold = 500; 

  public static final double torqueGain = 10;
  public static final double thrustGain = 1;

    protected Random aRandom;

    public SimulatorComponentOmnidrive() {
        this(new Random());
        System.err.println("Deprecated: using local Random object.");
    }

    public SimulatorComponentOmnidrive(Random pRandom) {
        aRandom = pRandom;
    }

  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    // Get all objects with an omni drive
    LinkedList<SimulatorObject> drivable = pCurrent.getObjects(NAME);

    for (SimulatorObject thisObject : drivable)
    {
      // Small debug routine hidden in this code

      // We know they must contain a driveState
      ObjectStateOmnidrive driveState = 
        (ObjectStateOmnidrive)thisObject.getState(NAME);

      ObjectState os = thisObject.getState(SimulatorComponentDynamics.NAME);
      if (os == null) continue;

      ObjectStateDynamics kinState = (ObjectStateDynamics)os;

      // To actually produce force, we also need the physics component of the
      //  NEXT state!
      SimulatorObject futureObj = pNext.getObject(thisObject);
      if (futureObj == null) continue;

      os = futureObj.getState(SimulatorComponentDynamics.NAME);

      if (os == null) continue;
      ObjectStateDynamics nextKinState = (ObjectStateDynamics)os;
      ObjectStateOmnidrive nextDriveState =
        (ObjectStateOmnidrive)futureObj.getState(NAME);

      // The Omnidrive state contains target velocities, while the 
      //  dynamics state contains actual velocities
      // @todo properly deal with this
      Vector2D v = driveState.getVelocity();
      // Convert v into object-space
      double dir = thisObject.getDirection();
   
      // Produce a force to provide the required velocity (which is in m/s)
      Vector2D localVel = kinState.getVelocity().rotate(-dir);
      Force fPID = simpleXYPid(driveState, kinState, localVel);
      nextDriveState.setForce(fPID);
      fPID.vec.timesEquals(thrustGain);

      // Produce a torque to provide the required angular velocity
      double torquePID = simpleTPID(driveState, kinState);

      double thrustError = driveState.getThrustError();
      double torqueError = driveState.getTorqueError();

      Vector2D endForceVec = fPID.vec.rotate(dir);
      double endTorque = torquePID * torqueGain;

      // Add some relative Gaussian noise to the thrust and torque
      endForceVec.plusEquals(endForceVec.times(aRandom.nextGaussian() * thrustError));
      endTorque += endTorque * aRandom.nextGaussian() * torqueError;
      
      nextKinState.addForce (new Force(endForceVec));
      nextKinState.addTorque (endTorque);

      // Rather than "consuming" the action, which leads to very saccaded
      //  movements when commands are issued slowly, we kill it if 500ms
      /// ( for now, 50 steps) have elapsed since the last command
      nextDriveState.setTime(driveState.getTime()+delta);

      // If enough steps have elapsed, reset data
      if (driveState.getTime() >= aCommandlessTimeThreshold)
      {
        nextDriveState.clearTime();
        nextDriveState.setVelocity(new Vector2D(0, 0));
        nextDriveState.setAngVelocity(0);
      }
      else // Otherwise, copy over the target velocity and ang. velocity 
      {
        // @todo this may be taken out once copyFrom is actually used to copy
        //  from state to state
        nextDriveState.setVelocity((Vector2D)v.clone());
        nextDriveState.setAngVelocity(driveState.getAngVelocity());
      }
    }
  }

  /** Method for computing the force that the omnidrive generates given
    *  a target velocity. Not quite there though, but at least it
    *  (for now) achieves the desired velocity, rather than creating some
    *  force.
    */
  public Force simpleXYPid(ObjectStateOmnidrive driveData, 
    ObjectStateDynamics dynData, Vector2D curVel)
  {
    double maxForce = driveData.getMaxForce();
    Vector2D targetVel = driveData.getVelocity();
    double coeff = driveData.getPIDCoefficient();

    Force lastForce = driveData.getForce();
    Vector2D fVec = lastForce.vec;

    // @todo include a last error, as in pid_control, to make things smoother
    Vector2D err = targetVel.minus(curVel);

    err.x = err.x * coeff;
    err.y = err.y * coeff;
    
    // Add the previous force to the error velocity vector, but only if they
    //  are pointing in the same directory
    if (fVec.dot(err) > 0)
        err.plusEquals(fVec);

    // Finally, cap the maximum force this PID produces to 10
    if (err.x > maxForce) err.x = maxForce;
    else if (err.x < -maxForce) err.x = -maxForce;
    if (err.y > maxForce) err.y = maxForce;
    else if (err.y < -maxForce) err.y = -maxForce;

    return new Force(err);
  }

  /** Method for computing the torque necessary to achieve the desired
    * angular velocity. See comment in simpleXYPID above.
    */
  public double simpleTPID(ObjectStateOmnidrive driveData,
    ObjectStateDynamics dynData)
  {
    double targetVel = driveData.getAngVelocity();
    double curVel = dynData.getAngVelocity();

    double err = targetVel - curVel;
    double coeff = driveData.getPIDCoefficient();

    double res = err * coeff;
    // Compensate for 'friction'
    // @todo take this out, of course
    res += curVel * 0.5;

    return res;
  }
}
