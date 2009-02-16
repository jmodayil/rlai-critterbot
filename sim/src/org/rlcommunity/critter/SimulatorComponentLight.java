/* Copyright 2009 Adam White
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

package org.rlcommunity.critter;

import java.util.Random;
import java.util.LinkedList;

/**
 * SimulatorComponentLight
 *
 * This component deals with light-emitting objects and light sensors,
 *  pairing the two into a happy match.
 *
 * @author Marc G. Bellemare
 */
public class SimulatorComponentLight implements SimulatorComponent {

    public static final String NAME = "light";

    protected Random aRandom;
    
    public SimulatorComponentLight() {
        this(new Random());
        System.err.println ("Deprecated: using local Random object.");
    }

    public SimulatorComponentLight(Random pRandom) {
        aRandom = pRandom;
    }

    /** Computes what light sensors should receive given the current state,
     *  and set the result in the next state.
     *
     * @param pCurrent The current state of the system (must not be modified)
     * @param pNext  The next state of the system (where results are stored)
     * @param delta  The number of milliseconds elapsed between pCurrent
     *   and pNext.
     */
    public void apply(SimulatorState pCurrent, SimulatorState pNext, int delta) {

        Scene scene = new Scene(pCurrent);
        Vector2D rayDirection;
        RayIntersection intersectData;
        Ray r;

        double dist =0;
        SimulatorObject sensor;
        ObjectStateLightSensor lightSensor;
        ObjectStateLightSensor oldLightSensor;
        SimulatorObject oldSensor;
        Vector2D sensorPosition = null;

        SimulatorObject source;
        ObjectStateLightSource lightSource;
        Vector2D srcPosition = null;

        LinkedList<SimulatorObject> sensors = pNext.getObjects(ObjectStateLightSensor.NAME);
        int numSensors = sensors.size();
        LinkedList<SimulatorObject> sources = pCurrent.getObjects(ObjectStateLightSource.NAME);
        int numSources = sources.size();
                        
        for (int Ksensor = 0; Ksensor < numSensors; Ksensor++) {
            sensor = sensors.get(Ksensor);
            lightSensor = (ObjectStateLightSensor) sensor.getState(ObjectStateLightSensor.NAME);
            sensorPosition = sensor.getPosition();


            oldSensor = pCurrent.getObject(sensor);
            oldLightSensor = (ObjectStateLightSensor) oldSensor.getState(ObjectStateLightSensor.NAME);

            double sensorWidth = oldLightSensor.getSensorWidth();
            int numPixels = oldLightSensor.getNumPixels();
            double sensorDepth = oldLightSensor.getSensorDepth();

            double angleBetweenRays = (Math.atan((sensorWidth / numPixels) / sensorDepth));

            double currentRayAngle = 0;
            //even or odd number of pixels..compute orientation of first ray in sensor (counter clockwise rotation from orientation of sensor).
            if (numPixels % 2 == 0) {
                currentRayAngle = sensor.getDirection() - angleBetweenRays / 2.0 + (Math.floor(numPixels / 2)) * angleBetweenRays;
            } else {
                currentRayAngle = oldSensor.getDirection() + (Math.floor(numPixels / 2)) * angleBetweenRays;            //correct if first ray rotates through PI -PI boarder
            }
            if (currentRayAngle > Math.PI) {
                currentRayAngle = -Math.PI + currentRayAngle - Math.PI;
            }
            double sumIntensity = 0.0; //store total intensity of each pixel in the sensor
            int maxIntensity=0;
                    
            //for each pixel in the sensor
            for (int Ipixel = 0; Ipixel < numPixels; Ipixel++) {
                //as we rotate clockwise through pixels may cross PI -PI boarder
                if (currentRayAngle < -Math.PI) {
                    currentRayAngle = Math.PI + currentRayAngle + Math.PI;                //create ray                 
                }
                rayDirection = Vector2D.unitVector(currentRayAngle);
                rayDirection.normalize();
                r = new Ray(sensorPosition, rayDirection);

                //remove robot so it doesn't block ray
                scene.removeSubtree(oldSensor.getRoot());

                //find out first point (object) ray i intersects with
                intersectData = scene.traceRay(r);

                if (intersectData != null) {//if the ray hits nothing then intensity is zero. We probably have no walls

                    //angle between robot's sensor ray and surface normal -- old model (not used)
                    //double angle1 = sensorPosition.minus(intersectData.point).direction();
                    
                    double angle2 = intersectData.normal.direction();
                    //double angleOfIncidence = Math.abs(angle1) - Math.abs(angle2);

                    //put robot back in...robot may block light on intersection point
                    scene.addSubtree(oldSensor.getRoot());

                    //for each light source sum up intensity at intersection point
                    for (int Jsource = 0; Jsource < numSources; Jsource++) {
                        double intensity = 0;

                        source = sources.get(Jsource);
                        lightSource = (ObjectStateLightSource) source.getState(ObjectStateLightSource.NAME);
                        srcPosition=source.getPosition();
                        

                        if(Ipixel == 0)maxIntensity +=lightSource.getIntensity();
                        //unlikely case where ray intersects the light source directly
                        double slope = (sensorPosition.x - srcPosition.x) / (sensorPosition.y - srcPosition.y);
                        double tol = Math.pow(10, -15);
                        if (Math.abs(slope - (rayDirection.x / rayDirection.y)) < tol) {
                            intensity = lightSource.getIntensity() * (1.0 / Math.pow(srcPosition.distance(sensorPosition), 2));
                            sumIntensity += intensity;
                        } else {

                            //light source should have no polygon...incase marc changes his name
                            scene.removeSubtree(source.getRoot());

                            //find angle between light source and surface normal
                            double angle1 = srcPosition.minus(intersectData.point).direction();
                            double angleOfIncidence2 = Math.abs(angle1) - Math.abs(angle2);

                            //is point illuminated?
                            if (scene.isVisible(srcPosition, intersectData.point)) {
                                double dist1 = srcPosition.distance(intersectData.point); 
                                double dist2 = intersectData.point.distance(sensorPosition);

                                int distancePower = 2;
                                
                                //old calc
                                //intensity =   lightSource.getIntensity() * (1.0 / Math.pow(dist1, 2)) * (1.0 / Math.pow(dist1, 2)) + Math.abs(Math.cos(angleOfIncidence) * Math.cos(angleOfIncidence2));
                                
                                //Lambertian reflectance model -- distance seems odd
                               intensity = lightSource.getIntensity()*Math.abs((srcPosition.minus(intersectData.point)).dot(intersectData.normal)*Math.cos(angleOfIncidence2))*(1.0 / Math.pow(dist1, distancePower));//*(1.0 / Math.pow(dist2, distancePower)));
                                
                                //sum up light from multiple sources and from each pixel
                                sumIntensity += intensity;

                            }
                     }
                                
                    }//loop over sources
                }//test for object intersection
                currentRayAngle -= angleBetweenRays; //next ray rotating clockwise

            } //loop over pixels


            //sensor reading is average of pixel readings, unless greater than sum intensity in the environment
            double noise = aRandom.nextGaussian();
            int reading = (int)(sumIntensity / numPixels + noise);
            if(reading<0) reading =0;
            if(reading > maxIntensity)lightSensor.setLightSensorValue(maxIntensity);
            else lightSensor.setLightSensorValue(reading);

 //System.out.println("sensor(" + Ksensor +") intensity = " + lightSensor.getLightSensorValue());
            
        } //loop over sensors
 //System.out.println("------");

    }
}

