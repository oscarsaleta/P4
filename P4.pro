TEMPLATE = subdirs
SUBDIRS = src-gui

INSTALL_DIR = $$PROJECT_ROOT/p4

bins.path = $$INSTALL_DIR/bin
bins.files = \
            $$BUILD_DIR/p4/p4 \
            $$BUILD_DIR/lyapunov/lyapunov \
            $$BUILD_DIR/separatrice/separatrice \
            $$PROJECT_ROOT/src-mpl/p4.m \
            $$PROJECT_ROOT/src-mpl/p4gcf.m \
            $$PROJECT_ROOT/help/p4smallicon.png \
            $$PROJECT_ROOT/help/portrait.png

helps.path = $$INSTALL_DIR/help
helps.files = \
            $$PROJECT_ROOT/help/*

sumtables.path = $$INSTALL_DIR
sumtables.extra = \
            (mkdir $$INSTALL_DIR/sumtables); \
            (chmod -R 777 $$INSTALL_DIR/sumtables); \
            (ln -s $$INSTALL_DIR/sumtables $$INSTALL_DIR/sum_tables)

INSTALLS += bins helps sumtables
