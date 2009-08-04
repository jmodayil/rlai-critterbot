package org.rlcommunity.disco.dropdata;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class DropArray extends DropData {

  final private DropData[] dropDatas;
  private final int size;
  private final String[] suffixes;
  private final DropData prototype;

  public DropArray(String label, int nbData) {
    this(label, getSuffixes(nbData));
  }

  public DropArray(DropData prototype, String label, int nbData) {
    this(prototype, label, getSuffixes(nbData));
  }

  public DropArray(String label, String... suffixes) {
    this(new DropVar(""), label, suffixes);
  }

  public DropArray(DropData prototype, String label, String... suffixes) {
    super(label, false);
    assert suffixes.length > 0;
    assert prototype != null;
    this.prototype = prototype;
    dropDatas = new DropData[suffixes.length];
    int sum = 0;
    for (int i = 0; i < suffixes.length; i++) {
      DropData data = prototype.clone(label + suffixes[i]);
      dropDatas[i] = data;
      sum += data.size();
    }
    size = sum;
    this.suffixes = suffixes;
  }

  @Override
  public DropData clone(String label) {
    return new DropArray(prototype, label, suffixes);
  }

  @Override
  public void getData(ByteBuffer buffer, int index) {
    int dataIndex = index;
    for (DropData data : dropDatas) {
      data.getData(buffer, dataIndex);
      dataIndex += data.size();
    }
  }

  @Override
  public void putData(ByteBuffer buffer, int index) {
    int dataIndex = index;
    for (DropData data : dropDatas) {
      data.putData(buffer, dataIndex);
      dataIndex += data.size();
    }
  }

  @Override
  public int size() {
    return size;
  }

  @Override
  protected boolean valueEquals(DropData other) {
    DropArray otherArray = (DropArray) other;
    for (int i = 0; i < dropDatas.length; i++)
      if (!otherArray.dropDatas[i].equals(dropDatas[i]))
        return false;
    return true;
  }

  private static String[] getSuffixes(int nbData) {
    String[] suffixes = new String[nbData];
    for (int i = 0; i < suffixes.length; i++)
      suffixes[i] = String.format("%d", i);
    return suffixes;
  }

  @Override
  public String toString() {
    String[] values = new String[dropDatas.length];
    for (int i = 0; i < values.length; i++)
      values[i] = dropDatas[i].toString();
    return label + ": " + Arrays.toString(values);
  }

  public DropData[] dropDatas() {
    return dropDatas;
  }
}
