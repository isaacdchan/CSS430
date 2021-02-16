import java.net.*;
import java.io.*;

public class Worker implements Runnable {
	private Socket client;
	Worker (Socket _client) {
			client = _client;
	}

	public void run() {
		try {
			PrintWriter pout = new PrintWriter(client.getOutputStream(), true);
			pout.println(new java.util.Date().toString());
			client.close();
		} catch (IOException e) {
			System.err.println("Socket error!");
		}
	}
}