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

import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class InterfaceInputStream
{
  protected final int INT_SIZE    = 4;
  protected final int SHORT_SIZE  = 2;
  protected final int BYTE_SIZE   = 1;

  protected DataInputStream aIn;
  protected ByteBuffer aBuffer;

  protected final ByteOrder INPUT_ENDIAN = ByteOrder.LITTLE_ENDIAN;

  public InterfaceInputStream(InputStream in)
  {
    aIn = new DataInputStream(in);
    // Make a buffer with initial capacity INT_SIZE
    aBuffer = ByteBuffer.allocate(INT_SIZE);
    aBuffer.order(INPUT_ENDIAN);
  }

  public void requireBufferSize(int size)
  {
    if (aBuffer.capacity() < size)
    {
      aBuffer.allocate(size);
      aBuffer.order(INPUT_ENDIAN);
      // Copy over old buffer?
      System.err.println ("Setting buffer size to "+size);
    }
    else
      // Clear the buffer if we did not allocate new space
      aBuffer.clear();
  }

  public int readInt() throws IOException
  {
    requireBufferSize(INT_SIZE);
    // Read data into the ByteBuffer
    aIn.readFully(aBuffer.array(), 0, INT_SIZE);
    
    return aBuffer.getInt();
  }

  public short readShort() throws IOException
  {
    requireBufferSize(SHORT_SIZE);
    aIn.readFully(aBuffer.array(), 0, SHORT_SIZE);

    return aBuffer.getShort();
  }

  public byte readByte() throws IOException
  {
    // The ByteBuffer doesn't support byte operations (?), and there is
    //  really no point
    return aIn.readByte();
  }

  public byte[] readBytes(int len) throws IOException
  {
    byte[] buf = new byte[len];
    aIn.readFully(buf, 0, len);
    
    return buf;
  }

  public String readString(int len) throws IOException
  {
    byte[] buf = readBytes(len); 

    return new String(buf, "US-ASCII");
  }

  public String readString() throws IOException
  {
    // First read the string length, then the data itself
    int len = readInt();
    return readString(len);
  }
}

