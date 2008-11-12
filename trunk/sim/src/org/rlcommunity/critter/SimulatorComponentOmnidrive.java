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

public class SimulatorComponentOmnidrive implements SimulatorComponent
{
  public static final String NAME = "omnidrive";
  
  public static int aCommandlessTimeThreshold = 500; 

  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    // Get all objects with an omni drive
    LinkedList<SimulatorObject> drivable = pCurrent.getObjects(NAME);

    for (SimulatorObject thisObject : drivable)
    {
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

      // The Omnidrive state contains target velocities, while the 
      //  dynamics state contains actual velocities
      // @@@ properly deal with this
      Vector2D v = driveState.getVelocity();
      // Convert v into object-space
      double dir = thisObject.getDirection();
   
      // Produce a force to provide the required velocity (which is in m/s)
      Vector2D localVel = kinState.getVelocity().rotate(-dir+Math.PI/2);
      Force fPID = simpleXYPid(driveState, kinState, localVel);

      nextKinState.addForce (new Force(fPID.vec.rotate(dir-Math.PI/2)));
     
      // @@@ Should also be PID-driven
      double torquePID = simpleTPID(driveState, kinState);
      nextKinState.addTorque (torquePID);

      // Rather than "consuming" the action, which leads to very saccaded
      //  movements when commands are issued slowly, we kill it if 500ms
      /// ( for now, 50 steps) have elapsed since the last command
      ObjectStateOmnidrive nextDriveState = 
        (ObjectStateOmnidrive)futureObj.getState(NAME);
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
        // @@@ this may be taken out once copyFrom is actually used to copy
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
    Vector2D targetVel = driveData.getVelocity();
    double coeff = driveData.getPIDCoefficient();

    // @@@ include a last error, as in pid_control, to make things smoother
    Vector2D err = targetVel.minus(curVel);

    err.x = err.x * coeff;
    err.y = err.y * coeff;
    
    // Assume we 'know' friction, and bump the resulting force up
    // @@@ this is of course, terribly bad
    err.x = err.x + curVel.x * 0.1;
    err.y = err.y + curVel.y * 0.1;

    // Finally, cap the maximum force this PID produces to 10
    if (err.x > 10) err.x = 10;
    else if (err.x < -10) err.x = -10;
    if (err.y > 10) err.y = 10;
    else if (err.y < -10) err.y = -10;

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
    // @@@ take this out, of course
    res += curVel * 0.5;

    return res;
  }
}
