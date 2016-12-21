include(P4.pri)

TEMPLATE = subdirs
SUBDIRS = src-gui src-mpl

INSTALL_DIR = $$PROJECT_ROOT/p4

bins.path = $$INSTALL_DIR/bin
bins.files = \
            $$PROJECT_ROOT/build/p4/p4 \
            $$PROJECT_ROOT/build/lyapunov/lyapunov \
            $$PROJECT_ROOT/build/separatrice/separatrice \
            $$PROJECT_ROOT/src-mpl/p4.m \
            $$PROJECT_ROOT/src-mpl/p4gcf.m \
            $$PROJECT_ROOT/help/p4smallicon.png \
            $$PROJECT_ROOT/help/portrait.png
bins.commands = \
            (cp $$BUILD_DIR/p4/p4 $$BUILD_DIR/lyapunov/lyapunov $$BUILD_DIR/separatrice/separatrice $$INSTALL_DIR/bin)

helps.path = $$INSTALL_DIR/help
helps.files = \
            $$PROJECT_ROOT/help/*

sumtables.path = $$INSTALL_DIR
sumtables.extra = \
            (mkdir $$INSTALL_DIR/sumtables); \
            (chmod -R 777 $$INSTALL_DIR/sumtables); \
            (ln -s $$INSTALL_DIR/sumtables $$INSTALL_DIR/sum_tables)

INSTALLS += bins helps sumtables
