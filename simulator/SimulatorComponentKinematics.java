/**
  * SimulatorComponentKinematics
  *
  * This SimulatorComponent encodes all the kinematics interactions. As well
  *  as modifying data for the ObjectStateKinematics state of objects, 
  *  it also is in charge of modifying position.
  *
  * @author SimulatorComponentKinematics
  */

public class SimulatorComponentKinematics implements SimulatorComponent
{
  public static final String NAME = "kinematics";

  public SimulatorComponentKinematics()
  {
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
  public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta)
  {
    // Walk through all the objects in the current state that can be 
    //  affected by physics
    for (SimulatorObject obj : pCurrent.getObjects())
    {
      // If no physics data, ignore this object
      ObjectState os = obj.getState(this.NAME);
      if (os == null)
        continue;
      ObjectStateKinematics kinData = (ObjectStateKinematics) os;

      // Find the corresponding object in the next state
      SimulatorObject newObj = pNext.getObject(obj.getId());
      ObjectStateKinematics newKinData = 
        (ObjectStateKinematics)newObj.getState(this.NAME);
      
      newKinData.clearForces();
      newKinData.clearTorque();
      // Apply Euler's method to the position, its derivative and second
      //  derivative

      // First compute the sum of forces
      // @@@ sum up
      Vector2D force = kinData.getForceSum().vec;
      Vector2D vel = kinData.getVelocity();
      double torque = kinData.getTorque();
      double avel = kinData.getAngVelocity();

	    // A very sad attempt at friction
	    force.x -= vel.x * .1;
	    force.y -= vel.y * .1;
	    torque -= avel * .5;
	  
	    newKinData.setAngVelocity(avel + torque / obj.momI);
	    newKinData.setVelocity(
        new Vector2D(vel.x + force.x / obj.mass, vel.y + force.y / obj.mass));

      newObj.setDirection(obj.aDir + avel * delta / 1000);
      newObj.setPosition(new Vector2D(obj.aPos.x + vel.x * delta / 1000,
                                 obj.aPos.y + vel.y * delta / 1000));
    }

	  // Now test for collisions, again very sad.
    // @@@ use SimulatorObject.aShape.intersects()!!


	  /*List<Line2D.Double> lines = ((Wall)aState.getObject(1)).getLines();
	  Iterator<Line2D.Double> i = lines.iterator();
	  while(i.hasNext()) {
		  Line2D.Double line = i.next();
		  double dist = line.ptSegDist(newX, newY);
		  //System.out.println(dist);
		  if(10 >= dist) {
			  System.out.println("Collision!!!");
			  test.aVel.x = test.aVel.y = 0;
			  newX = test.aPos.x;
			  newY = test.aPos.y;
		  }
	  }
	  test.aDir = newDir;
	  test.aPos.x = newX;
	  test.aPos.y = newY; */
  }
}
