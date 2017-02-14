include(../../P4.pri)
#
# SEPARATRICE PROJECT FILE.  Use qmake to build makefile
#
DESTDIR = $$BUILD_DIR/separatrice/

CONFIG  = console c++11
HEADERS = ../version.h
SOURCES = separatrice.cpp
