package org.rlcommunity.critter;

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
	  if('d' == key)
          debug = 1;
      else if('t' == key)
        thrust = 1;
      else if('s' == key)
        spin = 1;
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
