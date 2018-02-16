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

#include "P4PlotWnd.h"

#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "main.h"
#include "math_separatrice.h"
#include "p4application.h"
#include "plot_tools.h"
#include "win_curve.h"
#include "P4Event.h"
#include "P4GcfDlg.h"
#include "P4IntParamsDlg.h"
#include "win_isoclines.h"
#include "P4LegendWnd.h"
#include "P4LimitCyclesDlg.h"
#include "P4StartDlg.h"
#include "P4OrbitsDlg.h"
#include "win_print.h"
#include "win_separatrice.h"
#include "win_sphere.h"
#include "win_view.h"
#include "win_zoom.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

P4PlotWnd::P4PlotWnd(P4StartDlg *main) : QMainWindow()
{
    setContextMenuPolicy(Qt::NoContextMenu);

    QToolBar *toolBar1;
    QToolBar *toolBar2;
    parent_ = main;

    // setAttribute( Qt::WA_PaintOnScreen, true );
    // setAttribute( Qt::WA_PaintOutsidePaintEvent, true );

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    numZooms_ = 0;
    lastZoomIdentifier_ = 0;
    flagAllSepsPlotted_ = false;

    //    QPalette palette;
    //    palette.setColor(backgroundRole(), QXFIGCOLOR(bgColours::CBACKGROUND)
    //    );
    //    setPalette(palette);

    toolBar1 = new QToolBar("PlotBar1", this);
    toolBar1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    toolBar2 = new QToolBar("PlotBar2", this);
    toolBar2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    toolBar1->setMovable(false);
    toolBar2->setMovable(false);

    actClose_ = new QAction("Clos&e", this);
    actClose_->setShortcut(Qt::ALT + Qt::Key_E);
    connect(actClose_, &QAction::triggered, this, &P4PlotWnd::onBtnClose);
    toolBar1->addAction(actClose_);

    actRefresh_ = new QAction("&Refresh", this);
    actRefresh_->setShortcut(Qt::ALT + Qt::Key_R);
    connect(actRefresh_, &QAction::triggered, this, &P4PlotWnd::onBtnRefresh);
    toolBar1->addAction(actRefresh_);

    actLegend_ = new QAction("&Legend", this);
    actLegend_->setShortcut(Qt::ALT + Qt::Key_L);
    connect(actLegend_, &QAction::triggered, this, &P4PlotWnd::onBtnLegend);
    toolBar1->addAction(actLegend_);

    actOrbits_ = new QAction("&Orbits", this);
    actOrbits_->setShortcut(Qt::ALT + Qt::Key_O);
    connect(actOrbits_, &QAction::triggered, this, &P4PlotWnd::onBtnOrbits);
    toolBar1->addAction(actOrbits_);

    actIntParams_ = new QAction("&Integration Parameters", this);
    actIntParams_->setShortcut(Qt::ALT + Qt::Key_I);
    connect(actIntParams_, &QAction::triggered, this,
            &P4PlotWnd::onBtnIntParams);
    toolBar1->addAction(actIntParams_);

    actGCF_ = new QAction("&GCF", this);
    actGCF_->setShortcut(Qt::ALT + Qt::Key_G);
    connect(actGCF_, &QAction::triggered, this, &P4PlotWnd::onBtnGCF);
    toolBar1->addAction(actGCF_);

    actCurve_ = new QAction("&Curves", this);
    actCurve_->setShortcut(Qt::ALT + Qt::Key_C);
    connect(actCurve_, &QAction::triggered, this, &P4PlotWnd::onBtnCurve);
    toolBar1->addAction(actCurve_);

    actPlotSep_ = new QAction("Plot &Separatrice", this);
    actPlotSep_->setShortcut(Qt::ALT + Qt::Key_S);
    connect(actPlotSep_, &QAction::triggered, this, &P4PlotWnd::onBtnPlotSep);
    toolBar2->addAction(actPlotSep_);

    actPlotAllSeps_ = new QAction("Plot All Separa&trices", this);
    actPlotAllSeps_->setShortcut(Qt::ALT + Qt::Key_T);
    connect(actPlotAllSeps_, &QAction::triggered, this,
            &P4PlotWnd::onBtnPlotAllSeps);
    toolBar2->addAction(actPlotAllSeps_);

    actLimitCycles_ = new QAction("Limit C&ycles", this);
    actLimitCycles_->setShortcut(Qt::ALT + Qt::Key_Y);
    connect(actLimitCycles_, &QAction::triggered, this,
            &P4PlotWnd::onBtnLimitCycles);
    toolBar2->addAction(actLimitCycles_);

    actIsoclines_ = new QAction("Isoclines", this);
    connect(actIsoclines_, &QAction::triggered, this,
            &P4PlotWnd::onBtnIsoclines);
    toolBar2->addAction(actIsoclines_);

    actView_ = new QAction("&View", this);
    actView_->setShortcut(Qt::ALT + Qt::Key_V);
    connect(actView_, &QAction::triggered, this, &P4PlotWnd::onBtnView);
    toolBar2->addAction(actView_);

    actPrint_ = new QAction("&Print", this);
    actPrint_->setShortcut(Qt::ALT + Qt::Key_P);
    connect(actPrint_, &QAction::triggered, this, &P4PlotWnd::onBtnPrint);
    toolBar2->addAction(actPrint_);

    addToolBar(Qt::TopToolBarArea, toolBar1);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, toolBar2);

    connect(g_ThisVF, &QInputVF::saveSignal, this, &P4PlotWnd::onSaveSignal);
    connect(g_ThisVF, &QInputVF::loadSignal, this, &P4PlotWnd::onLoadSignal);

#ifdef TOOLTIPS

    actClose_->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    actRefresh_->setToolTip("Redraw the plot window");
    actLegend_->setToolTip("Show legend");
    actOrbits_->setToolTip("Opens \"Integrate Orbits\" window");
    actIntParams_->setToolTip("Opens \"Integration Parameters\" window");
    actGCF_->setToolTip("Opens Greatest-Common-Factor window.\n"
                        "Disabled if there is no GCF");
    actCurve_->setToolTip("Opens curve plot window.");
    actPlotSep_->setToolTip("Opens \"Plot separatrices\" window");
    actPlotAllSeps_->setToolTip(
        "Plots all separatrices of all singular points "
        "with default integration parameters.\n"
        "Change integration parameters if the effect is "
        "too small to be visible.");
    actLimitCycles_->setToolTip("Opens limit cycle window");
    actView_->setToolTip("Opens the \"View parameter\" window");
    actPrint_->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");

    sphere_ = new QWinSphere(this, statusBar(), false, 0, 0, 0, 0);
    legendWindow_ = new P4LegendWnd();
    orbitsWindow_ = new P4OrbitsDlg(this, sphere_);
    sepWindow_ = new QSepDlg(this, sphere_);
    intParamsWindow_ = new P4IntParamsDlg();
    viewParamsWindow_ = new P4ViewDlg(this);
    lcWindow_ = new P4LimitCyclesDlg(this, sphere_);
    gcfWindow_ = new P4GcfDlg(this, sphere_);
    curveWindow_ = new QCurveDlg(this, sphere_);
    isoclinesWindow_ = new QIsoclinesDlg(this, sphere_);
    g_LCWindowIsUp = false; // Limit cycles: initially hidden

    sphere_->show();
    setCentralWidget(sphere_);
    resize(NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW);

    intParamsWindow_->updateDlgData();
    viewParamsWindow_->updateDlgData();

    //  if( g_ThisVF->evaluated_ )
    setP4WindowTitle(this, "Phase Portrait");
    //  else
    //      SetP4WindowTitle( this, "Phase Portrait (*)" );
}

P4PlotWnd::~P4PlotWnd()
{
    zoomWindows_.clear();
    numZooms_ = 0;

    delete legendWindow_;
    legendWindow_ = nullptr;
    delete orbitsWindow_;
    orbitsWindow_ = nullptr;
    delete intParamsWindow_;
    intParamsWindow_ = nullptr;
    delete viewParamsWindow_;
    viewParamsWindow_ = nullptr;
    delete sepWindow_;
    sepWindow_ = nullptr;
    delete lcWindow_;
    lcWindow_ = nullptr;
    delete gcfWindow_;
    gcfWindow_ = nullptr;
    g_ThisVF->gcfDlg_ = nullptr;
    delete curveWindow_;
    curveWindow_ = nullptr;
    g_ThisVF->curveDlg_ = nullptr;
    delete isoclinesWindow_;
    isoclinesWindow_ = nullptr;
    g_ThisVF->isoclinesDlg_ = nullptr;
}

void P4PlotWnd::onSaveSignal()
{
    QString fname = g_ThisVF->getbarefilename().append(".conf");
    QSettings settings(fname, QSettings::NativeFormat);
    settings.setValue("P4PlotWnd/size", size());
    settings.setValue("P4PlotWnd/pos", pos());
    settings.setValue("P4PlotWnd/numZooms", numZooms_);
    settings.setValue("P4PlotWnd/allSeps", flagAllSepsPlotted_);
}

void P4PlotWnd::onLoadSignal()
{
    QString fname = g_ThisVF->getbarefilename().append(".conf");
    QSettings settings(fname, QSettings::NativeFormat);
    resize(settings.value("P4PlotWnd/size").toSize());
    move(settings.value("P4PlotWnd/pos").toPoint());

    numZooms_ = settings.value("P4PlotWnd/numZooms").toInt();
    if (numZooms_ != 0) {
        for (int i = 1; i <= numZooms_; i++) {
            QString zoomName = QString("QZoomWnd").append(i);
            settings.beginGroup(zoomName);
            int currentZoomId = settings.value("id").toInt();
            double currentZoomX1 = settings.value("x1").toDouble();
            double currentZoomX2 = settings.value("x2").toDouble();
            double currentZoomY1 = settings.value("y1").toDouble();
            double currentZoomY2 = settings.value("y2").toDouble();
            QZoomWnd *thiszoom =
                new QZoomWnd(this, currentZoomId, currentZoomX1, currentZoomY1,
                             currentZoomX2, currentZoomY2);
            thiszoom->show();
            thiszoom->raise();
            thiszoom->adjustHeight();
            thiszoom->resize(settings.value("size").toSize());
            thiszoom->move(settings.value("pos").toPoint());
            zoomWindows_.push_back(std::shared_ptr<QZoomWnd>(thiszoom));
            settings.endGroup();
        }
    }

    if (settings.value("P4PlotWnd/allSeps").toBool()) {
        onBtnPlotAllSeps();
    }
}

void P4PlotWnd::adjustHeight(void)
{
    sphere_->adjustToNewSize();
    resize(width(), height() + sphere_->idealh_ - sphere_->h_);
    sphere_->refresh();
    statusBar()->showMessage("Ready.");
}

void P4PlotWnd::signalChanged(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalChanged();
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++)
        (*it)->signalChanged();
}

void P4PlotWnd::signalEvaluating(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalEvaluating();
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++)
        (*it)->signalEvaluating();
}

void P4PlotWnd::signalEvaluated(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++)
        (*it)->signalEvaluated();
}

void P4PlotWnd::onBtnClose(void)
{
    P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
    g_p4app->postEvent(parent_, e1);
}

bool P4PlotWnd::close(void)
{
    P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
    g_p4app->postEvent(parent_, e1);

    return QMainWindow::close();
}

void P4PlotWnd::onBtnRefresh(void)
{
    getDlgData();
    sphere_->refresh();
}

void P4PlotWnd::onBtnLegend(void)
{
    if (legendWindow_ == nullptr)
        legendWindow_ = new P4LegendWnd();

    legendWindow_->show();
    legendWindow_->raise();
}

void P4PlotWnd::onBtnOrbits(void)
{
    orbitsWindow_->show();
    orbitsWindow_->raise();
}

void P4PlotWnd::onBtnIntParams(void)
{
    intParamsWindow_->show();
    intParamsWindow_->raise();
}

void P4PlotWnd::onBtnView(void)
{
    viewParamsWindow_->show();
    viewParamsWindow_->raise();
}

void P4PlotWnd::onBtnGCF(void)
{
    gcfWindow_->show();
    gcfWindow_->raise();
}

void P4PlotWnd::onBtnCurve(void)
{
    curveWindow_->show();
    curveWindow_->raise();
}

void P4PlotWnd::onBtnIsoclines(void)
{
    isoclinesWindow_->show();
    isoclinesWindow_->raise();
}

void P4PlotWnd::onBtnPlotSep(void)
{
    getDlgData();
    sepWindow_->show();
    sepWindow_->raise();
}

void P4PlotWnd::onBtnPlotAllSeps(void)
{
    getDlgData();
    sphere_->prepareDrawing();
    plot_all_sep(sphere_);
    sphere_->finishDrawing();
    flagAllSepsPlotted_ = true;
}

void P4PlotWnd::onBtnLimitCycles(void)
{
    lcWindow_->show();
    lcWindow_->raise();
}

void P4PlotWnd::onBtnPrint(void)
{
    int res;
    double lw;
    double ss;

    QPrintDlg *pdlg;
    pdlg = new QPrintDlg(this, 0);
    int result = pdlg->exec();

    res = pdlg->getChosenResolution();
    lw = pdlg->getChosenLineWidth();
    ss = pdlg->getChosenSymbolSize();

    delete pdlg;
    pdlg = nullptr;

    if (result != P4PRINT_NONE) {
        if (result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT) {
            g_p4printer->setResolution(res);

            QPrintDialog dialog(g_p4printer, this);
            if (!dialog.exec())
                return;

            res = g_p4printer->resolution();
        }

        if (result < 0)
            sphere_->preparePrinting(-result, true, res, lw, ss);
        else
            sphere_->preparePrinting(result, false, res, lw, ss);
        sphere_->print();
        sphere_->finishPrinting();
    }
}

void P4PlotWnd::configure(void)
{
    statusBar()->showMessage("Ready"); // reset status bar
    plot_l = spherePlotLine; // setup line/plot pointing to routines of the
                             // sphere_ window
    plot_p = spherePlotPoint;
    sphere_->setupPlot(); // setup sphere_ window (define pixel transformations)
    intParamsWindow_->updateDlgData(); // update data of integration parameters
    viewParamsWindow_->updateDlgData();
    orbitsWindow_->reset(); // reset forward/backward buttons to initial state
    sepWindow_->reset();
    lcWindow_->reset();
    gcfWindow_->reset();
    curveWindow_->reset();
    isoclinesWindow_->reset();

    sphere_->update();
    if (g_VFResults.gcf_ == nullptr) // reconfigure GCF button
        actGCF_->setEnabled(false);
    else
        actGCF_->setEnabled(true);
}

void P4PlotWnd::openZoomWindow(double x1, double y1, double x2, double y2)
{
    double swap;
    if (x1 > x2) {
        swap = x1;
        x1 = x2;
        x2 = swap;
    }
    if (y1 > y2) {
        swap = y1;
        y1 = y2;
        y2 = swap;
    }
    if (x1 == x2 || y1 == y2)
        return;

    QZoomWnd *newZoom =
        new QZoomWnd(this, ++lastZoomIdentifier_, x1, y1, x2, y2);
    newZoom->show();
    newZoom->raise();
    newZoom->adjustHeight();
    zoomWindows_.push_back(std::shared_ptr<QZoomWnd>(newZoom));
    numZooms_++;
}

void P4PlotWnd::closeZoomWindow(int id)
{
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++) {
        if ((*it)->zoomid_ == id) {
            zoomWindows_.erase(it);
            numZooms_--;
            return;
        }
    }
    return; // error, zoom window not found
}

void P4PlotWnd::customEvent(QEvent *_e)
{
    P4Event *e;
    double pcoord[3];
    double ucoord[2];
    double ucoord0[2];
    double ucoord1[2];
    struct DOUBLEPOINT *p;
    double x, y, x0, y0, x1, y1;
    int *oet;

    e = (P4Event *)_e;

    if (e->type() == TYPE_OPENZOOMWINDOW) {
        double *data1;
        data1 = (double *)(e->data());

        openZoomWindow(data1[0], data1[1], data1[2], data1[3]);
        delete data1;
        data1 = nullptr;
        return;
    }

    if (e->type() == TYPE_CLOSE_ZOOMWINDOW) {
        int *data2;
        data2 = (int *)(e->data());
        closeZoomWindow(*data2);
        delete data2;
        data2 = nullptr;
        return;
    }

    if (e->type() == TYPE_ORBIT_EVENT) {
        oet = (int *)(e->data());
        orbitsWindow_->orbitEvent(*oet);
        delete oet;
        oet = nullptr;
        return;
    }

    if (e->type() == TYPE_SELECT_ORBIT) {
        p = (struct DOUBLEPOINT *)(e->data());
        x = p->x;
        y = p->y;
        // void *win = *((void **)(p + 1));
        delete p;
        p = nullptr;

        // mouse clicked in position (x,y)  (world coordinates)

        if (MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) {
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

            orbitsWindow_->show();
            orbitsWindow_->setInitialPoint(ucoord[0], ucoord[1]);
            if (orbitsWindow_ != nullptr) {
                //((QWidget *)win)->activateWindow();
                orbitsWindow_->raise();
            }
        }
        return;
    }

    if (e->type() == TYPE_SELECT_LCSECTION) {
        p = (struct DOUBLEPOINT *)(e->data());
        x0 = p->x;
        y0 = p->y;
        p++;
        x1 = p->x;
        y1 = p->y;
        p--;
        delete p;
        p = nullptr;

        MATHFUNC(viewcoord_to_sphere)(x0, y0, pcoord);
        MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord0);

        MATHFUNC(viewcoord_to_sphere)(x1, y1, pcoord);
        MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord1);

        if (x0 == x1 && y0 == y1) {
            orbitsWindow_->show();
            orbitsWindow_->setInitialPoint(ucoord0[0], ucoord0[1]);
            return;
        }

        // mouse clicked in position (x,y)  (world coordinates)

        lcWindow_->setSection(ucoord0[0], ucoord0[1], ucoord1[0], ucoord1[1]);
        lcWindow_->show();
        lcWindow_->raise();
        return;
    }

    if (e->type() == TYPE_SEP_EVENT) {
        oet = (int *)(e->data());
        sepWindow_->sepEvent(*oet);
        delete oet;
        oet = nullptr;
        return;
    }

    QMainWindow::customEvent(e);
}

void P4PlotWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        P4Event *e1 =
            new P4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
        g_p4app->postEvent(parent_, e1);
    }
}

void P4PlotWnd::getDlgData(void)
{
    intParamsWindow_->getDataFromDlg();
    if (viewParamsWindow_->getDataFromDlg()) {
        // true when a big change occured in the view

        g_VFResults.setupCoordinateTransformations();
        configure();
    }
}
