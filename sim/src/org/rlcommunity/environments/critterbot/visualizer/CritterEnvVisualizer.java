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
		PollingVizComponent theAcrobotVisualizer= new CritterImageComponent(this);
		SelfUpdatingVizComponent theAcrobotCounter = new GenericScoreComponent(this);
		
		addVizComponentAtPositionWithSize(theAcrobotVisualizer,0,0,1.0,1.0);
		addVizComponentAtPositionWithSize(theAcrobotCounter,0,0,1.0,1.0);
	}

    public TinyGlue getTheGlueState() {
        return theGlueState;
    }
	
}
