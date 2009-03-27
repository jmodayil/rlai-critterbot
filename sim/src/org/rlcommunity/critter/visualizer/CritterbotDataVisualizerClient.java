/*
 * (c) 2009 Marc G. Bellemare.
 */

package org.rlcommunity.critter.visualizer;

import java.util.LinkedList;
import java.util.List;
import org.rlcommunity.critter.Clients.DropClient;
import org.rlcommunity.critter.Drops.CritterStateDrop;
import org.rlcommunity.critter.Drops.SimulatorDrop;

/** A drop client that passes the CritterStateDrops it receives on to a GUI.
 *
 * @author Marc G. Bellemare (mg17 at cs ualberta ca)
 */
public class CritterbotDataVisualizerClient implements DropClient {
  private CritterbotVisualizerFrame aGUI;

  public CritterbotDataVisualizerClient() {
    // Create the 'GUI'

    javax.swing.SwingUtilities.invokeLater(
        new Runnable() {

          public void run() {
            aGUI = new CritterbotVisualizerFrame();
            aGUI.setTitle("Critterbot GUI");
            aGUI.setVisible(true);
          }
        });
  }

  public List<SimulatorDrop> receive() {
    // The GUI, for now, does not generate any drops
    return new LinkedList<SimulatorDrop>();
  }

  public void send(SimulatorDrop pData) {
    // Parse the Critterbot data
    if (aGUI != null && pData instanceof CritterStateDrop) {
      aGUI.updateDisplay((CritterStateDrop) pData);
    }
  }

  public void die() {
    aGUI.dispose();
  }
}
