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
import org.rlcommunity.critter.Drops.*;

import java.util.List;
import java.util.LinkedList;

/**
 *
 * @author Brian Tanner
 * @author Marc G. Bellemare
 */
public class KeyboardClient implements DropClient, KeyListener {

    public int up;
    public int down;
    public int left;
    public int right;
    private int motor100;
    private int motor220;
    private int motor340;

    public boolean hasChange;

    public KeyboardClient() {
        up = down = left = right = 0;
        motor100 = motor220 = motor340 = 0;
        hasChange = false;
    }

    public void keyTyped(KeyEvent e) {
    }

    public void keyPressed(KeyEvent e) {
        hasChange = true;

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
        if (KeyEvent.VK_Q == e.getKeyCode()) {
            motor100 = 1;
        }
        if (KeyEvent.VK_W == e.getKeyCode()) {
            motor220 = 1;
        }
        if (KeyEvent.VK_A == e.getKeyCode()) {
            motor340 = 1;
        }
    }

    public void keyReleased(KeyEvent e) {
        hasChange = true;

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
        if (KeyEvent.VK_Q == e.getKeyCode()) {
            motor100 = 0;
        }
        if (KeyEvent.VK_W == e.getKeyCode()) {
            motor220 = 0;
        }
        if (KeyEvent.VK_A == e.getKeyCode()) {
            motor340 = 0;
        }
    }


    long lastDropTime=System.currentTimeMillis();
    long keyboardDropInterval=50;
    /**
     * This might be setting velocity to 0 when no command received.
     * @return
     */
    public List<SimulatorDrop> receive() {
        LinkedList<SimulatorDrop> dropList = new LinkedList<SimulatorDrop>();
        // Produce a drop iff: a key is pressed, or was released, and enough
        // time has elapsed
        if (!hasChange
                && System.currentTimeMillis() - lastDropTime < keyboardDropInterval) {
            return dropList;
        }

        CritterControlDrop controlDrop = new CritterControlDrop();
        hasChange = true;
        if (hasChange) {
            if (up > 0 || down > 0 || right > 0 || left > 0) {
                setDropWithXYThetaSpace(controlDrop);
            } else if (motor100 > 0 || motor220 > 0 || motor340 > 0) {
                setDropWithMotorSpace(controlDrop);
            }
        }
        hasChange = false;
        dropList.add(controlDrop);
        lastDropTime = System.currentTimeMillis();
        return dropList;
    }

    private void setDropWithMotorSpace(CritterControlDrop controlDrop) {
        controlDrop.motor_mode = CritterControlDrop.MotorMode.WHEEL_SPACE;
        controlDrop.m100_vel = motor100;
        controlDrop.m220_vel = motor220;
        controlDrop.m340_vel = motor340;
    }

    private void setDropWithXYThetaSpace(CritterControlDrop controlDrop) {
        double velocityX, angVel;
        int maxVel = 100; // 100m/s (or cm/s)
        int maxAngularVel = 27; // Roughly 1/2 a turn in a second
        velocityX = (up * maxVel - down * maxVel);
        angVel = (right * -maxAngularVel + left * maxAngularVel);
        controlDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;
        controlDrop.x_vel = (int) velocityX;
        controlDrop.y_vel = 0;
        controlDrop.theta_vel = (int) angVel;
    }

    public void send(SimulatorDrop pData) {
      // Do nothing - the keyboard client has no 'output' per se
    }
}
