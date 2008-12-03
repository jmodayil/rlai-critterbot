package org.rlcommunity.critter;

/**
 * SimulatorComponentLight
 *
 * This component deals with light-emitting objects and light sensors,
 *  pairing the two into a happy match.
 *
 * @author Marc G. Bellamre
 */
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.*;

public class SimulatorComponentLight implements SimulatorComponent {

    public static final String NAME = "light";

    public SimulatorComponentLight() {
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

        SimulatorObject sensor;
        ObjectStateLightSensor lightSensor;
        ObjectStateLightSensor oldLightSensor;
        SimulatorObject oldSensor;
        Vector2D sensorPosition;

        SimulatorObject source;
        ObjectStateLightSource lightSource;
        Vector2D srcPosition;

        for (int Ksensor = 0; Ksensor < pNext.getObjects(ObjectStateLightSensor.NAME).size(); Ksensor++) {            
            sensor = pNext.getObjects(ObjectStateLightSensor.NAME).get(Ksensor);
            lightSensor = (ObjectStateLightSensor) sensor.getState(ObjectStateLightSensor.NAME);
            sensorPosition = sensor.getPosition();

//        lightSensor.setNumPixels(3);
//        lightSensor.setSensorWidth(0.1);
//        lightSensor.setSensorDepth(50);

            oldSensor = pCurrent.getObject(sensor);
            oldLightSensor = (ObjectStateLightSensor) oldSensor.getState(ObjectStateLightSensor.NAME);

            double sensorWidth = oldLightSensor.getSensorWidth();
            int numPixels = oldLightSensor.getNumPixels();
            double sensorDepth = oldLightSensor.getSensorDepth();

            double angleBetweenRays = Math.atan((sensorWidth / numPixels) / sensorDepth);
            if (sensor.getDirection() < 0.0) {
                angleBetweenRays *= -1;
            }
            double currentRayAngle = sensor.getDirection() - (Math.floor(numPixels / 2)) * angleBetweenRays;

            double sumIntensity = 0; //store total intensity of each pixel in the sensor
            //for each pixel in the sensor
            for (int Ipixel = 0; Ipixel < numPixels; Ipixel++) {
                //figure out direction of ray i
                if (currentRayAngle > Math.PI) {
                    currentRayAngle = -Math.PI + Math.abs(currentRayAngle - Math.PI);
                }
                if (currentRayAngle < -Math.PI) {
                    currentRayAngle = Math.PI - Math.abs(currentRayAngle - Math.PI);
                }
                rayDirection = Vector2D.unitVector(currentRayAngle);
                rayDirection.normalize();

                r = new Ray(sensorPosition, rayDirection);

                scene.removeSubtree(oldSensor.getRoot());

                //find out first point (object) ray i intersects with
                intersectData = scene.traceRay(r);

                double angle1 = sensorPosition.minus(intersectData.point).direction();
                double angle2 = intersectData.normal.direction();//.minus(temp).direction();
                double angleOfIncidence = Math.abs(angle1) - Math.abs(angle2);
   
                
                scene.addSubtree(oldSensor.getRoot());
                
               // System.out.println("ray ("+Ipixel+") angle = " + currentRayAngle);

                for (int Jsource = 0; Jsource < pCurrent.getObjects(ObjectStateLightSource.NAME).size(); Jsource++) {
                    source = pCurrent.getObjects(ObjectStateLightSource.NAME).get(Jsource);
                    lightSource = (ObjectStateLightSource) source.getState(ObjectStateLightSource.NAME);

                    srcPosition = source.getPosition();
                    scene.removeSubtree(source.getRoot());
                    
                    angle1 = srcPosition.minus(intersectData.point).direction();
                    double angleOfIncidence2 = Math.abs(angle1) - Math.abs(angle2);                    

                    if (scene.isVisible(srcPosition, intersectData.point)) {
                        double intensity = lightSource.getIntensity() * (1.0 / Math.pow(srcPosition.distance(intersectData.point), 2)) + Math.abs(Math.cos(angleOfIncidence)*Math.cos(angleOfIncidence2)) * lightSource.getIntensity() * (1.0 / Math.pow(intersectData.point.distance(sensorPosition), 2));

                        //sum up light from multiple sources and from each pixel
                        sumIntensity += intensity;
                       // System.out.println("light");
                    } /*else {
                        System.out.println("no intersection with source");
                    }*/
                }//loop over sources

                currentRayAngle += angleBetweenRays; //next ray with increased angle

            } //loop over pixels


            //sensor reading is average of pixel readings
            lightSensor.setLightSensorValue(sumIntensity / numPixels);

            System.out.println("sensor("+Ksensor+") intensity = " + lightSensor.getLightSensorValue() );        
        }
            System.out.println("--------------");

    }
}

