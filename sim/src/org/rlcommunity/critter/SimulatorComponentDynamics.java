package org.rlcommunity.critter;

import java.util.List;
import java.util.Random;

/**
 * SimulatorComponentDynamics
 *
 * This SimulatorComponent encodes all the dynamics interactions. As well
 *  as modifying data for the ObjectStateDynamics state of objects, 
 *  it also is in charge of modifying position.
 *
 * @author Marc G. Bellemare
 * @author Anna Koop
 */
public class SimulatorComponentDynamics implements SimulatorComponent {

    public static final String NAME = "dynamics";
    public static final boolean collisionEnergySink = false;

    protected static boolean debugDetailedCollisions = false;
    protected static boolean debugCollisions = false;
    protected static boolean debugDynamicsData = false;

    protected Random aRandom;
    
    public SimulatorComponentDynamics() {
        this(new Random());
        System.err.println ("Deprecated: using local Random object.");
    }

    public SimulatorComponentDynamics(Random pRandom) {
        aRandom = pRandom;
    }

    /** Applies physics to the current state to obtain the next state.
     * Roughly speaking, the following happens (in order):
     * 
     *  1. Go through the list of forces for each object, assume each 
     *  force was applied for a unit of time, and calculate the
     *  resulting change in velocity and position. Store these values in pNext.
     *  2. Check for collisions in pNext. For every collision detected, 
     *  calculate the collision response as a force in pNext 
     *  and reset the position in pNext to the current position.
     * 
     *  Note that pNext will have bad information in it until this method
     *  modifies it.
     *
     *  @param pCurrent The current state of the system
     *  @param pNext    The next state of the system (modified by this method)
     *  @param delta    The amount of time (in ms) between the current and
     *         next states.
     */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) {        
        // Walk through all the objects in the current state that can be 
        //  affected by physics
        for (SimulatorObject obj : pCurrent.getObjects()) {
            boolean debugDisplayData =
                    (debugDynamicsData && obj.getLabel().equals("Critterbot"));

            // If no physics data, ignore this object
            ObjectState os = obj.getState(SimulatorComponentDynamics.NAME);
            if (os == null) {
                continue;
            }
            ObjectStateDynamics dynData = (ObjectStateDynamics) os;
            if (debugDisplayData) {
                System.out.println("Current velocity " + dynData.getVelocity() + " Position: "+obj.getPosition());
            }
            
            // Find the corresponding object in the next state
            SimulatorObject newObj = pNext.getObject(obj);
            if (newObj == null) {
                System.err.println ("Failed to find next for: "+obj.getLabel());
                List<SimulatorObject> objs = pNext.getObjects();
                
                for (SimulatorObject o : objs) {
                    System.err.println ("Object "+o.getId()+"    "+o.getLabel());
                }
            }

            ObjectStateDynamics newDynData =
                    (ObjectStateDynamics) newObj.getState(SimulatorComponentDynamics.NAME);
            assert (newDynData.getVelocity().length() == 0);

            // clean up any residue from before, the next state is solely 
            // determined by the current state and the forces acting on it
            // need some way of making sure this is function doesn't exit
            // without setting the position correctly
            // this should no longer be necessary
            newDynData.clearAll();

            Force friction =
                    new Force(dynData.calculateFriction(delta / 1000.0));

            if (debugDisplayData) {
                System.out.println(" Thrust " + dynData.getForceSum().vec);
                System.out.println(" Friction " + friction.vec);
            }
            double momI = dynData.getMomentInertia();
            double mass = dynData.getMass();
            
            Force thrust = dynData.getForceSum();
            //thrust.vec.plusEquals(friction.vec);
            double torque = dynData.getTorque();
            double wi = dynData.getAngVelocity();
            Vector2D vi = dynData.getVelocity();


            // A very sad attempt at friction
            // we're not using real friction because of a bug from interaction
            // between the thrust and friction
            double normalForce = dynData.getMass() * ObjectStateDynamics.GRAVITY;
            double frictionCoeff = 2.5;
            double frictionForce = normalForce * frictionCoeff;
            double torqueFrictionCoeff = 5;

            // @todo fix again - make sure to set thrust -velocity * mass in
            //  the worst case, never move the object 
            thrust.vec.x -= frictionCoeff * vi.x * mass;// frictionForce;
            thrust.vec.y -= frictionCoeff * vi.y * mass; // frictionForce;
            torque -= wi * torqueFrictionCoeff * momI;

            // Apply Euler's method to the position, its derivative and second
            //  derivative
            // @todo pull the integration method into a separate function

            newDynData.setAngVelocity(wi +
                    delta * torque / momI / 1000);
            // don't know about this
            newDynData.setVelocity((Vector2D) dynData.getVelocity().clone());
            newDynData.applyLinearForce(thrust, delta / 1000.0);

            SimulatorObject root = obj.getRoot();
            SimulatorObject newRoot = newObj.getRoot();

            // Although slightly incorrect, we will assume that we have strictly
            //  rigid bodies and that rotating or moving one part moves the whole
            double newDirect = root.aDir + wi * delta / 1000.0;
            newRoot.setDirection(newDirect);

            Vector2D pos = root.getPosition();
            
            newRoot.setPosition(new Vector2D(pos.x + vi.x * delta / 1000.0,
                    pos.y + vi.y * delta / 1000.0));
            if (debugDisplayData) {
                System.out.println(" Pre-collision next velocity " + newDynData.getVelocity());
            }
        }
        // at this point, all the dyn objects in pNext should have their position
        // direction, ang velocity and  velocity set
        checkForCollisions(pCurrent, pNext, delta);

    }

    /**
     * This runs the objects in pNext to see if they are in collision with anything
     * If there is a collision, the next position of the object(s) are reset to their
     * current positions.
     * The forces resulting from the collision are added to the force list for the 
     * state.
     * We assume that pCurrent contains no collisions.
     * This should result in pNext containing no collisions
     * 
     *  @param pCurrent The current state of the system
     *  @param pNext    The next state of the system (modified by this method)
     *  @param delta    The amount of time (in ms) between the current and
     *         next states.
     **/
    
    private void checkForCollisions(SimulatorState pCurrent, SimulatorState pNext, int delta) {
        boolean positionReset = true;

        while (positionReset) {
            positionReset = false;

            for (SimulatorObject obj : pNext.getObjects(SimulatorComponentDynamics.NAME)) {
                // we only need to check for collisions if the object
                // has moved
                if (!obj.geometryEquals(pCurrent.getObject(obj))) {
                    for (SimulatorObject compObj : pNext.getObjects(SimulatorComponentDynamics.NAME)) {

                        //ignore this if it is the same object

                        if (compObj.getId() == obj.getId()) {
                            continue;
                        }


                        //check for a collision between these
                        Collision pt = obj.collidesWith(compObj);
                        if (pt != null) {
                            Vector2D n = pt.normal;

                            positionReset = true;
                            SimulatorObject objP = pCurrent.getObject(obj);
                            SimulatorObject compObjP = pCurrent.getObject(compObj);


                            if (debugCollisions) {
                                System.out.println("Collision at " + pt.point +
                                        "between " + obj + " and " + compObj + "!");
                            }

                            Collision checkP = objP.collidesWith(compObjP);
                            if (checkP != null) {
                                if (debugCollisions) {
                                    System.out.println(" ORIGINAL POSITION IN COLLISION AT " + checkP.point);
                                }
                            }

                            // Get the dynamics data
                            ObjectStateDynamics o1 = (ObjectStateDynamics) obj.getState(SimulatorComponentDynamics.NAME);
                            ObjectStateDynamics o2 = (ObjectStateDynamics) compObj.getState(SimulatorComponentDynamics.NAME);
                            ObjectStateDynamics o1p = (ObjectStateDynamics) objP.getState(SimulatorComponentDynamics.NAME);
                            ObjectStateDynamics o2p = (ObjectStateDynamics) compObjP.getState(SimulatorComponentDynamics.NAME);

                            double colMagnitude;

                            if (collisionEnergySink) {
                                // @todo Times masses?
                                colMagnitude = o1.getVelocity().length() + o2.getVelocity().length();

                                // Collisions are energy sinks
                                o1.setVelocity(new Vector2D(0, 0));
                                o1.setAngVelocity(0);
                                o2.setVelocity(new Vector2D(0, 0));
                                o2.setAngVelocity(0);

                                obj.setGeometry(objP);
                                compObj.setGeometry(compObjP);
                            } else {
                                // calculate forces
                                double ma = o1.getMass();
                                double mb = o2.getMass();
                                double Ia = o1.getMomentInertia();
                                double Ib = o2.getMomentInertia();
                                double e = o1.getCoefficientRestitution(o2);
                                // For now, assume that the centers of mass c1 and c2 are the
                                //  positions
                                Vector2D ca = objP.getPosition();
                                Vector2D cb = compObjP.getPosition();
                                // Compute the moment arms
                                Vector2D ra = pt.point.minus(ca);
                                Vector2D rb = pt.point.minus(cb);

                                Vector2D va = o1p.getVelocity();
                                Vector2D vb = o2p.getVelocity();

                                double wa = o1p.getAngVelocity();
                                double wb = o2p.getAngVelocity();
                                
                                // The velocity of a at the point of collision, including angular
                                //  velocity
                                Vector2D vaPoint = new Vector2D(-ra.y * wa, ra.x * wa);
                                vaPoint.plusEquals(va);
                                Vector2D vbPoint = new Vector2D(-rb.y * wb, rb.x * wb);
                                vbPoint.plusEquals(vb);

                                SimulatorObject root, rootP;
                                root = obj.getRoot(); rootP = objP.getRoot();
                                SimulatorObject compRoot, compRootP;
                                compRoot = compObj.getRoot(); compRootP = compObjP.getRoot();

                                root.setGeometry(rootP);
                                compRoot.setGeometry(compRootP);

                                // The relative velocities at the point of intersection
                                Vector2D vab = vaPoint.minus(vbPoint);
                                
                                // Project v_ab onto the normal
                                Vector2D projVab = n.times(vab.dot(n));

                                // Compute the resulting impulse
                                // @todo clean up
                                double tauA = n.x * ra.y - n.y * ra.x;
                                tauA = tauA * tauA;
                                tauA /= Ia;
                                double tauB = n.x * rb.y - n.y * rb.x;
                                tauB = tauB * tauB;
                                tauB /= Ib;

                                double denom = 1.0 / ma + 1.0 / mb + tauA + tauB;
                                
                                Vector2D projImpulse = projVab.times(-(1 + e) / denom);

                                Vector2D vap = o1.getVelocity().plus(projImpulse.times(1.0/ma));
                                Vector2D vbp = o2.getVelocity().plus(projImpulse.times(-1.0/mb));
                                double wap =
                                        o1.getAngVelocity() + ra.cross(projImpulse) / Ia;
                                double wbp =
                                        o2.getAngVelocity() - rb.cross(projImpulse) / Ib;
                                if (debugDetailedCollisions) {
                                    System.out.println("<--");
                                    System.out.println ("P: "+pt.point);
                                    System.out.println ("R: "+ra+" "+rb);
                                    System.out.println ("N, e: "+n+" "+e);
                                    System.out.println ("V-: "+va+" "+vb);
                                    System.out.printf ("W-: %.4g %.4g\n", wa, wb);
                                    System.out.println ("vPoint: "+vaPoint+" "+vbPoint);
                                    System.out.println ("v_ab: "+vab);
                                    System.out.println ("Projection: "+projVab);
                                    System.out.println ("Impulse: "+projImpulse);
                                    System.out.println("V: "+vap+" "+vbp);
                                    System.out.printf("W: %.4g %.4g\n", wap, wbp);
                                    System.out.println("-->");
                                }

                                checkSpeed(vap, o1);
                                checkSpeed(vbp, o2);
                                o1.setVelocity(vap);
                                o2.setVelocity(vbp);
                                o1.setAngVelocity(wap);
                                o2.setAngVelocity(wbp);

                                // @todo this magitude is bogus
                                // Idea: store kinetic energy (other object's
                                //   relative velocity times mass)?
                                // @todo account for angular energy
                                colMagnitude = (vap.minus(va).length() + vbp.minus(vb).length());
                                double deltaW = (wap - wa + wbp - wb);
                                colMagnitude += deltaW*deltaW;
                            // angular velocity? Needs to be implemented
                            }

                            // Store the collision information in the next state
                            addCollision(pt, obj, colMagnitude);
                            addCollision(pt, compObj, colMagnitude);

                            //now double-check that collision has been resolved
                            // this check should fail. Pt should be null.
                            Collision testPt = obj.collidesWith(compObj);
                            if (testPt != null) {
                                //should probably throw exception here
                                if (debugCollisions) {
                                    System.out.println("Still in collision at " + pt.point + "!!!");
                                }
                            }

                            if(debugCollisions){
                                System.out.println("Post collision velocity " + o1.getVelocity() + " " + o2.getVelocity());
                            }
                        }
                    }
                }
            }
        } //end while we need to check for collisions
    }

    /**
     * This is a helpder function for setting the speed within legal ranges
     *
     * @param v - the vector to change the speed of
     * @param oDyn - the Dynamics state object that gives the max and min speed
     */
    public void checkSpeed(Vector2D v, ObjectStateDynamics oDyn) {
        double speed = v.length();
        if (speed < ObjectStateDynamics.TOL) {
            speed = 0;
            v.timesEquals(0);
        }
        if (speed > oDyn.getMaxSpeed()) {
            v.normalize();
            v.timesEquals(oDyn.getMaxSpeed());
        }
        if (speed < oDyn.getMinSpeed()) {
            v.normalize();
            v.timesEquals(oDyn.getMinSpeed());
        }
    }

    /**
     *  Store the collision information for the two objects involved in the
     *   collision.
     *
     * @param pCol The collision information structure.
     * @param pObject The first object involved.
     * @param pEnergy The kinetic energy of this collision
     */
    private void addCollision(final Collision pCol,
            final SimulatorObject pObject, final double pEnergy) {
        // Assume that both of these are non-null, otherwise we would not be 
        //   calling this method
        ObjectStateDynamics dynData = ObjectStateDynamics.retrieve(pObject);

        Collision newCol = new Collision(pCol);
        newCol.magnitude = pEnergy;
        
        // Note - we're not cloning here! the two objects will share the
        //  collision information
        dynData.addCollision(newCol);
    }
}
