/**
  * CritterControlDrop
  *
  * This class encapsulates data received from DisCo about commands sent
  *  to the robot. Somewhat a clone of CritterControlDrop.{h,cpp}.
  */

import java.io.IOException;
import java.util.EnumSet;

public class CritterControlDrop implements SimulatorDrop
{
  public enum MotorMode { WHEEL_SPACE, XYTHETA_SPACE };
  public enum LedMode { THING1, THING2, THING3 };

  public MotorMode motor_mode;
  public LedMode   led_mode;

  /** Velocities used in XYTHETA_SPACE */
  public int x_vel, y_vel, theta_vel;
  /** Velocities used in WHEEL_SPACE */
  public int m100_vel, m220_vel, m340_vel;

  /** Returns the size of the data contained in this drop.
    *  This must correspond to the amount of data written and read by
    *  writeData and readData, respectively. 
    * @return The number of bytes contained in this drop.
    */
  public int getSize()
  {
    return Integer.SIZE + // motor_mode
           Integer.SIZE + // led_mode
           3 * Integer.SIZE; // x_vel + y_vel + theta_vel OR m_vel's
  }

  /** Write control drop data out to a stream. The order in which things 
    *  are sent must be:
    *
    *  motor_mode, led_mode (as ints)
    *  x_vel, y_vel, theta_vel OR
    *   m100_vel, m220_vel, m340_vel
    *
    * @param pOut The output stream to which the data should be written
    */
  public void writeData(InterfaceOutputStream pOut) throws IOException
  {
    // Write modes
    pOut.writeInt(motor_mode.ordinal());
    pOut.writeInt(led_mode.ordinal());

    // Write 3 velocities
    switch (motor_mode)
    {
      case WHEEL_SPACE:
        pOut.writeInt(m100_vel);
        pOut.writeInt(m220_vel);
        pOut.writeInt(m340_vel);
        break;
      case XYTHETA_SPACE:
        pOut.writeInt(x_vel);
        pOut.writeInt(y_vel);
        pOut.writeInt(theta_vel);
        break;
      default:
        throw new IOException("Unknown motor mode.");
    }
  }
 
  /** Reverse of writeData; reads the drop from a DataInputStream
    *
    * @param pIn The input stream from which we read the data
    */
  public void readData(InterfaceInputStream pIn) throws IOException
  {
    // Read motor mode and convert it to an enum
    // Unfortunately, Java hates integer enums, so we have to circumvent this
    motor_mode = (MotorMode)EnumSet.range(MotorMode.WHEEL_SPACE, 
      MotorMode.XYTHETA_SPACE).toArray()[pIn.readInt()];

    // Read in the three velocities
    switch (motor_mode)
    {
      case WHEEL_SPACE:
        m100_vel = pIn.readInt();
        m220_vel = pIn.readInt();
        m340_vel = pIn.readInt();
        break;
      case XYTHETA_SPACE:
        x_vel = pIn.readInt();
        y_vel = pIn.readInt();
        theta_vel = pIn.readInt();
        break;
      default:
        throw new IOException("Unknown motor mode.");
    }
   
    // Read LED mode
    led_mode = (LedMode)EnumSet.range(LedMode.THING1, 
      LedMode.THING3).toArray()[pIn.readInt()];
  }

  public String toString()
  {
    return "Motor mode: "+motor_mode+
      " XYT ("+x_vel+","+y_vel+","+theta_vel+")"+
      " WHE: ("+m100_vel+","+m220_vel+","+m340_vel+")"+
      " Led mode: "+led_mode;
  }
}
