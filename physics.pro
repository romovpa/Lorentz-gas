QT += core gui

TEMPLATE = app
CONFIG -= console

TARGET = physics

HEADERS     = src/model.h \
              src/widget.h \
              src/window.h

SOURCES     = src/model.cpp \
              src/main.cpp \
              src/widget.cpp \
              src/window.cpp

FORMS       = src/window.ui

