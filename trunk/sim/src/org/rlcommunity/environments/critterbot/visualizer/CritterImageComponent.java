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
