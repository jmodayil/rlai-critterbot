/**
  * SimulatorDrop
  *
  * This interface defines how Drop-like objects should behave. In particular,
  *  classes that implement this interface should provide methods for 
  *  saving their data into a byte array and reading data from a byte array.
  */

import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.io.IOException;

public interface SimulatorDrop
{
  /** Write the drop's data to the given DataOutputStream */
  public void writeData(DataOutputStream pOut) throws IOException;
  /** Restore data from a given array */
  public void readData(DataInputStream pIn) throws IOException;
}
