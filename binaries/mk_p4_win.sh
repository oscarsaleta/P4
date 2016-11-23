#!/bin/bash

if [ ! -d p4_win ]
    then
        $(mkdir p4_win)
fi
if [ ! -d p4_win/bin ]
    then
        $(mkdir p4_win/bin)
fi
if [ ! -d p4_win/help ]
    then
        $(mkdir p4_win/help)
fi
if [ ! -d p4_win/sum_tables ]
    then
        $(mkdir p4_win/sum_tables)
        $(chmod 777 p4_win/sum_tables)
fi

$(cp ../src-mpl/p4*m p4_win/bin/)
$(cp ../src-gui/build-p4-Windows-Release/release/p4.exe p4_win/bin/)
$(cp ../src-gui/build-lyapunov-Windows-Release/release/lyapunov.exe p4_win/bin/)
$(cp ../src-gui/build-separatrice-Windows-Release/separatrice.exe p4_win/bin/)
$(cp ../help/*.* p4_win/help/)
$(cp -r ../help/screenshots/ p4_win/help/)
$(cp ../help/p4_flag.png p4_win/bin/portrait.png)
$(cp ../help/p4smallicon.png p4_win/bin/)
