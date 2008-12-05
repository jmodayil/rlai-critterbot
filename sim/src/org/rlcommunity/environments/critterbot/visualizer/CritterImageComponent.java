package org.rlcommunity.environments.critterbot.visualizer;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.AffineTransform;

import java.awt.image.RenderedImage;
import org.rlcommunity.environments.critterbot.messages.CritterScreenRequest;
import rlVizLib.visualization.PollingVizComponent;
import rlVizLib.visualization.VizComponentChangeListener;

public class CritterImageComponent implements PollingVizComponent {

    RenderedImage nextImage = null;

    public CritterImageComponent(CritterEnvVisualizer acrobotVisualizer) {
    }

    public void render(Graphics2D g) {
        RenderedImage thisImage = CritterScreenRequest.Execute().getImage();

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

    public boolean update() {
        return true;
    }
}
