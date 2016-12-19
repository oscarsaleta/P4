TEMPLATE = subdirs
SUBDIRS = src-gui

bins.path = p4/bin
bins.files = \
            src-gui/p4/p4 \
            src-gui/lyapunov/lyapunov \
            src-gui/separatrice/separatrice \
            src-mpl/p4.m \
            src-mpl/p4gcf.m \
            help/p4smallicon.png \
            help/portrait.png
bins.extra =

helps.path = p4/help
helps.files = \
            help
helps.extra =

sumtables.path = p4/sumtables
sumtables.files =
sumtables.extra = \
            (chmod -R 777 p4/sumtables); \
            (ln -s p4/sum_tables p4/sumtables)

INSTALLS += bins helps sumtables
