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
package org.rlcommunity.environments.critterbot.visualizer;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.util.Observable;
import java.util.Observer;
import org.rlcommunity.rlglue.codec.types.Observation;
import rlVizLib.general.TinyGlue;
import rlVizLib.visualization.SelfUpdatingVizComponent;
import rlVizLib.visualization.VizComponentChangeListener;

/**
 *
 * @author Brian Tanner
 */
class CritterSensorComponent implements SelfUpdatingVizComponent, Observer {

    VizComponentChangeListener theChangeListener = null;
    TinyGlue theTinyGlue = null;

    public CritterSensorComponent(CritterEnvVisualizer aThis) {
        theTinyGlue = aThis.getTheGlueState();
        theTinyGlue.addObserver(this);
    }

    public void setVizComponentChangeListener(VizComponentChangeListener arg0) {
        theChangeListener = arg0;
    }

    public void render(Graphics2D g) {
        //Draw things here
        Observation theSensorReadings = theTinyGlue.getLastObservation();
        AffineTransform saveAT = g.getTransform();
        g.scale(.001, .001);
        g.setFont(new Font("Serif", Font.PLAIN, 24));

        g.setColor(Color.black);
        g.drawString("Light: " + theSensorReadings.intArray[40], 0, 50);
        g.fillRect(0, 0, 50, 50);

        g.setTransform(saveAT);

    }

    //The world has changed!
    public void update(Observable o, Object arg) {
        //For now just always redraw
        if (theChangeListener != null) {
            theChangeListener.vizComponentChanged(this);
        }

    }
}
