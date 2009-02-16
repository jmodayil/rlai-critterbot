/* Copyright 2009 Michael Sokolsky
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
 * Wall
 *
 * Defines additional data and methods pertaining to a wall. Strictly
 *  speaking, the Wall class (and in general, subclasses of SimulatorObject)
 *  should only need to override the draw method. Data which is Wall-specific
 *  should be put in ObjectState*, not in here.
 *
 *  @author Mike Sokolsky, Marc G. Bellemare
 */

import java.awt.Color;
import java.awt.Graphics;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;
import java.util.List;

public class Wall extends SimulatorObject {

	private final ArrayList<Point2D.Double> walls;
	private final ArrayList<Line2D.Double> lines;

	public Wall(String pLabel, int pId) {
		super(pLabel, pId);

		walls = new ArrayList<Point2D.Double>();
		lines = new ArrayList<Line2D.Double>();
	}

	public Wall(String pLabel, int pId, Line2D.Double newWall) {
		super(pLabel, pId);

		walls = new ArrayList<Point2D.Double>();
		walls.add((Point2D.Double) newWall.getP1());
		walls.add((Point2D.Double) newWall.getP2());
		lines = new ArrayList<Line2D.Double>();
		lines.add(newWall);

	}

	public Wall(String pLabel, int pId, ArrayList<Point2D.Double> newWalls) {
		this(pLabel, pId);
		// @todo Add each point in turn?
		for (Point2D.Double pt : newWalls)
			addPoint(pt);
	}

	public void addPoint(Point2D.Double point) {
		if (walls.size() > 0)
			lines.add(new Line2D.Double(walls.get(walls.size() - 1), point));
		walls.add(point);
	}

	public void addPoint(double x, double y) {
		Point2D.Double point = new Point2D.Double(x, y);
		if (walls.size() > 0)
			lines.add(new Line2D.Double(walls.get(walls.size() - 1), point));
		walls.add(point);
	}

	public List<Line2D.Double> getLines() {
		return lines;
	}

	private double[] getXPoints() {

		double points[] = new double[walls.size()];
		for (int i = 0; i < points.length; i++) {
			points[i] = walls.get(i).x;
		}
		return points;
	}

	private double[] getYPoints() {

		double points[] = new double[walls.size()];
		for (int i = 0; i < points.length; i++) {
			points[i] = walls.get(i).y;
		}
		return points;
	}

	@Override
	public void draw(SimulatorGraphics g) {
		//super.draw(g);
		Color tempC = g.getColor();
		g.setColor(Color.green);
		g.drawPolyline(getXPoints(), getYPoints(), walls.size());
		g.setColor(tempC);
	}

	/** SimulatorObject interface */
	@Override
	public Object clone() {
		Wall sw = new Wall(this.aLabel, this.aId, this.walls);
		sw.copyFrom(this);

		// Clone the children
		for (SimulatorObject c : aChildren) {
			SimulatorObject childClone = (SimulatorObject) c.clone();
			sw.addChild(childClone);
		}
        
		return sw;
	}

	@Override
	protected void copyFrom(SimulatorObject obj) {
		super.copyFrom(obj);
		// Copy relevant data
	}
}
