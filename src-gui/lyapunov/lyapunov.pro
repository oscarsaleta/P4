#
# LYAPUNOV PROJECT FILE.  Use qmake to build makefile
#

CONFIG  +=  console c++11
QMAKE_CXXFLAGS += -g -O2
QMAKE_CFLAGS += -g -O2
QMAKE_LFLAGS += -g -O1
SOURCES =  lyapunov.cpp lypcoeff.cpp polynom.cpp \
           checktbl.cpp createtbl.cpp readvf.cpp
HEADERS =  lyapunov.h ../version.h
