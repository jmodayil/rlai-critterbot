package org.rlcommunity.disco.dropdata;

import java.nio.ByteBuffer;

public class DropVar extends DropData {

  private int value;

  public DropVar(String label) {
    super(label, false);
  }

  public int value() {
    return value;
  }

  protected DropVar(String label, boolean readOnly) {
    super(label, readOnly);
  }

  public DropVar(String label, int value) {
    super(label, true);
    this.value = value;
  }

  public void set(int value) {
    assert !readOnly;
    this.value = value;
  }

  @Override
  public DropData clone(String label) {
    if (readOnly)
      return new DropVar(label, value);
    return new DropVar(label);
  }

  @Override
  public void getData(ByteBuffer buffer, int index) {
    value = buffer.getInt(index);
  }

  @Override
  public void putData(ByteBuffer buffer, int index) {
    buffer.putInt(index, value);
  }

  @Override
  protected boolean valueEquals(DropData other) {
    return value == ((DropVar) other).value;
  }

  @Override
  public String toString() {
    return String.valueOf(value);
  }

  @Override
  public int size() {
    return IntSize;
  }
}
