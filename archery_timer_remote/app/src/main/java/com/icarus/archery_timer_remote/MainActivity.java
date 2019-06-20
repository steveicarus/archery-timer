package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.TextView;

import java.net.Socket;

public class MainActivity extends AppCompatActivity implements GetCommandResponse {
    private final static String DTAG = "MainActivity";

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
        Log.d(DTAG, "onCreate()");
        setContentView(R.layout.activity_main);
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(DTAG, "onStart()");
        ArcheryTimer app = getArcheryTimer();
        TextView end_number = findViewById(R.id.end_number);
        CheckBox practice_check = findViewById(R.id.practice_check);
        end_number.setText(app.get_end_number());
        practice_check.setChecked(app.get_practice_flag());
    }

    @Override
    protected void onStop() {
        super.onStop();
        ArcheryTimer app = getArcheryTimer();
        TextView end_number = findViewById(R.id.end_number);
        CheckBox practice_check = findViewById(R.id.practice_check);
        app.save_end_state(end_number.getText().toString(),
                           practice_check.isChecked() ? true : false);
        Log.d(DTAG, "onStop()");
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
        Socket timer_port = getArcheryTimer().get_socket();
        if (timer_port.isConnected())
            (new CommandResponse(this)).execute(cmd);
    }

    private void send_command(String cmd, String arg1, String arg2) {
        Socket timer_port = getArcheryTimer().get_socket();
        if (timer_port.isConnected())
            (new CommandResponse(this)).execute(cmd, arg1, arg2);
    }
    /*
     * Asynchronous response from the send_command (CommandResponse instance)
     * is handled in this method.
     */
    public void onCommandResponse(String cmd, String ok, String resp_code, String resp_text) {
        if (cmd .equals("version")) {
            set_version_string(resp_text);
            return;
        }

        if (cmd .equals("next-end")) {
            set_next_end_display(resp_code);
            return;
        }
    }

    @Override
    protected void onActivityResult(int req, int res, Intent intent) {
        /* This is called when the connection completes. */
        send_command("version");
    }


    public void next_end_button_click(View view) {
        TextView end_number = findViewById(R.id.end_number);
        CheckBox practice_check = findViewById(R.id.practice_check);
        String use_end = end_number.getText().toString();
        String use_practice = practice_check.isChecked() ? "practice" : "no-practice";
        send_command("next-end", use_end, use_practice);
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

    // This is called by the onCommandResponse() method when the
    // next-end command returns its response.
    public void set_next_end_display(String text) {
        ArcheryTimer app = getArcheryTimer();
        TextView end_number = findViewById(R.id.end_number);
        end_number.setText(text);
        CheckBox practice_check = findViewById(R.id.practice_check);
        app.save_end_state(end_number.getText().toString(),
                           practice_check.isChecked() ? true : false);
    }
}
