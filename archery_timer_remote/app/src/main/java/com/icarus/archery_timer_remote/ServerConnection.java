package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.net.InetAddress;
import java.net.Socket;

/*
 * This class implements the Server Connection screen. Connect or
 * disconnect from the server based on buttons that the user presses.
 */
public class ServerConnection extends AppCompatActivity {

    private static final String DTAG = "ServerConnection";

    ArcheryTimer getArcheryTimer() {
        return (ArcheryTimer) getApplication();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server_connection);
        TextView button = findViewById(R.id.discovered_server_button);
        button.setText("...");
    }

    protected void onStart() {
        super.onStart();
        TextView button = findViewById(R.id.discovered_server_button);
        getArcheryTimer().show_discovered_name(button);
    }

    protected void onStop() {
        super.onStop();
        getArcheryTimer().unshow_discovered_name();
        TextView button = findViewById(R.id.discovered_server_button);
        button.setText("...");
    }

    public void discovered_server_button_click(View view) {
        ArcheryTimer app = getArcheryTimer();
        InetAddress addr = app.get_discovered_addr();
        if (addr == null) {
            Log.d(DTAG, "discovered_server_button_click: No discovered server.");
            return;
        }
        int port = app.get_discovered_port();

        Log.d(DTAG, "Try Connect to " + addr.toString() + ":" + port);
        (new SocketMaker(this, addr, port)).execute();
    }

    public void try_connect_button_click(View view) {
        /* Get the parameters from the user. */
        EditText host_text = findViewById(R.id.edit_timer_address);
        EditText port_text = findViewById(R.id.edit_timer_port);
        String use_host = host_text.getText().toString();
        int use_port = Integer.parseInt(port_text.getText().toString());

        // Invoke asynchronous socket maker. Use the host name and
        // port number that the user typed in.
        Log.d(DTAG, "Try Connect to " + use_host + ":" + use_port);
        (new SocketMaker(this, use_host, use_port)).execute();
    }

    /*
     * Got a socket from the SocketMaker object, so set this as
     * the port for the application.
     */
    public void receive_opened_socket(Socket port) {
        Log.d(DTAG, "Received socket:" + port.toString());
        if (port.isConnected()) {
            getArcheryTimer().set_socket(port);
            setResult(RESULT_OK);
            finish();
        }
    }

    /* Disconnect by replacing the port with an unconnected socket. */
    public void disconnect_button_click(View view) {
        Log.d(DTAG, "Disconnect");
        getArcheryTimer().set_socket(new Socket());
    }

}
