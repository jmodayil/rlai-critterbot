package org.rlcommunity.disco.dropdata;

import java.nio.ByteBuffer;

public abstract class DropData {

  public static final int IntSize = Integer.SIZE / 8;
  public static final int CharSize = Byte.SIZE / 8;
  public static final int ByteSize = Byte.SIZE / 8;

  final public String label;
  final public boolean readOnly;

  protected DropData(String label, boolean readOnly) {
    this.label = label;
    this.readOnly = readOnly;
  }

  @Override
  public DropData clone() {
    return clone(label);
  }

  abstract public DropData clone(String label);

  abstract public int size();

  abstract public void getData(ByteBuffer buffer, int index);

  abstract public void putData(ByteBuffer buffer, int index);

  abstract protected boolean valueEquals(DropData other);

  @Override
  public boolean equals(Object obj) {
    if (obj == null)
      return false;
    if (super.equals(obj))
      return true;
    return valueEquals((DropData) obj);
  }
}
