#
# LYAPUNOV PROJECT FILE.  Use qmake to build makefile
#

CONFIG  +=  console c++11
QMAKE_CXXFLAGS += -g -O2
QMAKE_CFLAGS += -g -O2
QMAKE_LFLAGS += -g -O1
SOURCES =  lyapunov.cc lypcoeff.cc polynom.cc \
           checktbl.cc createtbl.cc readvf.cc
HEADERS =  lyapunov.h ../version.h
