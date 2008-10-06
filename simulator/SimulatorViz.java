import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.Timer;

/**
 * Visualization component for the Simulator.
 *
 * @author Mike Sokolsky
 * @version 0.1
 */

public final class SimulatorViz extends JFrame {

	private Timer timer;
	
	private static final int INTERVAL = 100;
    /**
      * 
      */
    public SimulatorViz(SimulatorEngine engine) {
    	
    	SimulatorDrawWorld world = new SimulatorDrawWorld(engine);
    	JScrollPane sPane = new JScrollPane(world);
    	sPane.setPreferredSize(new Dimension(500,500));
    	//sPane.add(world);
    	
    	//Container cp = this.getContentPane();
    	add(sPane);
    	setMaximumSize(world.getPreferredSize());
        world.requestFocusInWindow();
        pack();
        
        //setResizable(false);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
  
        /**
         * This is not how we want to do this, but it works for now
         */
        timer = new javax.swing.Timer(INTERVAL, 
        			new ActionListener() {
            			public void actionPerformed(ActionEvent e) {
            				repaint();
            			}
        			});
        timer.start();

        setVisible(true);
    }
}
