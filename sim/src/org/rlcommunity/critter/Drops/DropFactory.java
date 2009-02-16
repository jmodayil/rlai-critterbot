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
  * DropFactory
  *
  * Used to create new drop instances from a given drop name
  */

import org.rlcommunity.critter.*;

public class DropFactory
{
  public static SimulatorDrop create(String baseName) 
    throws ClassNotFoundException
  {
    // Assume that all drops are in the same package as us
    String packageName = 
      getStaticClass().getPackage().getName();
    String className = packageName + "." +baseName;

    Class<?> dropType  = Class.forName(className);
    // Create a new instance and cross your fingers - it has better be
    //  a subclass of SimulatorDrop
    try
    {
      SimulatorDrop newDrop = (SimulatorDrop) dropType.newInstance();
      return newDrop;
    }
    catch (Exception e)
    {
      throw new RuntimeException(e);
    }
  }

  public static Class getStaticClass()
  {
    CurrentClassGetter g = new CurrentClassGetter();
    return g.getStaticClass();
  }

  /** Encapsulates a method for finding the name of the current class
    * from a static method
    * Adapted from www.rgagnon.com/javadetails/java-0402.html
    */
  public static class CurrentClassGetter extends SecurityManager 
  {
    public Class getStaticClass() 
    {
      // This is fairly ugly
      return getClassContext()[1];
    }
  }

}
