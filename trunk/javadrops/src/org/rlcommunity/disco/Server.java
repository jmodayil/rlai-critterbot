package org.rlcommunity.disco;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import org.rlcommunity.disco.utils.Signal;

public class Server {
    public Signal<DiscoSocket> onAccept = new Signal<DiscoSocket>();
    private final ServerSocket socket;
    public final int port;
    protected Thread acceptThread = null;

    public Server() throws IOException {
	this(0);
    }

    public Server(int port) throws IOException {
	socket = new ServerSocket(port);
	this.port = socket.getLocalPort();
    }

    public DiscoSocket accept() throws IOException {
	Socket clientSocket = socket.accept();
	return new DiscoSocket(clientSocket);
    }

    public void runAcceptThread() {
	assert acceptThread == null;
	acceptThread = new Thread() {
	    @Override
	    public void run() {
		while (acceptThread != null)
		    try {
			onAccept.fire(accept());
		    } catch (IOException e) {
			e.printStackTrace();
			break;
		    }
		acceptThread = null;
	    }
	};
	acceptThread.start();
    }

    public void close() throws IOException {
	acceptThread = null;
	socket.close();
    }
}
