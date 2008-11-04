package org.rl.community.critter;

/**
  * ObjectStateOmnidrive
  *
  * Defines what constitutes the state of an object with an omnidirectional
  *  drive.
  *
  * @author Marc G. Bellemare
  */

public class ObjectStateOmnidrive implements ObjectState
{
  /** Target velocity */
  protected Vector2D aVel;
  /** Target angular velocity */
  protected double aAngVel;

  public ObjectStateOmnidrive()
  {
    aVel = new Vector2D(0,0);
    aAngVel = 0;
  }

  /** Returns the target velocity for this omni-drive
    *
    * @return Target velocity
    */
  public Vector2D getVelocity() { return aVel; }

  /** Returns the target angular velocity for this omni-drive
    *
    * @return Target angular velocity
    */
  public double getAngVelocity() { return aAngVel; }

  // @@@ to copy pVel or not to copy?
  /** Sets the target velocity for this omni-drive
    * @param pVel New target velocity
    */
  public void setVelocity(Vector2D pVel) { aVel = new Vector2D(pVel); }

  /** Sets the target angular velocity for this omni-drive
    * @param pAngVel New target angular velocity
    */
  public void setAngVelocity(double pAngVel) { aAngVel = pAngVel; }

  /** Copies over the relevant data from the given drop. Should probably
    *  be moved somewhere else, e.g. into a separate object which transforms
    *  drops into states.
    *
    * @param pDrop The drop containing the data of interest
    */
  public void setFromDrop(CritterControlDrop pDrop)
  {
    switch (pDrop.motor_mode)
    {
      case XYTHETA_SPACE:
        aVel = new Vector2D(pDrop.x_vel, pDrop.y_vel);
        aAngVel = pDrop.theta_vel;
        break;
      case WHEEL_SPACE:
      default:
        System.err.println ("Unimplemented motor mode: "+pDrop.motor_mode);
        break;
    }
  }


  /** ObjectState interface */
  public String getName()
  {
    return SimulatorComponentOmnidrive.NAME;
  }

  public Object clone()
  {
    ObjectStateOmnidrive newDrive = new ObjectStateOmnidrive();
    newDrive.copyFrom(this);
    
    return newDrive;
  }

  protected void copyFrom(ObjectStateOmnidrive org)
  {
    this.aVel = new Vector2D(org.aVel);
    this.aAngVel = org.aAngVel;
  }
}
