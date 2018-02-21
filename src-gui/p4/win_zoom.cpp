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

#include "win_zoom.h"

#include "custom.h"
#include "P4InputVF.h"
#include "main.h"
#include "p4application.h"
#include "plot_tools.h"
#include "P4Event.h"
#include "P4StartDlg.h"
#include "P4PlotWnd.h"
#include "P4PrintDlg.h"
#include "P4WinSphere.h"

#include <QDesktopWidget>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

QZoomWnd::QZoomWnd(P4PlotWnd *main, int id, double x1, double y1, double x2,
                   double y2)
    : QMainWindow()
{
    QToolBar *toolBar1;
    parent_ = main;
    zoomid_ = id;
    x1_ = x1;
    x2_ = x2;
    y1_ = y1;
    y2_ = y2;

    //    QPalette palette;
    //    palette.setColor(backgroundRole(), QXFIGCOLOR(bgColours::CBACKGROUND)
    //    );
    //    setPalette(palette);

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    toolBar1 = new QToolBar("ZoomBar1", this);
    toolBar1->setMovable(false);

    actClose_ = new QAction("Close", this);
    actClose_->setShortcut(Qt::ALT + Qt::Key_E);
    connect(actClose_, &QAction::triggered, this, &QZoomWnd::onBtnClose);
    toolBar1->addAction(actClose_);

    actRefresh_ = new QAction("Refresh", this);
    actRefresh_->setShortcut(Qt::ALT + Qt::Key_R);
    connect(actRefresh_, &QAction::triggered, this, &QZoomWnd::onBtnRefresh);
    toolBar1->addAction(actRefresh_);

    actPrint_ = new QAction("Print", this);
    actPrint_->setShortcut(Qt::ALT + Qt::Key_P);
    connect(actPrint_, &QAction::triggered, this, &QZoomWnd::onBtnPrint);
    toolBar1->addAction(actPrint_);

    connect(g_ThisVF, &QInputVF::saveSignal, this, &QZoomWnd::onSaveSignal);

#ifdef TOOLTIPS
    actClose_->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    actRefresh_->setToolTip("Redraw the plot window");
    actPrint_->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");
    addToolBar(Qt::TopToolBarArea, toolBar1);

    sphere_ = new P4WinSphere(this, statusBar(), true, x1_, y1_, x2_, y2_);
    sphere_->show();
    setCentralWidget(sphere_);
    resize(NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW);
    sphere_->setupPlot();

    //  if( g_ThisVF->evaluated_ )
    setP4WindowTitle(this, "Phase Portrait - Zoom");
    //  else
    //      SetP4WindowTitle( this, "Phase Portrait - Zoom (*)" );
}

QZoomWnd::~QZoomWnd()
{
    delete sphere_;
    sphere_ = nullptr;
}

void QZoomWnd::onSaveSignal()
{
    QSettings settings(g_ThisVF->getbarefilename().append(".conf"),
                       QSettings::NativeFormat);
    settings.beginGroup(QString("QZoomWnd").append(zoomid_));
    settings.setValue("id", zoomid_);
    settings.setValue("x1", x1_);
    settings.setValue("y1", y1_);
    settings.setValue("x2", x2_);
    settings.setValue("y2", y2_);

    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void QZoomWnd::signalChanged(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalChanged();
}

void QZoomWnd::signalEvaluating(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalEvaluating();
}

void QZoomWnd::signalEvaluated(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
}

void QZoomWnd::onBtnClose(void)
{
    int *data = new int;
    *data = zoomid_;

    P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
    g_p4app->postEvent(parent_, e1);
}

bool QZoomWnd::close(void)
{
    int *data = new int;
    *data = zoomid_;

    P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
    g_p4app->postEvent(parent_, e1);

    return QMainWindow::close();
}

void QZoomWnd::onBtnRefresh(void)
{
    parent_->getDlgData();
    sphere_->refresh();
}

void QZoomWnd::onBtnPrint(void)
{
    int res;
    double lw, ss;
    P4PrintDlg *pdlg;
    pdlg = new P4PrintDlg(this, 0);
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

void QZoomWnd::configure(void)
{
    statusBar()->showMessage("Ready"); // reset status bar
    plot_l = spherePlotLine; // setup line/plot pointing to routines of the
                             // sphere window
    plot_p = spherePlotPoint;
    sphere_->setupPlot(); // setup sphere window (define pixel transformations)
    sphere_->update();
    // delete print window
    // delete xfig window
}

void QZoomWnd::customEvent(QEvent *_e)
{
    P4Event *e;
    e = (P4Event *)_e;

    if (e->type() == TYPE_OPENZOOMWINDOW || e->type() == TYPE_ORBIT_EVENT ||
        e->type() == TYPE_SELECT_ORBIT || e->type() == TYPE_SEP_EVENT ||
        e->type() == TYPE_SELECT_LCSECTION) {
        P4Event *newe = new P4Event(e->type(), e->data());
        g_p4app->postEvent(parent_, newe);
        return;
    }

    QMainWindow::customEvent(e);
}

void QZoomWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        int *data = new int;
        *data = zoomid_;

        P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
        g_p4app->postEvent(parent_, e1);
    }
}

void QZoomWnd::adjustHeight(void)
{
    int w, h, m;
    double deltaw, deltah;

    sphere_->adjustToNewSize();

    w = width();
    h = height() + sphere_->idealh_ - sphere_->h_;

    m = g_p4app->desktop()->height();
    m -= m / 10; // occuppy at most 90% of the screen's height

    if (h > m) {
        deltah = (double)(h - m);
        deltaw = deltah;
        deltaw *= sphere_->dx_;
        deltaw /= sphere_->dy_;

        h -= (int)(deltah + 0.5);
        w -= (int)(deltaw + 0.5);
    } else if (sphere_->idealh_ < MINHEIGHTPLOTWINDOW) {
        deltah = (double)(MINHEIGHTPLOTWINDOW - sphere_->idealh_);
        deltaw = deltah;
        deltaw *= sphere_->dx_;
        deltaw /= sphere_->dy_;

        h += (int)(deltah + 0.5);
        w += (int)(deltaw + 0.5);
        m = g_p4app->desktop()->width();
        m -= m / 10;
        if (w > m)
            w = m; // occupy at most 90 % of the screen's width
    }
    resize(w, h);
    statusBar()->showMessage("Ready.");
}
