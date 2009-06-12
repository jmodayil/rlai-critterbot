/* Copyright 2009 Brian Tanner
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
package org.rlcommunity.environments.critterbot.visualizer;

import rlVizLib.general.TinyGlue;
import rlVizLib.visualization.AbstractVisualizer;
import rlVizLib.visualization.GenericScoreComponent;
import rlVizLib.visualization.SelfUpdatingVizComponent;
import rlVizLib.visualization.interfaces.GlueStateProvider;

public class PhysicalCritterVisualizer extends AbstractVisualizer implements GlueStateProvider{
    private TinyGlue theGlueState=null;
    
	public PhysicalCritterVisualizer(TinyGlue theGlueState)  {
		super();
                this.theGlueState=theGlueState;
		SelfUpdatingVizComponent theScore = new GenericScoreComponent(this);
                SelfUpdatingVizComponent theSensors = new CritterSensorComponent(this);
		
		addVizComponentAtPositionWithSize(theSensors,0,0,1.0,1.0);
		addVizComponentAtPositionWithSize(theScore,0,0,1.0,1.0);
	}

    public TinyGlue getTheGlueState() {
        return theGlueState;
    }
    
    @Override
        public String getName() {
        return "Physical Critter Bot Sensor Visualizer 1.0";
    }

	
}
