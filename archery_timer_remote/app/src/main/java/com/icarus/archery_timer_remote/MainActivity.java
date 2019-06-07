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

    /* Check if the port is connected, and if so start the command. */
    private void send_command(String cmd) {
        Socket timer_port = ((ArcheryTimer) this.getApplication()).get_port();
        if (timer_port.isConnected())
            (new CommandResponse(this)).execute(cmd);
    }

    @Override
    protected void onActivityResult(int req, int res, Intent intent) {
        /* This is called when the connection completes. */
        send_command("version");
    }

    public void next_end_button_click(View view) {
        send_command("next-end");
    }

    public void start_timer_button_click(View view) {
        send_command("start-timer");
    }

    public void pause_button_click(View view) {
        send_command("pause-timer");
    }

    public void fast_forward_button_click(View view) {
        send_command("fast-forward");
    }

    public void toggle_fullscreen_command() {
        send_command("toggle-fullscreen");
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
