package org.rlcommunity.disco;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.LinkedHashMap;
import java.util.Map;

import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.dropdata.DropString;
import org.rlcommunity.disco.utils.Listener;
import org.rlcommunity.disco.utils.Signal;

public class DiscoSocket {

    static private class DropSignal {

	final Drop drop;
	final Signal<Drop> signal;

	DropSignal(Drop drop, Signal<Drop> signal) {
	    this.drop = drop;
	    this.signal = signal;
	}
    }

    static final private ByteOrder INPUT_ENDIAN = ByteOrder.LITTLE_ENDIAN;
    private final Socket socket;
    private final DataInputStream in;
    private final DataOutputStream out;
    private final Map<String, DropSignal> nameToDropSignal = new LinkedHashMap<String, DropSignal>();
    private final DropString name = new DropString("name");

    public DiscoSocket(int port) throws UnknownHostException, IOException {
	this("localhost", port);
    }

    public DiscoSocket(String host, int serverPort)
	    throws UnknownHostException, IOException {
	this(new Socket(host, serverPort));
    }

    public DiscoSocket(Socket socket) throws IOException {
	this.socket = socket;
	in = new DataInputStream(socket.getInputStream());
	out = new DataOutputStream(socket.getOutputStream());
    }

    private ByteBuffer allocate(int capacity) {
	ByteBuffer buffer = ByteBuffer.allocate(capacity);
	buffer.order(INPUT_ENDIAN);
	return buffer;
    }

    public void send(Drop sendDrop) throws IOException {
	ByteBuffer buffer = allocate(sendDrop.packetSize);
	sendDrop.putData(buffer);
	out.write(buffer.array(), 0, buffer.capacity());
    }

    private void discardRecvData() throws IOException {
	int bufferSize = readSize();
	ByteBuffer buffer = allocate(bufferSize);
	in.readFully(buffer.array(), 0, buffer.capacity());
    }

    public void readName() throws IOException {
	int stringSize = readSize();
	ByteBuffer stringBuffer = allocate(stringSize);
	in.readFully(stringBuffer.array(), 0, stringBuffer.capacity());
	name.getData(stringBuffer, 0);
    }

    private int readSize() throws IOException {
	ByteBuffer sizeBuffer = allocate(DropData.IntSize);
	in.readFully(sizeBuffer.array(), 0, sizeBuffer.capacity());
	int size = sizeBuffer.getInt();
	return size;
    }

    public void recv(Drop receivedDrop) throws IOException {
	ByteBuffer buffer = allocate(receivedDrop.packetSize);
	in.readFully(buffer.array(), 0, buffer.capacity());
	receivedDrop.getData(buffer, false);
    }

    public void close() throws IOException {
	socket.close();
    }

    public void connect(Drop drop, Listener<Drop> listener) {
	DropSignal dropSignal = nameToDropSignal.get(drop.name());
	if (dropSignal == null) {
	    dropSignal = new DropSignal(drop, new Signal<Drop>());
	    nameToDropSignal.put(drop.name(), dropSignal);
	}
	if (listener != null)
	    dropSignal.signal.connect(listener);
    }

    public void recv() throws IOException {
	readName();
	DropSignal dropSignal = nameToDropSignal.get(name.value());
	if (dropSignal == null) {
	    discardRecvData();
	    return;
	}
	ByteBuffer dropBuffer = allocate(dropSignal.drop.size
		+ DropData.IntSize);
	in.readFully(dropBuffer.array(), 0, dropBuffer.capacity());
	dropSignal.drop.getData(dropBuffer, true);
	dropSignal.signal.fire(dropSignal.drop);
    }
}
