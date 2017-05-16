/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WIN_PLOT_H
#define WIN_PLOT_H

#include "win_legend.h"
#include "win_main.h"
#include "win_sphere.h"
#include "win_view.h"

#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QHideEvent>
#include <QMainWindow>

/* Forward-declarations to solve cross-include problems */
class QGcfDlg;         // in win_gcf.h
class QCurveDlg;       // in win_curve.h
class QIntParamsDlg;   // in win_params.h
class QSepDlg;         // in win_separatrices.h
class QOrbitsDlg;      // in win_orbits.h
class QLimitCyclesDlg; // in win_limitcycles.h
class QZoomWnd;        // in win_zoom.h

class QPlotWnd : public QMainWindow
{
    Q_OBJECT

  public:
    QPlotWnd(QStartDlg *);
    ~QPlotWnd();

    QIntParamsDlg *intParamsWindow_;
    QViewDlg *viewParamsWindow_;

  private:
    QStartDlg *parent_;
    QBoxLayout *mainLayout_;
    QAction *actClose_;
    QAction *actRefresh_;
    QAction *actLegend_;
    QAction *actOrbits_;
    QAction *actIntParams_;
    QAction *actView_;
    QAction *actGCF_;
    QAction *actCurve_;
    QAction *actPlotSep_;
    QAction *actPlotAllSeps_;
    QAction *actLimitCycles_;
    QAction *actPrint_;

    QLegendWnd *legendWindow_;
    QOrbitsDlg *orbitsWindow_;
    QSepDlg *sepWindow_;
    QLimitCyclesDlg *lcWindow_;
    QGcfDlg *gcfWindow_;
    QCurveDlg *curveWindow_;

    QWinSphere *sphere_; // main sphere

    int numZooms_;
    int lastZoomIdentifier_;
    QZoomWnd **zoomWindows_;

  public slots:
    void signalEvaluating(void);
    void signalEvaluated(void);
    void signalChanged(void);

    void onBtnClose(void);
    void onBtnRefresh(void);
    void onBtnLegend(void);
    void onBtnOrbits(void);
    void onBtnIntParams(void);
    void onBtnView(void);
    void onBtnGCF(void);
    void onBtnCurve(void);
    void onBtnPlotSep(void);
    void onBtnPlotAllSeps(void);
    void onBtnLimitCycles(void);
    void onBtnPrint(void);
    bool close(void);

    void openZoomWindow(double, double, double, double);
    void closeZoomWindow(int id);
    void configure(void);
    void customEvent(QEvent *e);
    void hideEvent(QHideEvent *h);
    void getDlgData(void);
    void adjustHeight(void);
};

#endif /* WIN_PLOT_H */
