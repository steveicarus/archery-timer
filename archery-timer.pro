TEMPLACE = app
TARGET = archery-timer

DEFINES += QZEROCONF_STATIC

QT += multimedia widgets

VERSION_STRING = $$system(git describe --always --dirty)

QMAKE_SUBSTITUTES = version_tag.h.in

mac {
    APP_Sounds.files = $$PWD/sounds

    APP_Sounds.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += APP_Sounds
}

FORMS = TimerControlBox.ui TimerMain.ui

HEADERS = TimerControlBox.h \
          TimerMain.h \
          TimerNetworkService.h \
	  version_tag.h

SOURCES = main.cpp \
          TimerControlBox.cpp \
          TimerMain.cpp \
          TimerNetworkService.cpp

#target.path = 
#INSTALLS += target

include(QtZeroConf/qtzeroconf.pri)
