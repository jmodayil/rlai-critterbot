/* Copyright 2009 Marc G. Bellemare
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

package org.rlcommunity.critter.Drops;

/**
  * CritterStateDrop
  *
  * This class defines the various bits of subjective data that the robot
  *  has access to. Simulator-wise, this is what we provide to agents. 
  *  It should be a copy of CritterStateDrop.{h,cpp} from the C++ side.
  */

import java.io.IOException;
import java.util.EnumSet;

import org.rlcommunity.critter.InterfaceInputStream;
import org.rlcommunity.critter.InterfaceOutputStream;

public class CritterStateDrop implements SimulatorDrop
{
  public class motor_struct
  {
    /** The command executed by the system (as opposed to the requested command) */
    public int command;
    /** The velocity of the corresponding wheel */
    public int velocity;
    /** The current being drawn by the motor */
    public int current;
    /** The temperature of the motor */
    public int temp;

    public void readData(InterfaceInputStream pIn) throws IOException
    {
      command = pIn.readInt();
      velocity = pIn.readInt();
      current = pIn.readInt();
      temp = pIn.readInt();
    }
    
    public void writeData(InterfaceOutputStream pOut) throws IOException
    {
      pOut.writeInt(command);
      pOut.writeInt(velocity);
      pOut.writeInt(current);
      pOut.writeInt(temp);
    }

    public int getSize()
    {
      return 4 * Integer.SIZE;
    }
  }

  public class vector_struct {
  
    public int x, y, z;

    public void readData(InterfaceInputStream pIn) throws IOException
    {
      x = pIn.readInt();
      y = pIn.readInt();
      z = pIn.readInt();
    }
    
    public void writeData(InterfaceOutputStream pOut) throws IOException
    {
      pOut.writeInt(x);
      pOut.writeInt(y);
      pOut.writeInt(z);
    }

    public int getSize()
    {
      return 3 * Integer.SIZE;
    }
  }

  public enum PowerSource { SHORE, BAT40, BAT160, BAT280 };

  /** The current power source of the robot */
  public PowerSource power_source;

  /** The bus voltage (currently unused) */
  public int bus_voltage;
  /** The state of the three batteries  */
  public int batv40, batv160, batv280;

  /** The state of the three motors */
  public motor_struct motor100, motor220, motor340;
  /** The accelerometer and magnetomer status (as 3D vectors) */
  public vector_struct accel, mag;

  /** The gyroscope information */
  public int rotation;
  /** An array of infrared distance sensor values */
  public int[] ir_distance;
  /** An array of IR light sensor values (currently unused) */
  public int[] ir_light;
  /** An array of light sensor values */
  public int[] light;
  /** An array of thermal sensor values (currently unused) */
  public int[] thermal;
  /** An array of bump sensor values */
  public int[] bump;

  /** Error flags reported by robot (unused by the simulator) */
  public int error_flags;
  public int cycle_time;

  public static final int IR_DISTANCE_SIZE   = 10;
  public static final int IR_LIGHT_SIZE      = 8;
  public static final int LIGHT_SIZE         = 4;
  public static final int THERMAL_SIZE       = 8;
  public static final int BUMP_SIZE          = 32;

  /** Returns the size of the data contained in this Drop.
    * This value must correspond to the amount of data sent/received in
    *  writeData/readData.
    *
    * @return The size of the data contained in this Drop
    */
  public int getSize()
  {
    return (Integer.SIZE + // power_source (as an int)
           Integer.SIZE + // bus_voltage
           3 * Integer.SIZE + // batv40 + batv160 + batv280
           motor100.getSize() +
           motor220.getSize() + 
           motor340.getSize() +
           accel.getSize() +
           mag.getSize() +
           Integer.SIZE + // rotation
           IR_DISTANCE_SIZE * Integer.SIZE +
           IR_LIGHT_SIZE * Integer.SIZE +
           LIGHT_SIZE * Integer.SIZE +
           THERMAL_SIZE * Integer.SIZE +
           BUMP_SIZE * Integer.SIZE +
           Integer.SIZE + // error_flags
           Integer.SIZE) // cycle_time
           / 8; // divide by 8 because all of these are bit sizes
  }

  /** Creates a new CritterStateDrop with default (mostly 0) values.
   * 
   */
  public CritterStateDrop() {
    power_source = PowerSource.SHORE;
    
    ir_distance = new int[IR_DISTANCE_SIZE];
    ir_light    = new int[IR_LIGHT_SIZE];
    light       = new int[LIGHT_SIZE];
    thermal     = new int[THERMAL_SIZE];
    bump        = new int[BUMP_SIZE];

    accel = new vector_struct();
    mag = new vector_struct();
    motor100 = new motor_struct();
    motor220 = new motor_struct();
    motor340 = new motor_struct();
  }

  /** Writes this data drop to the given output stream.
   *
   * @param pOut The output stream to which the drop should be written.
   *
   * @throws java.io.IOException
   */
  public void writeData(InterfaceOutputStream pOut) throws IOException {
    pOut.writeInt(power_source.ordinal());

    pOut.writeInt(bus_voltage);
    pOut.writeInt(batv40);
    pOut.writeInt(batv160);
    pOut.writeInt(batv280);

    motor100.writeData(pOut);
    motor220.writeData(pOut);
    motor340.writeData(pOut);
    accel.writeData(pOut);
    mag.writeData(pOut);

    pOut.writeInt(rotation);
    for (int i = 0; i < ir_distance.length; i++)
      pOut.writeInt(ir_distance[i]);
    for (int i = 0; i < ir_light.length; i++)
      pOut.writeInt(ir_light[i]);
    for (int i = 0; i < light.length; i++)
      pOut.writeInt(light[i]);
    for (int i = 0; i < thermal.length; i++)
      pOut.writeInt(thermal[i]);
    for (int i = 0; i < bump.length; i++)
      pOut.writeInt(bump[i]);

    pOut.writeInt(error_flags);
    pOut.writeInt(cycle_time);
  }

  /** Reads in a drop from the input stream.
   *
   * @param pIn The input stream from which the drop should be read.
   * 
   * @throws java.io.IOException
   */
  public void readData(InterfaceInputStream pIn) throws IOException
  {
    power_source = (PowerSource)EnumSet.range(PowerSource.SHORE,
      PowerSource.BAT280).toArray()[pIn.readInt()];
    
    // Read in all the data, one variable at a time
    bus_voltage = pIn.readInt();
    batv40 = pIn.readInt();
    batv160 = pIn.readInt();
    batv280 = pIn.readInt();

    motor100.readData(pIn);
    motor220.readData(pIn);
    motor340.readData(pIn);
    accel.readData(pIn);
    mag.readData(pIn);

    rotation = pIn.readInt();
    for (int i = 0; i < ir_distance.length; i++)
      ir_distance[i] = pIn.readInt();
    for (int i = 0; i < ir_light.length; i++)
      ir_light[i] = pIn.readInt();
    for (int i = 0; i < light.length; i++)
      light[i] = pIn.readInt();
    for (int i = 0; i < thermal.length; i++)
      thermal[i] = pIn.readInt();
    for (int i = 0; i < bump.length; i++)
      bump[i] = pIn.readInt();

    error_flags = pIn.readInt();
    cycle_time = pIn.readInt();
  }
}
