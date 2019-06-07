package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.IOException;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {

    private final static String DTAG = "MainActivity";

    /* Pointer to the main activity that other activities can use. */
    public static MainActivity global_activity;

    /* This is the connection to the timer device. */
    private static Socket timer_port = new Socket();

    /* Receive a port, and set it up for application use. */
    public static void set_port(Socket port) {
        try {
            timer_port.close();
        } catch (IOException ex) {
            Log.d(DTAG, "Error closing old port: " + ex.getMessage());
        }
        Log.d(DTAG, "set_port port=" + port.toString());
        timer_port = port;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        global_activity = this;
    }

    /** Called when the user taps the Connect button */
    public void connect_button_click(View view) {
        Intent intent = new Intent(this, ServerConnection.class);
        startActivityForResult(intent, 1);
    }

    /** Called with the user taps the Settings button */
    public void settings_button_click(View view) {
        Intent intent = new Intent(this, GadgetSettings.class);
        startActivity(intent);
    }

    @Override
    protected void onActivityResult(int req, int res, Intent intent) {
        /* This is called when the connection completes. */
        if (timer_port.isConnected())
            (new CommandResponse(this, timer_port)).execute("version");
    }

    public void next_end_button_click(View view) {
        if (timer_port.isConnected())
            (new CommandResponse(this, timer_port)).execute("next-end");
    }

    public void start_timer_button_click(View view) {
        if (timer_port.isConnected())
            (new CommandResponse(this, timer_port)).execute("start-timer");
    }

    public void pause_button_click(View view) {
        if (timer_port.isConnected())
            (new CommandResponse(this, timer_port)).execute("pause-timer");
    }

    public void fast_forward_button_click(View view) {
        if (timer_port.isConnected())
            (new CommandResponse(this, timer_port)).execute("fast-forward");
    }

    public void toggle_fullscreen_command() {
        (new CommandResponse(this, timer_port)).execute("toggle-fullscreen");
    }

    public void set_version_string(String text) {
        TextView view = findViewById(R.id.server_version_string);
        view.setText(text);
    }

    public void set_next_end_display(String text) {
        TextView view = findViewById(R.id.end_number);
        view.setText(text);
    }
}
