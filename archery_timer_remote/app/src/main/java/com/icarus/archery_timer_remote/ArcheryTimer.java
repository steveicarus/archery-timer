package com.icarus.archery_timer_remote;

import android.app.Application;
import android.util.Log;

import java.io.IOException;
import java.net.Socket;

/*
 * This is a place to store application instance global variables.
 * This class is called out as an application class, so there should
 * be exactly one persistent instance of this class for the application.
 */
public class ArcheryTimer extends Application {
    private final static String DTAG = "ArcheryTimer";

    private Socket timer_port_ = new Socket();

    public void set_port(Socket port) {
        try {
            timer_port_.close();
        } catch (IOException ex) {
            Log.d(DTAG, "Error closing old port: " + ex.getMessage());
        }
        Log.d(DTAG, "set_port port=" + port.toString());
        timer_port_ = port;
    }

    public Socket get_port() {
        return timer_port_;
    }
}
