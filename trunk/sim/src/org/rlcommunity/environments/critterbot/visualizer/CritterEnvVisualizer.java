package org.rlcommunity.environments.critterbot.visualizer;

import rlVizLib.general.TinyGlue;
import rlVizLib.visualization.AbstractVisualizer;
import rlVizLib.visualization.GenericScoreComponent;
import rlVizLib.visualization.PollingVizComponent;
import rlVizLib.visualization.SelfUpdatingVizComponent;
import rlVizLib.visualization.interfaces.GlueStateProvider;

public class CritterEnvVisualizer extends AbstractVisualizer implements GlueStateProvider{
    private TinyGlue theGlueState=null;
    
	public CritterEnvVisualizer(TinyGlue theGlueState)  {
		super();
                this.theGlueState=theGlueState;
		SelfUpdatingVizComponent theCritterImage= new CritterImageComponent(this);
		SelfUpdatingVizComponent theScore = new GenericScoreComponent(this);
                SelfUpdatingVizComponent theSensors = new CritterSensorComponent(this);
		
		addVizComponentAtPositionWithSize(theCritterImage,0,0,1.0,.5);
		addVizComponentAtPositionWithSize(theSensors,0,.5,1.0,.5);
		addVizComponentAtPositionWithSize(theScore,0,0,1.0,1.0);
	}

    public TinyGlue getTheGlueState() {
        return theGlueState;
    }
	
}
