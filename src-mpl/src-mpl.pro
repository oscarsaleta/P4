TEMPLATE = subdirs
SUBDIRS = mplstrip

SOURCES = blowup.tex    \
        gcd.tex         \
        infinity.tex    \
        main.tex        \
        num_coeff.tex   \
        p4gcf.tex       \
        realroot.tex    \
        tools.tex       \
        type.tex        \
        weakfocus.tex   \
        writelog.tex

p4m.target = p4.m
p4m.depends = $$SOURCES mplstrip/mplstrip
p4m.commands = \
    (make -f MakeTexMaple);

QMAKE_EXTRA_TARGETS += p4m
