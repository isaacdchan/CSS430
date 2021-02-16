import java.net.*;
import java.io.*;

public class DateServerMT {
	public static void main(String[] args) {
		if (args.length != 1) {
			System.err.println("Syntax: java DateSerMT <port>");
			System.exit(1);
		}

		int port = Integer.parseInt(args[0]);

		try {
			ServerSocket svr = new ServerSocket(port);
			Socket client = null;
			while (true) {
				try {
					client = svr.accept();
					System.out.println("Connection accepted");
				} catch (SocketTimeoutException e) {
					System.err.println("Unable to accept incoming socket connection");
				}

				if (client != null) {
					Worker worker = new Worker(client);
					Thread thread = new Thread(worker);
					thread.start();
				}
			}
		}
		catch (IOException ie) {}
	}
}
