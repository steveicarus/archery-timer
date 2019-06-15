package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.net.Socket;

/*
 * This Activity allows the user to edit settings for the Archery Timer.
 * We mostly use the "set" and "query-settings" network commands to make
 * the changes we want to make.
 */
public class GadgetSettings extends AppCompatActivity implements GetCommandResponse {
    final private String DTAG = "GadgetSettings";

    public ArcheryTimer getArcheryTimer() {
        return (ArcheryTimer)getApplication();
    }

    /* The query returns OK:0:<tokens>, where the tokens are all <key>=<value>.
     * Go through the tokens, and process all the values in the response.
     * Ignore any keys that we do not recognise.
     */
    private void process_query_settings(String resp_code, String resp_text) {
        Log.d(DTAG, "process_query_settings: resp=" + resp_code + ":" + resp_text);
        String[] text_parts = resp_text.split(" ");

        for (int idx = 0 ; idx < text_parts.length ; idx += 1) {
            String[] token_parts = text_parts[idx].split("=", 2);
            if (token_parts.length < 2) {
                Log.i(DTAG, "Ignoring bad query response token: " + text_parts[idx]);
                continue;
            }

            if (token_parts[0] .equals("callup-time")) {
                TextView view = findViewById(R.id.callup_time_text);
                view.setText(token_parts[1]);

            } else if (token_parts[0] .equals("end-time")) {
                TextView view = findViewById(R.id.end_time_text);
                view.setText(token_parts[1]);

            } else if (token_parts[0] .equals("warn-time")) {
                TextView view = findViewById(R.id.warn_time_text);
                view.setText(token_parts[1]);

            }
        }
    }

    public void onCommandResponse(String cmd, String ok, String resp_code, String resp_text) {
        if (cmd .equals("toggle-fullscreen")) {
            return;
        }

        if (cmd .equals("query-settings")) {
            process_query_settings(resp_code, resp_text);
            return;
        }
    }

    /* Check if the port is connected, and if so start the command. */
    private void send_command(String... args) {
        Socket timer_port = getArcheryTimer().get_socket();
        if (timer_port.isConnected())
            (new CommandResponse(this)).execute(args);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gadget_settings);
    }

    public void settings_revert_click(View view) {
        send_command("query-settings");
    }

    public void settings_done_click(View button_view) {
        Log.d(DTAG, "settings_done_click: NOT IMPLEMENTED");
        TextView view;

        view = findViewById(R.id.callup_time_text);
        String tmp = view.getText().toString();
        Integer callup_time = Integer.decode(tmp);
        String callup_time_arg = "callup-time=" + callup_time.toString();

        view = findViewById(R.id.end_time_text);
        tmp = view.getText().toString();
        Integer end_time = Integer.decode(tmp);
        String end_time_arg = "end-time=" + end_time.toString();

        view = findViewById(R.id.warn_time_text);
        tmp = view.getText().toString();
        Integer warn_time = Integer.decode(tmp);
        String warn_time_arg = "warn-time=" + warn_time.toString();

        send_command("set", callup_time_arg, end_time_arg, warn_time_arg);
    }

    public void toggle_fullscreen_click(View view) {
        send_command("toggle-fullscreen");
    }
}
