/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#pragma once

#include <QMainWindow>

#include <memory>
#include <vector>

#include "P4ZoomWnd.hpp"

class QAction;
class QBoxLayout;
class QEvent;
class QHideEvent;

class P4ArbitraryCurveDlg;
class P4GcfDlg;
class P4IntParamsDlg;
class P4IsoclinesDlg;
class P4LegendWnd;
class P4LimitCyclesDlg;
class P4OrbitsDlg;
class P4SepDlg;
class P4StartDlg;
class P4ViewDlg;
class P4WinSphere;

class P4PlotWnd : public QMainWindow
{
    Q_OBJECT

  public:
    P4PlotWnd(P4StartDlg *);

    P4IntParamsDlg *getIntParamsWindowPtr() const;
    P4ViewDlg *getViewParamsWindowPtr() const;

  private:
    P4StartDlg *parent_;

    P4IntParamsDlg *intParamsWindow_;
    P4ViewDlg *viewParamsWindow_;

    QBoxLayout *mainLayout_;
    QAction *actClose_;
    QAction *actRefresh_;
    QAction *actLegend_;
    QAction *actOrbits_;
    QAction *actIntParams_;
    QAction *actView_;
    QAction *actGCF_;
    QAction *actCurve_;
    QAction *actIsoclines_;
    QAction *actPlotSep_;
    QAction *actPlotAllSeps_;
    QAction *actLimitCycles_;
    QAction *actPrint_;

    P4LegendWnd *legendWindow_;
    P4OrbitsDlg *orbitsWindow_;
    P4SepDlg *sepWindow_;
    P4LimitCyclesDlg *lcWindow_;
    P4GcfDlg *gcfWindow_;
    P4ArbitraryCurveDlg *curveWindow_;
    P4IsoclinesDlg *isoclinesWindow_;

    P4WinSphere *sphere_; // main sphere

    int numZooms_;
    int lastZoomIdentifier_;
    std::vector<std::unique_ptr<P4ZoomWnd>> zoomWindows_;

    bool flagAllSepsPlotted_;

  public slots:
    // void signalEvaluating();
    void signalEvaluated();
    void signalChanged();

    void onBtnClose();
    void onBtnRefresh();
    void onBtnLegend();
    void onBtnOrbits();
    void onBtnIntParams();
    void onBtnView();
    void onBtnGCF();
    void onBtnCurve();     // FIXME
    void onBtnIsoclines(); // FIXME
    void onBtnPlotSep();
    void onBtnPlotAllSeps();
    void onBtnLimitCycles();
    void onBtnPrint();
    bool close();

    void openZoomWindow(double, double, double, double);
    void closeZoomWindow(int id);
    void configure();
    void customEvent(QEvent *e);
    void hideEvent(QHideEvent *h);
    void getDlgData();
    void adjustHeight();

    void onSaveSignal();
    void onLoadSignal();
};
