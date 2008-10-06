import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
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
	
	private SimulatorEngine engine;
	
	public Dimension getPreferredSize() {
        return new Dimension(x_size, y_size);
    }

	public SimulatorDrawWorld(SimulatorEngine _engine) {
		
		setFocusable(true);
		engine = _engine;
		//addKeyListener(engine.aAgentList.getFirst());
		this.setBackground(Color.white);
		setVisible(true);
		
	}
	
	public void paintComponent(Graphics g) {
		
		super.paintComponent(g);
		
		// Temporarily draw walls for the world;
		g.setColor(Color.black);
		g.drawRect(20, 20, x_size-40, y_size-40);
		drawAgents(g);
		
	}
	
	private void drawAgents(Graphics g) {
	
		/*Iterator<Agent> i = engine.getAgentList().iterator();
		
		while(i.hasNext()) {
			Agent a = i.next();
			a.drawAgent(g,a);
		}*/
	}
}
