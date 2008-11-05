package org.rlcommunity.critter;

/**
 * These exist all over the place, but for the sake of portability
 * and the fact that it is quite easy to implement, here it is.
 * 
 * @author Mike Sokolsky
 *
 */
public class Vector2D {

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
  public Vector2D addEquals(Vector2D vect)
  {
    this.x += vect.x;
    this.y += vect.y;
    
    return this;
  }

  /**
    * Subtracts the given vector from this one; does not modify this vector.
    * @param vect The vector to be subtracted this vector.
    * @return The result
    */
  public Vector2D minus(Vector2D vect)
  {
    return new Vector2D(this.x - vect.x, this.y - vect.y);
  }
	/**
	 * length()
	 * @return Length of the vector
	 */
	public double length() {
		return Math.sqrt(x*x + y*y);
	}
	
	/**
	 * direction()
	 * @return Direction of the vector (-PI, PI)
	 */
	public double direction() {
		return Math.atan2(y, x);
	}
	
	public boolean equals(Object o) {
		if(o.getClass() != this.getClass())
			return false;
		
		if(x == ((Vector2D)o).x && y == ((Vector2D)o).y)
			return true;
		else
			return false;
	}
	
	public String toString() {
		return "<" + Double.toString(x) + "," + Double.toString(y) + ">";
	}

  public Object clone()
  {
    return new Vector2D(x, y);
  }
}
