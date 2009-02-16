/* Copyright 2009 Thomas Degris
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

package org.rlcommunity.critter.svg;

import java.awt.geom.AffineTransform;
import java.awt.geom.PathIterator;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.rlcommunity.critter.ObjectStateDynamics;
import org.rlcommunity.critter.Polygon;
import org.rlcommunity.critter.SimulatorObject;
import org.rlcommunity.critter.SimulatorState;
import org.rlcommunity.critter.Vector2D;

import com.kitfox.svg.SVGElement;
import com.kitfox.svg.SVGElementException;
import com.kitfox.svg.SVGException;
import com.kitfox.svg.SVGUniverse;
import com.kitfox.svg.ShapeElement;
import com.kitfox.svg.animation.AnimationElement;
import java.net.URL;

public class Loader {

	static protected SVGUniverse universe = null;

	private int id;

	public Loader(int id) {
		this.id = id;
	}

	static protected URI load(String pictureName) {
        System.out.println(pictureName);
        try{
		String resourceName = String.format("/resources/%s.svg", pictureName);
                URL url = Loader.class.getResource(resourceName);
                assert url!=null : "Could not load resource: "+resourceName+" from the jar.";
                return universe.loadSVG(url);
        } catch (Throwable t) {System.exit(1);}
        return null;
	}
        
        public static void initSVGUniverse() {
                universe = new SVGUniverse();
        }


	static protected String getNativeTransformation(SVGElement element) {
		try {
			if (!element.hasAttribute("transform", AnimationElement.AT_XML)) {
				element.addAttribute("transform", AnimationElement.AT_XML, "");
				return "";
			}
			return element.getPresAbsolute("transform").getStringValue();
		} catch (SVGElementException e) {
			e.printStackTrace();
		}
		return "";
	}

	public SimulatorObject loadStaticObject(String objectName,
			Vector2D position, double direction) {
		SimulatorObject object = new SimulatorObject(objectName, id++);
		ShapeDrawing shapeDrawing = object.setSVG(objectName);
		shapeDrawing.resetNativeTranslation();
		walkInTreeForShapes(universe.getElement(shapeDrawing.root()),
				"",
				position,
				direction);
		object.setPosition(position);
		object.setDirection(direction);
		return object;
	}

	private void walkInTreeForShapes(SVGElement element, String transform,
			Vector2D position, double direction) {
		try {
			if (element.hasAttribute("stype", AnimationElement.AT_XML))
				addChild(element, transform, position, direction);
		} catch (SVGElementException e) {
			e.printStackTrace();
		}
		transform = transform + getNativeTransformation(element);
		Vector<SVGElement> children = new Vector<SVGElement>();
		element.getChildren(children);
		for (SVGElement child : children) {
			walkInTreeForShapes(child, transform, position, direction);
		}
	}

	static private List<Vector2D> createVertexList(ShapeElement shapeElement,
			String transform) {
		AffineTransform at = getTransform(transform);
		PathIterator iterator = shapeElement.getShape().getPathIterator(at);
		double[] coords = new double[6];
		List<Vector2D> vertexList = new ArrayList<Vector2D>();
		while (!iterator.isDone()) {
			int pathSegmentType = iterator.currentSegment(coords);
			if (pathSegmentType == PathIterator.SEG_LINETO) {
				vertexList.add(new Vector2D(coords[0], coords[1]));
			}
			iterator.next();
		}
		return vertexList;
	}

	private static AffineTransform getTransform(String transform) {
		if (transform.length() == 0)
			return null;

		String transformedSeparated = transform.replace(")", ");");
		String[] splitted = transformedSeparated.split(";");
		AffineTransform at = new AffineTransform();
		at.setToIdentity();
		for (String singleTransform : splitted) {
			try {
				at.concatenate(SVGElement.parseSingleTransform(singleTransform));
			} catch (SVGException e) {
				e.printStackTrace();
			}
		}
		return at;
	}

	private void addChild(SVGElement element, String transform,
			Vector2D position, double direction) {
		SimulatorObject object = new SimulatorObject("fromSVG", id++);
		Polygon shape = new Polygon();
		for (Vector2D vertex : createVertexList((ShapeElement) element,
				transform)) {
			shape.addPoint(vertex.x, vertex.y);
		}
		shape.doneAddPoints();
		object.setShape(shape);
		object.addState(new ObjectStateDynamics(0.5, 2));
		object.setPosition(position);
		object.setDirection(direction);
	}

	public int objectId() {
		return id;
	}
}
