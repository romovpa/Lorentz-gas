QT += core gui

TEMPLATE = app
CONFIG -= console

TARGET = lorentz

HEADERS = src/model.h \
          src/widget.h \
          src/window.h

SOURCES = src/model.cpp \
          src/main.cpp \
          src/widget.cpp \
          src/window.cpp

FORMS = src/window.ui

TRANSLATIONS = translations/lorentz_ru.ts

RESOURCES += resources.qrc

