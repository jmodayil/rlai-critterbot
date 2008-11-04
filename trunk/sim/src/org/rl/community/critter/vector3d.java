package org.rl.community.critter;


import java.io.IOException;

/*
 * Copyright 2008 Brian Tanner
 * http://bt-recordbook.googlecode.com/
 * brian@tannerpages.com
 * http://brian.tannerpages.com
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
import org.rl.community.critter.InterfaceInputStream;
import org.rl.community.critter.InterfaceOutputStream;

/**
 *
 * @author Brian Tanner
 */
public class vector3d {
  
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

