package org.rlcommunity.environments.critterbot.visualizer;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.AffineTransform;

import java.awt.geom.Ellipse2D;
import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;
import java.util.Observable;
import java.util.Observer;
import org.rlcommunity.environments.critterbot.messages.CritterScreenRequest;
import rlVizLib.visualization.SelfUpdatingVizComponent;
import rlVizLib.visualization.VizComponentChangeListener;

public class CritterImageComponent implements SelfUpdatingVizComponent, Observer {

    private CritterEnvVisualizer critterViz;
    RenderedImage nextImage=null;

    int lastUpdateStep = -1;

    public CritterImageComponent(CritterEnvVisualizer acrobotVisualizer) {
        critterViz = acrobotVisualizer;
        acrobotVisualizer.getTheGlueState().addObserver(this);
    }

    public void render(Graphics2D g) {
        RenderedImage thisImage=nextImage;
        if(thisImage==null)return;
        
        AffineTransform saveAT = g.getTransform();
        g.setColor(Color.WHITE);
        g.fill(new Rectangle(1, 1));
        g.scale(.002, .002);

        g.drawRenderedImage(thisImage, null);
        
        g.setTransform(saveAT);
    }
    /**
     * This is the object (a renderObject) that should be told when this component needs to be drawn again.
     */
    private VizComponentChangeListener theChangeListener;

    public void setVizComponentChangeListener(VizComponentChangeListener theChangeListener) {
        this.theChangeListener = theChangeListener;
    }

    /**
     * This will be called when TinyGlue steps.
     * @param o
     * @param arg
     */
    public void update(Observable o, Object arg) {
        if (theChangeListener != null) {
            nextImage=CritterScreenRequest.Execute().getImage();
            theChangeListener.vizComponentChanged(this);
        }
    }
}
