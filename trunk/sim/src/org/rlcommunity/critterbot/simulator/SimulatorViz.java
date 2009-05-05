/* Copyright 2009 Michael Sokolsky and Thomas Degris
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

package org.rlcommunity.critterbot.simulator;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.awt.event.KeyListener;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.Timer;

import org.rlcommunity.critterbot.javadrops.clients.KeyboardClient;

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
    public SimulatorViz(SimulatorEngine engine, KeyboardClient theKeyboardClient, KeyListener theKeyboardClient2){
        world = new SimulatorDrawWorld(engine);
        world.addKeyListener(theKeyboardClient);
        world.addKeyListener(theKeyboardClient2);
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
