# Prelude
AC_INIT([P4], [3.3.2], [osr@mat.uab.cat], [], [https://github.com/oscarsaleta/P4])
AC_DEFINE([VERSIONDATE], ["November, 2016"], [Month and year when this version was released])
AC_REVISION([$Revision: 1.0$])
AC_CONFIG_SRCDIR([src-gui/p4/main.cc])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror -Wno-portability])

# Check for C++ compiler
AC_LANG([C++])
AC_PROG_CXX

PKG_PROG_PKG_CONFIG

# Autotroll with Qt
AT_WITH_QT([gui widgets printsupport])

# Retrieve Qt compilation and linker flags
CPPFLAGS="`$PKG_CONFIG --cflags-only-I QtCore QtGui QtWidgets QtPrintSupport` $CPPFLAGS"
LDFLAGS="`$PKG_CONFIG --libs-only-L QtCore QtGui QtWidgets QtPrintSupport` $LDFLAGS"
LIBS="`$PKG_CONFIG --libs-only-l QtCore QtGui QtWidgets QtPrintSupport` $LIBS"

if ! `$PKG_CONFIG --atleast-version=4.6.0 QtCore`; then
   AC_MSG_ERROR([Qt >= 4.6.0 is required.])
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
AC_CONFIG_FILES([
                 Makefile
                 src-gui/Makefile
                 src-gui/p4/Makefile
                 src-gui/lyapunov/Makefile
                 src-gui/separatrice/Makefile
                 ])
AC_OUTPUT
