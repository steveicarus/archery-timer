package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.IOException;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {

    private final static String DTAG = "MainActivity";

    /* This is the connection to the timer device. */
    private static Socket timer_port = new Socket();

    /* Receive a port, and set it up for application use. */
    public static void set_port(Socket port) {
        try {
            timer_port.close();
        } catch (IOException ex) {
            Log.d(DTAG, "Error closing old port: " + ex.getMessage());
        }
        timer_port = port;

    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    /** Called when the user taps the Connect button */
    public void connect_button_click(View view) {
        Intent intent = new Intent(this, ServerConnection.class);
        startActivity(intent);
    }

    public void next_end_button_click(View view) {
    }

    public void start_timer_button_click(View view) {

    }

    public void pause_button_click(View view) {

    }

    public void fast_forward_button_click(View view) {

    }

}
