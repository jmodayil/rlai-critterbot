package org.rlcommunity.disco.utils;

import java.util.LinkedList;
import java.util.List;

public class Signal<T> {

  static public interface EventInfoGenerator<T> {
    T generate();
  }

  private final List<Listener<T>> listeners = new LinkedList<Listener<T>>();

  public Signal() {
    super();
  }

  public Listener<T> connect(Listener<T> listener) {
    assert listener != null;
    assert this.listeners.indexOf(listener) == -1;
    this.listeners.add(listener);
    return listener;
  }

  public void disconnect(Listener<T> listener) {
    boolean elementFound;
    elementFound = this.listeners.remove(listener);
    assert elementFound;
  }

  public final void fire(T eventInfo) {
    for (Listener<T> listener : this.listeners)
      listener.listen(eventInfo);
  }

  public final void fireIFN(EventInfoGenerator<T> eventInfoGenerator) {
    if (listeners.isEmpty())
      return;
    fire(eventInfoGenerator.generate());
  }
}
