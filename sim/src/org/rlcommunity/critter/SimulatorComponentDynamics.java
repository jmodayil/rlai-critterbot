package org.rlcommunity.critter;

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
    // @todo clean up
    boolean debugCollisions = false;
    boolean debugDynamicsData = false;

    public SimulatorComponentDynamics() {
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

            double newDirect = obj.aDir + wi * delta / 1000.0;
            newObj.setDirection(newDirect);
            
            newObj.setPosition(new Vector2D(obj.aPos.x + vi.x * delta / 1000.0,
                    obj.aPos.y + vi.y * delta / 1000.0));
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
                            // Store the collision information in the next state
                            addCollision(pt, obj, compObj);

                            Vector2D n = pt.normal;

                            positionReset = true;
                            SimulatorObject objP = pCurrent.getObject(obj);
                            SimulatorObject compObjP = pCurrent.getObject(compObj);


                            //@TODO!!!
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
                            obj.setGeometry(objP);
                            compObj.setGeometry(compObjP);

                            //now double-check that collision has been resolved
                            // this check should fail. Pt should be null.
                            Collision testPt = obj.collidesWith(compObj);
                            if (testPt != null) {
                                //should probably throw exception here
                                if (debugCollisions) {
                                    System.out.println("Still in collision at " + pt.point + "!!!");
                                }
                            }

                            // now get the thing we can actually modify
                            ObjectStateDynamics o1 = (ObjectStateDynamics) obj.getState(SimulatorComponentDynamics.NAME);
                            ObjectStateDynamics o2 = (ObjectStateDynamics) compObj.getState(SimulatorComponentDynamics.NAME);
                            ObjectStateDynamics o1p = (ObjectStateDynamics) objP.getState(SimulatorComponentDynamics.NAME);
                            ObjectStateDynamics o2p = (ObjectStateDynamics) compObjP.getState(SimulatorComponentDynamics.NAME);

                            if (collisionEnergySink) {
                                // Collisions are energy sinks
                                o1.setVelocity(new Vector2D(0, 0));
                                o1.setAngVelocity(0);
                                o2.setVelocity(new Vector2D(0, 0));
                                o2.setAngVelocity(0);

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
                                double raLen = ra.length();
                                Vector2D rb = pt.point.minus(cb);
                                double rbLen = rb.length();

                                Vector2D va = o1p.getVelocity();
                                Vector2D vb = o2p.getVelocity();

                                double wa = o1p.getAngVelocity();
                                double wb = o2p.getAngVelocity();
                                
                                // The velocity of a at the point of collision, including angular
                                //  velocity
                                Vector2D vaPoint = new Vector2D(-ra.y * wa*raLen, ra.x * wa*raLen);
                                vaPoint.plusEquals(va);
                                Vector2D vbPoint = new Vector2D(-rb.y * wb*rbLen, rb.x * wb*rbLen);
                                vbPoint.plusEquals(vb);

                                // The relative velocities at the point of intersection
                                Vector2D vab = vaPoint.minus(vbPoint);
                                
                                // Project v_ab onto the normal
                                Vector2D projVab = n.times(vab.dot(n));

                                // Compute the resulting impulse
                                //  Taken from Physics for Game Developers
                                double taua = ra.cross(n) / Ia;
                                double taub = rb.cross(n) / Ib;

                                Vector2D tmpa = new Vector2D(taua * ra.y, -taua * ra.x);
                                Vector2D tmpb = new Vector2D(taub * rb.y, -taub * rb.x);

                                double denom = 1.0 / ma + 1.0 / mb + 
                                        Math.abs(n.dot(tmpa)) + Math.abs(n.dot(tmpb));

                                Vector2D projImpulse = projVab.times(-(1 + e) / denom);

                                Vector2D vap = o1.getVelocity().plus(projImpulse).times(1.0 / ma);
                                Vector2D vbp = o2.getVelocity().plus(projImpulse).times(-1.0 / mb);
                                double wap =
                                        o1.getAngVelocity() + ra.cross(projImpulse) / Ia / raLen;
                                double wbp =
                                        o2.getAngVelocity() - rb.cross(projImpulse) / Ib / rbLen;
                                if (debugCollisions) {
                                    System.out.println("<--");
                                    System.out.println ("R: "+ra+" "+rb);
                                    System.out.println ("N: "+n);
                                    System.out.println ("Dots: "+n.dot(tmpa)+" "+n.dot(tmpb));
                                    System.out.println ("V-: "+va+" "+vb);
                                    System.out.printf ("W-: %.4g %.4g\n", wa, wb);
                                    System.out.println ("vPoint: "+vaPoint+" "+vbPoint);
                                    System.out.println ("Projection: "+projVab);
                                    System.out.println ("A: "+ taua + " " + tmpa + " " + denom);
                                    System.out.println ("B: "+ taub + " " + tmpb + " " + denom);
                                    System.out.println ("Impulse: "+projImpulse);
                                    System.out.println("V: "+vap+" "+vbp);
                                    System.out.printf("W: %.4g %.4g\n", wap, wbp);
                                    System.out.println("-->");
                                }
                                
                                // might want to check here for infinite mass?
                                // or just let it fall out of the math
                                /* double impulse = (-(1 + e) * vab.dot(n)) /
                                        (1 / ma + 1 / mb);

                                // this means post-thrust velocities aren't used. hmm.
                                Vector2D vap = o1.getVelocity().plus(n.times(impulse / ma));
                                Vector2D vbp = o2.getVelocity().minus(n.times(impulse / mb)); */

                                checkSpeed(vap, o1);
                                checkSpeed(vbp, o2);
                                o1.setVelocity(vap);
                                o2.setVelocity(vbp);
                                o1.setAngVelocity(wap);
                                o2.setAngVelocity(wbp);
                            // angular velocity? Needs to be implemented
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
     * @param pObj1 The first object involved.
     * @param pObj2 The second object involved.
     */
    private void addCollision(final Collision pCol,
            final SimulatorObject pObj1, final SimulatorObject pObj2) {
        // Assume that both of these are non-null, otherwise we would not be 
        //   calling this method
        ObjectStateDynamics dynData1 = ObjectStateDynamics.retrieve(pObj1);
        ObjectStateDynamics dynData2 = ObjectStateDynamics.retrieve(pObj2);

        // Note - we're not cloning here! the two objects will share the
        //  collision information
        dynData1.addCollision(pCol);
        dynData2.addCollision(pCol);
    }
}
