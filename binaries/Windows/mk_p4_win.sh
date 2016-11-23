#!/bin/bash

VERSION="$(awk '/VERSION.*".*"/{print $NF}' ~/git/P4/src-gui/version.h | awk -F'"' '$0=$2')"
DIR=p4_windows_$VERSION/p4
$(rm -r $DIR)

$(mkdir -p $DIR/bin $DIR/help $DIR/sum_tables)
$(chmod 777 $DIR/sum_tables)
$(ln -s $DIR/sum_tables $DIR/sumtables)

$(cp ~/git/P4/src-mpl/p4*m $DIR/bin/)
$(cp ~/git/P4/src-gui/build-p4-Windows-Release/release/p4.exe $DIR/bin/)
$(cp ~/git/P4/src-gui/build-lyapunov-Windows-Release/release/lyapunov.exe $DIR/bin/)
$(cp ~/git/P4/src-gui/build-separatrice-Windows-Release/release/separatrice.exe $DIR/bin/)
$(cp ~/git/P4/help/*.* $DIR/help/)
$(cp -r ~/git/P4/help/screenshots/ $DIR/help/)
$(cp ~/git/P4/help/p4_flag.png $DIR/bin/portrait.png)
$(cp ~/git/P4/help/p4smallicon.png $DIR/bin/)
$(cp ~/git/P4/help/newp4icon.png $P4_DIR/bin/)

$(iscc installer_script.iss)