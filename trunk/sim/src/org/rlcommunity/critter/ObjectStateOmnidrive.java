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

  /** 'Persistent' data */
  /** How much the PID should compensate for velocity errors; for now
    * a single value, most likely to be changed when other parts are in place 
    */
  protected double aPIDCoefficient;

  public ObjectStateOmnidrive()
  {
    this(0.5);
  }

  public ObjectStateOmnidrive(double pPIDCoeff)
  {
    aVel = new Vector2D(0,0);
    aAngVel = 0;
    aTimeSinceCommand = 0;

    aPIDCoefficient = pPIDCoeff;
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

  public double getPIDCoefficient() { return aPIDCoefficient; }


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

    this.aPIDCoefficient = org.aPIDCoefficient;
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
        int endX = (int)(objPos.x + aVel.x * Math.cos(objDir));
        int endY = (int)(objPos.y + aVel.x * Math.sin(objDir));

	      //g.drawLine((int)objPos.x, (int)objPos.y, endX, endY);
	      g.setColor(tempC);
      }
  }

  /** Provides a mean of clearing whatever data this ObjectState contains
    *  and resetting it to the default values. Meant to be used when 
    *  re-initializing a state.
    * 
    * For the Omnidrive, we do not clearTransient the data per-se as we do want it 
    *  to be persistent across time steps.
    */
  public void clearTransient()
  {
  }

}
