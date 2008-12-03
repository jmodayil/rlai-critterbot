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

public class SimulatorComponentCritterbotInterface implements SimulatorComponent
{
  public static final String NAME = "critterbot_interface";

  // Scales for the different drop values
  /** Accelerometer data is in g / 1024 */ 
  public static final double ACCEL_SCALE    = 
    1024.0 / ObjectStateDynamics.GRAVITY;

  public static final double LIGHT_SCALE    = 100.0;
  public static final double IRDIST_SCALE   = 100.0;

  protected DropInterface aDropInterface; 

  public SimulatorComponentCritterbotInterface(DropInterface pInterface)
  {
    aDropInterface = pInterface;
  }

  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    if (aDropInterface != null)
    {
      List<SimulatorDrop> drops = aDropInterface.receiveDrops();
      List<SimulatorObject> critters = 
        pCurrent.getObjects(this.NAME);

      for (SimulatorObject obj : critters)
      {
        ObjectStateCritterbotInterface iface = (ObjectStateCritterbotInterface)
          obj.getState(ObjectStateCritterbotInterface.NAME);
        
        SimulatorObject nextObj = pNext.getObject(obj);
        ObjectStateCritterbotInterface nextIface = 
          (ObjectStateCritterbotInterface) 
          nextObj.getState(ObjectStateCritterbotInterface.NAME);
    
        for (SimulatorDrop drop : drops)
        {
          // @@@ icky
          if (drop instanceof CritterControlDrop)
          {
            CritterControlDrop command = (CritterControlDrop)drop;
            
            // Set the NEXT object's Omnidrive data
            setFromDrop(nextObj, command);
          }
        }
        
        // Send the system state, if necessary
        while (iface.needsStateUpdate())
          aDropInterface.sendDrop(makeStateDrop(obj));
        
        // Increment the interface timer
        nextIface.setLastStateUpdate (iface.getLastStateUpdate() + delta);

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
  public void setFromDrop(SimulatorObject pObj, CritterControlDrop pDrop)
  {
    // Assume, for now, that this Critterbot has a Omnidrive; do nothing
    //  otherwise
    if (pObj.getState(SimulatorComponentOmnidrive.NAME) == null)
      return;

    ObjectStateOmnidrive driveData = (ObjectStateOmnidrive)
      pObj.getState(SimulatorComponentOmnidrive.NAME);
    
    // Clear the number of steps since the last command
    driveData.clearTime();

    // Based on the motor mode, set velocities appropriately
    switch (pDrop.motor_mode)
    {
      case XYTHETA_SPACE:
        // Units for the drop's x,y velocity are in cm/s, but for now 
        //  I'm putting them in m/s - don't forget to change it!
        // @units
        driveData.setVelocity(new Vector2D(pDrop.x_vel, pDrop.y_vel));
        // Units for the drop's angular velocity are in 1/(18PI) of a circle 
        //  per second, which is 1/9th of a radian per second
        driveData.setAngVelocity(pDrop.theta_vel/9.0);
        break;
      case WHEEL_SPACE:
      default:
        System.err.println ("Unimplemented motor mode: "+pDrop.motor_mode);
        break;
    }
  }


  public CritterStateDrop makeStateDrop(SimulatorObject pObject)
  {
    CritterStateDrop stateDrop = new CritterStateDrop();
    
    // Get all of this object's light sensors
    List<SimulatorObject> sensors = 
      pObject.getChildren(ObjectStateLightSensor.NAME);

    // Fill in the light data array
    int idx = 0;
    for (SimulatorObject sen : sensors)
    {
      ObjectStateLightSensor sData = (ObjectStateLightSensor)
        sen.getState(ObjectStateLightSensor.NAME);
      stateDrop.light[idx] = (int)(sData.getLightSensorValue() * LIGHT_SCALE);
      
      // Don't add more light data than we have space
      if (++idx == stateDrop.light.length) break;
    }

    sensors = pObject.getChildren(ObjectStateIRDistanceSensor.NAME);

    idx = 0;
    for (SimulatorObject sen : sensors)
    {
      ObjectStateIRDistanceSensor sData = (ObjectStateIRDistanceSensor)
        sen.getState(ObjectStateIRDistanceSensor.NAME);
      stateDrop.ir_distance[idx] = (int)(sData.getSensorValue() * IRDIST_SCALE);
      
      // Don't add more light data than we have space
      if (++idx == stateDrop.ir_distance.length) break;
    }

    // Add accelerometer data
    ObjectState os = pObject.getState(ObjectStateAccelerometer.NAME);

    if (os != null)
    {
      ObjectStateAccelerometer sData = (ObjectStateAccelerometer)os; 
      Vector2D xyAccel = sData.getSensorValue();
      double zAccel = sData.getZSensorValue();

      stateDrop.accel.x = (int)(xyAccel.x * ACCEL_SCALE);
      stateDrop.accel.y = (int)(xyAccel.y * ACCEL_SCALE);
      stateDrop.accel.z = (int)(zAccel * ACCEL_SCALE);
    }

    return stateDrop;
  }

}
