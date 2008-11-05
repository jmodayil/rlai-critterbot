package org.rlcommunity.critter;

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
      aBuffer.allocate(size);
      aBuffer.order(OUTPUT_ENDIAN);
      // Copy over old buffer?
      System.err.println ("InterfaceOutputStream: Setting buffer size to "+
        size);
    }
    else
      // Clear the buffer if we did not allocate new space
      aBuffer.clear();
  }

  public void writeInt(int v) throws IOException
  {
    requireBufferSize(INT_SIZE);
    // Write the int to the ByteBuffer, then write to the stream from
    //  the ByteBuffer
    aBuffer.putInt(v);
    aOut.write(aBuffer.array(), 0, INT_SIZE);
  }

  public void writeShort(short v) throws IOException
  {
    requireBufferSize(SHORT_SIZE);
    aBuffer.putShort(v);
    aOut.write(aBuffer.array(), 0, SHORT_SIZE);
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

