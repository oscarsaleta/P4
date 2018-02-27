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

#include "P4ArbitraryCurveDlg.h"
#include "P4Event.h"
#include "P4GcfDlg.h"
#include "P4InputVF.h"
#include "P4IntParamsDlg.h"
#include "P4IsoclinesDlg.h"
#include "P4LegendWnd.h"
#include "P4LimitCyclesDlg.h"
#include "P4OrbitsDlg.h"
#include "P4PrintDlg.h"
#include "P4SepDlg.h"
#include "P4StartDlg.h"
#include "P4WinSphere.h"
#include "custom.h"
#include "file_tab.h"
#include "main.h"
#include "math_separatrice.h"
#include "P4Application.h"
#include "plot_tools.h"
#include "P4ViewDlg.h"
#include "P4ZoomWnd.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

#include <utility>

P4PlotWnd::P4PlotWnd(std::shared<P4StartDlg> main)
    : QMainWindow(), parent_{std::move(main)}
{
    setContextMenuPolicy(Qt::NoContextMenu);

    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

    numZooms_ = 0;
    lastZoomIdentifier_ = 0;
    flagAllSepsPlotted_ = false;

    std::unique_ptr<QToolBar> toolBar1{
        std::make_unique<QToolBar>("PlotBar1", this)};
    toolBar1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    toolBar1->setMovable(false);

    std::unique_ptr<QToolBar> toolBar2{
        std::make_unique<QToolBar>("PlotBar2", this)};
    toolBar2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    toolBar2->setMovable(false);

    actClose_ = std::make_unique<QAction>("Clos&e", this);
    actClose_->setShortcut(Qt::ALT + Qt::Key_E);
    QObject::connect(actClose_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnClose);
    toolBar1->addAction(actClose_);

    actRefresh_ = std::make_unique<QAction>("&Refresh", this);
    actRefresh_->setShortcut(Qt::ALT + Qt::Key_R);
    QObject::connect(actRefresh_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnRefresh);
    toolBar1->addAction(actRefresh_);

    actLegend_ = std::make_unique<QAction>("&Legend", this);
    actLegend_->setShortcut(Qt::ALT + Qt::Key_L);
    QObject::connect(actLegend_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnLegend);
    toolBar1->addAction(actLegend_);

    actOrbits_ = std::make_unique<QAction>("&Orbits", this);
    actOrbits_->setShortcut(Qt::ALT + Qt::Key_O);
    QObject::connect(actOrbits_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnOrbits);
    toolBar1->addAction(actOrbits_);

    actIntParams_ = std::make_unique<QAction>("&Integration Parameters", this);
    actIntParams_->setShortcut(Qt::ALT + Qt::Key_I);
    QObject::connect(actIntParams_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnIntParams);
    toolBar1->addAction(actIntParams_);

    actGCF_ = std::make_unique<QAction>("&GCF", this);
    actGCF_->setShortcut(Qt::ALT + Qt::Key_G);
    QObject::connect(actGCF_, &QAction::triggered, this, &P4PlotWnd::onBtnGCF);
    toolBar1->addAction(actGCF_);

    actCurve_ = std::make_unique<QAction>("&Curves", this);
    // actCurve_->setShortcut(Qt::ALT + Qt::Key_C);
    QObject::connect(actCurve_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnCurve);
    toolBar1->addAction(actCurve_);

    actPlotSep_ = std::make_unique<QAction>("Plot &Separatrice", this);
    actPlotSep_->setShortcut(Qt::ALT + Qt::Key_S);
    QObject::connect(actPlotSep_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnPlotSep);
    toolBar2->addAction(actPlotSep_);

    actPlotAllSeps_ = std::make_unique<QAction>("Plot All Separa&trices", this);
    actPlotAllSeps_->setShortcut(Qt::ALT + Qt::Key_T);
    QObject::connect(actPlotAllSeps_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnPlotAllSeps);
    toolBar2->addAction(actPlotAllSeps_);

    actLimitCycles_ = std::make_unique<QAction>("Limit C&ycles", this);
    actLimitCycles_->setShortcut(Qt::ALT + Qt::Key_Y);
    QObject::connect(actLimitCycles_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnLimitCycles);
    toolBar2->addAction(actLimitCycles_);

    actIsoclines_ = std::make_unique<QAction>("Isoclines", this);
    QObject::connect(actIsoclines_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnIsoclines);
    toolBar2->addAction(actIsoclines_);

    actView_ = std::make_unique<QAction>("&View", this);
    actView_->setShortcut(Qt::ALT + Qt::Key_V);
    QObject::connect(actView_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnView);
    toolBar2->addAction(actView_);

    actPrint_ = std::make_unique<QAction>("&Print", this);
    actPrint_->setShortcut(Qt::ALT + Qt::Key_P);
    QObject::connect(actPrint_, &QAction::triggered, this,
                     &P4PlotWnd::onBtnPrint);
    toolBar2->addAction(actPrint_);

    addToolBar(Qt::TopToolBarArea, toolBar1);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, toolBar2);

    QObject::connect(gThisVF, &QInputVF::saveSignal, this,
                     &P4PlotWnd::onSaveSignal);
    QObject::connect(gThisVF, &QInputVF::loadSignal, this,
                     &P4PlotWnd::onLoadSignal);

#ifdef TOOLTIPS

    actClose_->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    actRefresh_->setToolTip("Redraw the plot window");
    actLegend_->setToolTip("Show legend");
    actOrbits_->setToolTip("Opens \"Integrate Orbits\" window");
    actIntParams_->setToolTip("Opens \"Integration Parameters\" window");
    actGCF_->setToolTip(
        "Opens Greatest-Common-Factor window.\n"
        "Disabled if there is no GCF");
    actCurve_->setToolTip("Opens window for plotting arbitrary curves.");
    actPlotSep_->setToolTip("Opens \"Plot separatrices\" window");
    actPlotAllSeps_->setToolTip(
        "Plots all separatrices of all singular points "
        "with default integration parameters.\n"
        "Change integration parameters if the effect is "
        "too small to be visible.");
    actLimitCycles_->setToolTip("Opens limit cycle window");
    actIsoclines_->setToolTip("Opens window for plotting isoclines");
    actView_->setToolTip("Opens the \"View parameter\" window");
    actPrint_->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");

    sphere_ =
        std::make_shared<P4WinSphere>(this, statusBar(), false, 0, 0, 0, 0);
    legendWindow_ = new P4LegendWnd();
    orbitsWindow_ = new P4OrbitsDlg(this, sphere_);
    sepWindow_ = new P4SepDlg(this, sphere_);
    intParamsWindow_ = new P4IntParamsDlg();
    viewParamsWindow_ = new P4ViewDlg(this);
    lcWindow_ = new P4LimitCyclesDlg(this, sphere_);
    gcfWindow_ = new P4GcfDlg(this, sphere_);
    curveWindow_ = new P4ArbitraryCurveDlg(this, sphere_);
    isoclinesWindow_ = new P4IsoclinesDlg(this, sphere_);
    gLCWindowIsUp = false;  // Limit cycles: initially hidden

    sphere_->show();
    setCentralWidget(sphere_);
    resize(NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW);

    intParamsWindow_->updateDlgData();
    viewParamsWindow_->updateDlgData();

    setP4WindowTitle(this, "Phase Portrait");
}

void P4PlotWnd::onSaveSignal()
{
    QString fname = gThisVF->getbarefilename().append(".conf");
    QSettings settings(fname, QSettings::NativeFormat);
    settings.setValue("P4PlotWnd/size", size());
    settings.setValue("P4PlotWnd/pos", pos());
    settings.setValue("P4PlotWnd/numZooms", numZooms_);
    settings.setValue("P4PlotWnd/allSeps", flagAllSepsPlotted_);
}

void P4PlotWnd::onLoadSignal()
{
    QString fname = gThisVF->getbarefilename().append(".conf");
    QSettings settings(fname, QSettings::NativeFormat);
    resize(settings.value("P4PlotWnd/size").toSize());
    move(settings.value("P4PlotWnd/pos").toPoint());

    numZooms_ = settings.value("P4PlotWnd/numZooms").toInt();
    if (numZooms_ != 0) {
        std::unique_ptr<P4ZoomWnd> thiszoom;
        for (int i = 1; i <= numZooms_; i++) {
            QString zoomName = QString("P4ZoomWnd").append(i);
            settings.beginGroup(zoomName);
            int currentZoomId = settings.value("id").toInt();
            double currentZoomX1 = settings.value("x1").toDouble();
            double currentZoomX2 = settings.value("x2").toDouble();
            double currentZoomY1 = settings.value("y1").toDouble();
            double currentZoomY2 = settings.value("y2").toDouble();
            thiszoom = std::make_unique<P4ZoomWnd>(this, currentZoomId,
                                                  currentZoomX1, currentZoomY1,
                                                  currentZoomX2, currentZoomY2);
            thiszoom->show();
            thiszoom->raise();
            thiszoom->adjustHeight();
            thiszoom->resize(settings.value("size").toSize());
            thiszoom->move(settings.value("pos").toPoint());
            zoomWindows_.push_back(std::move(thiszoom));
            settings.endGroup();
        }
    }

    if (settings.value("P4PlotWnd/allSeps").toBool()) {
        onBtnPlotAllSeps();
    }
}

void P4PlotWnd::adjustHeight()
{
    sphere_->adjustToNewSize();
    resize(width(), height() + sphere_->idealh_ - sphere_->h_);
    sphere_->refresh();
    statusBar()->showMessage("Ready.");
}

void P4PlotWnd::signalChanged()
{
    sphere_->signalChanged();
    std::vector<std::shared_ptr<P4ZoomWnd>>::const_iterator it;
    for (it = std::begin(zoomWindows_); it != std::end(zoomWindows_); it++)
        (*it)->signalChanged();
}

void P4PlotWnd::signalEvaluating()
{
    sphere_->signalEvaluating();
    std::vector<std::shared_ptr<P4ZoomWnd>>::const_iterator it;
    for (it = std::begin(zoomWindows_); it != std::end(zoomWindows_); it++)
        (*it)->signalEvaluating();
}

void P4PlotWnd::signalEvaluated()
{
    configure();
    std::vector<std::shared_ptr<P4ZoomWnd>>::const_iterator it;
    for (it = std::begin(zoomWindows_); it != std::end(zoomWindows_); it++)
        (*it)->signalEvaluated();
}

void P4PlotWnd::onBtnClose()
{
    std::unique_ptr<P4Event> e1{std::make_unique<P4Event>(
        static_cast<QEvent::Type> TYPE_CLOSE_PLOTWINDOW, nullptr)};
    gP4app->postEvent(parent_, e1);
}

bool P4PlotWnd::close()
{
    std::unique_ptr<P4Event> e1{std::make_unique<P4Event>(
        static_cast<QEvent::Type> TYPE_CLOSE_PLOTWINDOW, nullptr)};
    gP4app->postEvent(parent_, e1);

    return QMainWindow::close();
}

void P4PlotWnd::onBtnRefresh()
{
    getDlgData();
    sphere_->refresh();
}

void P4PlotWnd::onBtnLegend()
{
    if (!legendWindow_)
        legendWindow_ = std::make_unique<P4LegendWnd>();
    legendWindow_->show();
    legendWindow_->raise();
}

void P4PlotWnd::onBtnOrbits()
{
    orbitsWindow_->show();
    orbitsWindow_->raise();
}

void P4PlotWnd::onBtnIntParams()
{
    intParamsWindow_->show();
    intParamsWindow_->raise();
}

void P4PlotWnd::onBtnView()
{
    viewParamsWindow_->show();
    viewParamsWindow_->raise();
}

void P4PlotWnd::onBtnGCF()
{
    gcfWindow_->show();
    gcfWindow_->raise();
}

void P4PlotWnd::onBtnCurve()
{
    curveWindow_->show();
    curveWindow_->raise();
}

void P4PlotWnd::onBtnIsoclines()
{
    isoclinesWindow_->show();
    isoclinesWindow_->raise();
}

void P4PlotWnd::onBtnPlotSep()
{
    getDlgData();
    sepWindow_->show();
    sepWindow_->raise();
}

void P4PlotWnd::onBtnPlotAllSeps()
{
    getDlgData();
    sphere_->prepareDrawing();
    plot_all_sep(sphere_);
    sphere_->finishDrawing();
    flagAllSepsPlotted_ = true;
}

void P4PlotWnd::onBtnLimitCycles()
{
    lcWindow_->show();
    lcWindow_->raise();
}

void P4PlotWnd::onBtnPrint()
{
    std::unique_ptr<P4PrintDlg> pdlg{std::make_unique<P4PrintDlg>(this, 0)};
    int result{pdlg->exec()};
    int res{pdlg->getChosenResolution()};
    int lw{pdlg->getChosenLineWidth()};
    int ss{pdlg->getChosenSymbolSize()};
    pdlg.reset();

    if (result != P4PRINT_NONE) {
        if (result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT) {
            gP4printer->setResolution(res);

            QPrintDialog dialog(gP4printer, this);
            if (!dialog.exec())
                return;

            res = gP4printer->resolution();
        }

        if (result < 0)
            sphere_->preparePrinting(-result, true, res, lw, ss);
        else
            sphere_->preparePrinting(result, false, res, lw, ss);
        sphere_->print();
        sphere_->finishPrinting();
    }
}

void P4PlotWnd::configure()
{
    // reset status bar
    statusBar()->showMessage("Ready");
    // setup line/plot pointing to routines of the sphere_ window
    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
    // setup sphere_ window (define pixel transformations)
    sphere_->setupPlot();
    // update data of integration parameters
    intParamsWindow_->updateDlgData();
    viewParamsWindow_->updateDlgData();
    // reset forward/backward buttons to initial state
    orbitsWindow_->reset();
    sepWindow_->reset();
    lcWindow_->reset();
    gcfWindow_->reset();
    curveWindow_->reset();
    isoclinesWindow_->reset();

    sphere_->update();

    actGCF_->setEnabled(false);
    if (!gVFResults.vf_.empty()) {
        for (int i = gThisVF->numVF_ - 1; i >= 0; i--) {
            if (!gVFResults.vf_[i].gcf_.empty()) {
                actGCF_->setEnabled(true);
                break;
            }
        }
    }
}

void P4PlotWnd::openZoomWindow(double x1, double y1, double x2, double y2)
{
    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);

    if (x1 == x2 || y1 == y2)
        return;

    std::unique_ptr<P4ZoomWnd> newZoom{std::make_unique<P4ZoomWnd>(
        this, ++lastZoomIdentifier_, x1, y1, x2, y2)};
    newZoom->show();
    newZoom->raise();
    newZoom->adjustHeight();
    zoomWindows_.push_back(std::move(newZoom));
    numZooms_++;
}

void P4PlotWnd::closeZoomWindow(int id)
{
    std::vector<std::unique_ptr<P4ZoomWnd>>::iterator it;
    for (it = std::begin(zoomWindows_); it != std::end(zoomWindows_); it++) {
        if ((*it)->zoomid_ == id) {
            zoomWindows_.erase(it);
            numZooms_--;
            return;
        }
    }
    return;  // error, zoom window not found
}

// FIXME should think of how to
void P4PlotWnd::customEvent(QEvent *_e)
{
    P4Event e{reinterpret_cast<P4Event>(*_e)};
    double pcoord[3];
    double ucoord[2];
    double ucoord0[2];
    double ucoord1[2];
    double x, y, x0, y0, x1, y1;

    switch (e.type()) {
    case TYPE_OPENZOOMWINDOW:
        // FIXME quin lio
        double *data1{static_cast<double *>(e.data())};
        openZoomWindow(data1[0], data1[1], data1[2], data1[3]);
        delete data1;
        break;
    case TYPE_CLOSE_ZOOMWINDOW:
        int *data2{static_cast<int *>(e.data())};
        closeZoomWindow(data2.get());
        delete data2;
        break;
    case TYPE_ORBIT_EVENT:
        int *oet{static_cast<int *>(e.data())};
        orbitsWindow_->orbitEvent(*oet);
        delete oet;
        break;
    case TYPE_SELECT_ORBIT:
        DOUBLEPOINT *p{static_cast<DOUBLEPOINT *>(e.data())};
        p = (struct DOUBLEPOINT *)(e.data());
        x = p->x;
        y = p->y;
        // mouse clicked in position (x,y)  (world coordinates)
        if (MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) {
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            orbitsWindow_->show();
            orbitsWindow_->setInitialPoint(ucoord[0], ucoord[1]);
            if (orbitsWindow_)
                orbitsWindow_->raise();
        }
        delete p;
        break;
    case TYPE_SELECT_LCSECTION:
        DOUBLEPOINT *p{static_cast<DOUBLEPOINT *>(e.data())};
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
        break;
    case TYPE_SEP_EVENT:
        int *oet{static_cast<int *>(e.data())};
        sepWindow_->sepEvent(*oet);
        delete oet;
        break;
    }
}

void P4PlotWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
        gP4app->postEvent(parent_, e1);
    }
}

void P4PlotWnd::getDlgData()
{
    intParamsWindow_->getDataFromDlg();
    if (viewParamsWindow_->getDataFromDlg()) {
        // true when a big change occured in the view

        gVFResults.setupCoordinateTransformations();
        configure();
    }
}
