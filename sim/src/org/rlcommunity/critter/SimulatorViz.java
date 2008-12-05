package org.rlcommunity.critter;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.awt.event.KeyListener;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.Timer;
import org.rlcommunity.critter.Clients.KeyboardClient;

/**
 * Visualization component for the Simulator.
 *
 * @author Mike Sokolsky
 * @version 0.1
 */
public final class SimulatorViz extends JFrame {

    private Timer timer;
    private static final int INTERVAL = 100;
    private final SimulatorDrawWorld world;


    /**
     * 
     */
    public SimulatorViz(SimulatorEngine engine, KeyboardClient theKeyboardClient){
        world = new SimulatorDrawWorld(engine);
        world.addKeyListener(theKeyboardClient);
        JScrollPane sPane = new JScrollPane(world);

        //sPane.setPreferredSize(new Dimension(500,500));
        //sPane.add(world);

        //Container cp = this.getContentPane();
        add(sPane);
        //setMaximumSize(world.getPreferredSize());
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

    public void die() {
        this.dispose();
    }

//    void addKeyListenerToWorld(KeyboardClient theKeyboardClient) {
//        world.addKeyListener(theKeyboardClient);
//    }
    
}
