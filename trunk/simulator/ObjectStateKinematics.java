/**
  * ObjectStateKinematics
  *
  * Defines kinematics-related properties of an object.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;
import java.util.List;

public class ObjectStateKinematics implements ObjectState
{
  /** Kinematics state */
  protected Vector2D aVel;
  protected LinkedList<Force> aForces;
  
  protected double aRot;
  protected double aTorque;

  public ObjectStateKinematics()
  {
    aVel = new Vector2D(0,0);
    aRot = aTorque = 0;
  }

  /** Return the sum of forces acting on the object. Because it is meaningless
    *  to talk about a point of contact when many forces are in play,
    *  this new force has no 'source'.
    *
    * @return The sum of the forces
    */
  public Force getForceSum()
  { 
    Force sum = new Force(0,0);

    if (aForces == null) return sum;

    for (Force f : aForces)
      sum.vec.addEquals(f.vec);

    return sum;
  }

  public List<Force> getForces()
  {
    return aForces;
  }

  public void addForce(Force f)
  {
    if (aForces == null) aForces = new LinkedList<Force>();

    aForces.add(f);
  }

  public void clearForces()
  {
    if (aForces != null)
      aForces.clear();
  }

  public Vector2D getVelocity() { return aVel; }
  public void setVelocity(Vector2D v) { aVel = v; }

  public double getAngVelocity() { return aRot; }
  public void setAngVelocity(double v) { aRot = v; }

  public double getTorque() { return aTorque; }
  public void addTorque(double t) { aTorque += t; }
  public void setTorque(double t) { aTorque = t; }

  public void clearTorque() { aTorque = 0; }

  // @@@ add: forces should be a list which gets compounded
  // @@@    torque should have corr. methods addTorque to compound torques

  /** ObjectState interface */
  public String getName() { return SimulatorComponentKinematics.NAME; }

  public Object clone()
  {
    ObjectStateKinematics newKin = new ObjectStateKinematics();
    newKin.copyFrom(this);

    return newKin;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateKinematics kin = (ObjectStateKinematics) os;
    
    this.aVel = (Vector2D) kin.aVel.clone();
    this.aRot = kin.aRot;
    
    // Should we copy the forces over? by definition we shouldn't carry
    //  them from state to state, but...
  }
}

