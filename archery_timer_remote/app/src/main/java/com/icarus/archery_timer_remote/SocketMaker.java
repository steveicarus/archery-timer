package com.icarus.archery_timer_remote;

import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.net.InetAddress;
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
    private InetAddress addr_;
    private String hostname_;
    private int port_;

    SocketMaker(ServerConnection src, String host, int port) {
        Log.d(DTAG, "Start maker task.");
        src_ = src;
        addr_ = null;
        hostname_ = host;
        port_ = port;
    }

    SocketMaker(ServerConnection src, InetAddress addr, int port) {
        Log.d(DTAG, "Start maker task with InetAddr.");
        src_ = src;
        addr_ = addr;
        hostname_ = null;
        port_ = port;
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
        InetSocketAddress addr;

        if (hostname_ != null) {
            String use_hostname = hostname_;
            Log.d(DTAG, "use_hostname=" + use_hostname + ", use_port=" + port_);

            /* Turn the hostname and port to a socket address. */
            try {
                addr = new InetSocketAddress(use_hostname, port_);
            } catch (SecurityException e) {
                Log.d(DTAG, "SecurityException resolving name: " + e.getMessage());
                return port;
            } catch (IllegalArgumentException e) {
                Log.d(DTAG, "Illegal Argument to InsetSocketAddress.");
                return port;
            }
            if (addr.isUnresolved()) {
                Log.d(DTAG, "Unable to resolve " + use_hostname + ":" + port_);
                return port;
            } else {
                Log.d(DTAG, "Resolved address to: " + addr.toString());
            }
        } else {
            addr = new InetSocketAddress(addr_, port_);
        }

        Log.d(DTAG, "InetSocketAddress = " + addr.toString());

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
