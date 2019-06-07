package com.icarus.archery_timer_remote;

/*
 * This interface is for receiving responses from the CommandResponse class.
 */
public interface GetCommandResponse {
    ArcheryTimer getArcheryTimer();
    void onCommandResponse(String cmd, String ok, String resp);
}
