import java.awt.Color;
import java.awt.Graphics;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.geom.Point2D.Double;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;


public class Wall extends SimulatorObject {
	
	private ArrayList<Point2D.Double> walls;
	private ArrayList<Line2D.Double> lines;
	
	public Wall(String pLabel, int pId) {
		super(pLabel, pId);
		
		walls = new ArrayList<Point2D.Double>();
		lines = new ArrayList<Line2D.Double>();
	}
	
	public Wall(String pLabel, int pId, Line2D.Double newWall) {
		super(pLabel, pId);
		
		walls = new ArrayList<Point2D.Double>();
		walls.add((Point2D.Double)newWall.getP1());
		walls.add((Point2D.Double)newWall.getP2());
		lines = new ArrayList<Line2D.Double>();
		lines.add(newWall);
		
	}
	
	public Wall(String pLabel, int pId, ArrayList<Point2D.Double> newWalls) {
		super(pLabel, pId);
		
		walls = new ArrayList<Point2D.Double>(newWalls);
	}
	
	public void addPoint(Point2D.Double point) {
		if(walls.size() > 0)
			lines.add(new Line2D.Double(walls.get(walls.size() - 1), point));
		walls.add(point);
	}
	
	public void addPoint(double x, double y) {
		Point2D.Double point = new Point2D.Double(x,y);
		if(walls.size() > 0)
			lines.add(new Line2D.Double(walls.get(walls.size() - 1), point));
		walls.add(point);
	}
	
	public List<Line2D.Double> getLines() {
		return lines;
	}
	
	private int[] getXPoints() {
		
		int points[] = new int[walls.size()];
		for(int i = 0; i < points.length; i++) {
			points[i] = (int)walls.get(i).x;
		}
		return points;
	}
	
	private int[] getYPoints() {
		
		int points[] = new int[walls.size()];
		for(int i = 0; i < points.length; i++) {
			points[i] = (int)walls.get(i).y;
		}
		return points;
	}
	
	public void drawObj(Graphics g) {
		Color tempC = g.getColor();
		g.setColor(Color.green);
		g.drawPolyline(getXPoints(), getYPoints(), walls.size());
		g.setColor(tempC);
	}
}
