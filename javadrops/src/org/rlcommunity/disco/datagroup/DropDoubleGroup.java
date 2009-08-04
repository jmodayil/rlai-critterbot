/**
 * 
 */
package org.rlcommunity.disco.datagroup;

import org.rlcommunity.disco.Drop;
import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.dropdata.DropVar;

public class DropDoubleGroup extends DataGroup<Double> {
  public DropDoubleGroup(Drop drop) {
    super(drop);
  }

  @Override
  protected Double getValue(DropData dropData) {
    return (double) ((DropVar) dropData).value();
  }

  @Override
  protected boolean isDataSelected(DropData data) {
    return data instanceof DropVar;
  }

  @Override
  protected void setValue(DropData dropData, Double value) {
    ((DropVar) dropData).set((int) Math.round(value));
  }
}