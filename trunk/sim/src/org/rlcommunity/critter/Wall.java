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
			points[i] = (int) walls.get(i).y;
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

		return sw;
	}

	@Override
	protected void copyFrom(SimulatorObject obj) {
		super.copyFrom(obj);
		// Copy relevant data
	}
}
