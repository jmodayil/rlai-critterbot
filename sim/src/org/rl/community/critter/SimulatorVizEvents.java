package org.rl.community.critter;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;



public class SimulatorVizEvents implements KeyListener {

	public int up;
	public int down;
	public int left;
	public int right;
	
	public SimulatorVizEvents() {
		up = down = left = right = 0;
	}
	
	public void keyTyped(KeyEvent e) {
		
	}
	
	public void keyPressed(KeyEvent e) {
		if(KeyEvent.VK_UP == e.getKeyCode()) {
			up = 1;
		}
		if(KeyEvent.VK_DOWN == e.getKeyCode()) {
			down = 1;
		}
		if(KeyEvent.VK_LEFT == e.getKeyCode()) {
			left = 1;
		}
		if(KeyEvent.VK_RIGHT == e.getKeyCode()) {
			right = 1;
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
			left = 0;
		}
		if(KeyEvent.VK_RIGHT == e.getKeyCode()) {
			right = 0;
		}
	}
	
}
