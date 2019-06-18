package com.icarus.archery_timer_remote;

import android.app.Application;
import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.util.Log;
import android.widget.TextView;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;

/*
 * This is a place to store application instance global variables.
 * This class is called out as an application class, so there should
 * be exactly one persistent instance of this class for the application.
 */
public class ArcheryTimer extends Application implements NsdManager.DiscoveryListener {
    private final static String DTAG = "ArcheryTimer";

    // This is the service type that we search for to look for the Icarus
    // Archery Timer device that we will remote control.
    private final static String SERVICE_TYPE = "_icarus_archery_timer._tcp";

    private NsdManager nsdManager;

    // Receiver class to receive resolve results. This is a hook that
    // implements resolver callbacks and passes the results to the
    // ArcheryTimer application.
    class Resolver implements NsdManager.ResolveListener {
        private ArcheryTimer parent_;

        Resolver(ArcheryTimer parent) {
            parent_ = parent;
        }
        @Override
        public void onServiceResolved(NsdServiceInfo info) {
            parent_.onServiceResolved(info);
        }
        @Override
        public void onResolveFailed(NsdServiceInfo info, int error_code) {
            parent_.onResolveFailed(info, error_code);
        }
    }

    private NsdServiceInfo service_info_;
    private TextView waiting_for_service_info_;

    private String get_discovered_name() {
        if (service_info_ == null)
            return "* Searching... *";
        else
            return service_info_.getServiceName();
    }

    public void show_discovered_name(TextView widget) {
        widget.setText(get_discovered_name());
        waiting_for_service_info_ = widget;
    }

    public void unshow_discovered_name() {
        waiting_for_service_info_ = null;
    }

    public InetAddress get_discovered_addr() {
        if (service_info_ == null)
            return null;
        else
            return service_info_.getHost();
    }

    public int get_discovered_port() {
        if (service_info_ == null)
            return -1;
        else
            return service_info_.getPort();
    }

    // The timer_port_ is the Socket connection to the remote
    // device that we are connecting to.
    private Socket timer_socket_ = new Socket();

    public void set_socket(Socket port) {
        try {
            timer_socket_.close();
        } catch (IOException ex) {
            Log.d(DTAG, "Error closing old port: " + ex.getMessage());
        }
        Log.d(DTAG, "set_port port=" + port.toString());
        timer_socket_ = port;
    }

    public Socket get_socket() {
        return timer_socket_;
    }

    // This method is called when the application is created. Get the members
    // we need for future use, and start discovery running in the background.
    @Override
    public void onCreate() {
        Log.i(DTAG, "Create ArcheryTimer.");
        super.onCreate();
        service_info_ = null;
        nsdManager = (NsdManager)getSystemService(Context.NSD_SERVICE);
        nsdManager.discoverServices(SERVICE_TYPE, NsdManager.PROTOCOL_DNS_SD, this);
    }

    @Override
    public void onDiscoveryStarted(String service_type) {
        Log.d(DTAG, "Discover started for service type " + service_type);
    }

    @Override
    public void onDiscoveryStopped(String service_type) {
        Log.d(DTAG, "Discovery stopped for service type " + service_type);
    }

    @Override
    public void onServiceFound(NsdServiceInfo info) {
        Log.d(DTAG, "Service found.");
        nsdManager.resolveService(info, new Resolver(this));
    }

    @Override
    public void onServiceLost(NsdServiceInfo info) {
        Log.d(DTAG, "Service lost.");
    }

    public void onServiceResolved(NsdServiceInfo info) {
        Log.d(DTAG, "Service resolved.");
        InetAddress addr = info.getHost();
        int port = info.getPort();
        String name = info.getServiceName();
        Log.d(DTAG, "Service " + name + " resolved to " + addr.toString() + ":" + port);
        service_info_ = info;
        if (waiting_for_service_info_ != null)
            waiting_for_service_info_.setText(name);
    }

    public void onResolveFailed(NsdServiceInfo info, int error_code) {
        Log.d(DTAG, "Resolve failed.");
    }

    @Override
    public void onStartDiscoveryFailed(String service_type, int error_code) {
        Log.d(DTAG, "Start Discovery of " + service_type + " failed, error_code=" + error_code);
    }

    @Override
    public void onStopDiscoveryFailed(String service_type, int error_code) {
        Log.d(DTAG, "Stop Discovery of " + service_type + " failed, error_code=" + error_code);
    }
}
