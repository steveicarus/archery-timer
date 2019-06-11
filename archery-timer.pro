TEMPLACE = app
TARGET = archery-timer

QT += multimedia widgets

VERSION_STRING = $$system(git describe --always --dirty)

QMAKE_SUBSTITUTES = version_tag.h.in

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
