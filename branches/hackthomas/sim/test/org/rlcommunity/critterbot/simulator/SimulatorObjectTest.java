/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.rlcommunity.critterbot.simulator;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.rlcommunity.critterbot.simulator.Collision;
import org.rlcommunity.critterbot.simulator.ObjectStateBumpSensor;
import org.rlcommunity.critterbot.simulator.ObjectStateDynamics;
import org.rlcommunity.critterbot.simulator.ObjectStateLightSensor;
import org.rlcommunity.critterbot.simulator.ObjectStateOmnidrive;
import org.rlcommunity.critterbot.simulator.Polygon;
import org.rlcommunity.critterbot.simulator.SimulatorAgent;
import org.rlcommunity.critterbot.simulator.SimulatorObject;
import org.rlcommunity.critterbot.simulator.Vector2D;
import org.rlcommunity.critterbot.simulator.Wall;

import static org.junit.Assert.*;

/**
 *
 * @author anna
 */
public class SimulatorObjectTest {

    Wall w;
    SimulatorAgent sa;
    SimulatorObject hex;

    public SimulatorObjectTest() {
        w = new Wall("Wall", 1);
        w.addPoint(20, 20);
        w.addPoint(20, 480);
        w.addPoint(480, 480);
        w.addPoint(480, 20);
        w.addPoint(20, 20);

        // Make a polygon for the wall as well
        Polygon wallShape = new Polygon();
        // Interior
        wallShape.addPoint(20, 20);
        wallShape.addPoint(20, 480);
        wallShape.addPoint(480, 480);
        wallShape.addPoint(480, 20);
        wallShape.addPoint(20, 20);
        // Exterior
        wallShape.addPoint(0, 0);
        wallShape.addPoint(0, 500);
        wallShape.addPoint(500, 500);
        wallShape.addPoint(500, 0);
        wallShape.addPoint(0, 0);
        System.out.println("Wall");
        wallShape.doneAddPoints();

        // Note that this polygon self-intersects at the duplicated edge
        //  (0,0)-(20,20)
        // This polygon is also evil because everything falls within its bounding
        //  box
        w.setShape(wallShape);

        // Make the wall react to dynamics
        ObjectStateDynamics wallDyn = new ObjectStateDynamics(10000, 10000);
        wallDyn.setMaxSpeed(0);
        w.addState(wallDyn);

        sa = new SimulatorAgent("Bot", 2);

        Polygon agentShape = new Polygon();
        agentShape.addPoint(-0, 20);
        agentShape.addPoint(-7.5, 18.5);
        agentShape.addPoint(-14, 14);
        agentShape.addPoint(-18.5, 7.5);
        agentShape.addPoint(-20, 0);
        agentShape.addPoint(-18.5, -6.5);
        agentShape.addPoint(-16.5, -16);
        agentShape.addPoint(-13, -26);
        agentShape.addPoint(-8, -35.5);
        agentShape.addPoint(-1, -47);
        agentShape.addPoint(0, -48);
        agentShape.addPoint(-2, -40.5);
        agentShape.addPoint(-4, -32.5);
        agentShape.addPoint(-4.5, -20);
        agentShape.addPoint(-3, -20);
        agentShape.addPoint(3.5, -16);
        agentShape.addPoint(9, -16);
        agentShape.addPoint(15.5, -12.5);
        agentShape.addPoint(19, -6);
        agentShape.addPoint(20, 0);
        agentShape.addPoint(18.5, 7.5);
        agentShape.addPoint(14, 14);
        agentShape.addPoint(7.5, 18.5);
        System.out.println("Agent");
        agentShape.rotate(-Math.PI / 2, new Vector2D(0, 0));

        agentShape.doneAddPoints();

        sa.setShape(agentShape);

        sa.setPosition(new Vector2D(250, 250));
        // Give the agent a 'physics' state, with mass 4 and mom. of inertia 2
        sa.addState(new ObjectStateDynamics(4, 2));
        // Give the agent an omnidirectional drive
        sa.addState(new ObjectStateOmnidrive());
        sa.addState(new ObjectStateBumpSensor());
        sa.addState(new ObjectStateLightSensor());

        // Add an hexagonal obstacle
        hex = new SimulatorObject("Hex", 3);


        // Create the hex polygon
        Polygon hexShape = new Polygon();
        hexShape.addPoint(0, 0);
        hexShape.addPoint(-8, -6);
        hexShape.addPoint(-8, -16);
        hexShape.addPoint(0, -22);
        hexShape.addPoint(8, -16);
        hexShape.addPoint(8, -6);
        hexShape.translate(new Vector2D(0, 11));
        hexShape.doneAddPoints();

        hex.setShape(hexShape);

        // Important - set position after setting shape
        hex.setPosition(new Vector2D(100, 100));

        // Add dynamics to this object
        hex.addState(new ObjectStateDynamics(0.5, 2));
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    /**
     * Test of geometryEquals method, of class SimulatorObject.
     */
    @Test
    public void testGeometryEquals() {
    }

    /**
     * Test of setGeometry method, of class SimulatorObject.
     */
    @Test
    public void testSetGeometry() {
        SimulatorObject o1 = new SimulatorObject("Object 1", 1);
        SimulatorObject o2 = new SimulatorObject("Object 2", 2);
        o1.setPosition(new Vector2D(0, 0));
        o2.setPosition(new Vector2D(1, 1));
        o1.setGeometry(o2);
        assertTrue("Set geometry is not working", o2.getPosition().equals(o1.getPosition()));
    }

    @Test
    public void testCollidesWith() {
        Collision pt = sa.collidesWith(hex);
        assertTrue("Collision registered immediately", pt == null);
        sa.setPosition(new Vector2D(100, 150));
        pt = sa.collidesWith(hex);
        assertTrue("Collision not discovered with setPosition", pt != null);
        sa.setGeometry(hex);
        
    }
}