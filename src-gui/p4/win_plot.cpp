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

#include "win_plot.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "math_separatrice.h"
#include "p4application.h"
#include "plot_tools.h"
#include "win_event.h"
#include "win_gcf.h"
#include "win_curve.h"
#include "win_intparams.h"
#include "win_limitcycles.h"
#include "win_orbits.h"
#include "win_print.h"
#include "win_separatrice.h"
#include "win_zoom.h"

#include <QPrintDialog>
#include <QToolBar>

QPlotWnd::QPlotWnd(QStartDlg *main) : QMainWindow()
{
    QToolBar *toolBar1;
    QToolBar *toolBar2;
    parent_ = main;

    // setAttribute( Qt::WA_PaintOnScreen, true );
    // setAttribute( Qt::WA_PaintOutsidePaintEvent, true );

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    numZooms_ = 0;
    lastZoomIdentifier_ = 0;
    zoomWindows_ = nullptr;

    //    QPalette palette;
    //    palette.setColor(backgroundRole(), QXFIGCOLOR(CBACKGROUND) );
    //    setPalette(palette);

    toolBar1 = new QToolBar("PlotBar1", this);
    toolBar2 = new QToolBar("PlotBar2", this);
    toolBar1->setMovable(false);
    toolBar2->setMovable(false);

    actClose_ = new QAction("Clos&e", this);
    actClose_->setShortcut(Qt::ALT + Qt::Key_E);
    connect(actClose_, SIGNAL(triggered()), this, SLOT(onBtnClose()));
    toolBar1->addAction(actClose_);

    actRefresh_ = new QAction("&Refresh", this);
    actRefresh_->setShortcut(Qt::ALT + Qt::Key_R);
    connect(actRefresh_, SIGNAL(triggered()), this, SLOT(onBtnRefresh()));
    toolBar1->addAction(actRefresh_);

    actLegend_ = new QAction("&Legend", this);
    actLegend_->setShortcut(Qt::ALT + Qt::Key_L);
    connect(actLegend_, SIGNAL(triggered()), this, SLOT(onBtnLegend()));
    toolBar1->addAction(actLegend_);

    actOrbits_ = new QAction("&Orbits", this);
    actOrbits_->setShortcut(Qt::ALT + Qt::Key_O);
    connect(actOrbits_, SIGNAL(triggered()), this, SLOT(onBtnOrbits()));
    toolBar1->addAction(actOrbits_);

    actIntParams_ = new QAction("&Integration Parameters", this);
    actIntParams_->setShortcut(Qt::ALT + Qt::Key_I);
    connect(actIntParams_, SIGNAL(triggered()), this, SLOT(onBtnIntParams()));
    toolBar1->addAction(actIntParams_);

    actGCF_ = new QAction("&GCF", this);
    actGCF_->setShortcut(Qt::ALT + Qt::Key_G);
    connect(actGCF_, SIGNAL(triggered()), this, SLOT(onBtnGCF()));
    toolBar1->addAction(actGCF_);

    actCurve_ = new QAction("&Curve", this);
    actCurve_->setShortcut(Qt::ALT+Qt::Key_C); // NOTE: conflict?
    connect(actCurve_,SIGNAL(triggered()),this,SLOT(onBtnCurve()));
    toolBar1->addAction(actCurve_);

    actPlotSep_ = new QAction("Plot &Separatrice", this);
    actPlotSep_->setShortcut(Qt::ALT + Qt::Key_S);
    connect(actPlotSep_, SIGNAL(triggered()), this, SLOT(onBtnPlotSep()));
    toolBar2->addAction(actPlotSep_);

    actPlotAllSeps_ = new QAction("Plot All Separa&trices", this);
    actPlotAllSeps_->setShortcut(Qt::ALT + Qt::Key_T);
    connect(actPlotAllSeps_, SIGNAL(triggered()), this,
            SLOT(onBtnPlotAllSeps()));
    toolBar2->addAction(actPlotAllSeps_);

    actLimitCycles_ = new QAction("Limit C&ycles", this);
    actLimitCycles_->setShortcut(Qt::ALT + Qt::Key_Y);
    connect(actLimitCycles_, SIGNAL(triggered()), this,
            SLOT(onBtnLimitCycles()));
    toolBar2->addAction(actLimitCycles_);

    actView_ = new QAction("&View", this);
    actView_->setShortcut(Qt::ALT + Qt::Key_V);
    connect(actView_, SIGNAL(triggered()), this, SLOT(onBtnView()));
    toolBar2->addAction(actView_);

    actPrint_ = new QAction("&Print", this);
    actPrint_->setShortcut(Qt::ALT + Qt::Key_P);
    connect(actPrint_, SIGNAL(triggered()), this, SLOT(onBtnPrint()));
    toolBar2->addAction(actPrint_);

    addToolBar(Qt::TopToolBarArea, toolBar1);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, toolBar2);

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
    actPlotAllSeps_->setToolTip("Plots all separatrices of all singular points "
                               "with default integration parameters.\n"
                               "Change integration parameters if the effect is "
                               "too small to be visible.");
    actLimitCycles_->setToolTip("Opens limit cycle window");
    actView_->setToolTip("Opens the \"View parameter\" window");
    actPrint_->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");

    sphere_ = new QWinSphere(this, statusBar(), false, 0, 0, 0, 0);
    legendWindow_ = new QLegendWnd();
    orbitsWindow_ = new QOrbitsDlg(this, sphere_);
    sepWindow_ = new QSepDlg(this, sphere_);
    intParamsWindow_ = new QIntParamsDlg();
    viewParamsWindow_ = new QViewDlg(this);
    lcWindow_ = new QLimitCyclesDlg(this, sphere_);
    gcfWindow_ = new QGcfDlg(this, sphere_);
    curveWindow_ = new QCurveDlg(this,sphere_);
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

QPlotWnd::~QPlotWnd()
{
    if (numZooms_ != 0) {
        for (int i = 0; i < numZooms_; i++) {
            delete zoomWindows_[i];
            zoomWindows_[i] = nullptr;
        }
        delete zoomWindows_;
        zoomWindows_ = nullptr;
        numZooms_ = 0;
    }

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
}

void QPlotWnd::adjustHeight(void)
{
    sphere_->adjustToNewSize();
    resize(width(), height() + sphere_->idealh_ - sphere_->h_);
    sphere_->refresh();
    statusBar()->showMessage("Ready.");
}

void QPlotWnd::signalChanged(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalChanged();
    for (int i = 0; i < numZooms_; i++)
        zoomWindows_[i]->signalChanged();
}

void QPlotWnd::signalEvaluating(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalEvaluating();
    for (int i = 0; i < numZooms_; i++)
        zoomWindows_[i]->signalEvaluating();
}

void QPlotWnd::signalEvaluated(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
    for (int i = 0; i < numZooms_; i++)
        zoomWindows_[i]->signalEvaluated();
}

void QPlotWnd::onBtnClose(void)
{
    QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
    g_p4app->postEvent(parent_, e1);
}

bool QPlotWnd::close(void)
{
    QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
    g_p4app->postEvent(parent_, e1);

    return QMainWindow::close();
}

void QPlotWnd::onBtnRefresh(void)
{
    getDlgData();
    sphere_->refresh();
}

void QPlotWnd::onBtnLegend(void)
{
    if (legendWindow_ == nullptr)
        legendWindow_ = new QLegendWnd();

    legendWindow_->show();
    legendWindow_->raise();
}

void QPlotWnd::onBtnOrbits(void)
{
    orbitsWindow_->show();
    orbitsWindow_->raise();
}

void QPlotWnd::onBtnIntParams(void)
{
    intParamsWindow_->show();
    intParamsWindow_->raise();
}

void QPlotWnd::onBtnView(void)
{
    viewParamsWindow_->show();
    viewParamsWindow_->raise();
}

void QPlotWnd::onBtnGCF(void)
{
    gcfWindow_->show();
    gcfWindow_->raise();
}

void QPlotWnd::onBtnCurve(void)
{
    curveWindow_->show();
    curveWindow_->raise();
}

void QPlotWnd::onBtnPlotSep(void)
{
    getDlgData();
    sepWindow_->show();
    sepWindow_->raise();
}

void QPlotWnd::onBtnPlotAllSeps(void)
{
    getDlgData();
    sphere_->prepareDrawing();
    plot_all_sep(sphere_);
    sphere_->finishDrawing();
}

void QPlotWnd::onBtnLimitCycles(void)
{
    lcWindow_->show();
    lcWindow_->raise();
}

void QPlotWnd::onBtnPrint(void)
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

void QPlotWnd::configure(void)
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

    sphere_->update();
    if (g_VFResults.gcf_ == nullptr) // reconfigure GCF button
        actGCF_->setEnabled(false);
    else
        actGCF_->setEnabled(true);
}

void QPlotWnd::openZoomWindow(double x1, double y1, double x2, double y2)
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

    zoomWindows_ =
        (QZoomWnd **)realloc(zoomWindows_, sizeof(QZoomWnd *) * (numZooms_ + 1));
    zoomWindows_[numZooms_] =
        new QZoomWnd(this, ++lastZoomIdentifier_, x1, y1, x2, y2);
    zoomWindows_[numZooms_]->show();
    zoomWindows_[numZooms_]->raise();
    zoomWindows_[numZooms_]->adjustHeight();
    numZooms_++;
}

void QPlotWnd::closeZoomWindow(int id)
{
    int i;
    for (i = 0; i < numZooms_ - 1; i++) {
        if (zoomWindows_[i]->zoomid == id)
            break;
    }
    if (i == numZooms_)
        return; // error: zoom window not found???

    delete zoomWindows_[i];
    zoomWindows_[i] = nullptr;
    if (i != numZooms_ - 1)
        memmove(zoomWindows_ + i, zoomWindows_ + i + 1,
                sizeof(QZoomWnd *) * (numZooms_ - 1 - i));

    numZooms_--;
    if (numZooms_ == 0) {
        delete zoomWindows_; // free( zoomWindows_ );
        zoomWindows_ = nullptr;
    }
}

void QPlotWnd::customEvent(QEvent *_e)
{
    QP4Event *e;
    double pcoord[3];
    double ucoord[2];
    double ucoord0[2];
    double ucoord1[2];
    struct DOUBLEPOINT *p;
    double x, y, x0, y0, x1, y1;
    int *oet;

    e = (QP4Event *)_e;

    if (e->type() == TYPE_OPENZOOMWINDOW) {
        double *data1;
        data1 = (double *)(e->data());

        openZoomWindow(data1[0], data1[1], data1[2], data1[3]);
        delete data1; // free( data1 );
        data1 = nullptr;
        return;
    }

    if (e->type() == TYPE_CLOSE_ZOOMWINDOW) {
        int *data2;
        data2 = (int *)(e->data());
        closeZoomWindow(*data2);
        delete data2; // free( data2 );
        data2 = nullptr;
        return;
    }

    if (e->type() == TYPE_ORBIT_EVENT) {
        oet = (int *)(e->data());
        orbitsWindow_->orbitEvent(*oet);
        delete oet; // free(oet);
        oet = nullptr;
        return;
    }

    if (e->type() == TYPE_SELECT_ORBIT) {
        p = (struct DOUBLEPOINT *)(e->data());
        x = p->x;
        y = p->y;
        void *win = *((void **)(p + 1));
        delete p; // free( p );
        p = nullptr;

        // mouse clicked in position (x,y)  (world coordinates)

        if (MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) {
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

            orbitsWindow_->show();
            orbitsWindow_->setInitialPoint(ucoord[0], ucoord[1]);
            if (win != nullptr) {
                ((QWidget *)win)->activateWindow();
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
        delete p; // free( p );
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
        delete oet; // free(oet);
        oet = nullptr;
        return;
    }

    QMainWindow::customEvent(e);
}

void QPlotWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        QP4Event *e1 =
            new QP4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
        g_p4app->postEvent(parent_, e1);
    }
}

void QPlotWnd::getDlgData(void)
{
    intParamsWindow_->getDataFromDlg();
    if (viewParamsWindow_->getDataFromDlg()) {
        // true when a big change occured in the view

        g_VFResults.setupCoordinateTransformations();
        configure();
    }
}
