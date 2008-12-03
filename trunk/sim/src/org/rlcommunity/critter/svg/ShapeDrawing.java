package org.rlcommunity.critter.svg;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.net.URI;

import org.rlcommunity.critter.Vector2D;

import com.kitfox.svg.SVGDiagram;
import com.kitfox.svg.SVGElement;
import com.kitfox.svg.SVGElementException;
import com.kitfox.svg.SVGException;
import com.kitfox.svg.animation.AnimationElement;

public class ShapeDrawing {

	private final URI rootUrl;

	private SVGDiagram diagram = null;

	private SVGElement element = null;

	private String nativeTransformation = null;

	private final Vector2D nativeTranslation = new Vector2D();

	public ShapeDrawing(String pictureName) {
		rootUrl = Loader.load(pictureName);
		if (rootUrl == null) {
			return;
		}
		diagram = Loader.universe.getDiagram(rootUrl);
		diagram.setIgnoringClipHeuristic(true);
		element = Loader.universe.getElement(rootUrl);
		nativeTransformation = Loader.getNativeTransformation(element);
		setNativeTranslation();
	}

	private void setNativeTranslation() {
		Rectangle2D boundingBox = diagram.getViewRect();
		nativeTranslation.x = boundingBox.getWidth() * -0.5;
		nativeTranslation.y = boundingBox.getHeight() * -0.5;
	}

	public void resetNativeTranslation() {
		nativeTranslation.x = 0;
		nativeTranslation.y = 0;
	}

	public void draw(Graphics g, Vector2D position, double direction) {
		if (diagram == null)
			return;
		try {
			setTransform(position, direction);
			diagram.render((Graphics2D) g);
		} catch (SVGException e) {
			e.printStackTrace();
			return;
		}
	}

	private void setTransform(Vector2D position, double direction) {
		// Transformations are applied from right to left, cool isn't it ?
		String transformation = String.format("translate(%s,%s) rotate(%s) translate(%s,%s) %s",
				position.x,
				position.y,
				Math.toDegrees(direction),
				nativeTranslation.x,
				nativeTranslation.y,
				nativeTransformation);
		try {
			element.setAttribute("transform",
					AnimationElement.AT_XML,
					transformation);
			Loader.universe.updateTime();
		} catch (SVGElementException e) {
			e.printStackTrace();
		} catch (SVGException e) {
			e.printStackTrace();
		}
	}

	public URI root() {
		return rootUrl;
	}
}
