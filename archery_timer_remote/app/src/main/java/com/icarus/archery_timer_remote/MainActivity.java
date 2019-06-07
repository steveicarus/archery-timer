package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.net.Socket;

public class MainActivity extends AppCompatActivity implements GetCommandResponse {
    // private final static String DTAG = "MainActivity";

    /*
     * Return the ArcheryTimer application instance for this object.
     * This is needed by the GetCommandResponse interface.
     */
    public ArcheryTimer getArcheryTimer() {
        return (ArcheryTimer)getApplication();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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
        Socket timer_port = getArcheryTimer().get_port();
        if (timer_port.isConnected())
            (new CommandResponse(this)).execute(cmd);
    }

    /*
     * Asynchronous response from the send_command (CommandResponse instance)
     * is handled in this method.
     */
    public void onCommandResponse(String cmd, String ok, String resp) {
        if (cmd .equals("version")) {
            set_version_string(resp);
            return;
        }

        if (cmd .equals("next-end")) {
            set_next_end_display(resp);
            return;
        }
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

    public void set_version_string(String text) {
        TextView view = findViewById(R.id.server_version_string);
        view.setText(text);
    }

    public void set_next_end_display(String text) {
        TextView view = findViewById(R.id.end_number);
        view.setText(text);
    }
}
