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
    
    /** Rotates this vector by a given angle, and returns it (as a new instance).
     *
     * @param dir The angle of rotation
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


}
