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

package org.rlcommunity.critterbot.simulator;

import com.sun.javadoc.SourcePosition;
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

        int numDirectLightView=0;
        double dist =0;
        SimulatorObject sensor;
        ObjectStateLightSensor lightSensor;
        ObjectStateLightSensor oldLightSensor;
        SimulatorObject oldSensor;
        Vector2D sensorPosition = null;

        SimulatorObject source;
        ObjectStateLightSource lightSource;
        Vector2D srcPosition = null;
        Ray saveRay = null;

        double lowerAngle, highAngle=0;

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

            double currentRayAngle = 0, intialAngle;
            
            //ONLY odd number of pixels..compute orientation of first ray in sensor ( clockwise rotation from orientation of sensor). 
            intialAngle = oldSensor.getDirection() + (Math.floor(numPixels / 2)) * angleBetweenRays;            //correct if first ray rotates through PI -PI boarder
            
            
            if (intialAngle > Math.PI) {
                intialAngle = -Math.PI + intialAngle - Math.PI;
            }
            
            currentRayAngle = intialAngle;
            double sumIntensity = 0.0; //store total intensity of each pixel in the sensor
            int maxIntensity=0;

            oldLightSensor.intersections = new LinkedList<RayIntersection>();
            numDirectLightView = 0;

            lowerAngle = intialAngle;
            //for each pixel in the sensor
            for (int Ipixel = 0; Ipixel < numPixels; Ipixel++) {
                //as we rotate counterclockwise through pixels may cross PI -PI boarder
                if (currentRayAngle < -Math.PI) {
                    currentRayAngle = Math.PI + currentRayAngle + Math.PI;                //create ray                 
                }
                rayDirection = Vector2D.unitVector(currentRayAngle);
                r = new Ray(sensorPosition, rayDirection);

                //remove robot so it doesn't block ray
                scene.removeSubtree(oldSensor.getRoot());

                //find out first point (object) ray i intersects with
                RayIntersection ri = scene.traceRay(r);
                intersectData = ri;
                oldLightSensor.intersections.add(ri);

                if (intersectData != null) {//if the ray hits nothing then intensity is zero. We probably have no walls

                    //angle between robot's sensor ray and surface normal -- old model (not used)
                    
                    double angle2 = intersectData.normal.direction();

                    //put robot back in...robot may block light on intersection point
                    scene.addSubtree(oldSensor.getRoot());

                    //for each light source sum up intensity at intersection point
                    for (int Jsource = 0; Jsource < numSources; Jsource++) {
                        double intensity = 0;

                        source = sources.get(Jsource);
                        lightSource = (ObjectStateLightSource) source.getState(ObjectStateLightSource.NAME);
                        srcPosition=source.getPosition();
                        
                        //keep track of some sort of maximum possible light intensity
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
                            double angle1 = -srcPosition.minus(intersectData.point).direction();
                            double tmpAngle2 = -angle2;
                            
                            //convert to 2PI coordinate system
                            if(tmpAngle2 < 0)tmpAngle2 = tmpAngle2 + 2.0*Math.PI;
                            if(angle1 < 0)angle1 = -angle1 + 2.0*Math.PI;
                            double angleOfIncidence2 = Math.abs(tmpAngle2 - angle1);
                        

                            //is point illuminated?
                            if (scene.isVisible(srcPosition, intersectData.point)) {
                                double dist1 = srcPosition.distance(intersectData.point); 
                                double dist2 = intersectData.point.distance(sensorPosition);
                                
                                double reflectPen = 1.0;//0.5

                                int distancePower = 2;
                                
                                //old calc
                                //intensity =   lightSource.getIntensity() * (1.0 / Math.pow(dist1, 2)) * (1.0 / Math.pow(dist1, 2)) + Math.abs(Math.cos(angleOfIncidence) * Math.cos(angleOfIncidence2));
                                
                                //Lambertian reflectance model -- distance seems odd
                              // intensity = reflectPen*lightSource.getIntensity()*Math.abs((srcPosition.minus(intersectData.point)).dot(intersectData.normal)*Math.cos(angleOfIncidence2))*(1.0 / Math.pow(dist1+dist2, distancePower));//*(1.0 / Math.pow(dist2, distancePower)));
                               intensity = reflectPen*lightSource.getIntensity()*Math.abs((srcPosition.minus(intersectData.point)).dot(intersectData.normal)*(Math.pow(Math.cos(angleOfIncidence2),2)))*(1.0 / Math.pow(dist1+dist2, distancePower));//*(1.0 / Math.pow(dist2, distancePower)));
                                        
                                //sum up light from multiple sources and from each pixel
                                sumIntensity += intensity;

                            }
                     }
                        /* if(Ipixel == numPixels-1)
                        {
                            scene.removeSubtree(oldSensor.getRoot());
                               
                            double directLightTol = Math.PI/4.0;
                            double angleOfForwardLight = -srcPosition.minus(sensorPosition).direction();                            
                            double sensorAngle = -sensor.getDirection();
                                
                            //convert to 2PI angle system for convience 
                            if(sensorAngle < 0) sensorAngle = 2*Math.PI + sensorAngle;
                            if(angleOfForwardLight < 0) angleOfForwardLight = 2*Math.PI + angleOfForwardLight;

                                boolean nearOrigin = false;
                                if(sensorAngle > angleOfForwardLight)
                                {
                                    nearOrigin = (2*Math.PI - sensorAngle + angleOfForwardLight) < directLightTol;
                                }
                                else
                                    nearOrigin = (2*Math.PI - angleOfForwardLight + sensorAngle) < directLightTol;
                                if(Math.abs(sensorAngle - angleOfForwardLight) < directLightTol || nearOrigin)
                                {
                                    numDirectLightView++;
                                    intensity = lightSource.getIntensity() * (1.0 / (Math.pow(srcPosition.distance(sensorPosition), 2)));
                                    sumIntensity += intensity;
                                }                        
                             scene.addSubtree(oldSensor.getRoot());
                        } */
                                
                    }//loop over sources
                }//test for object intersection

                highAngle = currentRayAngle;
                currentRayAngle -= angleBetweenRays; //next ray rotating clockwise

                saveRay = r;

            } //loop over pixels

            //Add one ray that is based on direct line of site to the light source

            //scene.addSubtree(oldSensor.getRoot());

            double lineOSSum = 0;
            int lineOSCount = 0;
             for (int Jsource = 0; Jsource < numSources; Jsource++)
             {
                  double intensity = 0;

                  source = sources.get(Jsource);
                  lightSource = (ObjectStateLightSource) source.getState(ObjectStateLightSource.NAME);
                  srcPosition=source.getPosition();

                    scene.removeSubtree(oldSensor.getRoot());

                  Vector2D d = Vector2D.unitVector(oldSensor.getDirection());
                  Vector2D v = srcPosition.minus(sensorPosition);
                 // Vector2D r = Vector2D.unitVector(saveRay.)

                  //Vector2D v = Vector2D.unitVector(Math.atan2(sensorPosition.minus(srcPosition).x, sensorPosition.minus(srcPosition).y));


                  //Vector2D v = sensorPosition.minus(srcPosition);

                 // double myBoundingAngle = (highAngle + lowerAngle)/2.0;
                 // double angle = Math.acos(u.dot(v) / (u.length() / v.length()));

                   // if(Ksensor==0)System.out.println("low = " + lowerAngle + " quest = "+ angle + " high = "+highAngle);
                 // if(scene.isVisible(srcPosition, sensorPosition))
                  if(angle(v,d) <= angle(saveRay.dir,d) && scene.isVisible(srcPosition, sensorPosition))
                  {
                    //if(Ksensor==0)System.out.println("i can see the light");
                    double dis = srcPosition.distance(sensorPosition);
                    intensity = lightSource.getIntensity() * (1.0 / (Math.pow(srcPosition.distance(sensorPosition), 2)));
                    lineOSCount = 1;
                  }
                  //else
                  //  if(Ksensor==0)System.out.println("i CAN NOT see the light");

                scene.addSubtree(oldSensor.getRoot());

                  lineOSSum += intensity;
             }

            //sensor reading is average of pixel readings, unless greater than sum intensity in the environment
            double noise = aRandom.nextGaussian() * lightSensor.getError();
            double doubleReading = (sumIntensity + lineOSSum) / (numPixels+numDirectLightView+lineOSCount);

            int reading = (int)(doubleReading * (1.0 + noise));
            if(reading < 0) reading =0;
            if(reading > maxIntensity)lightSensor.setLightSensorValue(maxIntensity);
            else lightSensor.setLightSensorValue(reading);

 //System.out.println("sensor(" + Ksensor +") intensity = " + lightSensor.getLightSensorValue());
            
        } //loop over sensors
 //System.out.println("------");

    }

    public double angle(Vector2D v1, Vector2D v2)
    {
      return Math.acos(v1.dot(v2)/(v1.length()*v2.length()));
    }
}

