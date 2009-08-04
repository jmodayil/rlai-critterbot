package org.rlcommunity.disco.utils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class Legend {
  public static final String DEFAULT_COMMAND_PREFIX = "Command";
  final private Map<String, Integer> legend = new LinkedHashMap<String, Integer>();
  private Integer[] actionIndexes;

  public Legend(String... labels) {
    this(Arrays.asList(labels));
  }

  public Legend(List<String> labels) {
    for (int i = 0; i < labels.size(); i++) {
      String label = labels.get(i);
      assert !label.equals("Time");
      legend.put(label, i);
    }
    actionIndexes = computeActionIndexes(DEFAULT_COMMAND_PREFIX);
  }

  public void setCommandPrefix(String commandPrefix) {
    actionIndexes = computeActionIndexes(commandPrefix);
  }

  final public int timeIndex() {
    return 0;
  }

  final public int indexOf(String label) {
    return legend.get(label);
  }

  public Integer[] actionIndexes() {
    return actionIndexes;
  }

  private Integer[] computeActionIndexes(String commandPrefix) {
    List<Integer> actionIndexes = new ArrayList<Integer>();
    for (Map.Entry<String, Integer> entry : legend.entrySet())
      if (entry.getKey().indexOf(commandPrefix) > -1)
        actionIndexes.add(entry.getValue());
    Integer[] result = new Integer[actionIndexes.size()];
    actionIndexes.toArray(result);
    return result;
  }

  public int nbLabels() {
    return legend.size();
  }

  public Map<String, Integer> legend() {
    return legend;
  }

}
