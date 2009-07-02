/**
 * 
 */
package org.rlcommunity.critterbot.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;

import javax.swing.JFrame;
import javax.swing.JPanel;

/**
 * @author sokolsky
 *
 */
public class RichScrollingGraph extends JPanel {

	private static final long serialVersionUID = 1L;
	private static final int PADDING = 20;
	private static final int DEFAULT_SAMPLES = 1000;
	private static final int DEFAULT_RATE = 1;
	
	private int sampleRate;

	private double yMin, yMax;
	private int numSamples;
	private double history[];
	private int sampleStep;
	private double sampleAvg;
	private double sampleMin, sampleMax;
	
	/**
	 * @param min Minimum Value
	 * @param max Maximum Value
	 * @param samples Number of samples to keep in history
	 * @param rate Number of samples to average per data point
	 */
	public RichScrollingGraph(double min, double max, int samples, int rate) {
		sampleRate = rate;
		sampleStep = 1;
		initAxis(min,max,samples);
		history = new double[numSamples];
		this.setPreferredSize(new Dimension(300,300));
		this.setDoubleBuffered(true);
		this.getDrawArea();
		this.setVisible(true);
	}
	
	private void initAxis(double _yMin, double _yMax, int samples) {
		numSamples = samples;
		yMin = _yMin;
		yMax = _yMax;
	}
	
	private Rectangle getDrawArea() {
		Dimension mySize = this.getSize();
		Rectangle area = new Rectangle();
		area.x = PADDING + 1;
		area.y = PADDING + 1;
		area.width = mySize.width - (PADDING + 1) * 2;
		area.height = mySize.height - (PADDING + 1) * 2;
		return area;
	}
	
	private void drawAxis(Graphics g) {
		Dimension mySize = this.getSize();
		g.setColor(new Color(230,230,230));
		g.fillRect(0, 0, mySize.width, mySize.height);
		g.setColor(Color.WHITE);
		g.fillRect(PADDING, PADDING, 
				mySize.width - PADDING * 2, mySize.height - PADDING * 2);
		g.setColor(Color.BLACK);
		g.drawRect(PADDING, PADDING, 
				mySize.width - PADDING * 2, mySize.height - PADDING * 2);
	}
	
	private void drawGraph(Graphics g) {
		Rectangle area = getDrawArea();
		g.setClip(area.x, area.y, area.width + 1, area.height + 1);
		double xStep = area.width / ((double)numSamples - 1);
		double yStep = area.height / (yMax - yMin);
		int lastYPoint = (int)(area.y + area.height - (history[0] * yStep));
		
		int lastXPoint = (int)(area.x);
		for(int i = 0; i < numSamples - 1; i++) {
			int xPoint = (int)(area.x + xStep * (i+1));
			int yPoint = (int)(area.y + area.height - (history[i+1] * yStep));
			g.setColor(Color.RED);
			g.drawLine(lastXPoint,
					lastYPoint,
					xPoint,
					yPoint);
			if(i < numSamples - 2) {
				g.setColor(Color.BLUE);
				g.drawRect(xPoint - 1,
						yPoint - 1,
						2, 2);
			}
			lastXPoint = xPoint;
			lastYPoint = yPoint;
		}
	}
	
	public void addData(double data) {
		sampleAvg += data;
		if(sampleStep == 1)
			sampleMin = sampleMax = data;
		else {
			if(data < sampleMin)
				data = sampleMin;
			if(data > sampleMax)
				data = sampleMax;
		}
		if(sampleStep < sampleRate) {
			sampleStep++;
			return;
		}
		else {
			data = sampleAvg / sampleRate;
			sampleAvg = 0;
			sampleStep = 1;
		}
		for(int i = 0; i < history.length - 1; i++) {
			history[i] = history[i + 1];
		}
		history[history.length - 1] = data;
		this.repaint();
	}
	
	/**
	 * @param g Graphics context to paint to
	 */
	public void paint(Graphics g) {
		drawAxis(g);
		drawGraph(g);
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		JFrame jf = new JFrame();
		RichScrollingGraph rsg = new RichScrollingGraph(0,255,100,10);
		jf.add(rsg);
		jf.pack();
		jf.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jf.setVisible(true);
		while(true) {
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			rsg.addData(Math.random() > 0.5 ? 0 :  255);
		}

	}

}
