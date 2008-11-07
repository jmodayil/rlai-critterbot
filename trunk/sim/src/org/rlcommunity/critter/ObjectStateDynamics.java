package org.rlcommunity.critter;

/**
  * ObjectStateDynamics
  *
  * Defines properties of an object necessary for movement (eg for velocity,
 * collisions, friction, etc, etc).
  *
  * @author Marc G. Bellemare
 * @author Anna Koop
  */

import java.util.LinkedList;
import java.util.List;

public class ObjectStateDynamics implements ObjectState
{
  public static final double MIN_MASS = 0.000001; // 1 mg
  public static final double MIN_MOMENT_INERTIA = MIN_MASS * 1; // 1 mg m^2

  /** Dynamics state */
  /** Object velocity, in m/s */
  protected Vector2D aVel;
  /** Forces that will be applied to the object this time step (in N) */
  protected LinkedList<Force> aForces;
 
  /** Angular velocity, in rad/s */
  protected double aAngVel;
  /** Torque, in ? */
  protected double aTorque;

  /** Object mass, in kg */ 
  protected double aMass;
  /** Object moment of inertia, in kg m^2 */
  protected double aMomI;

  /** Creates a new dynamics state component with a particular mass and 
    *  moment of inertia.
    *
    * @param pMass The mass of the object to which this state component belongs,
    *   in kilograms
    * @param pMomentI The moment of inertia of the object to which this state
    *   component belongs, in kg m^2
    */
  public ObjectStateDynamics(double pMass, double pMomentInertia)
  {
    aMass = pMass;
    aMomI = pMomentInertia;

    aVel = new Vector2D(0,0);
    aAngVel = aTorque = 0;
  }

  /** Creates a nearly massless Dynamics state. Because a minimum mass 
    *  is recommended by classical physics, we use it here as well.
    *  Massless objects (e.g. invisible light, magnetic, etc, sources)
    *  should not be given an ObjectStateDynamics.
    */
  public ObjectStateDynamics()
  {
    this(MIN_MASS,MIN_MOMENT_INERTIA);
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

  public double getAngVelocity() { return aAngVel; }
  public void setAngVelocity(double v) { aAngVel = v; }

  public double getTorque() { return aTorque; }
  public void addTorque(double t) { aTorque += t; }
  public void setTorque(double t) { aTorque = t; }

  public void clearTorque() { aTorque = 0; }

  public void setMass(double m) { aMass = m; }
  public double getMass() { return aMass; }
  
  public void setMomentInertia(double m) { aMomI = m; }
  public double getMomentInertia() { return aMomI; }

  /** ObjectState interface */
  public String getName() { return SimulatorComponentDynamics.NAME; }

  public Object clone()
  {
    ObjectStateDynamics newDyn = new ObjectStateDynamics();
    newDyn.copyFrom(this);

    return newDyn;
  }

  protected void copyFrom(ObjectState os)
  {
    ObjectStateDynamics dyn = (ObjectStateDynamics) os;
    
    this.aVel = (Vector2D) dyn.aVel.clone();
    this.aAngVel = dyn.aAngVel;

    this.aMass = dyn.aMass;
    this.aMomI = dyn.aMomI;

    // Should we copy the forces over? by definition we shouldn't carry
    //  them from state to state, but...
  }
}

