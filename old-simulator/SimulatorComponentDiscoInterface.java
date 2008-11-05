/**
  * SimulatorComponentDiscoInterface
  *
  * A (very long-named) class that converts drops received from Disco
  *  (via a DropServer) into simulator data, and vice-versa.
  *
  * @author Marc G. Bellemare
  */

import java.util.List;

public class SimulatorComponentDiscoInterface implements SimulatorComponent
{
  public static final String NAME = "discointerface";

  protected DropServer aSubjServer;
  protected DropServer aObjServer;

  /** A simple method of slowing down the speed at which drops are sent;
    * needs to be fixed */
  public int aStateThrottle;

  public SimulatorComponentDiscoInterface(DropServer subjectiveServer,
    DropServer objectiveServer)
  {
    aSubjServer = subjectiveServer;
    aObjServer = objectiveServer;
  }

  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    if (aSubjServer != null)
    {
      List<SimulatorDrop> drops = aSubjServer.receiveDrops();
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
          aSubjServer.sendDrop(makeStateDrop(agent));
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
      stateDrop.accel.x = (int)(f.vec.x * 1000);
      stateDrop.accel.y = (int)(f.vec.y * 1000);
    }

    return stateDrop;
  }

}
