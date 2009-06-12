/* Copyright 2009 Michael Sokolsky
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

/**
 * SimulatorAgent
 *
 * This class defines agent-specific methods and values. This includes
 *  sensors (which should be specific classes, or at least bundled into a
 *  Sensor class), actions and such.
 *
 * @author Marc G. Bellemare
 */

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.geom.AffineTransform;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;

public class SimulatorAgent extends SimulatorObject {
	/** Three values representing the current robot command */
	protected double aCmdX, aCmdY, aCmdTheta;

	/** Robot Image (for now) */
	private Image robotop;

	/**
	 * Creates a new instance of a SimulatorAgent
	 */
	public SimulatorAgent(String pLabel, int pId) {
		super(pLabel, pId);
		aCmdX = aCmdY = aCmdTheta;
		try {
                        String resourceName="/resources/robotop.png";
			URL url = this.getClass().getResource(resourceName);
                        assert url!=null : "Could not load resource: "+resourceName+" from the jar.";
			robotop = ImageIO.read(url);
      // Was initially: 40,68
			robotop = robotop.getScaledInstance(27, 40, Image.SCALE_SMOOTH);
		} catch (IOException ioe) {
			System.out.println(ioe.toString());
		}
	}

	@Override
	public void draw(SimulatorGraphics g) {
		if (true) {
			AffineTransform oldXform = g.getTransform();
			AffineTransform newXform = (AffineTransform) (oldXform.clone());
			newXform.rotate(Math.PI / 2 + aDir,
                    g.scaleToDouble(aPos.x), g.scaleToDouble(aPos.y));
			g.setTransform(newXform);
			g.drawImage(robotop, aPos.x - 0.13, aPos.y - 0.12, null);
			g.setTransform(oldXform);

			Color tempC = g.getColor();
			g.setColor(Color.lightGray);
			//g.drawString(aLabel, aPos.x + 0.2, aPos.y + 0.2);
			g.setColor(tempC);
		} else if (false) {
            if (true)
                throw new UnsupportedOperationException("Code has not been ported to use SimulatorGraphics.");
			Color tempC = g.getColor();
			g.setColor(Color.black);
			g.drawOval((int) aPos.x - 10, (int) aPos.y - 10, 20, 20);
			g.setColor(Color.red);
			g.drawLine((int) aPos.x,
					(int) aPos.y,
					(int) (aPos.x + 10 * Math.cos(aDir)),
					(int) (aPos.y + 10 * Math.sin(aDir)));
			g.setColor(Color.lightGray);
			g.drawString(aLabel, (int) aPos.x + 10, (int) aPos.y + 20);
			g.setColor(tempC);
		}
		super.draw(g);
	}

	@Override
	public Object clone() {
		SimulatorAgent sa = new SimulatorAgent(this.aLabel, this.aId);
		sa.copyFrom(this);

		// Clone the object's children
		for (SimulatorObject c : aChildren) {
			SimulatorObject childClone = (SimulatorObject) c.clone();
			sa.addChild(childClone);
		}

		return sa;
	}

	@Override
	protected void copyFrom(SimulatorObject obj) {
		super.copyFrom(obj);
		SimulatorAgent org = (SimulatorAgent) obj;

		// Make a copy of relevant agent data
		this.aCmdX = org.aCmdX;
		this.aCmdY = org.aCmdY;
		this.aCmdTheta = org.aCmdTheta;
	}
}
