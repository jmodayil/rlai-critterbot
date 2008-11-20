/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author anna
 */
public class Vector2DTest {

        Vector2D unitX = new Vector2D(1,0);
        Vector2D unitY = new Vector2D(0,1);
        Vector2D ones = new Vector2D(1,1);
    public Vector2DTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    /**
     * Test of plusEquals method, of class Vector2D.
     */
    @Test
    public void testPlusEquals() {
        Vector2D v = new Vector2D(-1, 1);
        v.plusEquals(unitX);
        v.plusEquals(unitY);
        assertTrue("Plus equals fails on negative x",v.x==0);
        assertTrue("Plus equals fails on positive y",v.y==2);
    }

    /**
     * Test of plus method, of class Vector2D.
     */
    @Test
    public void testPlus_Vector2D() {
        Vector2D v = unitX.plus(unitY);
        assertTrue("Plus fails",v.x==1 );
        assertTrue("Plus fails",v.y==1);
    }

    /**
     * Test of minus method, of class Vector2D.
     */
    @Test
    public void testMinus_Vector2D() {
    }

    /**
     * Test of dot method, of class Vector2D.
     */
    @Test
    public void testDot() {
        Vector2D v = new Vector2D(.7, .8);
        double r1 = v.dot(unitX);
        double r2 = v.dot(unitY);
        double r3 = v.dot(ones);
        assertTrue("Dot fails on x", r1==.7);
        assertTrue("Dot fails on y", r2==.8);
        assertTrue("Dot fails on ones", r3==1.5);
    }

    /**
     * Test of times method, of class Vector2D.
     */
    @Test
    public void testTimes() {
    }

    /**
     * Test of minus method, of class Vector2D.
     */
    @Test
    public void testMinus_double() {
    }

    /**
     * Test of plus method, of class Vector2D.
     */
    @Test
    public void testPlus_double() {
    }

    /**
     * Test of rotate method, of class Vector2D.
     */
    @Test
    public void testRotate() {
    }

    /**
     * Test of length method, of class Vector2D.
     */
    @Test
    public void testLength() {
        assertTrue("Length fails on unit vector x", unitX.length()==1);
        assertTrue("Length fails on unit vector y", unitY.length()==1);
        Vector2D v = new Vector2D(.4, -.3);
        assertTrue("Length fails on negative value", v.length()==.5);
    }

    /**
     * Test of direction method, of class Vector2D.
     */
    @Test
    public void testDirection() {
    }

    /**
     * Test of normalize method, of class Vector2D.
     */
    @Test
    public void testNormalize() {
    }

    /**
     * Test of equals method, of class Vector2D.
     */
    @Test
    public void testEquals() {
    }

    /**
     * Test of toString method, of class Vector2D.
     */
    @Test
    public void testToString() {
    }

    /**
     * Test of clone method, of class Vector2D.
     */
    @Test
    public void testClone() {
    }

    /**
     * Test of reverse method, of class Vector2D.
     */
    @Test
    public void testReverse() {
    }

    /**
     * Test of min method, of class Vector2D.
     */
    @Test
    public void testMin() {
        Vector2D v1 = new Vector2D(.7, .8);
        Vector2D v2 = new Vector2D(-.7, .8);
        Vector2D v3 = new Vector2D(1, 0);
        
        assertTrue("Min fails", v1.min(unitX)==unitX);
        assertTrue("Min fails", v2.min(unitX)==unitX);
        assertTrue("Min fails", v3.min(unitX)==v3);
    }

    /**
     * Test of max method, of class Vector2D.
     */
    @Test
    public void testMax() {
   Vector2D v1 = new Vector2D(.7, .8);
        Vector2D v2 = new Vector2D(-.7, .8);
        Vector2D v3 = new Vector2D(1, 0);
        
        assertTrue("Max fails", v1.max(unitX)==v1);
        assertTrue("Max fails", v2.max(unitX)==v2);
        assertTrue("Max fails", v3.max(unitX)==v3);
    }
}