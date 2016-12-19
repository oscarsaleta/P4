# Prelude
AC_INIT([P4], [3.3.2], [osr@mat.uab.cat], [], [https://github.com/oscarsaleta/P4])
AC_DEFINE([VERSIONDATE], ["December, 2016"], [Month and year when this version was released])
AC_REVISION([$Revision: 1.0$])
AC_CONFIG_SRCDIR([src-gui/p4/main.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror -Wno-portability])

# Check for C++ compiler
AC_LANG([C++])
AC_PROG_CXX
#AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AX_CXX_COMPILE_STDCXX([11],[ext],[mandatory])
CXXFLAGS="$CXXFLAGS -std=gnu++11"

AC_PROG_MKDIR_P
PKG_PROG_PKG_CONFIG

# Check if pkg-config exists
if test -z "$PKG_CONFIG"; then
    AC_MSG_ERROR([pkg-config is required.])
fi

# Autotroll with Qt
AT_WITH_QT([gui widgets printsupport])

# Retrieve Qt compilation and linker flags
if test `$PKG_CONFIG --exists QtCore && $PKG_CONFIG --exists QtGui \
        && $PKG_CONFIG --exists QtWidgets && $PKG_CONFIG --exists QtPrintSupport`; then
    if ! `$PKG_CONFIG --atleast-version=4.6.0 QtCore`; then
        AC_MSG_ERROR([Qt >= 5.2.1 is required.])
    fi
    CPPFLAGS="`$PKG_CONFIG --cflags-only-I QtCore QtGui QtWidgets QtPrintSupport` $CPPFLAGS"
    LDFLAGS="`$PKG_CONFIG --libs-only-L QtCore QtGui QtWidgets QtPrintSupport` $LDFLAGS"
    LIBS="`$PKG_CONFIG --libs-only-l QtCore QtGui QtWidgets QtPrintSupport` $LIBS"
else 
    if ! `$PKG_CONFIG --atleast-version=5.2.1 Qt5Core`; then
        AC_MSG_ERROR([Qt >= 5.2.1 is required.])
    fi
    CPPFLAGS="`$PKG_CONFIG --cflags-only-I Qt5Core Qt5Gui Qt5Widgets Qt5PrintSupport` $CPPFLAGS"
    LDFLAGS="`$PKG_CONFIG --libs-only-L Qt5Core Qt5Gui Qt5Widgets Qt5PrintSupport` $LDFLAGS"
    LIBS="`$PKG_CONFIG --libs-only-l Qt5Core Qt5Gui Qt5Widgets Qt5PrintSupport` $LIBS"
fi

AC_CHECK_PROGS(MOC, [moc-qt5 moc-qt4 moc])
AC_CHECK_PROGS(UIC, [uic-qt5 uic-qt4 uic])
AC_CHECK_PROGS(RCC, [rcc])
 if test -z "$MOC" || test -z "$UIC" || test -z "$RCC"; then
   AC_MSG_ERROR([Qt utility programs moc, uic, and rcc are required.])
fi



# Check for libraries

# Check for header files
AC_HEADER_STDC

# Check for typedefs, structures, and compiler characteristics

# Check for library functions

# Output files
AC_CONFIG_HEADERS([config.h])
#AC_CONFIG_LINKS([help:help src-mpl:src-mpl])
AC_CHECK_HEADERS([config.h])
AC_CONFIG_FILES([
                 Makefile
                 src-gui/Makefile
                 src-gui/p4/Makefile
                 src-gui/lyapunov/Makefile
                 src-gui/separatrice/Makefile
                 ])
AC_OUTPUT
