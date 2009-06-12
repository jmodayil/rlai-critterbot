/* Copyright 2009 Thomas Degris
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

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;



public class SimulatorVizEvents implements KeyListener {

	public int up;
	public int down;
	public int left;
	public int right;
	public int debug;
    public int thrust, spin;
    
	public SimulatorVizEvents() {
		thrust = spin = 0;
        debug = up = down = left = right = 0;
	}
	
	public void keyTyped(KeyEvent e) {
        char key = e.getKeyChar();
/*	  if('d' == key)
          debug = 1;
      else if('t' == key)
        thrust = 1;
      else if('p' == key)
        spin = 1; */
  }
	
	public void keyPressed(KeyEvent e) {
		if(KeyEvent.VK_UP == e.getKeyCode()) {
			up = 1;
		}
		if(KeyEvent.VK_DOWN == e.getKeyCode()) {
			down = 1;
		}
		if(KeyEvent.VK_LEFT == e.getKeyCode()) {
			right = 1;
		}
		if(KeyEvent.VK_RIGHT == e.getKeyCode()) {
			left = 1;
		}
	}
	
	public void keyReleased(KeyEvent e) {
		if(KeyEvent.VK_UP == e.getKeyCode()) {
			up = 0;
		}
		if(KeyEvent.VK_DOWN == e.getKeyCode()) {
			down = 0;
		}
		if(KeyEvent.VK_LEFT == e.getKeyCode()) {
			right = 0;
		}
		if(KeyEvent.VK_RIGHT == e.getKeyCode()) {
			left = 0;
		}
	}
	
}
