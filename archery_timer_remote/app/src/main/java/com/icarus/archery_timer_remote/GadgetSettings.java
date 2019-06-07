package com.icarus.archery_timer_remote;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import java.net.Socket;

public class GadgetSettings extends AppCompatActivity implements GetCommandResponse {

    public ArcheryTimer getArcheryTimer() {
        return (ArcheryTimer)getApplication();
    }

    public void onCommandResponse(String cmd, String ok, String resp) {

    }

    /* Check if the port is connected, and if so start the command. */
    private void send_command(String cmd) {
        Socket timer_port = getArcheryTimer().get_port();
        if (timer_port.isConnected())
            (new CommandResponse(this)).execute(cmd);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gadget_settings);
    }

    public void toggle_fullscreen_click(View view) {
        send_command("toggle-fullscreen");
    }
}
