/**
  * ObjectStatePhysics
  *
  * Defines physics-related properties of an object.
  *
  * @author Marc G. Bellemare
  */

public class ObjectStatePhysics implements ObjectState
{
  protected Vector2D aVel;
  protected Vector2D aForce;
  
  protected double aRot;
  protected double aTorque;

  public ObjectStatePhysics()
  {
    aForce = new Vector2D(0,0); 
    aVel = new Vector2D(0,0);
    aRot = aTorque = 0;
  }

  public Vector2D getForce() { return aForce; }
  public void setForce(Vector2D f) { aForce = f; }

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
    this.aForce = (Vector2D) phys.aForce.clone();

    this.aRot = phys.aRot;
    this.aTorque = phys.aTorque;
  }
}

