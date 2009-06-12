/* Copyright 2009 Brian Tanner
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

package org.rlcommunity.environments.critterbot.visualizer;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.AffineTransform;

import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;
import java.util.Observable;
import java.util.Observer;
import org.rlcommunity.environments.critterbot.messages.CritterScreenRequest;
import org.rlcommunity.rlglue.codec.types.Observation;
import org.rlcommunity.rlglue.codec.types.Reward_observation_terminal;
import rlVizLib.visualization.PollingVizComponent;
import rlVizLib.visualization.SelfUpdatingVizComponent;
import rlVizLib.visualization.VizComponentChangeListener;
import rlVizLib.visualization.interfaces.GlueStateProvider;

public class CritterImageComponent implements SelfUpdatingVizComponent,Observer {

    RenderedImage nextImage = null;

    public CritterImageComponent(GlueStateProvider theVisualizer) {
        theVisualizer.getTheGlueState().addObserver(this);
    }

    public void render(Graphics2D g) {
        BufferedImage thisImage = CritterScreenRequest.Execute().getImage();

        AffineTransform saveAT = g.getTransform();
        g.setColor(Color.WHITE);
        g.fill(new Rectangle(1, 1));
        g.scale(.002, .002);

        g.drawImage(thisImage, null, null);
 //       g.drawImage(thisImage.getScaledInstance(500, 500, Image.SCALE_SMOOTH), null, null);

        g.setTransform(saveAT);
    }
    /**
     * This is the object (a renderObject) that should be told when this component needs to be drawn again.
     */
    private VizComponentChangeListener theChangeListener;

    public void setVizComponentChangeListener(VizComponentChangeListener theChangeListener) {
        this.theChangeListener = theChangeListener;
    }

    public boolean update() {
        return true;
    }

    public void update(Observable o, Object arg) {
        if (theChangeListener != null) {
            if(arg instanceof Observation){
                theChangeListener.vizComponentChanged(this);
            }
            if(arg instanceof Reward_observation_terminal){
                theChangeListener.vizComponentChanged(this);
            }
        }
    }
}
