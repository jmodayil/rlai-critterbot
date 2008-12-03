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
import org.rlcommunity.critter.SimulatorComponentCritterbotInterface;
import org.rlcommunity.critter.SimulatorComponentDynamics;
import org.rlcommunity.critter.SimulatorComponentOmnidrive;
import org.rlcommunity.critter.SimulatorEngine;
import org.rlcommunity.critter.SimulatorViz;
import org.rlcommunity.critter.Vector2D;
import org.rlcommunity.rlglue.codec.EnvironmentInterface;
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
    Observation theObservation=new Observation(0,5,0);

    public String env_init() {
        int objPort, subjPort;

        objPort = 2323;
        subjPort = 2324;

        System.out.println("Creating simulator engine...");
        engine = new SimulatorEngine();
        engine.addComponent(new SimulatorComponentDynamics());
        engine.addComponent(new SimulatorComponentOmnidrive());

        // Get the first agent from the engine
        //SimulatorAgent agent = engine.getAgentList().getFirst();
  
        // @@@ MGB: I've taken out the Disco client - to add it, look
        //  at SimulatorMain for the part that creates a DiscoInterfaceServer
        // Create a drop server to send and receive robot (subjective) data
        robotServ = new DropInterface();
//        robotServ.addClient(new KeyboardClient());
        robotServ.addClient(this);

        engine.addComponent(
          new SimulatorComponentCritterbotInterface(robotServ));

        javax.swing.SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                new SimulatorViz(engine,null);
            }
        });
        System.out.println("Environment inited");

        int taskSpecVersion = 2;
        return taskSpecVersion + ":e:5_[f,f,f,f,f]_[-300,300]_[-300,300]_[0,500]_[0,500]_[-"+Math.PI+","+Math.PI+"]:1_[i]_[0,3]:[-1,0]";
    }

    public Observation env_start() {
        return theObservation;
    }

    public Reward_observation_terminal env_step(Action theAction) {
        lastAction=theAction;
        stepThings();
        double theReward=theObservation.doubleArray[1]*theObservation.doubleArray[1];
        return new Reward_observation_terminal(theReward,theObservation,false);
    }

    private void stepThings() {
        int stateThrottle = 0;
        int maxThrottle=5;

        for (stateThrottle = 0; stateThrottle <= maxThrottle; stateThrottle++) {
      engine.step();

//      try {
//		Thread.sleep(9);
//      } catch (InterruptedException e) {
//		// TODO Auto-generated catch block
//		e.printStackTrace();
//      }

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
    long keyboardDropInterval=1;
    public List<SimulatorDrop> receive() {
        double velocityX,  torque;
        int maxVel=25;
        int maxTorque=10;
       
        LinkedList<SimulatorDrop> dropList = new LinkedList<SimulatorDrop>();

        if(System.currentTimeMillis()-lastDropTime >= keyboardDropInterval)
        {
          int up,down,left,right;
          up=down=left=right=0;
        
          if(lastAction.intArray[0]==0)up=1;
          if(lastAction.intArray[0]==1)down=1;
          if(lastAction.intArray[0]==2)left=1;
          if(lastAction.intArray[0]==3)right=1;
        
          velocityX = (up * maxVel - down * maxVel);
          torque = (left * -maxTorque + right * maxTorque);
          CritterControlDrop controlDrop = new CritterControlDrop();
          controlDrop.motor_mode = CritterControlDrop.MotorMode.XYTHETA_SPACE;
          controlDrop.x_vel = (int) velocityX;
          controlDrop.y_vel = 0;
          controlDrop.theta_vel = (int) torque;
          lastDropTime=System.currentTimeMillis();

          dropList.add(controlDrop);
        }

      return dropList;
    }

    public void send(SimulatorDrop pData) {
        CritterStateDrop theStateDrop=(CritterStateDrop)pData;
        theObservation.doubleArray[0]=theStateDrop.accel.x;
        theObservation.doubleArray[1]=theStateDrop.accel.y;
        Vector2D thePos=engine.getAgentList().get(0).getPosition();
        theObservation.doubleArray[2]=thePos.x;
        theObservation.doubleArray[3]=thePos.y;
        theObservation.doubleArray[4]=engine.getAgentList().get(0).getDirection();
        
//        System.out.println(theObservation.doubleArray[0]);
//        System.out.println(theObservation.doubleArray[1]);
    }
}
