TEMPLACE = app
TARGET = archery-timer

QT += multimedia widgets

FORMS = TimerControlBox.ui TimerMain.ui

HEADERS = TimerControlBox.h \
          TimerMain.h \
          TimerNetworkService.h

SOURCES = main.cpp \
          TimerControlBox.cpp \
          TimerMain.cpp \
          TimerNetworkService.cpp

#target.path = 
#INSTALLS += target
