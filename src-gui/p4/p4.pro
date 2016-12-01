QT += gui
QT += widgets
QT += printsupport
CONFIG += qt
CONFIG += static
CONFIG += c++14
CONFIG += fwhopr

SOURCES += win_view.cc \
    win_vf.cc \
    win_sphere.cc \
    win_settings.cc \
    win_print.cc \
    win_plot.cc \
    win_params.cc \
    win_orbits.cc \
    win_main.cc \
    win_limitcycles.cc \
    win_legend.cc \
    win_intparams.cc \
    win_gcf.cc \
    win_find.cc \
    win_about.cc \
    print_xfig.cc \
    print_postscript.cc \
    print_points.cc \
    print_bitmap.cc \
    plot_tools.cc \
    plot_points.cc \
    p4settings.cc \
    math_separatrice.cc \
    math_polynom.cc \
    math_p4.cc \
    math_orbits.cc \
    math_numerics.cc \
    math_limitcycles.cc \
    math_intblowup.cc \
    math_gcf.cc \
    math_findpoint.cc \
    math_charts.cc \
    math_changedir.cc \
    main.cc \
    file_vf.cc \
    file_tab.cc \
    file_paths.cc \
    win_zoom.cc \
    p4application.cc \
    win_event.cc \
    win_separatrice.cc \

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
