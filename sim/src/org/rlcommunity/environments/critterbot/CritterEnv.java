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
import org.rl.community.critter.Clients.ClientHandlerInterface;
import org.rl.community.critter.Clients.KeyboardClient;
import org.rl.community.critter.CritterControlDrop;
import org.rl.community.critter.CritterStateDrop;
import org.rl.community.critter.DropServer;
import org.rl.community.critter.ObjectStateOmnidrive;
import org.rl.community.critter.SimulatorAgent;
import org.rl.community.critter.SimulatorComponentDiscoInterface;
import org.rl.community.critter.SimulatorComponentKinematics;
import org.rl.community.critter.SimulatorComponentOmnidrive;
import org.rl.community.critter.SimulatorDrop;
import org.rl.community.critter.SimulatorEngine;
import org.rl.community.critter.SimulatorObject;
import org.rl.community.critter.SimulatorViz;
import org.rlcommunity.rlglue.codec.EnvironmentInterface;
import org.rlcommunity.rlglue.codec.types.Action;
import org.rlcommunity.rlglue.codec.types.Observation;
import org.rlcommunity.rlglue.codec.types.Reward_observation_terminal;
import org.rlcommunity.rlglue.codec.util.EnvironmentLoader;

/**
 *
 * @author Brian Tanner
 */
public class CritterEnv implements EnvironmentInterface, ClientHandlerInterface {

    DropServer robotServ = null;
    SimulatorEngine engine = null;
    Observation theObservation=new Observation(0,2,0);

    public String env_init() {
        int objPort, subjPort;

        objPort = 2323;
        subjPort = 2324;

        System.out.println("Creating simulator engine...");
        engine = new SimulatorEngine();
        engine.addComponent(new SimulatorComponentKinematics());
        engine.addComponent(new SimulatorComponentOmnidrive());

        // Get the first agent from the engine
        //SimulatorAgent agent = engine.getAgentList().getFirst();

        System.out.println("Starting servers on ports " + objPort + " and " + subjPort);
        // Create a drop server to send and receive robot (subjective) data
        robotServ = new DropServer(subjPort);
//        robotServ.addClient(new KeyboardClient());
        robotServ.addClient(this);
        robotServ.start();

        engine.addComponent(new SimulatorComponentDiscoInterface(robotServ, null));

        javax.swing.SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                new SimulatorViz(engine,null);
            }
        });
        System.out.println("Environment inited");

        int taskSpecVersion = 2;
        return taskSpecVersion + ":e:1_[f,f]_[-1000000,1000000]_[-1000000,1000000]:4_[i,i,i,i]_[0,1]_[0,1]_[0,1]_[0,1]:[-1,0]";
    }

    public Observation env_start() {
        System.out.println("Env start");
        return theObservation;
    }

    public Reward_observation_terminal env_step(Action theAction) {
        System.out.println("Env step");
        lastAction=theAction;
        System.out.println(theAction.intArray.length);
        stepThings();
        return new Reward_observation_terminal(0,theObservation,false);
    }

    private void stepThings() {
        int stateThrottle = 0;
        int maxThrottle=5;

        for (stateThrottle = 0; stateThrottle <= maxThrottle; stateThrottle++) {
      engine.step();

      try {
		Thread.sleep(9);
      } catch (InterruptedException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
      }

      }
      //objServer.sendlastAction.intArray[0]date(engine.getState());
      //subjServer.sendlastAction.intArray[0]date();
      //subjServer.receiveData();
    }


    public void env_cleanup() {
    }

    public String env_message(String arg0) {
        return null;
    }

    public static void main(String[] args) {
        EnvironmentLoader L = new EnvironmentLoader(new CritterEnv());
        L.run();
    }
    Action lastAction = new Action(0, 4);

    long lastDropTime=System.currentTimeMillis();
    long keyboardDropInterval=50;
    public SimulatorDrop receive() {
        double velocityX,  torque;
        int maxVel=25;
        int maxTorque=10;
        

        // If any of the visualizer keys are pressed, we override the omnidrive
        //  @@@ This needs to be moved somewhere else or ...
        if (lastAction.intArray[0] > 0 || lastAction.intArray[1] > 0 || lastAction.intArray[3] > 0 || lastAction.intArray[2] > 0) {
        if(System.currentTimeMillis()-lastDropTime<keyboardDropInterval){
            return null;
        }

        velocityX = (lastAction.intArray[0] * maxVel - lastAction.intArray[1] * maxVel);
            torque = (lastAction.intArray[3] * -maxTorque + lastAction.intArray[2] * maxTorque);
            CritterControlDrop controlDrop = new CritterControlDrop();
            controlDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;
            controlDrop.x_vel = (int) velocityX;
            controlDrop.y_vel = 0;
            controlDrop.theta_vel = (int) torque;
            lastDropTime=System.currentTimeMillis();
            return controlDrop;
        }
        
        return null;
    }

    public void send(SimulatorDrop pData) {
        CritterStateDrop theStateDrop=(CritterStateDrop)pData;
        theObservation.doubleArray[0]=theStateDrop.accel.x;
        theObservation.doubleArray[1]=theStateDrop.accel.y;
//        System.out.println(theObservation.doubleArray[0]);
//        System.out.println(theObservation.doubleArray[1]);
    }
}
