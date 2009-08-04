package org.rlcommunity.disco.dropdata;

import java.nio.ByteBuffer;

public class DropString extends DropData {

  private String value;

  public DropString(String label) {
    this(label, "", false);
  }

  public DropString(String label, String name) {
    this(label, name, true);
  }

  public DropString(String label, String value, boolean readonly) {
    super(label, readonly);
    this.value = value;
  }

  @Override
  public DropData clone(String label) {
    return new DropString(label);
  }

  public boolean checkString(ByteBuffer buffer) {
    if (buffer.getInt(0) != value.length())
      return false;
    byte[] charName = new byte[value.length()];
    for (int i = 0; i < value.length(); i++)
      charName[i] = buffer.get(i + DropData.IntSize);
    return value.equals(new String(charName));
  }

  @Override
  public void getData(ByteBuffer buffer, int index) {
    final int stringSize = buffer.capacity();
    byte[] byteName = new byte[stringSize];
    for (int i = 0; i < stringSize; i++)
      byteName[i] = buffer.get(i);
    value = new String(byteName);
  }

  @Override
  public void putData(ByteBuffer buffer, int index) {
    buffer.putInt(value.length());
    byte[] byteName = value.getBytes();
    for (int i = 0; i < value.length(); i++)
      buffer.put(i + DropData.IntSize, byteName[i]);
  }

  @Override
  public int size() {
    return value.length() * DropData.CharSize + DropData.IntSize;
  }

  @Override
  protected boolean valueEquals(DropData other) {
    return value.equals(((DropString) other).value);
  }

  public String value() {
    return value;
  }
}
