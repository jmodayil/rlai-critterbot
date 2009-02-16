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
  * SimulatorDrop
  *
  * This interface defines how Drop-like objects should behave. In particular,
  *  classes that implement this interface should provide methods for 
  *  saving their data into a byte array and reading data from a byte array.
  */

import java.io.IOException;

import org.rlcommunity.critter.InterfaceInputStream;
import org.rlcommunity.critter.InterfaceOutputStream;

public interface SimulatorDrop
{
  /** Returns the size of the drop (amount of bytes written/read by
    *  writeData/readData.
    */
  public int getSize();

  /** Write the drop's data to the given output stream */
  public void writeData(InterfaceOutputStream pOut) throws IOException;
  /** Restore data from a given array */
  public void readData(InterfaceInputStream pIn) throws IOException;
}
