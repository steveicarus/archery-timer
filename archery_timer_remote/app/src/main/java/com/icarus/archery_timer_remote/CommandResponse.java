package com.icarus.archery_timer_remote;

import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

/*
 * This class implements a command/response cycle with the server.
 * The client passes in a socket to use for communications, and
 * the MainActivity to be notified when the response returns.
 */
public class CommandResponse extends AsyncTask<String, Integer, String> {
    private final static String DTAG = "CommandResponse";

    private MainActivity src_;
    private Socket port_;
    private String command_;

    CommandResponse(MainActivity src) {
        Log.d(DTAG, "Start CommandResponse.");
        src_ = src;
        port_ = ((ArcheryTimer)src.getApplication()).get_port();
    }

    @Override
    protected String doInBackground(String... args) {
        OutputStream wfd;
        InputStream rfd;

        command_ = args[0];
        String response = "";

        try {
            wfd = port_.getOutputStream();
            rfd = port_.getInputStream();
        } catch (IOException ex) {
            Log.i(DTAG, "Unable to get streams? " + ex.getMessage());
            return response;
        }

        /* Write the command with a terminating newline. */
        Log.d(DTAG, "Write command: " + args[0]);
        try {
            byte[] cmd = args[0].getBytes(StandardCharsets.US_ASCII);
            wfd.write(cmd);
            wfd.write('\n');
            wfd.flush();
        } catch (IOException ex) {
            Log.i(DTAG, "Error writing to remote: " + ex.getMessage());
            return "";
        }

        /* Read the response until we get a newline, or the channel fails. */
        final int BUFSIZE = 100;
        byte[] buf = new byte[BUFSIZE];
        int ptr = 0;
        try {
            int inb = rfd.read();
            while (ptr < BUFSIZE && inb >= 0 && inb != '\n') {
                buf[ptr] = (byte)inb;
                ptr += 1;
                inb = rfd.read();
            }
        } catch (IOException ex) {
            Log.i(DTAG, "Error reading from remote: " + ex.getMessage());
        }

        /* Turn the result bytes into a string, and return to the main thread. */
        response = new String(buf, 0, ptr, StandardCharsets.US_ASCII);
        Log.d(DTAG, "Got response: " + response);
        return response;
    }

    @Override
    protected void onPostExecute(String resp) {
        String resp_parts[] = resp.split(":", 2);
        if (resp_parts.length == 0)
            return;

        String resp_ok = resp_parts[0];
        String resp_code = "";
        if (resp_parts.length >= 2)
            resp_code = resp_parts[1];

        if (command_ == "version") {
            src_.set_version_string(resp_code);
            return;
        }

        if (command_ == "next-end") {
            src_.set_next_end_display(resp_code);
            return;
        }
    }
}
