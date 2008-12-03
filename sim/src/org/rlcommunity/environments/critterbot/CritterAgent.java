/*
 * Copyright 2008 Brian Tanner
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

import org.rlcommunity.rlglue.codec.AgentInterface;
import org.rlcommunity.rlglue.codec.types.Action;
import org.rlcommunity.rlglue.codec.types.Observation;
import org.rlcommunity.rlglue.codec.util.AgentLoader;

/**
 *
 * @author Brian Tanner
 */
public class CritterAgent implements AgentInterface {

    Action theAction = new Action(4, 0, 0);

    public void agent_init(String arg0) {
        theAction.intArray[0]=1;
        theAction.intArray[1]=50;
        
        //1 0 0 20
        //1 0 0 -20
    }

    public Action agent_start(Observation theObservation) {
        System.out.println("Agent_Start received observation with " + theObservation.intArray.length + " and " + theObservation.doubleArray.length + " double elements");
        return theAction;
    }

    public Action agent_step(double theReward, Observation theObservation) {
        int[] theArray=theObservation.intArray;
        for (int i : theArray) {
            System.out.print(i+" ");
        }
        System.out.println();
        return theAction;
    }

    public void agent_end(double arg0) {
        System.out.println("Agent_end");
    }

    public void agent_cleanup() {
        System.out.println("Cleanup");
    }

    public String agent_message(String arg0) {
        System.out.println("agent_message");
        return "Critter Agent doesn't respond to messages.";
    }

    public static void main(String[] args) {
        AgentLoader L = new AgentLoader(new CritterAgent());
        L.run();
    }
}
