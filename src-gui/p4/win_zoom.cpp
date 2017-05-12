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

#include "win_zoom.h"

#include "custom.h"
#include "main.h"
#include "p4application.h"
#include "plot_tools.h"
#include "win_event.h"
#include "win_print.h"

#include <QDesktopWidget>
#include <QPrintDialog>
#include <QToolBar>

QZoomWnd::QZoomWnd(QPlotWnd *main, int id, double x1, double y1, double x2,
                   double y2)
    : QMainWindow()
{
    QToolBar *toolBar1;
    parent = main;
    zoomid = id;

    //    QPalette palette;
    //    palette.setColor(backgroundRole(), QXFIGCOLOR(CBACKGROUND) );
    //    setPalette(palette);

    if (p4smallicon != nullptr)
        setWindowIcon(*p4smallicon);

    toolBar1 = new QToolBar("ZoomBar1", this);
    toolBar1->setMovable(false);

    ActClose = new QAction("Close", this);
    ActClose->setShortcut(Qt::ALT + Qt::Key_E);
    connect(ActClose, SIGNAL(triggered()), this, SLOT(OnBtnClose()));
    toolBar1->addAction(ActClose);

    ActRefresh = new QAction("Refresh", this);
    ActRefresh->setShortcut(Qt::ALT + Qt::Key_R);
    connect(ActRefresh, SIGNAL(triggered()), this, SLOT(OnBtnRefresh()));
    toolBar1->addAction(ActRefresh);

    ActPrint = new QAction("Print", this);
    ActPrint->setShortcut(Qt::ALT + Qt::Key_P);
    connect(ActPrint, SIGNAL(triggered()), this, SLOT(OnBtnPrint()));
    toolBar1->addAction(ActPrint);

#ifdef TOOLTIPS
    ActClose->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    ActRefresh->setToolTip("Redraw the plot window");
    ActPrint->setToolTip("Opens the print window");
#endif

    statusBar()->showMessage("Ready");
    addToolBar(Qt::TopToolBarArea, toolBar1);

    sphere = new QWinSphere(this, statusBar(), true, x1, y1, x2, y2);
    sphere->show();
    setCentralWidget(sphere);
    resize(NOMINALWIDTHPLOTWINDOW, NOMINALHEIGHTPLOTWINDOW);
    sphere->SetupPlot();

    //  if( ThisVF->evaluated_ )
    setP4WindowTitle(this, "Phase Portrait - Zoom");
    //  else
    //      SetP4WindowTitle( this, "Phase Portrait - Zoom (*)" );
}

QZoomWnd::~QZoomWnd()
{
    delete sphere;
    sphere = nullptr;
}

void QZoomWnd::signalChanged(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere->signalChanged();
}

void QZoomWnd::signalEvaluating(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere->signalEvaluating();
}

void QZoomWnd::signalEvaluated(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
}

void QZoomWnd::OnBtnClose(void)
{
    int *data = new int;
    *data = zoomid;

    QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
    p4app->postEvent(parent, e1);
}

bool QZoomWnd::close(void)
{
    int *data = new int;
    *data = zoomid;

    QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
    p4app->postEvent(parent, e1);

    return QMainWindow::close();
}

void QZoomWnd::OnBtnRefresh(void)
{
    parent->getDlgData();
    sphere->refresh();
}

void QZoomWnd::OnBtnPrint(void)
{
    int res;
    double lw, ss;
    QPrintDlg *pdlg;
    pdlg = new QPrintDlg(this, 0);
    int result = pdlg->exec();
    res = pdlg->GetChosenResolution();
    lw = pdlg->GetChosenLineWidth();
    ss = pdlg->GetChosenSymbolSize();

    delete pdlg;
    pdlg = nullptr;

    if (result != P4PRINT_NONE) {
        if (result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT) {
            p4printer->setResolution(res);
            QPrintDialog dialog(p4printer, this);
            if (!dialog.exec())
                return;
            res = p4printer->resolution();
        }

        if (result < 0)
            sphere->preparePrinting(-result, true, res, lw, ss);
        else
            sphere->preparePrinting(result, false, res, lw, ss);
        sphere->print();
        sphere->finishPrinting();
    }
}

void QZoomWnd::configure(void)
{
    statusBar()->showMessage("Ready"); // reset status bar
    plot_l = spherePlotLine; // setup line/plot pointing to routines of the
                             // sphere window
    plot_p = spherePlotPoint;
    sphere->SetupPlot(); // setup sphere window (define pixel transformations)
    sphere->update();
    // delete print window
    // delete xfig window
}

void QZoomWnd::customEvent(QEvent *_e)
{
    QP4Event *e;
    e = (QP4Event *)_e;

    if (e->type() == TYPE_OPENZOOMWINDOW || e->type() == TYPE_ORBIT_EVENT ||
        e->type() == TYPE_SELECT_ORBIT || e->type() == TYPE_SEP_EVENT ||
        e->type() == TYPE_SELECT_LCSECTION) {
        QP4Event *newe = new QP4Event(e->type(), e->data());
        p4app->postEvent(parent, newe);
        return;
    }

    QMainWindow::customEvent(e);
}

void QZoomWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        int *data = new int;
        *data = zoomid;

        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_ZOOMWINDOW, data);
        p4app->postEvent(parent, e1);
    }
}

void QZoomWnd::AdjustHeight(void)
{
    int w, h, m;
    double deltaw, deltah;

    sphere->adjustToNewSize();

    w = width();
    h = height() + sphere->idealh - sphere->h;

    m = p4app->desktop()->height();
    m -= m / 10; // occuppy at most 90% of the screen's height

    if (h > m) {
        deltah = (double)(h - m);
        deltaw = deltah;
        deltaw *= sphere->dx;
        deltaw /= sphere->dy;

        h -= (int)(deltah + 0.5);
        w -= (int)(deltaw + 0.5);
    } else if (sphere->idealh < MINHEIGHTPLOTWINDOW) {
        deltah = (double)(MINHEIGHTPLOTWINDOW - sphere->idealh);
        deltaw = deltah;
        deltaw *= sphere->dx;
        deltaw /= sphere->dy;

        h += (int)(deltah + 0.5);
        w += (int)(deltaw + 0.5);
        m = p4app->desktop()->width();
        m -= m / 10;
        if (w > m)
            w = m; // occupy at most 90 % of the screen's width
    }
    resize(w, h);
    statusBar()->showMessage("Ready.");
}
