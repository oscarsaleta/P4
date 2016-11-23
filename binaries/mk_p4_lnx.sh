#!/bin/bash

DIR=p4_lnx

if [ ! -d $DIR ]
then
    $(mkdir $DIR)
fi
if [ ! -d $DIR/bin ]
then
    $(mkdir $DIR/bin)
fi
if [ ! -d $DIR/help ]
then
    $(mkdir $DIR/help)
fi
if [ ! -d $DIR/sum_tables ]
then
    $(mkdir $DIR/sum_tables)
    $(chmod 777 $DIR/sum_tables)
fi

$(cp ../src-mpl/p4*m $DIR/bin/)
$(cp ../src-gui/build-p4-Desktop-Release/p4 $DIR/bin/)
$(cp ../src-gui/build-lyapunov-Desktop-Release/lyapunov $DIR/bin/)
$(cp ../src-gui/build-separatrice-Desktop-Release/separatrice $DIR/bin/)
$(cp ../help/*.* $DIR/help/)
$(cp -r ../help/screenshots/ $DIR/help/)
$(cp ../help/p4_flag.png $DIR/bin/portrait.png)
$(cp ../help/p4smallicon.png $DIR/bin/)

VERSION="$(awk '/VERSION.*".*"/{print $NF}' ../src-gui/version.h | awk -F'"' '$0=$2')"

tar -cvzf p4_linux_v$VERSION.tar.gz $DIR