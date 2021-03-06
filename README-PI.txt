
Notes for setting up a Raspberry Pi

* Configure the WiFi to be a station (preferred)

This is the default configuration for Raspbian. You presumably have installed
the desktop version, so use the default settings to connect to your WiFi
network.

* Configure the Wi Fi to be an access point (optional)

NOTE: This option can be used if you are setting the module up to work
in an environment that does not otherwise have WiFi. In this case, set
the module up to be the Access Point.
The instructions to do this are here:
https://www.raspberrypi.org/documentation/configuration/wireless/access-point.md

Go ahead and set up NAT through the ethernet, so that the device can
still get on the internet through a wired link. Follow the
instructions to add NAT (masquerading) through the device. This allows debug
access, and software updates.

Do NOT set the device up as a bridge. Make it an access point so that
we can setup up (in dhcpcd.conf) the interface with the fixed ip
address 192.168.4.1/24. This makes it easier to guess the IP address
of the device.

Set up the ssid in the /etc/hostapd/hostapd.conf file to be
"Icarus Archery Gadget" to be consistent with any other documentation
WRT this tool. This ssid is not a requirement anywhere, but there is
no reason to be unique.

Set up the wpa_passphrase in /etc/hostapd/hostapd.conf to be a
reasonable password, especially if you are connecting the gadget to
your wired LAN (and in turn on to the internet). This gadget can act
as a router into your LAN. If you don't want that during shoots, then
just pull out the ethernet cable to disconnect from the LAN.

* Install pulseaudio

This is necessary to support sound through QT, and it is apparently
not installed by default.

* Install xscreensaver

This is needed to provide a way to control the screen
saver. Paradoxically, we need to install the screen saver so that we
can turn screen saving off. After installing xscreensaver, go into the
root Preferences menu to disable the screen saver manually.

* Install qt5 devel tools: qtbase5-devel

The archery-timer program is a qt5 program.

* Install avahi client devel files: libavahi-common-dev, libavahi-core-dev, libavahi-client-dev

This is necessary to support ZeroConf on the PI when building the
archery-timer program. There might be missing dependencies that these
packages have as well. For testing and debugging, you may want to also
install avahi-utils.

* Installing the archery-timer from source

Unpack the archery-timer tarball into the /home/pi directory. Then
build the application like so:

  $ cd /home/pi/archery-timer
  $ qmake
  $ make

That should create the archery-timer executable. Set it up so that the
program is started when the pi user logs in, by copying the .desktop
file in place:

  $ cd /home/pi/.config
  $ mkdir autostart
  $ cd autostart
  $ ln -s /home/pi/archery-timer/archery-timer.desktop

Now, whenever the pi user is logged in to the desktop, the archery
timer program is started. It is a good idea to set up the pi so that
the pi user is auto-logged in graphically. Thus, the pi becomes a
device you just turn on.
