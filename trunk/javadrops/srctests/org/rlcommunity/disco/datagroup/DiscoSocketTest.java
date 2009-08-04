package org.rlcommunity.disco.datagroup;

import java.io.IOException;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.rlcommunity.disco.DiscoSocket;
import org.rlcommunity.disco.Drop;
import org.rlcommunity.disco.Server;
import org.rlcommunity.disco.utils.Listener;

public class DiscoSocketTest {
    private Server server;
    private DiscoSocket socket02;
    private DiscoSocket socket01 = null;
    private Drop lastDropReceived = null;

    @Before
    public void setUp() throws IOException {
	server = new Server();
	socket02 = new DiscoSocket(server.port);
	socket01 = server.accept();
    }

    @After
    public void setDown() throws IOException {
	socket01.close();
	socket02.close();
	server.close();
    }

    @Test
    public void testServerClientCommunication() throws IOException {
	Drop sendDrop = new Drop("NoTime", DropTest.dropData01);
	DropVarGroup sendGroup = new DropVarGroup(sendDrop);
	Drop receivedDrop = new Drop("NoTime", DropTest.dropData01);

	sendGroup.set(45);
	Assert.assertFalse(sendDrop.equals(receivedDrop));
	socket01.send(sendDrop);
	socket02.recv(receivedDrop);
	Assert.assertEquals(sendDrop, receivedDrop);

	sendGroup.set(60);
	Assert.assertFalse(sendDrop.equals(receivedDrop));
	socket02.send(sendDrop);
	socket01.recv(receivedDrop);
	Assert.assertEquals(sendDrop, receivedDrop);
    }

    @Test
    public void testServerClientCommunicationWithTime() throws IOException {
	Drop sendDrop = new Drop("WithTime", DropTest.dropData02);
	DropVarGroup sendGroup = new DropVarGroup(sendDrop);
	Drop receivedDrop = new Drop("WithTime", DropTest.dropData02);

	sendGroup.setWithTime(20, 45);
	Assert.assertFalse(sendDrop.equals(receivedDrop));
	socket01.send(sendDrop);
	socket02.recv(receivedDrop);
	Assert.assertEquals(sendDrop, receivedDrop);

	sendGroup.setWithTime(30, 60);
	Assert.assertFalse(sendDrop.equals(receivedDrop));
	socket02.send(sendDrop);
	socket01.recv(receivedDrop);
	Assert.assertEquals(sendDrop, receivedDrop);
    }

    @Test
    public void testServerClientCommunicationWithFinal() throws IOException {
	Drop sendDrop = new Drop("WithFinal", DropTest.dropData03);
	DropVarGroup sendGroup = new DropVarGroup(sendDrop);
	Drop receivedDrop = new Drop("WithFinal", DropTest.dropData04);

	sendGroup.set(45, 90);
	Assert.assertFalse(sendDrop.equals(receivedDrop));
	socket01.send(sendDrop);
	socket02.recv(receivedDrop);
	Assert.assertEquals(sendDrop, receivedDrop);
    }

    @Test
    public void testServerClientCommunicationWithArray() throws IOException {
	Drop sendDrop = new Drop("WithArray", DropTest.dropData05);
	DropVarGroup sendGroup = new DropVarGroup(sendDrop);
	Drop receivedDrop = new Drop("WithArray", DropTest.dropData05);

	sendGroup.set(4, 5, 6, 7, 8);
	Assert.assertFalse(sendDrop.equals(receivedDrop));
	socket01.send(sendDrop);
	socket02.recv(receivedDrop);
	Assert.assertEquals(sendDrop, receivedDrop);
    }

    @Test
    public void testServerCommunicationWithSignals() throws IOException {
	Drop sendDropType01 = new Drop("Type01", DropTest.dropData01);
	DropVarGroup sendGroupType01 = new DropVarGroup(sendDropType01);
	Drop recvDropType01 = new Drop("Type01", DropTest.dropData01);
	Drop sendDropType02 = new Drop("Type02", DropTest.dropData01);
	DropVarGroup sendGroupType02 = new DropVarGroup(sendDropType02);
	Drop recvDropType02 = new Drop("Type02", DropTest.dropData01);

	socket02.connect(recvDropType01, new Listener<Drop>() {
	    @SuppressWarnings("synthetic-access")
	    public void listen(Drop drop) {
		lastDropReceived = drop;
	    }
	});
	socket02.connect(recvDropType02, null);

	sendGroupType01.set(60);
	socket01.send(sendDropType01);
	socket02.recv();
	Assert.assertEquals(sendDropType01, recvDropType01);
	Assert.assertFalse(sendDropType01.equals(recvDropType02));
	Assert.assertSame(recvDropType01, lastDropReceived);

	sendGroupType02.set(40);
	socket01.send(sendDropType02);
	socket02.recv();
	Assert.assertEquals(sendDropType02, recvDropType02);
	Assert.assertFalse(sendDropType02.equals(recvDropType01));
	Assert.assertNotSame(recvDropType02, lastDropReceived);

    }
}
