/**
 * 
 */
package org.rlcommunity.disco.datagroup;

import org.rlcommunity.disco.Drop;
import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.dropdata.DropVar;

public class DropVarGroup extends DataGroup<Integer> {
  public DropVarGroup(Drop drop) {
    super(drop);
  }

  @Override
  protected Integer getValue(DropData dropData) {
    return ((DropVar) dropData).value();
  }

  @Override
  protected boolean isDataSelected(DropData data) {
    return data instanceof DropVar;
  }

  @Override
  protected void setValue(DropData dropData, Integer value) {
    ((DropVar) dropData).set(value);
  }

}