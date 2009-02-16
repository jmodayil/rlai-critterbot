/* Copyright 2009 Michael Sokolsky, Anna Koop and Marc G. Bellemare
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

package org.rlcommunity.critter;

/**
 * These exist all over the place, but for the sake of portability
 * and the fact that it is quite easy to implement, here it is.
 * 
 * @author Mike Sokolsky
 *
 */
public class Vector2D {
    
    public static final Vector2D ZERO = new Vector2D(0,0);
    /** The tolerance under which we consider a coordinate to be 0; 
      * most likely not the proper place for this */
    public static final double ZERO_TOL = 10E-16;

    public double x;
    public double y;

    /**
     * New Vector2D with 0 length
     */
    public Vector2D() {
        x = 0;
        y = 0;
    }

    /**
     * New Vector2D
     * @param _x X Value
     * @param _y Y Value
     */
    public Vector2D(double _x, double _y) {
        x = _x;
        y = _y;
    }

    /**
     * New Vector2D
     * @param vect Copy value of this vector
     */
    public Vector2D(Vector2D vect) {
        x = vect.x;
        y = vect.y;
    }

    /**
     * Adds the given vector to this one.
     * @param vect The vector to be added to this vector.
     * @return The result (as this)
     */
    public Vector2D plusEquals(Vector2D vect) {
        this.x += vect.x;
        this.y += vect.y;

        return this;
    }

    /**
     * Adds the given vector to this one and returns the result as a 
     *  new object.
     *
     * @param vect The vector to be added to this vector.
     * @return The result (as a new vector)
     */
    public Vector2D plus(Vector2D vect) {
        return new Vector2D(this.x + vect.x, this.y + vect.y);
    }

    /**
     * Subtracts the given vector from this one; does not modify this vector.
     * @param vect The vector to be subtracted this vector.
     * @return The result
     */
    public Vector2D minus(Vector2D vect) {
        return new Vector2D(this.x - vect.x, this.y - vect.y);
    }
    
    /**
     * Returns the dot product between this vector and the argument
     * @param v The vector to be dot-producted
     * @return the result
     */
    public double dot(Vector2D v) {
        return this.x*v.x+this.y*v.y;
    }

    /** Returns the perpendicular component of the cross-product between
     *   this vector and the argument.
     *
     * @param v The vector to be cross-producted
     * @return The third component of the result
     */
    public double cross(Vector2D v) {
        return this.x *v.y - this.y*v.x;
    }
    
    public Vector2D times(double d) {
        return new Vector2D(d*x, d*y);
    }

    public void timesEquals(double d) {
        this.x = d*x;
        this.y = d*y;
    }

    public Vector2D minus(double d) {
        return new Vector2D(x-d, y-d);
    }

    public Vector2D plus(double d) {
        return new Vector2D(x+d, y+d);
    }
    
    public double distance(Vector2D v){
        return Math.sqrt( Math.pow(this.x - v.x,2.0) + Math.pow(this.y - v.y, 2.0) );
    }
    
    /** Rotates this vector by a given angle, and returns it (as a new instance).
     *
     * @param angle The angle of rotation
     * @return A rotated version of this vector
     */
    public Vector2D rotate(double angle) {
        return new 
          Vector2D(this.x * Math.cos(angle) - this.y * Math.sin(angle),
                   this.y * Math.cos(angle) + this.x * Math.sin(angle));
    }

    /**
     * length()
     * @return Length of the vector
     */
    public double length() {
        return Math.sqrt(x * x + y * y);
    }

    /**
     * direction()
     * @return Direction of the vector (-PI, PI)
     */
    public double direction() {
        return Math.atan2(y, x);
    }

    /** Normalizes the vector so that it has unit length.
     *  If this is the (0,0) vector, it is left unchanged.
     */
    public void normalize() {
        double l = length();
        if (l > 0) {
            x /= l;
            y /= l;
        }
    }

    /** Quantizes the vector so that its coordinates are set to 0 if they 
      *  are below the ZERO_TOL threshold.
      *
      * @return This vector (not a copy)
      */
    public Vector2D zeroize() {
      if (Math.abs(this.x) < ZERO_TOL)
        this.x = 0;
      if (Math.abs(this.y) < ZERO_TOL)
        this.y = 0;

      return this;
    }

    public boolean equals(Object o) {
        if (o.getClass() != this.getClass()) {
            return false;
        }
        if (x == ((Vector2D) o).x && y == ((Vector2D) o).y) {
            return true;
        } else {
            return false;
        }
    }

    public boolean equalsTolerance(Vector2D v, double tol) {
        if(Math.abs(x-v.x)>tol)
            return false;
        else if (Math.abs(y-v.y)>tol)
            return false;
        else
            return true;
    }

    @Override
    public String toString() {
        return "<" + Double.toString(x) + "," + Double.toString(y) + ">";
    }

    @Override
    public Object clone() {
        return new Vector2D(x, y);
    }

    public Vector2D reverse() {
        return new Vector2D(-x, -y);
    }
    
    public Vector2D min(Vector2D v) {
        if(this.length()<=v.length())
            return this;
        else
            return v;
        
    }
    
    public Vector2D max(Vector2D v) {
        if(this.length()>=v.length())
            return this;
        else
            return v;
    }

    public Vector2D getUnit() {
        Vector2D v = (Vector2D) this.clone();
        v.normalize();
        return v;
    }

    public Vector2D divide(double d) {
        Vector2D v = (Vector2D)this.clone();
        v.x = v.x/d;
        v.y = v.y/d;
        return v;
    }

    public void divideEquals(double d) {
        this.x = this.x/d;
        this.y = this.y/d;
    }




    /** Creates a unit vector in the given direction
      */
    public static Vector2D unitVector (double pDir)
    {
      return new Vector2D(Math.cos(pDir), Math.sin(pDir));
    }
}
