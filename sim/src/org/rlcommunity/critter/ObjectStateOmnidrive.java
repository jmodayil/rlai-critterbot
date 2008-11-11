package org.rlcommunity.critter;

/**
  * ObjectStateOmnidrive
  *
  * Defines what constitutes the state of an object with an omnidirectional
  *  drive.
  *
  * @author Marc G. Bellemare
  */

import org.rlcommunity.critter.Drops.CritterControlDrop;

import java.awt.Graphics;
import java.awt.Color;

public class ObjectStateOmnidrive implements ObjectState
{
  /** Target velocity */
  protected Vector2D aVel;
  /** Target angular velocity */
  protected double aAngVel;

  protected int aTimeSinceCommand; 

  public ObjectStateOmnidrive()
  {
    aVel = new Vector2D(0,0);
    aAngVel = 0;
    aTimeSinceCommand = 0;
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

  /**
    * Sets the time-since-command counter.
    *
    * @param pSeconds The new number of ms since the last command was
    *  given.
    */
  public void setTime(int pMillis) { aTimeSinceCommand = pMillis; }
  /** Resets the time-since-command counter.
    */
  public void clearTime() { aTimeSinceCommand = 0; }

  public int getTime() { return aTimeSinceCommand; }

  /** Copies over the relevant data from the given drop. Should probably
    *  be moved somewhere else, e.g. into a separate object which transforms
    *  drops into states.
    *
    *  IMPORTANT: If this gets removed, clearTime() should be called after
    *   setting the drive values.
    *
    * @param pDrop The drop containing the data of interest
    */
  public void setFromDrop(CritterControlDrop pDrop)
  {
    // Clear the number of steps since the last command
    clearTime();

    // Based on the motor mode, set velocities appropriately
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

    this.aTimeSinceCommand = org.aTimeSinceCommand;
  }

  public void draw(Graphics g, SimulatorObject parent)
  {
      // protected Vector2D aVel;
      Vector2D objPos = parent.getPosition();
      double objDir = parent.getDirection();

      if (aVel.x != 0)
      {
	      Color tempC = g.getColor();
	      g.setColor(Color.red);
        // We're only drawing the x component here!
        int endX = (int)(objPos.x + aVel.x * Math.sin(objDir));
        int endY = (int)(objPos.y + aVel.x * Math.cos(objDir));

	      g.drawLine((int)objPos.x, (int)objPos.y, endX, endY);
	      g.setColor(tempC);
      }
  }
}
