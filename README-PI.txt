
Notes for setting up a Raspberry Pi

* Configure the Wi Fi to be an access point

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
