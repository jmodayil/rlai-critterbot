package org.rlcommunity.disco;

import java.nio.ByteBuffer;
import java.util.Arrays;

import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.dropdata.DropString;
import org.rlcommunity.disco.dropdata.DropTime;


public class Drop {
  public final int size;
  public final int packetSize;
  private final DropData[] dropDatas;
  private final DropString dropName;
  private final DropTime dropTime;

  public Drop(String name, DropData... dropData) {
    assert name != null;
    dropName = new DropString("Name", name);
    dropDatas = new DropData[dropData.length];
    DropData dropTime = null;
    for (int i = 0; i < dropData.length; i++) {
      DropData cloned = dropData[i].clone();
      dropDatas[i] = cloned;
      if (cloned.label.equals(DropTime.TIMELABEL)) {
        assert dropTime == null;
        dropTime = cloned;
      }
    }
    this.dropTime = (DropTime) dropTime;
    size = computeSize();
    packetSize = size + headerSize();
  }

  private int headerSize() {
    return dropName.size() + DropData.IntSize;
  }

  private int computeSize() {
    int sum = 0;
    for (DropData data : dropDatas)
      sum += data.size();
    return sum;
  }

  public void putData(ByteBuffer buffer) {
    dropName.putData(buffer, 0);
    buffer.putInt(dropName.size(), size);
    int index = headerSize();
    for (DropData data : dropDatas) {
      data.putData(buffer, index);
      index += data.size();
    }
  }

  public void getData(ByteBuffer buffer, boolean skipHeader) {
    int index = 0;
    if (!skipHeader) {
      assert dropName.checkString(buffer);
      assert buffer.getInt(dropName.size()) == size;
      index = headerSize();
    }
    else {
      assert buffer.getInt(0) == size;
      index = DropData.IntSize;
    }
    for (DropData data : dropDatas) {
      data.getData(buffer, index);
      index += data.size();
    }
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null)
      return false;
    if (super.equals(obj))
      return true;
    Drop other = (Drop) obj;
    if (size != other.size || !dropName.equals(other.dropName))
      return false;
    for (int i = 0; i < dropDatas.length; i++)
      if (!dropDatas[i].equals(other.dropDatas[i]))
        return false;
    return true;
  }

  @Override
  public String toString() {
    String[] values = new String[dropDatas.length];
    for (int i = 0; i < values.length; i++)
      values[i] = dropDatas[i].toString();
    return dropName.value() + ": " + Arrays.toString(values);
  }

  public String name() {
    return dropName.value();
  }

  public double time() {
    return dropTime.time();
  }

  public DropData[] dropDatas() {
    return dropDatas;
  }

  public void setTime(long time) {
    dropTime.set(time);
  }
}
