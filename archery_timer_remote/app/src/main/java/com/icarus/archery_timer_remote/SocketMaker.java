package com.icarus.archery_timer_remote;

import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;

/*
 * This class handles socket operations with the network. We put these operations
 * in an AsyncTask so that they don't interfere with the operation of the UI.
 * Network operations are, after all, inherently asynchronous.
 */

/*
 * Make a connected socket from the host name and port string.
 */
class SocketMaker extends AsyncTask<String, Integer, Socket> {

    private final static String DTAG = "SocketMaker";

    private ServerConnection src_;

    SocketMaker(ServerConnection src) {
        Log.d(DTAG, "Start maker task.");
        src_ = src;
    }

    /*
     * This method is run in the background, with the host and port
     * names, and tries to connect a socket.
     */
    @Override
    protected Socket doInBackground(String... args) {
        /*
         * This is the socket to return. If this method succeeds,
         * the socket will be connected. Otherwise, it will be
         * left unconnected.
         */
        Socket port = new Socket();

        /* Get from the argument list the host name and port. */
        if (args.length < 2) {
            Log.d(DTAG, "Not enough arguments.");
            return port;
        }

        String use_hostname = args[0];
        int use_port = Integer.parseInt(args[1]);
        Log.d(DTAG, "use_hostname="+use_hostname+", use_port="+use_port);

        /* Turn the hostname and port to a socket address. */
        InetSocketAddress addr;
        try {
            addr = new InetSocketAddress(use_hostname, use_port);
        } catch (SecurityException e) {
            Log.d(DTAG, "SecurityException resolving name: " + e.getMessage());
            return port;
        } catch (IllegalArgumentException e) {
            Log.d(DTAG, "Illegal Argument to InsetSocketAddress.");
            return port;
        }
        if (addr.isUnresolved()) {
            Log.d(DTAG, "Unable to resolve " + use_hostname + ":" + use_port);
            return port;
        } else {
            Log.d(DTAG, "Resolved address to: " + addr.toString());
        }

        /* Make a socket and try to connect it. */
        try {
            port.connect(addr, 5000);
        } catch (SocketTimeoutException ex) {
            Log.d(DTAG, "Timeout connecting to " + addr.toString() + ": " + ex.getMessage());
        } catch (IOException e) {
            Log.d(DTAG, "IOException calling connect:" + e.getMessage());
            return port;
        }

        /* Return the connected socket to the main thread. */
        return port;
    }

    @Override
    protected void onPostExecute(Socket port) {
        Log.d(DTAG, port.isConnected()? "Socket is connected." : "Socket is NOT connected.");
        src_.receive_opened_socket(port);
    }
}
