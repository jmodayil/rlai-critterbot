/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critterbot.simulator;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.rlcommunity.critterbot.simulator.ObjectStateDynamics;
import org.rlcommunity.critterbot.simulator.Vector2D;

import static org.junit.Assert.*;

/**
 *
 * @author anna
 */
public class ObjectStateDynamicsTest {

    ObjectStateDynamics o1 = new ObjectStateDynamics(1,1);
    ObjectStateDynamics o2 = new ObjectStateDynamics(2, 2);
    static ObjectStateDynamics wall = new ObjectStateDynamics(
            ObjectStateDynamics.MAX_MASS, ObjectStateDynamics.MAX_MOMENT_INERTIA);
    static ObjectStateDynamics moving = new ObjectStateDynamics(
            ObjectStateDynamics.MIN_MASS, ObjectStateDynamics.MIN_MOMENT_INERTIA);

    public ObjectStateDynamicsTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
        wall.setMaxSpeed(0);
        moving.setMinSpeed(.1);
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    /**
     * Test friction calculation
     */
    @Test
    public void checkCalculateFriction() {
        o1.setVelocity(1,0);
        o1.setMass(1);
        o1.setCoefficientFrictionDyn(.05);
        Vector2D f = o1.calculateFriction(1);
        assertTrue("Friction is not being calculated correctly", f.x==.5);
        assertTrue("Friction is not in an opposing direction", f.getUnit().reverse().
                equalsTolerance(o1.getVelocity().getUnit(), 0.0001));
        o1.setCoefficientFrictionDyn(0.5);
        f = o1.calculateFriction(1);
        // need to have stopping force returned rather than force of friction
        assertTrue("Friction is creating speed", f.x==1);
    }

    /**
     * Test static friction
     */
    @Test
    public void checkCalculateFrictionStatic() {
        o1.setVelocity(0,0);
        o1.setMass(1);
        o1.setCoefficientFrictionStatic(.5);
        Vector2D f = o1.calculateFriction(1);
        assertTrue("Friction is positive for stationary object", f.length()==0);
    }


    /**
     * Test the clone method
     */
    @Test
    public void checkClone() {
        ObjectStateDynamics o3 = (ObjectStateDynamics) wall.clone();
        // test this more thoroughly
        assertTrue("Clone retained maxSpeed", o3.getMaxSpeed()==wall.getMaxSpeed());
        assertTrue("Clone retained minSpeed", o3.getMinSpeed()==wall.getMinSpeed());
        assertTrue("Clone retained velocity", o3.getVelocity().equals(wall.getVelocity()));
        assertTrue("Clone retained angular velocity", o3.getAngVelocity()==wall.getAngVelocity());
        //assertTrue("Clone retained maxSpeed", o3.getMaxSpeed()==wall.getMaxSpeed());
        //assertTrue("Clone retained maxSpeed", o3.getMaxSpeed()==wall.getMaxSpeed());
    }
    // test copy from


}