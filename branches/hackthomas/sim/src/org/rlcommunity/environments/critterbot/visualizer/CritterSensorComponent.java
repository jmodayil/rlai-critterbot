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
import java.awt.Stroke;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.util.Observable;
import java.util.Observer;
import org.rlcommunity.rlglue.codec.types.Observation;
import rlVizLib.general.TinyGlue;
import rlVizLib.visualization.SelfUpdatingVizComponent;
import rlVizLib.visualization.VizComponentChangeListener;
import rlVizLib.visualization.interfaces.GlueStateProvider;

/**
 *
 * @author Brian Tanner
 */
class CritterSensorComponent implements SelfUpdatingVizComponent, Observer {

    VizComponentChangeListener theChangeListener = null;
    TinyGlue theTinyGlue = null;

    public CritterSensorComponent(GlueStateProvider theVisualizer) {
        theTinyGlue=theVisualizer.getTheGlueState();
        theTinyGlue.addObserver(this);
    }

    public void setVizComponentChangeListener(VizComponentChangeListener arg0) {
        theChangeListener = arg0;
    }

    public void render(Graphics2D g) {
        //Draw things here
        Observation theSensorReadings = theTinyGlue.getLastObservation();
        if(theSensorReadings==null)
            return;
        AffineTransform saveAT = g.getTransform();
        g.scale(.001, .001);
        g.setFont(new Font("Serif", Font.PLAIN, 50));
        int light[] = {0, 0, 0, 0};
        if((light[0] = theSensorReadings.intArray[40]/3) > 400)
            light[0] = 400;
        if((light[1] = theSensorReadings.intArray[41]/3) > 400)
            light[1] = 400;
        if((light[2] = theSensorReadings.intArray[42]/3) > 400)
            light[2] = 400;
        if((light[3] = theSensorReadings.intArray[43]/3) > 400)
            light[3] = 400;
        g.setColor(Color.black);
        g.drawString("Light", 154, 70);
        g.drawString("Distance", 425, 70);
        g.drawString("Accel", 155, 570);
        g.drawString("Rotation", 430, 570);
        for(int i = 0; i < 4; i++) {
            g.setColor(Color.gray);
            g.drawRect(159 + i * 30, 99, 22, 402);
            g.setColor(Color.yellow);
            g.fillRect(160 + i * 30, 500 - light[i], 20, light[i]);
        }
        for(int i = 0; i < 10; i++) {
            g.setColor(Color.gray);
            g.drawRect(399 + 30 * i, 244, 22, 257);
            g.setColor(Color.blue);
            g.fillRect(400 + 30 * i, 500 - theSensorReadings.intArray[i + 22],
                20, theSensorReadings.intArray[i + 22]);
        }
        g.setColor(Color.lightGray);
        g.drawRect(65, 600, 300, 300);
        g.drawOval(402, 602, 296, 296);
        int accelX = theSensorReadings.intArray[16];
        int accelY = theSensorReadings.intArray[17];
        //int accelZ = theSensorReadings.intArray[18];
        int rot = theSensorReadings.intArray[19];
        g.setColor(Color.red);
        g.drawLine(215, 750, 215 - accelY, 750 - accelX);
        g.drawArc(400, 600, 300, 300, 90, (int)(rot / 5));
        g.drawArc(404, 604, 292, 292, 90, (int)(rot / 5));

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
