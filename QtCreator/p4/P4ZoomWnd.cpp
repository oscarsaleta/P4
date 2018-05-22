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

#include "P4ZoomWnd.hpp"

#include <QAction>
#include <QDesktopWidget>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

#include <cmath>
#include <memory>

#include "P4Application.hpp"
#include "P4Event.hpp"
#include "P4InputVF.hpp"
#include "P4PlotWnd.hpp"
#include "P4PrintDlg.hpp"
#include "P4StartDlg.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "plot_tools.hpp"

P4ZoomWnd::P4ZoomWnd(P4PlotWnd *main, int id, double x1, double y1, double x2,
                     double y2)
    : QMainWindow(), zoomid_{id}, parent_{main}, x1_{x1}, x2_{x2}, y1_{y1},
      y2_{y2}
{
    if (gP4smallIcon != nullptr)
        setWindowIcon(*gP4smallIcon);

    auto toolBar1 = new QToolBar{"ZoomBar1", this};
    toolBar1->setMovable(false);

    auto actClose = new QAction{"Close", this};
    actClose->setShortcut(Qt::ALT + Qt::Key_E);
    QObject::connect(actClose, &QAction::triggered, this,
                     &P4ZoomWnd::onBtnClose);
    toolBar1->addAction(actClose);

    auto actRefresh = new QAction{"Refresh", this};
    actRefresh->setShortcut(Qt::ALT + Qt::Key_R);
    QObject::connect(actRefresh, &QAction::triggered, this,
                     &P4ZoomWnd::onBtnRefresh);
    toolBar1->addAction(actRefresh);

    auto actPrint = new QAction{"Print", this};
    actPrint->setShortcut(Qt::ALT + Qt::Key_P);
    QObject::connect(actPrint, &QAction::triggered, this,
                     &P4ZoomWnd::onBtnPrint);
    toolBar1->addAction(actPrint);

    QObject::connect(gThisVF, &P4InputVF::saveSignal, this,
                     &P4ZoomWnd::onSaveSignal);

#ifdef TOOLTIPS
    actClose->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    actRefresh->setToolTip("Redraw the plot window");
    actPrint->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");
    addToolBar(Qt::TopToolBarArea, toolBar1);

    sphere_ = new P4WinSphere{statusBar(), true, x1_, y1_, x2_, y2_, this};
    sphere_->show();
    setCentralWidget(sphere_);
    resize(NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW);
    sphere_->setupPlot();

    setP4WindowTitle(this, "Phase Portrait - Zoom");
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

void P4ZoomWnd::signalChanged() { sphere_->signalChanged(); }

/*void P4ZoomWnd::signalEvaluating()
{
    sphere_->signalEvaluating();
}*/

void P4ZoomWnd::signalEvaluated() { configure(); }

void P4ZoomWnd::onBtnClose()
{
    auto data = std::make_unique<int>(zoomid_);
    auto e1 = new P4Event{static_cast<QEvent::Type>(TYPE_CLOSE_ZOOMWINDOW),
                          data.release()};
    gP4app->postEvent(parent_, e1);
}

bool P4ZoomWnd::close()
{
    onBtnClose();
    return QMainWindow::close();
}

void P4ZoomWnd::onBtnRefresh()
{
    parent_->getDlgData();
    sphere_->refresh();
}

void P4ZoomWnd::onBtnPrint()
{
    auto pdlg = new P4PrintDlg{static_cast<Qt::WindowFlags>(0), this};
    int result{pdlg->exec()};
    int res{pdlg->getChosenResolution()};
    double lw{pdlg->getChosenLineWidth()};
    double ss{pdlg->getChosenSymbolSize()};

    if (result != P4PRINT_NONE) {
        if (result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT) {
            gP4printer->setResolution(res);
            QPrintDialog dialog{gP4printer, this};
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
    // reset status bar
    statusBar()->showMessage("Ready");
    // setup line/plot pointing to routines of the sphere window
    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
    // setup sphere window (define pixel transformations)
    sphere_->setupPlot();
    sphere_->update();
}

void P4ZoomWnd::customEvent(QEvent *_e)
{
    auto e = static_cast<P4Event *>(_e);

    if (e->type() == TYPE_OPENZOOMWINDOW || e->type() == TYPE_ORBIT_EVENT ||
        e->type() == TYPE_SELECT_ORBIT || e->type() == TYPE_SEP_EVENT ||
        e->type() == TYPE_SELECT_LCSECTION) {
        auto newe = new P4Event{e->type(), e->data()};
        gP4app->postEvent(parent_, newe);
        return;
    }

    QMainWindow::customEvent(e);
}

void P4ZoomWnd::hideEvent(QHideEvent *h)
{
    if (!isMinimized()) {
        auto data = std::make_unique<int>(zoomid_);
        auto e1 = new P4Event{static_cast<QEvent::Type>(TYPE_CLOSE_ZOOMWINDOW),
                              data.release()};
        gP4app->postEvent(parent_, e1);
    }
}

void P4ZoomWnd::adjustHeight()
{
    double deltaw, deltah;

    sphere_->adjustToNewSize();

    int w{width()};
    int h{height() + sphere_->idealh_ - sphere_->h_};

    int m{gP4app->desktop()->height()};
    m -= m / 10; // occuppy at most 90% of the screen's height

    if (h > m) {
        deltah = static_cast<double>(h - m);
        deltaw = deltah * sphere_->dx_ / sphere_->dy_;

        h -= std::round(deltah + 0.5);
        w -= std::round(deltaw + 0.5);
    } else if (sphere_->idealh_ < MINHEIGHTPLOTWINDOW) {
        deltah = static_cast<double>(MINHEIGHTPLOTWINDOW - sphere_->idealh_);
        deltaw = deltah * sphere_->dx_ / sphere_->dy_;

        h += std::round(deltah + 0.5);
        w += std::round(deltaw + 0.5);
        m = gP4app->desktop()->width();
        m -= m / 10;
        if (w > m)
            w = m; // occupy at most 90 % of the screen's width
    }
    resize(w, h);
    statusBar()->showMessage("Ready.");
}
