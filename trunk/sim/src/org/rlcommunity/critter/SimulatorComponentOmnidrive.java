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
      ObjectStateOmnidrive driveState = (ObjectStateOmnidrive)thisObject.getState(NAME);

      // To actually produce force, we also need the physics component of the
      //  NEXT state!
      SimulatorObject futureObj = pNext.getObject(thisObject);
      if (futureObj == null) continue;

      ObjectState os = futureObj.getState(SimulatorComponentDynamics.NAME);

      if (os == null) continue;
      ObjectStateDynamics kinState = (ObjectStateDynamics)os;

      // The Omnidrive state contains target velocities, while the 
      //  dynamics state contains actual velocities
      // @@@ properly deal with this
      Vector2D v = driveState.getVelocity();
      // Convert v into object-space
      double dir = thisObject.getDirection();
     
      kinState.addForce (new Force(
              new Vector2D(v.y * Math.cos(dir) + v.x * Math.sin(dir),
                           v.x * Math.cos(dir) - v.y * Math.sin(dir))));
      
      kinState.addTorque (driveState.getAngVelocity());

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
}
