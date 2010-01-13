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

package org.rlcommunity.critterbot.javadrops.drops;

/**
  * CritterStateDrop
  *
  * This class defines the various bits of subjective data that the robot
  *  has access to. Simulator-wise, this is what we provide to agents. 
  *  It should be a copy of CritterStateDrop.{h,cpp} from the C++ side.
  */

import java.io.IOException;
import java.util.Calendar;
import java.util.EnumSet;

import org.rlcommunity.critterbot.javadrops.InterfaceInputStream;
import org.rlcommunity.critterbot.javadrops.InterfaceOutputStream;

public class CritterStateDrop implements SimulatorDrop
{
  /* This class replicates the Disco concept of USeconds, however due to
   * most Java constructs only dealing will milliseconds, when exporting
   * data we drop the 3 least significant figures. */
  public class USeconds
  {
      static final long MAX_USEC = 999999;
      long tv_sec;
      long tv_usec;
      
      public void readData(InterfaceInputStream pIn) throws IOException
      {
          // We want these to be considered _UNSIGNED_ (a dirty word in Java)
          tv_sec = pIn.readUnsignedInt();
          tv_usec = pIn.readUnsignedInt();
          if(tv_usec > MAX_USEC)
              tv_usec = MAX_USEC;
      }
      
      public void writeData(InterfaceOutputStream pOut) throws IOException
      {
          pOut.writeUnsignedInt(tv_sec);
          pOut.writeUnsignedInt(tv_sec);
      }
      
      public int getSize()
      {
         return 2 * Integer.SIZE;
      }
      
      public long getTimeInMillis()
      {
          /* Yes, there are constants here.  But the number of milliseconds
          *  in a second will hopefully never change */
          return tv_sec * 1000 + (tv_usec / 1000);
      }
  }
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

  public enum DataSource { ROBOT, SIMULATOR, LOGFILE };
  
  public enum PowerSource { SHORE, BAT40, BAT160, BAT280 };

  // @todo this needs to be made to correspond with the C++ drops
  public enum ChargeState { UNPLUGGED, CHARGE_40, CHARGE_40_160,
    CHARGE_160, CHARGE_160_280, CHARGE_280, CHARGE_TOPOFF,
    CHARGE_COMPLETE, CHARGE_ERROR};

  public static final int MONITOR_STATE_MOTOR_TEMPERATURE = 0x01;
  
  /** This replaces the USeconds construct in Disco, although
   *  we do lose 3 characters of precision on the time.
   */
  public USeconds time;
  
  /** What produced this drop? */
  public DataSource data_source;
  
  /** The current power source of the robot */
  public PowerSource power_source;

  public ChargeState charge_state;
  
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

  /** State of the ARM monitor (bit map). */
  public int monitor_state;
  
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
    return (Integer.SIZE + // data_source (as an int)
           time.getSize() +
           Integer.SIZE + // power_source (as an int)
           Integer.SIZE + // charge_state (as an int)
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
           Integer.SIZE + // cycle_time
           Integer.SIZE) // monitor_state
           / 8; // divide by 8 because all of these are bit sizes
  }

  /** Creates a new CritterStateDrop with default (mostly 0) values.
   * 
   */
  public CritterStateDrop() {
    power_source = PowerSource.SHORE;
    data_source  = DataSource.SIMULATOR;
    charge_state = ChargeState.UNPLUGGED;
    
    ir_distance = new int[IR_DISTANCE_SIZE];
    ir_light    = new int[IR_LIGHT_SIZE];
    light       = new int[LIGHT_SIZE];
    thermal     = new int[THERMAL_SIZE];
    bump        = new int[BUMP_SIZE];

    time = new USeconds();
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
   
    time.writeData(pOut);
    pOut.writeInt(data_source.ordinal());
    pOut.writeInt(power_source.ordinal());
    pOut.writeInt(ChargeStateToInt(charge_state));
    
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
    pOut.writeInt(monitor_state);
  }

  protected int ChargeStateToInt(ChargeState pState) {
      switch (pState) {
          case UNPLUGGED: return 0;
          case CHARGE_40: return 2;
          case CHARGE_40_160: return 4;
          case CHARGE_160: return 5;
          case CHARGE_160_280: return 7;
          case CHARGE_280: return 8;
          case CHARGE_TOPOFF: return 9;
          case CHARGE_COMPLETE: return 10;
          case CHARGE_ERROR: return 200;
          default: throw new IllegalArgumentException("Unhandled charge state: "+pState);
      }
  }

  /** The charge states are ordered but not contiguous */
  protected ChargeState IntToChargeState(int pInt) {
      switch (pInt) {
          case 0: return ChargeState.UNPLUGGED;
          case 2: return ChargeState.CHARGE_40;
          case 4: return ChargeState.CHARGE_40_160;
          case 5: return ChargeState.CHARGE_160;
          case 7: return ChargeState.CHARGE_160_280;
          case 8: return ChargeState.CHARGE_280;
          case 9: return ChargeState.CHARGE_TOPOFF;
          case 10: return ChargeState.CHARGE_COMPLETE;
          case 200: return ChargeState.CHARGE_ERROR;
          // Not quite correct, but better than throwing an exception,
          //  especially because other charge states *actually* exist
          default: {
              System.err.println ("Unknown charge state "+pInt);
              return ChargeState.UNPLUGGED;
          }
      }
  }

  /** Reads in a drop from the input stream.
   *
   * @param pIn The input stream from which the drop should be read.
   * 
   * @throws java.io.IOException
   */
  public void readData(InterfaceInputStream pIn,int pDropSize) throws IOException
  {
    assert (pDropSize == getSize());

    time.readData(pIn);
    data_source = (DataSource)EnumSet.range(DataSource.ROBOT,
      DataSource.LOGFILE).toArray()[pIn.readInt()];
    power_source = (PowerSource)EnumSet.range(PowerSource.SHORE,
      PowerSource.BAT280).toArray()[pIn.readInt()];
    charge_state = IntToChargeState(pIn.readInt());
    
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
    monitor_state = pIn.readInt();
  }
}
