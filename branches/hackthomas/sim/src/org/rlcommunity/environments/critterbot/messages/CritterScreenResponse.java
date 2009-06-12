/* Mountain Car Domain
 * Copyright (C) 2007, Brian Tanner brian@tannerpages.com (http://brian.tannerpages.com/)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */
package org.rlcommunity.environments.critterbot.messages;

import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import org.apache.commons.codec.binary.Base64;
import rlVizLib.messaging.AbstractResponse;
import rlVizLib.messaging.GenericMessage;
import rlVizLib.messaging.MessageUser;
import rlVizLib.messaging.MessageValueType;
import rlVizLib.messaging.NotAnRLVizMessageException;
import rlVizLib.messaging.environment.EnvMessageType;

public class CritterScreenResponse extends AbstractResponse {
    BufferedImage theImage=null;

    public CritterScreenResponse(BufferedImage theImage) {
        this.theImage=theImage;
    }

    public CritterScreenResponse(String responseMessage) throws NotAnRLVizMessageException {
            GenericMessage theGenericResponse = new GenericMessage(responseMessage);
            String thePayLoadString = theGenericResponse.getPayLoad();
            theImage=readEncodedPayloadFromString(thePayLoadString);
    }

    @Override
    public String makeStringResponse() {
        try {
            StringBuffer theResponseBuffer = new StringBuffer();
            theResponseBuffer.append("TO=");
            theResponseBuffer.append(MessageUser.kBenchmark.id());
            theResponseBuffer.append(" FROM=");
            theResponseBuffer.append(MessageUser.kEnv.id());
            theResponseBuffer.append(" CMD=");
            theResponseBuffer.append(EnvMessageType.kEnvResponse.id());
            theResponseBuffer.append(" VALTYPE=");
            theResponseBuffer.append(MessageValueType.kString.id());
            theResponseBuffer.append(" VALS=");
            ByteArrayOutputStream BOS = new ByteArrayOutputStream();
            ImageIO.write(theImage, "png", new BufferedOutputStream(BOS));
            BOS.close();
            byte[] theStringBytes = BOS.toByteArray();
            byte[] b64encoded=Base64.encodeBase64(theStringBytes);
            String theBytesAsString = new String(b64encoded);
            theResponseBuffer.append(theBytesAsString);
            return theResponseBuffer.toString();
        } catch (IOException ex) {
            Logger.getLogger(CritterScreenResponse.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return null;

    }

    public BufferedImage getImage(){
        return theImage;
    }

    private static BufferedImage readEncodedPayloadFromString(String thePayLoadString) {
        try {
           byte[] encodedPayload=thePayLoadString.getBytes();
           byte[] payLoadInBytes = Base64.decodeBase64(encodedPayload);
           ByteArrayInputStream BIS = new ByteArrayInputStream(payLoadInBytes);
           BufferedImage theImage=ImageIO.read(new BufferedInputStream(BIS));
           BIS.close();
           return theImage;
        } catch (IOException ex) {
            return null;
        }
    }
};