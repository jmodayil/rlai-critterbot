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

package org.rlcommunity.critterbot.javadrops;

/**
  * InterfaceInputStream
  *
  * Implements a C++/Java socket interface. The goal here is to get rid of
  *  endianess issues.
  *
  * Author: Marc G. Bellemare
  *
  */

import java.io.DataOutputStream;
import java.io.OutputStream;
import java.io.IOException;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class InterfaceOutputStream
{
  protected final int DOUBLE_SIZE = 8;
  protected final int FLOAT_SIZE  = 4;
  protected final int INT_SIZE    = 4;
  protected final int SHORT_SIZE  = 2;
  protected final int BYTE_SIZE   = 1;

  protected DataOutputStream aOut;
  protected ByteBuffer aBuffer;

  protected final ByteOrder OUTPUT_ENDIAN = ByteOrder.LITTLE_ENDIAN;

  public InterfaceOutputStream(OutputStream out)
  {
    aOut = new DataOutputStream(out);
    // Make a buffer with initial capacity INT_SIZE
    aBuffer = ByteBuffer.allocate(INT_SIZE);
    aBuffer.order(OUTPUT_ENDIAN);
  }

  public void requireBufferSize(int size)
  {
    if (aBuffer.capacity() < size)
    {
      aBuffer = ByteBuffer.allocate(size);
      aBuffer.order(OUTPUT_ENDIAN);
      // Copy over old buffer?
      System.err.println ("InterfaceOutputStream: Setting buffer size to "+
        size);
    }
    else
      // Clear the buffer if we did not allocate new space
      aBuffer.clear();
  }

  public void writeDouble(double v) throws IOException {
      requireBufferSize(DOUBLE_SIZE);
      aBuffer.putDouble(v);
      aOut.write(aBuffer.array(), 0, DOUBLE_SIZE);
  }

  public void writeFloat(float v) throws IOException {
      requireBufferSize(FLOAT_SIZE);
      aBuffer.putFloat(v);
      aOut.write(aBuffer.array(), 0, FLOAT_SIZE);
  }

  public void writeUnsignedInt(long v) throws IOException 
  {
      requireBufferSize(INT_SIZE);
      aBuffer.putInt((int)(v));
      aOut.write(aBuffer.array(), 0, INT_SIZE);
  }
  
  public void writeInt(int v) throws IOException
  {
    requireBufferSize(INT_SIZE);
    // Write the int to the ByteBuffer, then write to the stream from
    //  the ByteBuffer
    aBuffer.putInt(v);
    aOut.write(aBuffer.array(), 0, INT_SIZE);
  }

  public void writeUnsignedShort(int v) throws IOException
  {
    requireBufferSize(SHORT_SIZE);
    aBuffer.putShort((short)(v & 0xFFFF));
    aOut.write(aBuffer.array(), 0, SHORT_SIZE);
  }
  
  public void writeShort(short v) throws IOException
  {
    requireBufferSize(SHORT_SIZE);
    aBuffer.putShort(v);
    aOut.write(aBuffer.array(), 0, SHORT_SIZE);
  }

  public void writeUnsignedByte(short v) throws IOException
  {
    aOut.writeByte(v & 0xFF);
  }
  
  public void writeByte(byte v) throws IOException
  {
    // In this case we can write the byte directly
    // The ByteBuffer does not support single-byte operations anyway
    aOut.writeByte(v);
  }

  public void writeBytes(byte[] array, int offset, int len) throws IOException
  {
    aOut.write(array, offset, len);
  }

  public void writeString(String s) throws IOException
  {
    int len = s.length();
    // First send the string length, then the string data (in ASCII)
    writeInt(len);
    writeBytes(s.getBytes("US-ASCII"), 0, len);
  }
}

