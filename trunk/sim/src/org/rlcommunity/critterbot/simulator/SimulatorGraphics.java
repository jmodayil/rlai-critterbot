/* Copyright 2009 Marc G. Bellemare
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

/*
 * (c) 2009 Marc G. Bellemare and the University of Alberta.
 */

package org.rlcommunity.critterbot.simulator;

import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.image.ImageObserver;
import java.text.AttributedCharacterIterator;

/**
 * A wrapper class for a Graphics object which applies an additional scaling
 *   factor to all methods. Not the same as using an affine transform.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class SimulatorGraphics extends Graphics {

    private double aScale;
    private Graphics aGfx;

    private int[] aScaledXPoints;
    private int[] aScaledYPoints;

    public boolean aNoScaling = false;
    
    public SimulatorGraphics(Graphics pGfx, double pScale) {
        aScale = pScale;
        aGfx = pGfx;
    }

    /**
     * Scales the given list of points and stores the result in a private array.
     * 
     * @param xPoints The x coordinates of the points.
     * @param yPoints The y coordinates of the points.
     */
    private void scalePoints(int[] xPoints, int[] yPoints) {
      if (aNoScaling) throw new UnsupportedOperationException("Cannot refuse to scale point arrays.");
      
        // Recreate the array of interest
        if (aScaledXPoints == null || xPoints.length != aScaledXPoints.length) {
            aScaledXPoints = new int[xPoints.length];
            aScaledYPoints = new int[yPoints.length];
        }

        for (int i = 0; i < xPoints.length; i++) {
            aScaledXPoints[i] = scale(xPoints[i]);
            aScaledYPoints[i] = scale(yPoints[i]);
        }
    }

    /**
     * Scales the given list of points and stores the result in a private array.
     *
     * @param xPoints The x coordinates of the points.
     * @param yPoints The y coordinates of the points.
     */
    private void scalePoints(double[] xPoints, double[] yPoints) {
      if (aNoScaling) throw new UnsupportedOperationException("Cannot refuse to scale point arrays.");

      // Recreate the array of interest
        if (aScaledXPoints == null || xPoints.length != aScaledXPoints.length) {
            aScaledXPoints = new int[xPoints.length];
            aScaledYPoints = new int[yPoints.length];
        }

        for (int i = 0; i < xPoints.length; i++) {
            aScaledXPoints[i] = scale(xPoints[i]);
            aScaledYPoints[i] = scale(yPoints[i]);
        }
    }

    private int scale (int p) {
        if (aNoScaling) return p;
        return (int)Math.round(p * aScale);
    }

    private int scale (double p) {
      if (aNoScaling) return (int)Math.round(p);
        return (int)Math.round(p * aScale);
    }

    public double scaleToDouble (double p) {
        return p * aScale;
    }
    
    /**** Graphics interface ****/

    public void translate(int x, int y) {
        aGfx.translate(x, y);
    }

    public void setXORMode(Color c1) {
        aGfx.setXORMode(c1);
    }

    public void setPaintMode() {
        aGfx.setPaintMode();
    }

    public void setFont(Font font) {
        aGfx.setFont(font);
    }

    public void setColor(Color c) {
        aGfx.setColor(c);
    }

    public void setClip(Shape clip) {
        aGfx.setClip(clip);
    }

    public void setClip(int x, int y, int width, int height) {
        aGfx.setClip(x, y, width, height);
    }

    public FontMetrics getFontMetrics(Font f) {
        return aGfx.getFontMetrics(f);
    }

    public Font getFont() {
        return aGfx.getFont();
    }

    public Color getColor() {
        return aGfx.getColor();
    }

    public Rectangle getClipBounds() {
        return aGfx.getClipBounds();
    }

    public Shape getClip() {
        return aGfx.getClip();
    }

    public void fillRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        aGfx.fillRoundRect(scale(x), scale(y), scale(width), scale(height),
                scale(arcWidth), scale(arcHeight));
    }

    public void fillRect(int x, int y, int width, int height) {
        aGfx.fillRect(scale(x), scale(y), scale(width), scale(height));
    }

    public void fillPolygon(int[] xPoints, int[] yPoints, int nPoints) {
        scalePoints(xPoints, yPoints);
        aGfx.fillPolygon(aScaledXPoints, aScaledYPoints, nPoints);
    }

    public void fillOval(int x, int y, int width, int height) {
        aGfx.fillOval(scale(x), scale(y), scale(width), scale(height));
    }

    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        aGfx.fillArc(scale(x), scale(y), scale(width), scale(height), startAngle, arcAngle);
    }

    public void drawString(AttributedCharacterIterator iterator, int x, int y) {
        aGfx.drawString(iterator, scale(x), scale(y));
    }

    public void drawString(String str, int x, int y) {
        aGfx.drawString(str, scale(x), scale(y));
    }

    public void drawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        aGfx.drawRoundRect(scale(x), scale(y), scale(width), scale(height),
                scale(arcWidth), scale(arcHeight));
    }

    public void drawPolyline(int[] xPoints, int[] yPoints, int nPoints) {
        scalePoints (xPoints, yPoints);

        aGfx.drawPolyline(aScaledXPoints, aScaledYPoints, nPoints);
    }

    public void drawPolygon(int[] xPoints, int[] yPoints, int nPoints) {
        scalePoints (xPoints, yPoints);

        aGfx.drawPolygon(aScaledXPoints, aScaledYPoints, nPoints);
    }

    public void drawOval(int x, int y, int width, int height) {
        aGfx.drawOval(x, y, width, height);
    }

    public void drawLine(int x1, int y1, int x2, int y2) {
        aGfx.drawLine(scale(x1), scale(y1), scale(x2), scale(y2));
    }

    public void drawRect(double x1, double y1, double x2, double y2) {
        aNoScaling = true;
        aGfx.drawLine(scale(x1), scale(y1), scale(x2), scale(y2));
        aNoScaling = false;
    }

    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, Color bgcolor, ImageObserver observer) {
        return aGfx.drawImage(img, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, bgcolor, observer);
    }

    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, ImageObserver observer) {
        return aGfx.drawImage(img, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, observer);
    }

    public boolean drawImage(Image img, int x, int y, int width, int height, Color bgcolor, ImageObserver observer) {
        return aGfx.drawImage(img, x, y, width, height, bgcolor, observer);
    }

    public boolean drawImage(Image img, int x, int y, Color bgcolor, ImageObserver observer) {
        return aGfx.drawImage(img, x, y, bgcolor, observer);
    }

    public boolean drawImage(Image img, int x, int y, int width, int height, ImageObserver observer) {
        return aGfx.drawImage(img, x, y, width, height, observer);
    }

    public boolean drawImage(Image img, int x, int y, ImageObserver observer) {
        return aGfx.drawImage(img, x, y, observer);
    }

    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        aGfx.drawArc(x, y, width, height, startAngle, arcAngle);
    }

    public void dispose() {
        aGfx.dispose();
    }

    public Graphics create(int x, int y, int width, int height) {
        return aGfx.create(x, y, width, height);
    }

    public Graphics create() {
        return aGfx.create();
    }

    public void copyArea(int x, int y, int width, int height, int dx, int dy) {
        aGfx.copyArea(x, y, width, height, dx, dy);
    }

    public void clipRect(int x, int y, int width, int height) {
        aGfx.clipRect(x, y, width, height);
    }

    public void clearRect(int x, int y, int width, int height) {
        aGfx.clearRect(x, y, width, height);
    }


    /*** Additional methods for better precision in scaling */

    
    public void drawOval(double x, double y, double width, double height) {
        aGfx.drawOval(scale(x), scale(y), scale(width), scale(height));
    }

    public void drawRoundRect(double x, double y, double width, double height,
            double arcWidth, double arcHeight) {
        aGfx.drawRoundRect(scale(x), scale(y), scale(width), scale(height),
                scale(arcWidth), scale(arcHeight));
    }

    public void drawLine(double x1, double y1, double x2, double y2) {
        aGfx.drawLine(scale(x1), scale(y1), scale(x2), scale(y2));
    }

    public void drawPolyline(double[] xPoints, double[] yPoints, int nPoints) {
        scalePoints (xPoints, yPoints);

        aGfx.drawPolyline(aScaledXPoints, aScaledYPoints, nPoints);
    }
    
    public void drawImage(Image pImage, double x, double y, ImageObserver observer) {
        aGfx.drawImage(pImage, scale(x), scale(y), observer);
    }

    public void drawString(String pString, double x, double y) {
        aGfx.drawString(pString, scale(x), scale(y));
    }


    
    /*** Methods that assume aGfx is a Graphics2D */
    AffineTransform getTransform() {
        Graphics2D g2d = (Graphics2D) aGfx;
        return g2d.getTransform();
    }

    void setTransform(AffineTransform pTransform) {
        Graphics2D g2d = (Graphics2D) aGfx;
        g2d.setTransform(pTransform);
    }

    public Graphics2D g2d() {
        return (Graphics2D) aGfx;
    }
}
