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

#ifndef WIN_PLOT_H
#define WIN_PLOT_H

#include <QMainWindow>

#include <memory>

class QCurveDlg;
class QGcfDlg;
class QIsoclinesDlg;
class QIntParamsDlg;
class QLegendWnd;
class QLimitCyclesDlg;
class QOrbitsDlg;
class QSepDlg;
class P4StartDlg;
class QViewDlg;
class QWinSphere;
class QZoomWnd;

class QAction;
class QBoxLayout;
class QEvent;
class QHideEvent;

class QPlotWnd : public QMainWindow
{
    Q_OBJECT

  public:
    QPlotWnd(P4StartDlg *);
    ~QPlotWnd();

    QIntParamsDlg *intParamsWindow_;
    QViewDlg *viewParamsWindow_;

  private:
    P4StartDlg *parent_;
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

    QLegendWnd *legendWindow_;
    QOrbitsDlg *orbitsWindow_;
    QSepDlg *sepWindow_;
    QLimitCyclesDlg *lcWindow_;
    QGcfDlg *gcfWindow_;
    QCurveDlg *curveWindow_;
    QIsoclinesDlg *isoclinesWindow_;

    QWinSphere *sphere_; // main sphere

    int numZooms_;
    int lastZoomIdentifier_;
    std::vector<std::shared_ptr<QZoomWnd>> zoomWindows_;

    bool flagAllSepsPlotted_;

  public slots:
    void signalEvaluating();
    void signalEvaluated();
    void signalChanged();

    void onBtnClose();
    void onBtnRefresh();
    void onBtnLegend();
    void onBtnOrbits();
    void onBtnIntParams();
    void onBtnView();
    void onBtnGCF();
    void onBtnCurve();
    void onBtnIsoclines();
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

#endif /* WIN_PLOT_H */
