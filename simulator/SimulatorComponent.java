/**
  * SimulatorComponent
  *
  * Defines a generic SimulatorEngine component.
  *
  * @author Marc G. Bellemare
  */


public interface SimulatorComponent
{
  /** Apply this component to the current state, storing the results in
    *  the next state. Note that the next state may (and is likely to) contain 
    *  information computed by other components, so components should play
    *  nice and only modify the variables that they correspond to.
    *
    *  @param pCurrent The current state (should not be modified)
    *  @param pNext    The next state, in which the results of applying the
    *       this component should be stored.
    *  @param delta    The amount of time (in ms) elapsed between 
    *       the current and next states.
    */
  public void apply (SimulatorState pCurrent, SimulatorState pNext, int delta);
}
