#!/bin/bash

VERSION="$(awk '/VERSION.*".*"/{print $NF}' ~/git/P4/src-gui/version.h | awk -F'"' '$0=$2')"
DIR=p4_linux_"$VERSION"_debug/p4
$(rm -r $DIR)

$(mkdir -p $DIR/bin $DIR/help $DIR/sum_tables)
$(chmod 777 $DIR/sum_tables)
$(ln -s $DIR/sum_tables $DIR/sumtables)

$(cp ~/git/P4/src-mpl/p4*m $DIR/bin/)
$(cp ~/git/P4/src-gui/build-p4-Desktop-Debug/p4 $DIR/bin/)
$(cp ~/git/P4/src-gui/build-lyapunov-Desktop-Debug/lyapunov $DIR/bin/)
$(cp ~/git/P4/src-gui/build-separatrice-Desktop-Debug/separatrice $DIR/bin/)
$(cp ~/git/P4/help/*.* $DIR/help/)
$(cp -r ~/git/P4/help/screenshots/ $DIR/help/)
$(cp ~/git/P4/help/p4_flag.png $DIR/bin/portrait.png)
$(cp ~/git/P4/help/p4smallicon.png $DIR/bin/)
$(cp ~/git/P4/help/newp4icon.png $P4_DIR/bin/)


tar -cvzf p4_linux_v"$VERSION"_debug.tar.gz $DIR
