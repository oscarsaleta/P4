QT += gui
QT += widgets
QT += printsupport
CONFIG += qt
CONFIG += static
CONFIG += c++14
CONFIG += fwhopr

SOURCES += win_view.cpp \
    win_vf.cpp \
    win_sphere.cpp \
    win_settings.cpp \
    win_separatrices.cpp \
    win_print.cpp \
    win_plot.cpp \
    win_params.cpp \
    win_orbits.cpp \
    win_main.cpp \
    win_limitcycles.cpp \
    win_legend.cpp \
    win_intparams.cpp \
    win_gcf.cpp \
    win_find.cpp \
    win_about.cpp \
    print_xfig.cpp \
    print_postscript.cpp \
    print_points.cpp \
    print_bitmap.cpp \
    plot_tools.cpp \
    plot_points.cpp \
    p4settings.cpp \
    math_separatrice.cpp \
    math_polynom.cpp \
    math_p4.cpp \
    math_orbits.cpp \
    math_numerics.cpp \
    math_limitcycles.cpp \
    math_intblowup.cpp \
    math_gcf.cpp \
    math_findpoint.cpp \
    math_charts.cpp \
    math_changedir.cpp \
    main.cpp \
    file_vf.cpp \
    file_tab.cpp \
    file_paths.cpp \
    win_zoom.cpp \
    p4application.cpp \
    win_event.cpp
HEADERS += win_view.h \
    win_vf.h \
    win_sphere.h \
    win_settings.h \
    win_print.h \
    win_plot.h \
    win_params.h \
    win_orbits.h \
    win_main.h \
    win_limitcycles.h \
    win_legend.h \
    win_intparams.h \
    win_gcf.h \
    win_find.h \
    win_event.h \
    win_about.h \
    p4application.h \
    math_p4.h \
    file_vf.h \
    custom.h \
    color.h \
    win_zoom.h \
    ../version.h \
    file_paths.h \
    p4settings.h \
    main.h \
    file_tab.h \
    math_changedir.h \
    math_charts.h \
    math_separatrice.h \
    win_separatrice.h \
    math_findpoint.h \
    math_gcf.h \
    math_intblowup.h \
    plot_tools.h \
    math_polynom.h \
    math_orbits.h \
    math_limitcycles.h \
    math_numerics.h \
    plot_points.h \
    print_points.h \
    print_bitmap.h \
    print_postscript.h \
    print_xfig.h
RC_FILE = p4.rc
