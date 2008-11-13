package org.rlcommunity.critter;

/**
 * SimulatorComponentDynamics
 *
 * This SimulatorComponent encodes all the dynamics interactions. As well
 *  as modifying data for the ObjectStateDynamics state of objects, 
 *  it also is in charge of modifying position.
 *
 * @author Marc G. Bellamre
 * @author Anna Koop
 */
public class SimulatorComponentDynamics implements SimulatorComponent {

    public static final String NAME = "dynamics";

    public SimulatorComponentDynamics() {
    }

    /** Applies physics to the current state to obtain the next state.
     * Roughly speaking, the following happens (in order):
     *
     *  1. The differential equations for position, velocity and acceleration 
     *     are applied according to Euler's method, results are stored in pNext
     *  2. We test for collisions based on the resulting positions in pNext
     *  3. Collisions update positions in pNext (stay as pCurrent?), generate
     *     forces which are added to the force list in pNext
     *  4. Repeat 2-3 to test for new collisions (ugh this might be messy)
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
            // @@@ Added by MGB - remove ASAP ---
            SimulatorObject futureObj = pNext.getObject(obj);

            if (futureObj.getState("bumpsensor") != null)
            {
              ObjectStateBumpSensor bs = (ObjectStateBumpSensor)
                futureObj.getState("bumpsensor");
              bs.clearForces();
            }
            // --- end MGB code

            // If no physics data, ignore this object
            ObjectState os = obj.getState(SimulatorComponentDynamics.NAME);
            if (os == null) {
                continue;
            }
            ObjectStateDynamics dynData = (ObjectStateDynamics) os;

            // Find the corresponding object in the next state
            SimulatorObject newObj = pNext.getObject(obj);
            ObjectStateDynamics newDynData =
                    (ObjectStateDynamics) newObj.getState(SimulatorComponentDynamics.NAME);


            newDynData.clearForces();
            newDynData.clearTorque();
            // Apply Euler's method to the position, its derivative and second
            //  derivative

            // First compute the sum of forces
            // @@@ sum up
            Vector2D force = dynData.getForceSum().vec;
            Vector2D vel = dynData.getVelocity();
            double torque = dynData.getTorque();
            double avel = dynData.getAngVelocity();

            // A very sad attempt at friction
            force.x -= vel.x * .1;
            force.y -= vel.y * .1;
            torque -= avel * .5;

            double mass = dynData.getMass();

            newDynData.setAngVelocity(avel + torque / dynData.getMomentInertia());
            newDynData.setVelocity(
                    new Vector2D(vel.x + force.x / mass, vel.y + force.y / mass));

            double newDirect = obj.aDir + avel * delta / 1000;
            while (newDirect >= Math.PI) {
                newDirect -= Math.PI * 2;
            }
            while (newDirect < -Math.PI) {
                newDirect += Math.PI * 2;
            }
            newObj.setDirection(newDirect);
            newObj.setPosition(new Vector2D(obj.aPos.x + vel.x * delta / 1000,
                    obj.aPos.y + vel.y * delta / 1000));
        }

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

            for (SimulatorObject obj : pNext.getObjects()) {
                // if this object hasn't moved, we don't need
                // to do further checks (because a possible 
                // collision will be from the *other* thing having moved)
                if (!obj.geometryEquals(pCurrent.getObject(obj))) {
                    for (SimulatorObject compObj : pNext.getObjects()) {

                        //ignore this if it is the same object
                        if (compObj.getId() == obj.getId()) {
                            continue;                        //check for a collision between these
                        }
                        Vector2D pt = obj.intersects(compObj);
                        if (pt != null) {
                            // Added by MGB
                            //  Anna, you can take this part out. I apologize
                            //  for conflicts!
                            debugAddBump(obj, pt);
                            debugAddBump(compObj, pt);
                            // End added by MGB

                            positionReset = true;
                            resetPosition(obj, pCurrent);
                            //this may or may not change things, but
                            // it should be okay either way

                            resetPosition(compObj, pCurrent);

                            System.out.println("Collision at " + pt +
                                    "between " + obj + " and " + compObj + "!");
                            // calculate forces

                            ObjectStateDynamics os = (ObjectStateDynamics) obj.getState(SimulatorComponentDynamics.NAME);
                            if (os != null) {
                                os.setVelocity(new Vector2D(0, 0));
                                os.setAngVelocity(0);
                                os.clearTorque();
                            }
                            ObjectStateDynamics cos = (ObjectStateDynamics) obj.getState(this.NAME);
                            if (cos != null) {
                                cos.setVelocity(new Vector2D(0, 0));
                                cos.setAngVelocity(0);
                                cos.clearTorque();
                            }
                        }
                    }
                }
            }
        } //end while we need to check for collisions
    }

    /**
     * Little helper function for resetting positions
     **/
    private void resetPosition(SimulatorObject obj, SimulatorState pCurrent) {
        obj.setGeometry(pCurrent.getObject(obj));
    }


    /** Added by MGB - remove at your leisure */
    public void debugAddBump (SimulatorObject obj, Vector2D pt)
    {
      ObjectState os = obj.getState(ObjectStateBumpSensor.NAME);

      if (os == null) return;

      ObjectStateBumpSensor bs = (ObjectStateBumpSensor)os;

      bs.addForce(new Force(new Vector2D(5,5), pt));
    }
}
