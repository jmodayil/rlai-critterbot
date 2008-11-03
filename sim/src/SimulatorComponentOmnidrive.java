package org.rl.community.critter;

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

  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    // Get all objects with an omni drive
    LinkedList<SimulatorObject> drivable = pCurrent.getObjects(NAME);

    for (SimulatorObject o : drivable)
    {
      // We know they must contain a driveState
      ObjectStateOmnidrive driveState = (ObjectStateOmnidrive)o.getState(NAME);

      // To actually produce force, we also need the physics component of the
      //  NEXT state!
      SimulatorObject futureObj = pNext.getObject(o.getId());
      if (futureObj == null) continue;

      ObjectState os = futureObj.getState(SimulatorComponentKinematics.NAME);

      if (os == null) continue;
      ObjectStateKinematics kinState = (ObjectStateKinematics)os;

      // The Omnidrive state contains target velocities, while the 
      //  Kinematics state contains actual velocities
      // @@@ properly deal with this
      kinState.addForce (new Force(driveState.getVelocity()));
      kinState.addTorque (driveState.getAngVelocity());

      // For now, "consume" the action by setting the next state's action
      //  back to 0
      ObjectStateOmnidrive nextDriveState = 
        (ObjectStateOmnidrive)futureObj.getState(NAME);
      nextDriveState.setVelocity(new Vector2D(0,0));
      nextDriveState.setAngVelocity(0);
    }
  }
}
