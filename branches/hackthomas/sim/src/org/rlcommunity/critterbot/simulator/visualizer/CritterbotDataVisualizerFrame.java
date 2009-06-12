/*
 * CritterViz.java
 *
 * Created on March 12, 2009, 10:37 AM
 */

package org.rlcommunity.critterbot.simulator.visualizer;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.Timer;

import org.rlcommunity.critterbot.javadrops.drops.CritterStateDrop;
import org.rlcommunity.critterbot.javadrops.drops.DropInterface;

/**
 *
 * @author  critterbot
 */
public class CritterbotDataVisualizerFrame extends JFrame implements WindowListener{
    
    DropInterface di;
    int batState;
    //CritterVizLogs logWindow;
    JFrame me;
    private Timer timer;
    private static final int INTERVAL = 100;
    private boolean updated;

    private boolean doEventsNotifications;
    
    /** A semaphore to prevent data updating while redrawing */
    private final Object dataMutex = new Object();

    CritterStateDrop lastStateDrop;
    
    /** Creates new form CritterViz */
    public CritterbotDataVisualizerFrame(final DropInterface _di) {
        di = _di;
        batState = 0;
        initComponents();
        me = this;
        updated = true;
        
        timer = new javax.swing.Timer(INTERVAL,
                new ActionListener() {

                    public void actionPerformed(ActionEvent e) {
                      if (updated) {
                        completeUpdateDisplay();
                        repaint();
                        updated = false;
                      }
                    }
                });
        timer.start();
    }
    
    /** Sets whether the visualizer should notify the user of voltage-related
     *   events (and other bad things).
     * 
     * @param pNotify
     */
    public void setEventsNotifications(boolean pNotify) {
      doEventsNotifications = pNotify;
    }

    private void showLogWindow() {
        /*if(logWindow == null)
            javax.swing.SwingUtilities.invokeLater( 
                    new Runnable() {
                        public void run() {
                            logWindow = new CritterVizLogs(di,(WindowListener)me);
                            logWindow.setLocationRelativeTo(me);
                            logWindow.setLocation(me.getWidth(), 0);
                            logWindow.setVisible(true);
                        }
                    }
            ); */
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        jLabel1 = new javax.swing.JLabel();
        jPanel1 = new javax.swing.JPanel();
        jPanelIR = new javax.swing.JPanel();
        ir0 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir1 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir2 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir3 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir4 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir5 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir6 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir7 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir8 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        ir9 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        jPanelLight = new javax.swing.JPanel();
        light0 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        light1 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        light2 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        light3 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        jPanelPower = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        voltage = new javax.swing.JTextField();
        jPanelMotor = new javax.swing.JPanel();
        jPanel2 = new javax.swing.JPanel();
        mspeed0 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        mspeed1 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        mspeed2 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        jPanel8 = new javax.swing.JPanel();
        mcurrent0 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        mcurrent1 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        mcurrent2 = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        jPanel9 = new javax.swing.JPanel();
        mtemp0 = new org.rlcommunity.critterbot.gui.RichDisplayBar(160);
        mtemp1 = new org.rlcommunity.critterbot.gui.RichDisplayBar(160);
        mtemp2 = new org.rlcommunity.critterbot.gui.RichDisplayBar(160);
        jPanelInertial = new javax.swing.JPanel();
        jPanel11 = new javax.swing.JPanel();
        rotation = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        jPanel12 = new javax.swing.JPanel();
        accelx = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        accely = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        accelz = new org.rlcommunity.critterbot.gui.RichDisplayBar();
        jMenuWindow = new javax.swing.JMenuBar();
        jMenuFile = new javax.swing.JMenu();
        jMenuFileExit = new javax.swing.JMenuItem();
        jMenu2 = new javax.swing.JMenu();
        jMenuWindowLogs = new javax.swing.JMenuItem();

        jLabel1.setText("jLabel1");

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
        getContentPane().setLayout(new java.awt.CardLayout());

        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Critterbot GUI", javax.swing.border.TitledBorder.CENTER, javax.swing.border.TitledBorder.DEFAULT_POSITION));
        jPanel1.setMinimumSize(new java.awt.Dimension(500, 500));
        jPanel1.setPreferredSize(new java.awt.Dimension(600, 500));
        jPanel1.setLayout(new java.awt.GridBagLayout());

        jPanelIR.setBorder(javax.swing.BorderFactory.createTitledBorder("IR Distance Sensors"));
        jPanelIR.setLayout(new java.awt.GridLayout(1, 0));

        ir0.setRange(0, 255);
        ir0.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir0);

        ir1.setRange(0, 255);
        ir1.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir1);

        ir2.setRange(0, 255);
        ir2.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir2);

        ir3.setRange(0, 255);
        ir3.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir3);

        ir4.setRange(0, 255);
        ir4.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir4);

        ir5.setRange(0, 255);
        ir5.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir5);

        ir6.setRange(0, 255);
        ir6.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir6);

        ir7.setRange(0, 255);
        ir7.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir7);

        ir8.setRange(0, 255);
        ir8.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir8);

        ir9.setRange(0, 255);
        ir9.setLayout(new java.awt.GridLayout(1, 0));
        jPanelIR.add(ir9);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 6.0;
        gridBagConstraints.weighty = 6.0;
        jPanel1.add(jPanelIR, gridBagConstraints);

        jPanelLight.setBorder(javax.swing.BorderFactory.createTitledBorder("Light Sensors"));
        jPanelLight.setLayout(new java.awt.GridLayout(1, 0));

        light0.setRange(0, 800);
        light0.setLayout(new java.awt.GridLayout(1, 0));
        jPanelLight.add(light0);

        light1.setRange(0, 800);
        light1.setLayout(new java.awt.GridLayout(1, 0));
        jPanelLight.add(light1);

        light2.setRange(0, 800);
        light2.setLayout(new java.awt.GridLayout(1, 0));
        jPanelLight.add(light2);

        light3.setRange(0, 800);
        light3.setLayout(new java.awt.GridLayout(1, 0));
        jPanelLight.add(light3);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        jPanel1.add(jPanelLight, gridBagConstraints);

        jPanelPower.setBorder(javax.swing.BorderFactory.createTitledBorder("Power"));

        jLabel2.setFont(new java.awt.Font("DejaVu Sans", 0, 18)); // NOI18N
        jLabel2.setText("Charge");
        jPanelPower.add(jLabel2);

        voltage.setColumns(4);
        voltage.setEditable(false);
        voltage.setFont(new java.awt.Font("DejaVu Sans", 0, 18));
        voltage.setText("0.00");
        jPanelPower.add(voltage);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        jPanel1.add(jPanelPower, gridBagConstraints);

        jPanelMotor.setBorder(javax.swing.BorderFactory.createTitledBorder("Motors"));
        jPanelMotor.setLayout(new java.awt.GridLayout(1, 0));

        jPanel2.setBorder(javax.swing.BorderFactory.createTitledBorder("Speed"));
        jPanel2.setLayout(new java.awt.GridLayout(1, 0));

        mspeed0.setRange(-35, 35);
        mspeed0.setLayout(new java.awt.GridLayout(1, 0));
        jPanel2.add(mspeed0);

        mspeed1.setRange(-35, 35);
        mspeed1.setLayout(new java.awt.GridLayout(1, 0));
        jPanel2.add(mspeed1);

        mspeed2.setRange(-35, 35);
        mspeed2.setLayout(new java.awt.GridLayout(1, 0));
        jPanel2.add(mspeed2);

        jPanelMotor.add(jPanel2);

        jPanel8.setBorder(javax.swing.BorderFactory.createTitledBorder("Current"));
        jPanel8.setLayout(new java.awt.GridLayout(1, 0));

        mcurrent0.setRange(0, 90);
        mcurrent0.setLayout(new java.awt.GridLayout(1, 0));
        jPanel8.add(mcurrent0);

        mcurrent1.setRange(0, 90);
        mcurrent1.setLayout(new java.awt.GridLayout(1, 0));
        jPanel8.add(mcurrent1);

        mcurrent2.setRange(0, 90);
        mcurrent2.setLayout(new java.awt.GridLayout(1, 0));
        jPanel8.add(mcurrent2);

        jPanelMotor.add(jPanel8);

        jPanel9.setBorder(javax.swing.BorderFactory.createTitledBorder("Temperature"));
        jPanel9.setLayout(new java.awt.GridLayout(1, 0));

        mtemp0.setRange(40, 175);
        mtemp0.setLayout(new java.awt.GridLayout(1, 0));
        jPanel9.add(mtemp0);

        mtemp1.setRange(40, 175);
        mtemp1.setLayout(new java.awt.GridLayout(1, 0));
        jPanel9.add(mtemp1);

        mtemp2.setRange(40, 175);
        mtemp2.setLayout(new java.awt.GridLayout(1, 0));
        jPanel9.add(mtemp2);

        jPanelMotor.add(jPanel9);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 6.0;
        jPanel1.add(jPanelMotor, gridBagConstraints);

        jPanelInertial.setBorder(javax.swing.BorderFactory.createTitledBorder("Inertial Sensors"));
        jPanelInertial.setLayout(new java.awt.GridBagLayout());

        jPanel11.setBorder(javax.swing.BorderFactory.createTitledBorder("Gyroscope"));
        jPanel11.setLayout(new java.awt.GridLayout(1, 0));

        rotation.setRange(-420,420);
        rotation.setLayout(new java.awt.GridLayout(1, 0));
        jPanel11.add(rotation);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        jPanelInertial.add(jPanel11, gridBagConstraints);

        jPanel12.setBorder(javax.swing.BorderFactory.createTitledBorder("Accelerometer"));
        jPanel12.setLayout(new java.awt.GridLayout(1, 0));

        accelx.setRange(-2048, 2048);
        accelx.setLayout(new java.awt.GridLayout(1, 0));
        jPanel12.add(accelx);

        accely.setRange(-2048, 2048);
        accely.setLayout(new java.awt.GridLayout(1, 0));
        jPanel12.add(accely);

        accelz.setRange(-2048, 2048);
        accelz.setLayout(new java.awt.GridLayout(1, 0));
        jPanel12.add(accelz);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.5;
        gridBagConstraints.weighty = 1.0;
        jPanelInertial.add(jPanel12, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.7;
        jPanel1.add(jPanelInertial, gridBagConstraints);

        getContentPane().add(jPanel1, "card2");

        jMenuFile.setText("File");

        jMenuFileExit.setText("Exit");
        jMenuFileExit.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuFileExitActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuFileExit);

        jMenuWindow.add(jMenuFile);

        jMenu2.setText("Window");

        jMenuWindowLogs.setText("Logs");
        jMenuWindowLogs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuWindowLogsActionPerformed(evt);
            }
        });
        jMenu2.add(jMenuWindowLogs);

        jMenuWindow.add(jMenu2);

        setJMenuBar(jMenuWindow);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jMenuWindowLogsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuWindowLogsActionPerformed
        showLogWindow();
}//GEN-LAST:event_jMenuWindowLogsActionPerformed

    private void jMenuFileExitActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuFileExitActionPerformed
        System.exit(0);
}//GEN-LAST:event_jMenuFileExitActionPerformed
    
    public void windowOpened(WindowEvent we) {
        
    }
    public void windowClosed(WindowEvent we) {
        System.out.println("Closing logWindow");
        //logWindow = null;
    }
    public void windowClosing(WindowEvent we) {
        
    }
    public void windowIconified(WindowEvent we) {
        
    }
    public void windowDeiconified(WindowEvent we) {
        
    }
    public void windowActivated(WindowEvent we) {
        
    }
    public void windowDeactivated(WindowEvent we) {
        
    }
    
    /**
     * @param args the command line arguments
     */
    /*public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new CritterViz().setVisible(true);
            }
        });
    }*/
    
    public void updateDisplay(CritterStateDrop state) {
      synchronized(dataMutex) {
        lastStateDrop = state;

        updated = true;
      }
    }

    /** Actually update the values before repainting */
    public void completeUpdateDisplay() {
      synchronized (dataMutex) {
        if (lastStateDrop != null) {
          ir0.updateValue(lastStateDrop.ir_distance[0]);
          ir1.updateValue(lastStateDrop.ir_distance[1]);
          ir2.updateValue(lastStateDrop.ir_distance[2]);
          ir3.updateValue(lastStateDrop.ir_distance[3]);
          ir4.updateValue(lastStateDrop.ir_distance[4]);
          ir5.updateValue(lastStateDrop.ir_distance[5]);
          ir6.updateValue(lastStateDrop.ir_distance[6]);
          ir7.updateValue(lastStateDrop.ir_distance[7]);
          ir8.updateValue(lastStateDrop.ir_distance[8]);
          ir9.updateValue(lastStateDrop.ir_distance[9]);
          light0.updateValue(lastStateDrop.light[0]);
          light1.updateValue(lastStateDrop.light[1]);
          light2.updateValue(lastStateDrop.light[2]);
          light3.updateValue(lastStateDrop.light[3]);
          accelx.updateValue(lastStateDrop.accel.x);
          accely.updateValue(lastStateDrop.accel.y);
          accelz.updateValue(lastStateDrop.accel.z);
          rotation.updateValue(lastStateDrop.rotation);
          mspeed0.updateValue(lastStateDrop.motor100.velocity);
          mspeed1.updateValue(lastStateDrop.motor220.velocity);
          mspeed2.updateValue(lastStateDrop.motor340.velocity);
          mcurrent0.updateValue(lastStateDrop.motor100.current);
          mcurrent1.updateValue(lastStateDrop.motor220.current);
          mcurrent2.updateValue(lastStateDrop.motor340.current);
          mtemp0.updateValue(lastStateDrop.motor100.temp);
          mtemp1.updateValue(lastStateDrop.motor220.temp);
          mtemp2.updateValue(lastStateDrop.motor340.temp);
          voltage.setText(Float.toString(((float) lastStateDrop.batv40) / 100)+"%");
          voltage.setBackground(Color.white);
        }
      }
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private org.rlcommunity.critterbot.gui.RichDisplayBar accelx;
    private org.rlcommunity.critterbot.gui.RichDisplayBar accely;
    private org.rlcommunity.critterbot.gui.RichDisplayBar accelz;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir0;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir1;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir2;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir3;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir4;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir5;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir6;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir7;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir8;
    private org.rlcommunity.critterbot.gui.RichDisplayBar ir9;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JMenu jMenu2;
    private javax.swing.JMenu jMenuFile;
    private javax.swing.JMenuItem jMenuFileExit;
    private javax.swing.JMenuBar jMenuWindow;
    private javax.swing.JMenuItem jMenuWindowLogs;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel12;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JPanel jPanelIR;
    private javax.swing.JPanel jPanelInertial;
    private javax.swing.JPanel jPanelLight;
    private javax.swing.JPanel jPanelMotor;
    private javax.swing.JPanel jPanelPower;
    private org.rlcommunity.critterbot.gui.RichDisplayBar light0;
    private org.rlcommunity.critterbot.gui.RichDisplayBar light1;
    private org.rlcommunity.critterbot.gui.RichDisplayBar light2;
    private org.rlcommunity.critterbot.gui.RichDisplayBar light3;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mcurrent0;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mcurrent1;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mcurrent2;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mspeed0;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mspeed1;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mspeed2;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mtemp0;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mtemp1;
    private org.rlcommunity.critterbot.gui.RichDisplayBar mtemp2;
    private org.rlcommunity.critterbot.gui.RichDisplayBar rotation;
    private javax.swing.JTextField voltage;
    // End of variables declaration//GEN-END:variables
    
}
