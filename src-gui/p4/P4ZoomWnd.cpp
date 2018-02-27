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

#include "P4ZoomWnd.h"

#include "P4Application.h"
#include "P4Event.h"
#include "P4InputVF.h"
#include "P4PlotWnd.h"
#include "P4PrintDlg.h"
#include "P4StartDlg.h"
#include "P4WinSphere.h"
#include "custom.hpp"
#include "main.hpp"
#include "plot_tools.h"

#include <QDesktopWidget>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

P4ZoomWnd::P4ZoomWnd(P4PlotWnd *main, int id, double x1, double y1, double x2,
                     double y2)
    : QMainWindow(), parent_{main}, zoomid_{id}, x1_{x1},x2_{x2},y1_{y1},y2_{y2}
{
    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

    std::unique_ptr<QToolBar> toolBar1{
        std::make_unique<QToolBar>("ZoomBar1", this)};
    toolBar1->setMovable(false);

    actClose_ = std::make_unique<QAction>("Close", this);
    actClose_->setShortcut(Qt::ALT + Qt::Key_E);
    QObject::connect(actClose_.get(), &QAction::triggered, this,
                     &P4ZoomWnd::onBtnClose);
    toolBar1->addAction(actClose_.get());

    actRefresh_ = std::make_unique<QAction>("Refresh", this);
    actRefresh_->setShortcut(Qt::ALT + Qt::Key_R);
    QObject::connect(actRefresh_.get(), &QAction::triggered, this,
                     &P4ZoomWnd::onBtnRefresh);
    toolBar1->addAction(actRefresh_.get());

    actPrint_ = std::make_unique<QAction>("Print", this);
    actPrint_->setShortcut(Qt::ALT + Qt::Key_P);
    QObject::connect(actPrint_.get(), &QAction::triggered, this,
                     &P4ZoomWnd::onBtnPrint);
    toolBar1->addAction(actPrint_.get());

    QObject::connect(gThisVF.get(), &QInputVF::saveSignal, this,
                     &P4ZoomWnd::onSaveSignal);

#ifdef TOOLTIPS
    actClose_->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    actRefresh_->setToolTip("Redraw the plot window");
    actPrint_->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");
    addToolBar(Qt::TopToolBarArea, toolBar1);

    sphere_ = std::make_unique<P4WinSphere>(this, statusBar(), true, x1_, y1_, x2_, y2_);
    sphere_->show();
    setCentralWidget(sphere_);
    resize(NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW);
    sphere_->setupPlot();

    //  if( gThisVF->evaluated_ )
    setP4WindowTitle(this, "Phase Portrait - Zoom");
    //  else
    //      SetP4WindowTitle( this, "Phase Portrait - Zoom (*)" );
}

P4ZoomWnd::~P4ZoomWnd()
{
    delete sphere_;
    sphere_ = nullptr;
}

void P4ZoomWnd::onSaveSignal()
{
    QSettings settings(gThisVF->getbarefilename().append(".conf"),
                       QSettings::NativeFormat);
    settings.beginGroup(QString("P4ZoomWnd").append(zoomid_));
    settings.setValue("id", zoomid_);
    settings.setValue("x1", x1_);
    settings.setValue("y1", y1_);
    settings.setValue("x2", x2_);
    settings.setValue("y2", y2_);

    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void P4ZoomWnd::signalChanged()
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalChanged();
}

void P4ZoomWnd::signalEvaluating()
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalEvaluating();
}

void P4ZoomWnd::signalEvaluated()
{
    //  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
}

void P4ZoomWnd::onBtnClose()
{
    int *data = new int;
    *data = zoomid_;

    P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
    gP4app->postEvent(parent_, e1);
}

bool P4ZoomWnd::close()
{
    int *data = new int;
    *data = zoomid_;

    P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
    gP4app->postEvent(parent_, e1);

    return QMainWindow::close();
}

void P4ZoomWnd::onBtnRefresh()
{
    parent_->getDlgData();
    sphere_->refresh();
}

void P4ZoomWnd::onBtnPrint()
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

void P4ZoomWnd::configure()
{
    statusBar()->showMessage("Ready");  // reset status bar
    plot_l = spherePlotLine;  // setup line/plot pointing to routines of the
                              // sphere window
    plot_p = spherePlotPoint;
    sphere_->setupPlot();  // setup sphere window (define pixel transformations)
    sphere_->update();
    // delete print window
    // delete xfig window
}

void P4ZoomWnd::customEvent(QEvent *_e)
{
    P4Event *e;
    e = (P4Event *)_e;

    if (e->type() == TYPE_OPENZOOMWINDOW || e->type() == TYPE_ORBIT_EVENT ||
        e->type() == TYPE_SELECT_ORBIT || e->type() == TYPE_SEP_EVENT ||
        e->type() == TYPE_SELECT_LCSECTION) {
        P4Event *newe = new P4Event(e->type(), e->data());
        gP4app->postEvent(parent_, newe);
        return;
    }

    QMainWindow::customEvent(e);
}

void P4ZoomWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        int *data = new int;
        *data = zoomid_;

        P4Event *e1 = new P4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
        gP4app->postEvent(parent_, e1);
    }
}

void P4ZoomWnd::adjustHeight()
{
    int w, h, m;
    double deltaw, deltah;

    sphere_->adjustToNewSize();

    w = width();
    h = height() + sphere_->idealh_ - sphere_->h_;

    m = gP4app->desktop()->height();
    m -= m / 10;  // occuppy at most 90% of the screen's height

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
        m = gP4app->desktop()->width();
        m -= m / 10;
        if (w > m)
            w = m;  // occupy at most 90 % of the screen's width
    }
    resize(w, h);
    statusBar()->showMessage("Ready.");
}
