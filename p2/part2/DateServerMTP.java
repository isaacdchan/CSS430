import java.net.*;
import java.io.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;


public class DateServerMTP {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Syntax: java DateSerMT <port>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);

        try {
            ServerSocket svr = new ServerSocket(port);
            Socket client = null;

            ExecutorService threadExecutor = Executors.newCachedThreadPool();

            while (true) {
                try {
                    client = svr.accept();
                } catch (SocketTimeoutException e) {
                    System.err.println("Unable to accept incoming socket connection");
                }

                if (client != null) {
                    Worker worker = new Worker(client);
                    threadExecutor.execute(worker);
                }
            }
        }
        catch (IOException ie) {}
    }
}
