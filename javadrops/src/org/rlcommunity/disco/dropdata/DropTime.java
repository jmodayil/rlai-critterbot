package org.rlcommunity.disco.dropdata;

import java.nio.ByteBuffer;

public class DropTime extends DropData {

  static final public String TIMELABEL = "Time";
  private long time;

  public DropTime() {
    super(TIMELABEL, false);
  }

  public void set() {
    set(System.currentTimeMillis());
  }

  public void set(long time) {
    this.time = time;
  }

  @Override
  public int size() {
    return 2 * IntSize;
  }

  @Override
  public void putData(ByteBuffer buffer, int index) {
    buffer.putInt(index, (int) time);
  }

  @Override
  public void getData(ByteBuffer buffer, int index) {
    long longValue = buffer.getInt(index);
    time = 0xFFFFFFFFL & longValue;
  }

  @Override
  public DropData clone(String label) {
    return new DropTime();
  }

  @Override
  protected boolean valueEquals(DropData other) {
    return time == ((DropTime) other).time;
  }

  @Override
  public String toString() {
    return String.valueOf(time()) + "s";
  }

  public double time() {
    return time / 1000.0;
  }
}
