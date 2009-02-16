/*
 * Copyright 2008 Thomas Degris
 * http://bt-recordbook.googlecode.com/
 * brian@tannerpages.com
 * http://brian.tannerpages.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package org.rlcommunity.critter.Clients;

import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.rlcommunity.critter.Drops.CritterStateDrop;
import org.rlcommunity.critter.Drops.SimulatorDrop;

/**
 *
 * @author Thomas
 */
public class DumpClient implements DropClient {

    private class Dump {

        final private StringBuilder dump = new StringBuilder();

        void append(int value) {
            dump.append(value);
            separator();
        }

        private void separator() {
            dump.append(" ");
        }

        void write() {
            dump.append('\n');
            try {
                file.write(dump.toString());
                file.flush();
            } catch (IOException ex) {
                Logger.getLogger(DumpClient.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    final private String dumpFilePath;
    final private FileWriter file;

    public DumpClient(String dumpFilePath) {
        this.dumpFilePath = dumpFilePath;
        FileWriter newFile = null;
        try {
            newFile = new FileWriter(dumpFilePath);
        } catch (IOException ex) {
            Logger.getLogger(DumpClient.class.getName()).log(Level.SEVERE, null, ex);
        }
        file = newFile;
        writeHeader();
    }

    public List<SimulatorDrop> receive() {
        return null;
    }

    public void send(SimulatorDrop pData) {
        CritterStateDrop drop = (CritterStateDrop) pData;
        Dump dump = new Dump();

        dump.append(drop.cycle_time);

        dump.append(drop.motor100.command);
        dump.append(drop.motor100.velocity);
        dump.append(drop.motor100.current);

        dump.append(drop.motor220.command);
        dump.append(drop.motor220.velocity);
        dump.append(drop.motor220.current);

        dump.append(drop.motor340.command);
        dump.append(drop.motor340.velocity);
        dump.append(drop.motor340.current);

        dump.append(drop.accel.x);
        dump.append(drop.accel.y);
        dump.append(drop.accel.z);

        dump.append(drop.rotation);

        for (int ir = 0; ir < 10; ir++)
            dump.append(drop.ir_distance[ir]);

        for (int lght = 0; lght < 4; lght++)
            dump.append(drop.light[lght]);

        dump.write();
    }

    private void writeHeader() {
        try {
            file.write("Time Motor0_Command Motor0_Speed Motor0_Current Motor1_Command Motor1_Speed Motor1_Current Motor2_Command Motor2_Speed Motor2_Current AccelX AccelY AccelZ RotationVel IR0 IR1 IR2 IR3 IR4 IR5 IR6 IR7 IR8 IR9 Light0 Light1 Light2 Light3\n");
        } catch (IOException ex) {
            Logger.getLogger(DumpClient.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
