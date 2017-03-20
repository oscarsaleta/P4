#
# LYAPUNOV_MPF PROJECT FILE.  Use qmake to build makefile
#

CONFIG  =  console
SOURCES =  lyapunov_mpf.cpp lypcoeff_mpf.cpp polynom_mpf.cpp \
           checktbl_mpf.cpp createtbl_mpf.cpp readvf_mpf.cpp
HEADERS =  lyapunov_mpf.h ../version.h

LIBS += -L$$PWD/../../gmp/lib/ -lgmp

INCLUDEPATH += $$PWD/../../gmp/include
DEPENDPATH += $$PWD/../../gmp/include

LIBS += -L$$PWD/../../mpfr/lib/ -llibmpfr

INCLUDEPATH += $$PWD/../../mpfr/include
DEPENDPATH += $$PWD/../../mpfr/include


