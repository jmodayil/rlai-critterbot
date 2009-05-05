/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critterbot.simulator;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.rlcommunity.critterbot.simulator.ObjectStateDynamics;
import org.rlcommunity.critterbot.simulator.SimulatorComponentDynamics;
import org.rlcommunity.critterbot.simulator.Vector2D;

import static org.junit.Assert.*;

/**
 *
 * @author anna
 */
public class SimulatorComponentDynamicsTest {

    SimulatorComponentDynamics s = new SimulatorComponentDynamics();
    ObjectStateDynamics o1 = new ObjectStateDynamics(1,1);
    ObjectStateDynamics o2 = new ObjectStateDynamics(2, 2);
    static ObjectStateDynamics wall = new ObjectStateDynamics(
            ObjectStateDynamics.MAX_MASS, ObjectStateDynamics.MAX_MOMENT_INERTIA);

    public SimulatorComponentDynamicsTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
        wall.setMaxSpeed(0);
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    /**
     * Test of apply method, of class SimulatorComponentDynamics.
     */
    @Test
    public void testApply() {
    }

    /**
     * Test of checkSpeed method, of class SimulatorComponentDynamics.
     */
    @Test
    public void testCheckSpeed() {
        Vector2D v = new Vector2D(1, -1);
        s.checkSpeed(v, o1);
        assertTrue("checkSpeed unnecessarily resets speed", v.x==1);
        assertTrue("checkSpeed unnecessarily resets speed", v.y==-1);
        s.checkSpeed(v, wall);
        assertTrue("CheckSpeed does not reset to 0", v.length()==0);
        s.checkSpeed(v, o1);

    }


}