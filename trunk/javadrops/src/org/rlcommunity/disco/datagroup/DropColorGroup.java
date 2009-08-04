/**
 * 
 */
package org.rlcommunity.disco.datagroup;

import java.awt.Color;

import org.rlcommunity.disco.Drop;
import org.rlcommunity.disco.dropdata.DropColor;
import org.rlcommunity.disco.dropdata.DropData;


public class DropColorGroup extends DataGroup<Color> {

  public DropColorGroup(Drop drop) {
    super(drop);
  }

  @Override
  protected boolean isDataSelected(DropData data) {
    return data instanceof DropColor;
  }

  @Override
  protected void setValue(DropData dropData, Color value) {
    ((DropColor) dropData).set(value);
  }

  @Override
  protected Color getValue(DropData dropData) {
    return ((DropColor) dropData).color();
  }

}