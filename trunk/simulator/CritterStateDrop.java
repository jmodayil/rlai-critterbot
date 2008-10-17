/**
  * CritterStateDrop
  *
  * This class defines the various bits of subjective data that the robot
  *  has access to. Simulator-wise, this is what we provide to agents. 
  */

import java.io.IOException;
import java.io.DataInputStream;
import java.io.DataOutputStream;

public class CritterStateDrop implements SimulatorDrop
{
  public int bus_voltage;

  public void writeData(DataOutputStream pOut) throws IOException
  {
    pOut.writeInt(bus_voltage);
  }

  public void readData(InterfaceInputStream pIn) throws IOException
  {
    bus_voltage = pIn.readInt();
  }
}
