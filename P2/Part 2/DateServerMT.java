import java.net.*;
import java.io.*;

public class Worker extends Thread {
    private Socket client;
    Worker (Socket _client) {
        client = _client;
    }

    public void run() {
        PrintWriter pout = new PrintWriter(client.getOutputStream(), true);
        pout.println(new java.util.Date().toString());
        client.close();
    }
}

public class DateServer430 {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Syntax: java DateSerMT <port>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);
        port = 6014;

        try {
            ServerSocket svr = new ServerSocket(port);
            Socket client = null;
            while (true) {
                try {
                    client = svr.accept();
                } catch (SocketTimeoutException e) {
                    System.err.println("Unable to accept incoming socket connection")
                }

                if (client != null) {
                    Worker worker = new Worker(client);
                    worker.start();
                }
            }
        }
        catch (IOException ie) {}
    }
}