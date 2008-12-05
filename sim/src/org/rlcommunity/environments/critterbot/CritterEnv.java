/*
 * CopylastAction.intArray[3] 2008 Brian Tanner
 * http://bt-recordbook.googlecode.com/
 * brian@tannerpages.com
 * http://brian.tannerpages.com
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package org.rlcommunity.environments.critterbot;

import java.util.List;
import java.util.LinkedList;

import org.rlcommunity.critter.Clients.DropClient;
import org.rlcommunity.critter.Drops.*;
import org.rlcommunity.critter.SimulatorComponentAccelerometer;
import org.rlcommunity.critter.SimulatorComponentBump;
import org.rlcommunity.critter.SimulatorComponentCritterbotInterface;
import org.rlcommunity.critter.SimulatorComponentDynamics;
import org.rlcommunity.critter.SimulatorComponentGyroscope;
import org.rlcommunity.critter.SimulatorComponentIRDistance;
import org.rlcommunity.critter.SimulatorComponentLight;
import org.rlcommunity.critter.SimulatorComponentOmnidrive;
import org.rlcommunity.critter.SimulatorEngine;
import org.rlcommunity.critter.SimulatorViz;
import org.rlcommunity.critter.Vector2D;
import org.rlcommunity.rlglue.codec.EnvironmentInterface;
import org.rlcommunity.rlglue.codec.taskspec.TaskSpec;
import org.rlcommunity.rlglue.codec.taskspec.TaskSpecVRLGLUE3;
import org.rlcommunity.rlglue.codec.taskspec.ranges.DoubleRange;
import org.rlcommunity.rlglue.codec.taskspec.ranges.IntRange;
import org.rlcommunity.rlglue.codec.types.Action;
import org.rlcommunity.rlglue.codec.types.Observation;
import org.rlcommunity.rlglue.codec.types.Reward_observation_terminal;
import org.rlcommunity.rlglue.codec.util.EnvironmentLoader;

/**
 *
 * @author Brian Tanner
 */
public class CritterEnv implements EnvironmentInterface, DropClient {

    DropInterface robotServ = null;
    SimulatorEngine engine = null;
    SimulatorViz theSimulatorVizualizer=null;
    Observation theObservation = new Observation(84, 0, 0);

    public String env_init() {
        int objPort, subjPort;

        objPort = 2323;
        subjPort = 2324;



        // Get the first agent from the engine
        //SimulatorAgent agent = engine.getAgentList().getFirst();

        // @@@ MGB: I've taken out the Disco client - to add it, look
        //  at SimulatorMain for the part that creates a DiscoInterfaceServer
        // Create a drop server to send and receive robot (subjective) data
        robotServ = new DropInterface();
//        robotServ.addClient(new KeyboardClient());
        robotServ.addClient(this);

        System.out.println("Creating simulator engine...");
        engine = new SimulatorEngine();

        engine.addComponent(new SimulatorComponentDynamics());
        engine.addComponent(new SimulatorComponentLight());
        engine.addComponent(new SimulatorComponentBump());
        engine.addComponent(new SimulatorComponentOmnidrive());
        engine.addComponent(new SimulatorComponentAccelerometer());
        engine.addComponent(new SimulatorComponentGyroscope());
        engine.addComponent(
                new SimulatorComponentCritterbotInterface(robotServ));
        engine.addComponent(new SimulatorComponentIRDistance());

        javax.swing.SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                theSimulatorVizualizer=new SimulatorViz(engine, null);
            }
        });
        System.out.println("Environment inited");

        TaskSpecVRLGLUE3 theTaskSpec=new TaskSpecVRLGLUE3();
        theTaskSpec.setDiscountFactor(.99);
        IntRange theObsRange=new IntRange(80);
        theObsRange.setMinUnspecified();
        theObsRange.setMaxUnspecified();
        theTaskSpec.addDiscreteObservation(theObsRange);
        
        IntRange theModeAction=new IntRange(1,1);
        IntRange theActionRange=new IntRange(-100,100,3);
        theTaskSpec.addDiscreteAction(theModeAction);
        theTaskSpec.addDiscreteAction(theActionRange);
        
        theTaskSpec.setRewardRange(new DoubleRange(0,100));
        theTaskSpec.setExtra("EnvName:ExpandedCritter");
        TaskSpec.checkTaskSpec(theTaskSpec.toTaskSpec());
        
        return theTaskSpec.toTaskSpec();
    }

    public Observation env_start() {
        return theObservation;
    }

    public Reward_observation_terminal env_step(Action theAction) {
        lastAction = theAction;
        stepThings();
        double theReward = theObservation.intArray[40];
//        theReward=
        return new Reward_observation_terminal(theReward, theObservation, false);
    }

    private void stepThings() {
        int stateThrottle = 0;
        int maxThrottle = 25;

        for (stateThrottle = 0; stateThrottle <= maxThrottle; stateThrottle++) {
            engine.step(10);
            try{
            Thread.sleep(10);
            }catch(Exception e){
                
            }
        }
    }

    public void env_cleanup() {
        System.out.println("Cleanup called");
        theSimulatorVizualizer.die();
        theSimulatorVizualizer=null;
        robotServ=null;
        engine=null;
    }

    public String env_message(String arg0) {
        return null;
    }

    public static void main(String[] args) {
        EnvironmentLoader L = new EnvironmentLoader(new CritterEnv());
        L.run();
    }
    Action lastAction = new Action(0, 4);
    long lastDropTime = System.currentTimeMillis();
    long keyboardDropInterval = 1;

    /**
     * This is where actions are received for the simulator
     * @return
     */
    public List<SimulatorDrop> receive() {
        LinkedList<SimulatorDrop> dropList = new LinkedList<SimulatorDrop>();

        if (System.currentTimeMillis() - lastDropTime >= keyboardDropInterval) {
            CritterControlDrop controlDrop = new CritterControlDrop();
            controlDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;
            controlDrop.x_vel=lastAction.intArray[1];
            controlDrop.y_vel=lastAction.intArray[2];
            controlDrop.theta_vel=lastAction.intArray[3];
            
            lastDropTime = System.currentTimeMillis();

            dropList.add(controlDrop);
        }

        return dropList;
    }

    
    /**
     * This is to send away from the simulator
     * @param pData
     */
    public void send(SimulatorDrop pData) {
        CritterStateDrop theStateDrop = (CritterStateDrop) pData;
        int place = 0;
        int i = 0;
        theObservation.intArray[place++] = theStateDrop.bus_voltage;
        theObservation.intArray[place++] = theStateDrop.batv40;
        theObservation.intArray[place++] = theStateDrop.batv160;
        theObservation.intArray[place++] = theStateDrop.batv280;
        theObservation.intArray[place++] = theStateDrop.motor100.velocity;
        theObservation.intArray[place++] = theStateDrop.motor100.current;
        theObservation.intArray[place++] = theStateDrop.motor100.temp;
        theObservation.intArray[place++] = theStateDrop.motor220.velocity;
        theObservation.intArray[place++] = theStateDrop.motor220.current;
        theObservation.intArray[place++] = theStateDrop.motor220.temp;
        theObservation.intArray[place++] = theStateDrop.motor340.velocity;
        theObservation.intArray[place++] = theStateDrop.motor340.current;
        theObservation.intArray[place++] = theStateDrop.motor340.temp;
        theObservation.intArray[place++] = theStateDrop.mag.x;
        theObservation.intArray[place++] = theStateDrop.mag.y;
        theObservation.intArray[place++] = theStateDrop.mag.z;
        theObservation.intArray[place++] = theStateDrop.accel.x;
        theObservation.intArray[place++] = theStateDrop.accel.y;
        theObservation.intArray[place++] = theStateDrop.accel.z;
        theObservation.intArray[place++] = theStateDrop.rotation;
        theObservation.intArray[place++] = theStateDrop.error_flags;
        theObservation.intArray[place++] = theStateDrop.cycle_time;

        for (i = 0; i < 10; i++) {
            theObservation.intArray[place++] = theStateDrop.ir_distance[i];
        }
        for (i = 0; i < 8; i++) {
            theObservation.intArray[place++] = theStateDrop.ir_light[i];
        }
        for (i = 0; i < 4; i++) {
            theObservation.intArray[place++] = theStateDrop.light[i];
        }
        for (i = 0; i < 8; i++) {
            theObservation.intArray[place++] = theStateDrop.thermal[i];
        }
        for (i = 0; i < 32; i++) {
            theObservation.intArray[place++] = theStateDrop.bump[i];
        }
    }
}
