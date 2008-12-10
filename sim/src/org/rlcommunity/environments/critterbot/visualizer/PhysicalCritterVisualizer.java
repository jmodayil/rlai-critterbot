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
