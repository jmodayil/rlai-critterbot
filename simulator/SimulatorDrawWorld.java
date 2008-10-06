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
		
		drawObjs(g);
		drawAgents(g);
		
	}
	
	private void drawAgents(Graphics g) {
		Iterator<SimulatorAgent> i = engine.getAgentList().iterator();
		
		while(i.hasNext()) {
			i.next().drawAgent(g);
		}
	}
	
	private void drawObjs(Graphics g) {
		Iterator<SimulatorObject> i = engine.getObjList().iterator();
		
		while(i.hasNext()) {
			i.next().drawObj(g);
		}
	}
}
