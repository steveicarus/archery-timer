# archery-timer

Copyright (c) 2019 Stephen Williams <steve@icarus.com>


This program implements timing functions for archery tournaments. It supports
the call to the line, times ends, and supports multiple lines. The program displays
the main window on a screen that the archers of a tournament can see (and hear)
so that it handles line timing.

The aspects of
a shooting cycle are the callup (archers to the line), the "end" (the shooting
time), and "clear" (shooting is done). When there is only one line shooting, the
order of events is:

* Callup (2 whistles) - The archers are called to the line. The callup timer starts counting.

* Line's Hot (1 whistle) - The archers are clear to shoot. They have until the end
timer runs out to shoot the arrows of the end.

* Clear (3 whistles) - The end is done. The program for the end stops until it is
manually started for the next end.

If there are 2 lines shooting, the sequence is a little different:

* Callup line 1 (2 whistles) - Archers from the first shooting line are called up.

* Line's Hot (1 whistle) - The archers of the first line shoot their arrows for the
end.

* Callup line 2 (2 whistles) - Archers from the second shooting line are called up.

* Line's hot (1 whistle) - The archers of the second line shoot their arrows.

* Clear (3 whistles) - The end is done.

The program counts ends, and displays the end number, and also displays the line that
is currently called up or shooting. Note that the A-B and C-D lines alternate which is
shooting first. This follows the process used by World Archery.

The timer can be controlled on screen with a control panel window, or remotely by
a supplied Android app that sends network commands. The idea is to run the program
on a small computer (i.e. Raspberry pi) displaying on a monitor downrange, with a
range manager controlling it behind the line.

## Keyboard Shortcuts

The main timer window responds to these keyboard commands:

* Esc - Toggle fullscreen mode

* Space - Toggle the control box visibility

* f - Fast forward button
* m - Makeup End button
* n - Next End button
* Down (down arrow) - Skip button
* Enter or Return - Start timer
* p - Pause timer
* e - Emergency stop

## Using the Archery Timer Control Panel

The Archery Timer Control panel uses networking to connect to the timer.
The source for the application is part of this distribution, is the
archery_timer_control subdirectory. Start the Archery Timer main application
on one machine on the LAN, and start the Archery Timer Control panel on
another machine on the same LAN. The panel will locate the Archery Timer,
and you can select and connect to the timer. Then the control panel buttons
operate the timer, remotely, with the obvkous button meanings. Only one
Archery Timer Control (or Archery Timer Remote app) can be connected to
the timer at any given time.

## Using the Android App

The Archery Timer Remote app uses WiFi to connect to the timer. The
source for the application is part of this distribution, but the
precompiled app should also be available for download on the Google
Play Store for free. So start by downloading the app onto your device.

The intent is to run the device on a Rasperry-PI, as configured in
README-PI.txt. Starting there, turn on the device. The WiFi network
"Icarus Archery Gadget" should appear as a valid network. Connect to
that network, so that your phone is on the same LAN as the device.

To connect your phone to the device, start the "Archery Timer Remote"
app, and tap the "Connect" button. In the next screen, you should see
instructions for the next step. If all goes well, the first button you
see will say "Icarus Archery Timer". Tap that button. The device will
say "connected" in the upper right of the screen, and you are sent
back to the remote control screen.

If the first button says "searching..." there may be a problem with
finding the device. Try copying the "port" number on the top right
corner of the device screen into the "Port Number" field, then tab
"Try to Connect".

Once you are connected, you can use the app to set up the timer, set
up an end, strart timing, etc.

## Build

The program was originally implemented on Linux using QT5, but should build on
any system that supports QT5, including Linux, MacOSX, and Windows. The instructions
below are linux specific.

First, start with the source directory, which contains the archery-timer.pro file.
This file is the input to the "qmake" program. Run "qmake" to generate the Makefile.
If the archery-timer.pro file is changed, run qmake again to remake the Makefile.

Next, type "make" to build the archery-timer program using the Makefile that was
generated by qmake.

To build the Archery Timer Control application, cd into the "arcchery_timer_control"
subdirectory, and build like any other Qt application: "qmake", then "make".
