package org.rlcommunity.critterbot.javadrops.drops;

import java.io.IOException;

import org.rlcommunity.critterbot.javadrops.InterfaceInputStream;
import org.rlcommunity.critterbot.javadrops.InterfaceOutputStream;
import org.rlcommunity.critterbot.javadrops.drops.SimulatorDrop;

public class WiiDiscoDrop implements SimulatorDrop {

	public enum WII_BUTTON {

		WII_BUTTON_A,
		WII_BUTTON_B,
		WII_BUTTON_PLUS,
		WII_BUTTON_MINUS,
		WII_BUTTON_HOME,
		WII_BUTTON_1,
		WII_BUTTON_2,
		NUM_WII_BUTTONS
	};
	
	@Override
	public int getSize() {
		return 2 + 4*2;
	}

	@Override
	public void readData(InterfaceInputStream pIn, int pDropSize)
			throws IOException {
		
		assert (pDropSize == getSize());
		
		dpadX = pIn.readInt();
		dpadY = pIn.readInt();
		byte numBytes = pIn.readByte();
		assert(numBytes == 1);
		buttons = pIn.readByte();

	}

	@Override
	public void writeData(InterfaceOutputStream pOut) throws IOException {
		// TODO Auto-generated method stub
		pOut.writeInt(dpadX);
		pOut.writeInt(dpadY);
		pOut.writeByte(buttons);

	}
	
	public boolean GetButton(WII_BUTTON button)
	{
		return (buttons & 0x1<<button.ordinal()) != 0;
	}
	
	int dpadX, dpadY;
	byte buttons;
	

}
