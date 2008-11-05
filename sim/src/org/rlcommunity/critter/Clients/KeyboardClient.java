/*
 * Copyright 2008 Brian Tanner
 * http://bt-recordbook.googlecode.com/
 * brian@tannerpages.com
 * http://brian.tannerpages.com
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package org.rlcommunity.critter.Clients;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import org.rlcommunity.critter.CritterControlDrop;
import org.rlcommunity.critter.SimulatorDrop;

/**
 *
 * @author Brian Tanner
 */
public class KeyboardClient implements ClientHandlerInterface, KeyListener {

    public int up;
    public int down;
    public int left;
    public int right;

    public KeyboardClient() {
        up = down = left = right = 0;
    }

    public void keyTyped(KeyEvent e) {
    }

    public void keyPressed(KeyEvent e) {
        if (KeyEvent.VK_UP == e.getKeyCode()) {
            up = 1;
        }
        if (KeyEvent.VK_DOWN == e.getKeyCode()) {
            down = 1;
        }
        if (KeyEvent.VK_LEFT == e.getKeyCode()) {
            left = 1;
        }
        if (KeyEvent.VK_RIGHT == e.getKeyCode()) {
            right = 1;
        }
    }

    public void keyReleased(KeyEvent e) {
        if (KeyEvent.VK_UP == e.getKeyCode()) {
            up = 0;
        }
        if (KeyEvent.VK_DOWN == e.getKeyCode()) {
            down = 0;
        }
        if (KeyEvent.VK_LEFT == e.getKeyCode()) {
            left = 0;
        }
        if (KeyEvent.VK_RIGHT == e.getKeyCode()) {
            right = 0;
        }
    }


    long lastDropTime=System.currentTimeMillis();
    long keyboardDropInterval=50;
    /**
     * This might be setting velocity to 0 when no command received.
     * @return
     */
    public SimulatorDrop receive() {
        double velocityX,  torque;
        int maxVel=25;
        int maxTorque=10;
        

        // If any of the visualizer keys are pressed, we override the omnidrive
        //  @@@ This needs to be moved somewhere else or ...
        if (up > 0 || down > 0 || right > 0 || left > 0) {
        if(System.currentTimeMillis()-lastDropTime<keyboardDropInterval){
            return null;
        }

        velocityX = (up * maxVel - down * maxVel);
            torque = (right * -maxTorque + left * maxTorque);
            CritterControlDrop controlDrop = new CritterControlDrop();
            controlDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;
            controlDrop.x_vel = (int) velocityX;
            controlDrop.y_vel = 0;
            controlDrop.theta_vel = (int) torque;
            lastDropTime=System.currentTimeMillis();
            return controlDrop;
        }
        
        return null;
    }

    public void send(SimulatorDrop pData) {
        System.err.println("Keyboard client doesnt send drops.");
    }
}
