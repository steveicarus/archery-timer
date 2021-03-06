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

    private GetCommandResponse src_;
    private Socket port_;
    private String command_;

    CommandResponse(GetCommandResponse src) {
        Log.d(DTAG, "Start CommandResponse.");
        src_ = src;
        port_ = src_.getArcheryTimer().get_socket();
    }

    @Override
    protected String doInBackground(String... args) {
        OutputStream wfd;
        InputStream rfd;

        /* Save the command name, as that is handed to the response handler. */
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
        String full_command = new String(command_);
        for (int idx = 1 ; idx < args.length ; idx += 1) {
            full_command = full_command + " " + args[idx];
        }

        Log.d(DTAG, "Write command:" + command_ +  ":" + full_command);
        try {
            byte[] cmd = full_command.getBytes(StandardCharsets.US_ASCII);
            wfd.write(cmd);
            wfd.write('\n');
            wfd.flush();
        } catch (IOException ex) {
            Log.i(DTAG, "Error writing to remote: " + ex.getMessage());
            return "";
        }

        /* Read the response until we get a newline, or the channel fails. */
        final int BUFSIZE = 500;
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
        String resp_parts[] = resp.split(":", 3);
        if (resp_parts.length == 0)
            return;

        String resp_ok = resp_parts[0];
        String resp_code = "";
        String resp_text = "";
        if (resp_parts.length >= 2)
            resp_code = resp_parts[1];
        if (resp_parts.length >= 3)
            resp_text = resp_parts[2];

        src_.onCommandResponse(command_, resp_ok, resp_code, resp_text);
    }
}
