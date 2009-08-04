package org.rlcommunity.disco.dropdata;

import java.awt.Color;
import java.nio.ByteBuffer;

public class DropColor extends DropData {

  private Color color = Color.BLACK;

  public DropColor(String label) {
    super(label, true);
  }

  public void set(Color color) {
    this.color = color;
  }

  public Color color() {
    return color;
  }

  @Override
  public DropData clone(String label) {
    return new DropColor(label);
  }

  @Override
  public void getData(ByteBuffer buffer, int index) {
  }

  @Override
  public void putData(ByteBuffer buffer, int index) {
  }

  @Override
  public int size() {
    return ByteSize * 3;
  }

  @Override
  protected boolean valueEquals(DropData other) {
    return true;
  }

}
