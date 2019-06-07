package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

import java.io.IOException;
import java.net.Socket;

public class ServerConnection extends AppCompatActivity {

    private static final String DTAG = "ServerConnection";

    ArcheryTimer getArcheryTimer() {
        return (ArcheryTimer) getApplication();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server_connection);
    }

    public void try_connect_button_click(View view) {
        /* Get the parameters from the user. */
        EditText host_text = findViewById(R.id.edit_timer_address);
        EditText port_text = findViewById(R.id.edit_timer_port);
        String use_host = host_text.getText().toString();
        String use_port = port_text.getText().toString();

         /* Invoke asynchronous socket maker. */
        Log.d(DTAG, "Try Connect to " + use_host + ":" + use_port);
        (new SocketMaker(this)).execute(use_host, use_port);
    }

    /*
     * Got a socket from the SocketMaker object, so set this as
     * the port for the application.
     */
    public void receive_opened_socket(Socket port) {
        Log.d(DTAG, "Received socket:" + port.toString());
        if (port.isConnected()) {
            getArcheryTimer().set_port(port);
            setResult(RESULT_OK);
            finish();
        }
    }

    /* Disconnect by replacing the port with an unconnected socket. */
    public void disconnect_button_click(View view) {
        Log.d(DTAG, "Disconnect");
        getArcheryTimer().set_port(new Socket());
    }

}
