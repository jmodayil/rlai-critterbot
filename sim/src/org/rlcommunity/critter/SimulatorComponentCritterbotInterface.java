package org.rlcommunity.critter;

/**
  * SimulatorComponentCritterbotInterface
  *
  * A (very long-named) class that converts drops received from Disco
  *  (via a DropServer) into simulator data, and vice-versa.
  *
  * @author Marc G. Bellemare
  */

import java.util.List;

public class SimulatorComponentCritterbotInterface implements SimulatorComponent
{
  public static final String NAME = "critterbot_interface";

  protected DropInterface aDropInterface; 

  /** A simple method of slowing down the speed at which drops are sent;
    * needs to be fixed */
  public int aStateThrottle;

  public SimulatorComponentCritterbotInterface(DropInterface pInterface)
  {
    aDropInterface = pInterface;
  }

  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    if (aDropInterface != null)
    {
      List<SimulatorDrop> drops = aDropInterface.receiveDrops();
      List<SimulatorObject> drivable = 
        pNext.getObjects(SimulatorComponentOmnidrive.NAME);

      for (SimulatorDrop drop : drops)
      {
        // @@@ icky
        if (drop instanceof CritterControlDrop)
        {
          CritterControlDrop command = (CritterControlDrop)drop;
          for (SimulatorObject obj : drivable)
          {
            ObjectStateOmnidrive driveData = (ObjectStateOmnidrive)
              obj.getState(SimulatorComponentOmnidrive.NAME);

            driveData.setFromDrop(command);
          }
        }
      }
      
      // Send the system state
      // Technically we should do this with each object which... ?
      SimulatorAgent agent = pCurrent.getAgents().getFirst();
      
      if (agent != null)
      {
        if (++aStateThrottle >= 100)
        {
          aDropInterface.sendDrop(makeStateDrop(agent));
          aStateThrottle = 0;
        }
      }
    }
  }
        
  public CritterStateDrop makeStateDrop(SimulatorObject pObject)
  {
    CritterStateDrop stateDrop = new CritterStateDrop();
    
    ObjectState os;

    os = pObject.getState(SimulatorComponentKinematics.NAME);

    // Set the kinematics data
    if (os != null)
    {
      ObjectStateKinematics kinData = (ObjectStateKinematics)os;
      Force f = kinData.getForceSum();
      // @@@ Needs to be converted into proper units (I believe in g's)
//      stateDrop.accel.x = (int)(f.vec.x * 1000);
//      stateDrop.accel.y = (int)(f.vec.y * 1000);
      Vector2D V=kinData.getVelocity();
      stateDrop.accel.x=(int) V.x;
      stateDrop.accel.y=(int) V.y;
      
              
    }

    return stateDrop;
  }

}
