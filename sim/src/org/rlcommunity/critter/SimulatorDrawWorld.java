package org.rlcommunity.critter;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.Iterator;

import javax.swing.JPanel;

/**
 * 
 * This is the actual component that draws the main simulator window.
 * 
 * @author Mike Sokolsky
 *
 */
public class SimulatorDrawWorld extends JPanel {

	private static final int x_size = 500;
	private static final int y_size = 500;

	protected double pixelsPerMeter = 100;
  
  private final SimulatorEngine engine;
	
	public SimulatorDrawWorld(SimulatorEngine _engine) {
		
		setFocusable(true);
		engine = _engine;
		//addKeyListener(engine.aAgentList.getFirst());
		this.setBackground(Color.white);
		setVisible(true);
		
	}

  /** Returns the scale of the rendering, in pixels per meter.
    *
    * @return Scale at which the environment is rendered
    */
  public double getScale() { return pixelsPerMeter; }
  /** Sets the scale of the rendering, in pixels per meter.
    *  Larger numbers imply that the world will be rendered bigger,
    *  and vice-versa for smaller numbers.
    *
    * @param pScale The new scale to render at, in pixels per meter
    */
  public void setScale(double pScale) { pixelsPerMeter = pScale; }

	public Dimension getPreferredSize() {
        return new Dimension(x_size, y_size);
    }

    public void paintComponent(Graphics g) {
        // Wrap the Graphics object in a SimulatorGraphics interface to provide
        // scaling capabilities
        SimulatorGraphics sg = new SimulatorGraphics(g, pixelsPerMeter);

        super.paintComponent(sg);
        drawObjects(sg);
    }
	
    private void drawObjects(SimulatorGraphics g) {
        // Java releases the semaphore on engine if it leaves the synchronized
        //  block via an exception, so synchronizing on engine will not cause
        //  the GUI to freeze if the main thread dies
        synchronized (engine) {
            for (SimulatorObject obj : engine.getRootObjects()) {
                obj.draw(g);
            }
        }
    }
}
