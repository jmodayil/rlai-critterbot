/**
  * ObjectStatePhysics
  *
  * Defines physics-related properties of an object.
  *
  * @author Marc G. Bellemare
  */

import java.util.LinkedList;
import java.util.List;

public class ObjectStatePhysics implements ObjectState
{
  protected Vector2D aVel;
  protected LinkedList<Vector2D> aForces;
  
  protected double aRot;
  protected double aTorque;

  public ObjectStatePhysics()
  {
    aVel = new Vector2D(0,0);
    aRot = aTorque = 0;
  }

  public Vector2D getForceSum()
  { 
    Vector2D sum = new Vector2D(0,0);

    if (aForces == null) return sum;

    for (Vector2D f : aForces)
      sum.addEquals(f);

    return sum;
  }

  public List<Vector2D> getForces()
  {
    return aForces;
  }

  public void addForce(Vector2D f)
  {
    if (aForces == null) aForces = new LinkedList<Vector2D>();

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
  public void setTorque(double t) { aTorque = t; }

  // @@@ add: forces should be a list which gets compounded
  // @@@    torque should have corr. methods addTorque to compound torques

  /** ObjectState interface */
  public String getName() { return SimulatorComponentPhysics.NAME; }

  public Object clone()
  {
    ObjectStatePhysics newPhys = new ObjectStatePhysics();
    newPhys.copyFrom(this);

    return newPhys;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStatePhysics phys = (ObjectStatePhysics) os;
    
    this.aVel = (Vector2D) phys.aVel.clone();
    // Should we copy the forces over? by definition we shouldn't carry
    //  them from state to state, but...

    this.aRot = phys.aRot;
    this.aTorque = phys.aTorque;
  }
}

