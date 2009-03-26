/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.rlcommunity.critter.Drops;

import java.io.IOException;
import org.rlcommunity.critter.InterfaceInputStream;
import org.rlcommunity.critter.InterfaceOutputStream;

/**
 *
 * @author critterbot
 */
public class CritterLogTagDrop implements SimulatorDrop {

    public String name;
    public String text;
    
    public CritterLogTagDrop(String n, String t) {
        text = t;
        name = n;
    }
    
    public int getSize() {
        return Integer.SIZE / 8 + // Size of name
               Integer.SIZE / 8 + // Size of text
               name.length() + text.length();
    }

    public void writeData(InterfaceOutputStream pOut) throws IOException {
        pOut.writeString(name);
        //int len = 100 - text.length();
        pOut.writeString(text);
        //while(len-- > 0)
        //    pOut.writeByte((byte)0);
        
    }

    public void readData(InterfaceInputStream pIn, int size) throws IOException {
        name = pIn.readString();
        text = pIn.readString();
    }
}
