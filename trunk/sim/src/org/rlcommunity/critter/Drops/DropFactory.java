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
