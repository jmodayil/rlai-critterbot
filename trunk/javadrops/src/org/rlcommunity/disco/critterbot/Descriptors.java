package org.rlcommunity.disco.critterbot;

import org.rlcommunity.disco.dropdata.DropArray;
import org.rlcommunity.disco.dropdata.DropColor;
import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.dropdata.DropTime;
import org.rlcommunity.disco.dropdata.DropVar;

public class Descriptors {

    static final public DropData[] observationDescriptor = {
	    new DropTime(),
	    new DropVar("DataSource"),
	    new DropVar("Voltage"),
	    new DropVar("BusVoltage"),
	    new DropArray("Bat", 3),
	    new DropArray("Motor0", "Command", "Speed", "Current",
		    "Temperature"),
	    new DropArray("Motor1", "Command", "Speed", "Current",
		    "Temperature"),
	    new DropArray("Motor2", "Command", "Speed", "Current",
		    "Temperature"), new DropArray("Accel", "X", "Y", "Z"),
	    new DropArray("Mag", "X", "Y", "Z"), new DropVar("RotationVel"),
	    new DropArray("IR", 10), new DropArray("IRLight", 8),
	    new DropArray("Light", 4), new DropArray("Thermal", 8),
	    new DropArray("Bump", 32), new DropVar("ErrorFlags"),
	    new DropVar("CycleTime") };

    static final public DropData[] actionDescriptor = {
	    new DropVar("MotorMode", 0),
	    new DropArray("VelocityCommand", "M100", "M220", "M340"),
	    new DropVar("LedMode", 0),
	    new DropArray(new DropColor(""), "Led", 16) };
}
