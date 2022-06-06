TEMPLATE = app
TARGET = archery-timer-control

DEFINES += QZEROCONF_STATIC

QT += network widgets

VERSION_STRING = $$system(git describe --always --dirty)

QMAKE_SUBSTITUTES = version_tag.h.in

FORMS = ArcheryTimerControl.ui

HEADERS = ArcheryTimerControl.h \
          TimerProtocol.h \
          TimerServiceItem.h \
	  version_tag.h

SOURCES = main.cpp \
          ArcheryTimerControl.cpp \
          TimerProtocol.cpp \
          TimerServiceItem.cpp

#target.path = 
#INSTALLS += target

include(../QtZeroConf/qtzeroconf.pri)
