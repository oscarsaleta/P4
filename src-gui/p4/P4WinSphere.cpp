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

#include "win_sphere.h"

#include "file_vf.h"
#include "main.h"
#include "math_curve.h"
#include "math_findpoint.h"
#include "math_gcf.h"
#include "math_isoclines.h"
#include "math_limitcycles.h"
#include "math_orbits.h"
#include "math_p4.h"
#include "math_separatrice.h"
#include "p4application.h"
#include "plot_points.h"
#include "plot_tools.h"
#include "print_bitmap.h"
#include "print_points.h"
#include "print_postscript.h"
#include "print_xfig.h"
#include "P4Event.h"
#include "win_main.h"
#include "win_print.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QResizeEvent>
#include <QStatusBar>
#include <QTimer>

#include <cmath>

static QPixmap *s_p4pixmap = nullptr;
static double s_p4pixmap_dpm = 0;

// TODO: canviar per vector de spheres
int P4WinSphere::sm_numSpheres = 0;
P4WinSphere **P4WinSphere::sm_SphereList = nullptr;

/*
    Coordinates on the sphere:

    - in a local study: the coordinates are cartesian
    - in a normal window: the coordinates are mapped to a square of width/height
   2.2
      Inside this square, the circle at infinity is displayed as a circle with
   radius 1.

    - in a zoom window: the coordinates are a part of the coordinates from a
   normal window.
*/

// parameters x1,... are irrelevant if isZoom is false

P4WinSphere::P4WinSphere(QWidget *parent, QStatusBar *bar, bool isZoom,
                         double x1, double y1, double x2, double y2)
    : QWidget(parent)
{
    reverseYAxis_ = false;
    painterCache_ = nullptr;
    isPainterCacheDirty_ = true;
    anchorMap_ = nullptr;
    refreshTimeout_ = nullptr;
    selectingTimer_ = nullptr;

    //    setAttribute( Qt::WA_PaintOnScreen );

    sm_SphereList = (P4WinSphere **)realloc(
        sm_SphereList, sizeof(P4WinSphere *) * (sm_numSpheres + 1));
    sm_SphereList[sm_numSpheres++] = this;
    if (sm_numSpheres > 1) {
        sm_SphereList[sm_numSpheres - 2]->next_ = this;
    }

    parentWnd_ = parent;
    setMinimumSize(MINWIDTHPLOTWINDOW,
                   MINHEIGHTPLOTWINDOW);  // THIS IS THE MINIMUM SIZE
    w_ = width();
    h_ = height();
    idealh_ = w_;

    selectingX_ = 0;
    selectingY_ = 0;
    selectingPointRadius_ = 0;
    selectingPointStep_ = 0;

    horPixelsPerMM_ = ((double)w_) / ((double)widthMM());
    verPixelsPerMM_ = ((double)h_) / ((double)heightMM());

    setMouseTracking(true);
    msgBar_ = bar;
    selectingZoom_ = false;
    selectingLCSection_ = false;
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(windowFlags());
    next_ = nullptr;

    iszoom_ = isZoom;
    if (isZoom) {
        x0_ = x1;
        y0_ = y1;

        x1_ = x2;
        y1_ = y2;
    }

    circleAtInfinity_ = nullptr;
    plCircle_ = nullptr;

    paintedXMin_ = 0;
    paintedXMax_ = w_;
    paintedYMin_ = 0;
    paintedYMax_ = h_;
}

/*
    Keyboard codes:

    F       forward integration of orbit
    C       continue integration of orbit
    B       backward integration of orbit
    D       delete orbit
    A       delete all orbits

    Shift+C continue integrate separatrice
    Shift+N next separatrice
    Shift+I integrate next separatrice
    Shift+S start integrate separatrice
    Shift+A delete all separatrices???
*/

void P4WinSphere::keyPressEvent(QKeyEvent *e)
{
    int id;
    int *data1;
    Qt::KeyboardModifiers bs;

    id = e->key();
    bs = e->modifiers();
    if (id == Qt::Key_F && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // F: integrate orbit forwards in time

        data1 = new int;
        *data1 = 1;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }
    if (id == Qt::Key_C && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // C: continue integrate orbit

        data1 = new int;
        *data1 = 0;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }
    if (id == Qt::Key_B && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // B: integrate orbit backwards in time

        data1 = new int;
        *data1 = -1;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }
    if (id == Qt::Key_D && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // D: delete orbit

        data1 = new int;
        *data1 = 2;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }

    if (id == Qt::Key_A && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // A: delete all orbits

        data1 = new int;
        *data1 = 3;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }

    if (id == Qt::Key_C && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+C:  continue integrating separatrice

        data1 = new int;
        *data1 = 0;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }

    if (id == Qt::Key_N && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+N: select next separatrice

        data1 = new int;
        *data1 = 3;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }

    if (id == Qt::Key_I && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+I: integrate next separatrice

        data1 = new int;
        *data1 = 2;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }

    if (id == Qt::Key_S && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+S: start integrate separatrice

        data1 = new int;
        *data1 = 1;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }

    e->ignore();
}

static QString makechartstring(int p, int q, bool isu1v1chart, bool negchart)
{
    QString buf;

    if (isu1v1chart) {
        // make { x = +/- 1/z2^p, y = z1/z2^q }

        if (p != 1 && q != 1)
            buf.sprintf("{x=%d/z2^%d,y=z1/z2^%d}", (int)(negchart ? -1 : 1), p,
                        q);
        else if (p == 1 && q != 1)
            buf.sprintf("{x=%d/z2,y=z1/z2^%d}", (int)(negchart ? -1 : 1), q);
        else if (p != 1 && q == 1)
            buf.sprintf("{x=%d/z2^%d,y=z1/z2}", (int)(negchart ? -1 : 1), p);
        else
            buf.sprintf("{x=%d/z2,y=z1/z2}", (int)(negchart ? -1 : 1));
    } else {
        // make { x = 1/z2^p, y = +/- z1/z2^q }

        if (p != 1 && q != 1)
            buf.sprintf("{x=z1/z2^%d,y=%d/z2^%d}", p, (int)(negchart ? -1 : 1),
                        q);
        else if (p == 1 && q != 1)
            buf.sprintf("{x=z1/z2,y=%d/z2^%d}", (int)(negchart ? -1 : 1), q);
        else if (p != 1 && q == 1)
            buf.sprintf("{x=z1/z2^%d,y=%d/z2}", p, (int)(negchart ? -1 : 1));
        else
            buf.sprintf("{x=z1/z2,y=%d/z2}", (int)(negchart ? -1 : 1));
    }

    return (QString)buf;
}

void P4WinSphere::setupPlot()
{
    struct P4POLYLINES *t;
    QPalette palette;

    spherebgcolor_ = bgColours::CBACKGROUND;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor_));
    setPalette(palette);

    while (circleAtInfinity_ != nullptr) {
        t = circleAtInfinity_;
        circleAtInfinity_ = t->next;
        delete t;
        t = nullptr;
    }
    while (plCircle_ != nullptr) {
        t = plCircle_;
        plCircle_ = t->next;
        delete t;
        t = nullptr;
    }

    if (!iszoom_) {
        switch (g_VFResults.typeofview_) {
        case TYPEOFVIEW_PLANE:
        case TYPEOFVIEW_U1:
        case TYPEOFVIEW_U2:
        case TYPEOFVIEW_V1:
        case TYPEOFVIEW_V2:
            x0_ = g_VFResults.xmin_;
            y0_ = g_VFResults.ymin_;
            x1_ = g_VFResults.xmax_;
            y1_ = g_VFResults.ymax_;
            break;
        case TYPEOFVIEW_SPHERE:
            x0_ = -1.1;
            y0_ = -1.1;
            x1_ = 1.1;
            y1_ = 1.1;
            break;
        }
    }

    dx_ = x1_ - x0_;
    dy_ = y1_ - y0_;

    double idealhd;

    idealhd = w_;
    idealhd = (idealhd / dx_) * dy_;

    idealh_ = (int)(idealhd + .5);

    switch (g_VFResults.typeofview_) {
    case TYPEOFVIEW_PLANE:
        chartstring_ = "";
        break;
    case TYPEOFVIEW_SPHERE:
        chartstring_ = "";
        break;
    case TYPEOFVIEW_U1:
        chartstring_ =
            makechartstring(g_VFResults.p_, g_VFResults.q_, true, false);
        break;
    case TYPEOFVIEW_U2:
        chartstring_ =
            makechartstring(g_VFResults.p_, g_VFResults.q_, false, false);
        break;
    case TYPEOFVIEW_V1:
        chartstring_ =
            makechartstring(g_VFResults.p_, g_VFResults.q_, true, true);
        break;
    case TYPEOFVIEW_V2:
        chartstring_ =
            makechartstring(g_VFResults.p_, g_VFResults.q_, false, true);
        break;
    }

    if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
        circleAtInfinity_ =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (g_VFResults.plweights_)
            plCircle_ = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }

    isPainterCacheDirty_ = true;
}

P4WinSphere::~P4WinSphere()
{
    int i;

    struct P4POLYLINES *t;
    while (circleAtInfinity_ != nullptr) {
        t = circleAtInfinity_;
        circleAtInfinity_ = t->next;
        delete t;
        t = nullptr;
    }
    while (plCircle_ != nullptr) {
        t = plCircle_;
        plCircle_ = t->next;
        delete t;
        t = nullptr;
    }

    for (i = 0; i < sm_numSpheres; i++) {
        if (sm_SphereList[i] == this)
            break;
    }
    if (i == sm_numSpheres)
        return;  // error: sphere not found?

    if (i > 0)
        sm_SphereList[i - 1]->next_ = next_;

    if (i < sm_numSpheres - 1)
        memmove(sm_SphereList + i, sm_SphereList + i + 1,
                sizeof(P4WinSphere *) * (sm_numSpheres - i - 1));

    sm_numSpheres--;

    if (painterCache_ != nullptr) {
        delete painterCache_;
        painterCache_ = nullptr;
    }

    if (anchorMap_ != nullptr) {
        delete anchorMap_;
        anchorMap_ = nullptr;
    }

    if (refreshTimeout_ != nullptr) {
        delete refreshTimeout_;
        refreshTimeout_ = nullptr;
    }
    if (selectingTimer_ != nullptr) {
        delete selectingTimer_;
        selectingTimer_ = nullptr;
    }
}

void P4WinSphere::loadAnchorMap()
{
    int x1_, y1_;
    int x2, y2;
    int aw, ah;
    int s;
    if (selectingZoom_) {
        x1_ = zoomAnchor1_.x();
        y1_ = zoomAnchor1_.y();
        x2 = zoomAnchor2_.x();
        y2 = zoomAnchor2_.y();
    } else if (selectingLCSection_) {
        x1_ = lcAnchor1_.x();
        y1_ = lcAnchor1_.y();
        x2 = lcAnchor2_.x();
        y2 = lcAnchor2_.y();
    } else
        return;

    if (x1_ > x2) {
        s = x1_;
        x1_ = x2;
        x2 = s;
    }
    if (y1_ > y2) {
        s = y1_;
        y1_ = y2;
        y2 = s;
    }
    if (x1_ < 0)
        x1_ = 0;
    if (y1_ < 0)
        y1_ = 0;
    if (x2 >= width())
        x2 = width() - 1;
    if (y2 >= height())
        y2 = height() - 1;

    aw = x2 - x1_ + 1;
    ah = y2 - y1_ + 1;

    /*     QString ms = msgBar_->currentMessage();
         QString as;
         as.sprintf( " Load: (%d,%d,%d,%d)", x1_,y1_,aw,ah );
         msgBar_->showMessage(as+ms);
    */
    if (anchorMap_ != nullptr) {
        if (anchorMap_->width() < aw || anchorMap_->height() < ah) {
            delete anchorMap_;
            anchorMap_ = nullptr;
            anchorMap_ = new QPixmap(aw, ah);
        }
    } else {
        anchorMap_ = new QPixmap(aw, ah);
    }

    if (painterCache_ == nullptr) {
        delete anchorMap_;
        anchorMap_ = nullptr;
        return;
    }

    QPainter paint(anchorMap_);
    if (selectingZoom_) {
        // only copy rectangular edges, not inside
        //   paint.drawPixmap (    0,    0, aw,  1, *painterCache_, x1_, y1_,
        //   aw, 1
        //   );
        //     paint.drawPixmap (    0, ah-1, aw,  1, *painterCache_, x1_, y2,
        //     aw,
        //     1 );
        //       paint.drawPixmap (    0,    0,  1, ah, *painterCache_, x1_,
        //       y1_,
        //       1, ah );
        //         paint.drawPixmap ( aw-1, 0,     1, ah, *painterCache_, x2,
        //         y1_,
        //         1, ah );

        paint.drawPixmap(0, 0, aw, ah, *painterCache_, x1_, y1_, aw, ah);

    } else {
        paint.drawPixmap(0, 0, aw, ah, *painterCache_, x1_, y1_, aw, ah);
    }
}

void P4WinSphere::saveAnchorMap()
{
    int x1_, y1_;
    int x2, y2;
    int aw, ah;
    int s;

    if (anchorMap_ == nullptr || painterCache_ == nullptr ||
        (!selectingZoom_ && !selectingLCSection_))
        return;

    if (selectingZoom_) {
        x1_ = zoomAnchor1_.x();
        y1_ = zoomAnchor1_.y();
        x2 = zoomAnchor2_.x();
        y2 = zoomAnchor2_.y();
    }
    if (selectingLCSection_) {
        x1_ = lcAnchor1_.x();
        y1_ = lcAnchor1_.y();
        x2 = lcAnchor2_.x();
        y2 = lcAnchor2_.y();
    }

    if (x1_ > x2) {
        s = x1_;
        x1_ = x2;
        x2 = s;
    }
    if (y1_ > y2) {
        s = y1_;
        y1_ = y2;
        y2 = s;
    }
    if (x1_ < 0)
        x1_ = 0;
    if (y1_ < 0)
        y1_ = 0;
    if (x2 >= width())
        x2 = width() - 1;
    if (y2 >= height())
        y2 = height() - 1;

    aw = x2 - x1_ + 1;
    ah = y2 - y1_ + 1;
    /*
         QString ms = msgBar_->currentMessage();
         QString as;
         as.sprintf( " Save: (%d,%d,%d,%d)", x1_,y1_,aw,ah );
         msgBar_->showMessage(as+ms);
    */
    QPainter paint(painterCache_);

    if (selectingZoom_) {
        // only copy rectangular edges, not inside
        //         paint.drawPixmap ( x1_, y1_, aw,  1, *anchorMap_,    0,    0,
        //         aw, 1 );
        //       paint.drawPixmap ( x1_, y2, aw,  1, *anchorMap_,    0, ah-1,
        //       aw,
        //       1 );
        //     paint.drawPixmap ( x1_, y1_,  1, ah, *anchorMap_,    0,    0, 1,
        //     ah
        //     );
        //   paint.drawPixmap ( x2, y1_,  1, ah, *anchorMap_, aw-1,    0, 1, ah
        //   );
        paint.drawPixmap(x1_, y1_, aw, ah, *anchorMap_, 0, 0, aw, ah);
    } else {
        paint.drawPixmap(x1_, y1_, aw, ah, *anchorMap_, 0, 0, aw, ah);
    }

    update(x1_, y1_, aw, ah);
}

void P4WinSphere::adjustToNewSize()
{
    double idealhd;
    double reqratio;
    double ratio;
    QString buf;
    struct P4POLYLINES *t;

    w_ = width();
    h_ = height();

    idealhd = w_;
    idealhd = (idealhd / dx_) * dy_;

    idealh_ = (int)(idealhd + .5);

    reqratio = (((double)w_) / horPixelsPerMM_) / (idealh_ / verPixelsPerMM_);
    ratio = (((double)w_) / horPixelsPerMM_) / (((double)h_) / verPixelsPerMM_);

    buf.sprintf("Aspect Ratio = %f\n", (float)(ratio / reqratio));
    msgBar_->showMessage(buf);

    while (circleAtInfinity_ != nullptr) {
        t = circleAtInfinity_;
        circleAtInfinity_ = t->next;
        delete t;
        t = nullptr;
    }
    while (plCircle_ != nullptr) {
        t = plCircle_;
        plCircle_ = t->next;
        delete t;
        t = nullptr;
    }
    if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
        circleAtInfinity_ =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (g_VFResults.plweights_)
            plCircle_ = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }

    if (painterCache_ != nullptr) {
        delete painterCache_;
        painterCache_ = nullptr;
        painterCache_ = new QPixmap(size());
        isPainterCacheDirty_ = false;

        QPainter paint(painterCache_);
        paint.fillRect(0, 0, width(), height(),
                       QColor(QXFIGCOLOR(bgColours::CBACKGROUND)));

        if (g_VFResults.singinf_)
            paint.setPen(QXFIGCOLOR(CSING));
        else
            paint.setPen(QXFIGCOLOR(CLINEATINFINITY));

        staticPainter_ = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if (g_VFResults.typeofview_ != TYPEOFVIEW_PLANE) {
            if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
                if (g_VFResults.plweights_)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }
        // during resizing : only plot essential information
        //      plotSeparatrices();
        //      plotGcf();
        //      DrawOrbits(this);
        //      DrawLimitCycles(this);
        plotPoints();

        QColor c = QXFIGCOLOR(WHITE);
        c.setAlpha(128);
        paint.setPen(c);
        paint.drawText(0, 0, width(), height(),
                       Qt::AlignHCenter | Qt::AlignVCenter, "Resizing ...  ");

        staticPainter_ = nullptr;

        if (refreshTimeout_ != nullptr)
            refreshTimeout_->stop();
        else {
            refreshTimeout_ = new QTimer();
            connect(refreshTimeout_, &QTimer::timeout, this,
                    &P4WinSphere::refreshAfterResize);
        }
        refreshTimeout_->start(500);
    }
}

void P4WinSphere::refreshAfterResize()
{
    if (refreshTimeout_ != nullptr) {
        delete refreshTimeout_;
        refreshTimeout_ = nullptr;
    }
    refresh();
}

void P4WinSphere::resizeEvent(QResizeEvent *e)
{
    if (e->size() == e->oldSize())
        return;

    adjustToNewSize();
}

void P4WinSphere::paintEvent(QPaintEvent *p)
{
    UNUSED(p);
    if (g_ThisVF->evaluating_)
        return;

    if (painterCache_ == nullptr || isPainterCacheDirty_) {
        if (painterCache_ == nullptr)
            painterCache_ = new QPixmap(size());
        isPainterCacheDirty_ = false;

        QPainter paint(painterCache_);
        paint.fillRect(0, 0, width(), height(),
                       QColor(QXFIGCOLOR(bgColours::CBACKGROUND)));

        if (g_VFResults.singinf_)
            paint.setPen(QXFIGCOLOR(CSING));
        else
            paint.setPen(QXFIGCOLOR(CLINEATINFINITY));

        staticPainter_ = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if (g_VFResults.typeofview_ != TYPEOFVIEW_PLANE) {
            if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
                if (g_VFResults.plweights_)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }
        plotSeparatrices();
        plotGcf();
        plotCurve();
        drawIsoclines();
        drawOrbits(this);
        drawLimitCycles(this);
        plotPoints();
        staticPainter_ = nullptr;
    }

    QPainter widgetpaint(this);
    widgetpaint.drawPixmap(0, 0, *painterCache_);

    if (selectingPointStep_ != 0) {
        widgetpaint.setPen(QXFIGCOLOR(WHITE));
        widgetpaint.setBrush(Qt::NoBrush);
        widgetpaint.drawEllipse(selectingX_ - selectingPointRadius_,
                                selectingY_ - selectingPointRadius_,
                                selectingPointRadius_ + selectingPointRadius_,
                                selectingPointRadius_ + selectingPointRadius_);
    }
}

void P4WinSphere::markSelection(int x1_, int y1_, int x2, int y2,
                                int selectiontype)
{
    int bx1, by1, bx2, by2;

    if (painterCache_ == nullptr)
        return;

    bx1 = (x1_ < x2) ? x1_ : x2;
    bx2 = (x1_ < x2) ? x2 : x1_;
    by1 = (y1_ < y2) ? y1_ : y2;
    by2 = (y1_ < y2) ? y2 : y1_;

    if (bx1 < 0)
        bx1 = 0;
    if (by1 < 0)
        by1 = 0;
    if (bx2 >= width())
        bx2 = width() - 1;
    if (by2 >= height())
        by2 = height() - 1;

    QPainter p(painterCache_);
    QColor c;

    switch (selectiontype) {
    case 0:
        c = QXFIGCOLOR(WHITE);
        c.setAlpha(32);
        p.setPen(QXFIGCOLOR(WHITE));
        p.setBrush(c);
        if (bx1 == bx2 || by1 == by2)
            p.drawLine(bx1, by1, bx2, by2);
        else
            p.drawRect(bx1, by1, bx2 - bx1, by2 - by1);
        break;

    case 1:
        p.setPen(QXFIGCOLOR(WHITE));
        p.drawLine(x1_, y1_, x2, y2);
        break;
    }
    update(bx1, by1, bx2 - bx1 + 1, by2 - by1 + 1);
}

void P4WinSphere::mouseMoveEvent(QMouseEvent *e)
{
    int x, y;
    double wx, wy;
    double ucoord[2];
    QString buf;

    x = e->x();
    y = e->y();

    wx = coWorldX(x);
    wy = coWorldY(y);

    /*  double ratio;
        ratio = ( ((double)w_)/horPixelsPerMM_ ) / (
       ((double)h_)/verPixelsPerMM_ );
        sprintf( buf, "Aspect Ratio = %f\n", (float)ratio );
        msgBar_->showMessage(buf);
        return;
    */
    double pcoord[3];
    if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord)) {
        switch (g_VFResults.typeofview_) {
        case TYPEOFVIEW_PLANE:
            if (g_VFResults.typeofstudy_ == TYPEOFSTUDY_ONE)
                buf.sprintf("Local study   (x,y) = (%f,%f)", (float)wx,
                            (float)wy);
            else
                buf.sprintf("Planar view   (x,y) = (%f,%f)", (float)wx,
                            (float)wy);
            break;
        case TYPEOFVIEW_SPHERE:
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

            if (g_VFResults.p_ == 1 && g_VFResults.q_ == 1)
                buf.sprintf("The Poincare sphere  (x,y) = (%f,%f)",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The P-L sphere of type (%d,%d)  (x,y) = (%f,%f)",
                            g_VFResults.p_, g_VFResults.q_, (float)ucoord[0],
                            (float)ucoord[1]);
            break;
        case TYPEOFVIEW_U1:
            MATHFUNC(sphere_to_U1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U1 chart  (z2,z1) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The V1' chart (z2,z1) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            buf.append(chartstring_);
            break;
        case TYPEOFVIEW_V1:
            MATHFUNC(sphere_to_V1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!g_VFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V1 chart  (z2,z1) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The U1' chart (z2,z1) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            buf.append(chartstring_);
            break;
        case TYPEOFVIEW_U2:
            MATHFUNC(sphere_to_U2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U2 chart  (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The V2' chart (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            buf.append(chartstring_);
            break;
        case TYPEOFVIEW_V2:
            MATHFUNC(sphere_to_V2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!g_VFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V2 chart  (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The U2' chart (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            buf.append(chartstring_);
            break;
        }
    } else {
        switch (g_VFResults.typeofview_) {
        case TYPEOFVIEW_PLANE:
            if (g_VFResults.typeofstudy_ == TYPEOFSTUDY_ONE)
                buf.sprintf("Local study");
            else
                buf.sprintf("Planar view");
            break;
        case TYPEOFVIEW_SPHERE:
            if (g_VFResults.p_ == 1 && g_VFResults.q_ == 1)
                buf.sprintf("The Poincare sphere");
            else
                buf.sprintf("The P-L sphere of type (%d,%d)", g_VFResults.p_,
                            g_VFResults.q_);
            break;
        case TYPEOFVIEW_U1:
            buf.sprintf("The U1 chart");
            break;
        case TYPEOFVIEW_V1:
            buf.sprintf("The V1 chart");
            break;
        case TYPEOFVIEW_U2:
            buf.sprintf("The U2 chart");
            break;
        case TYPEOFVIEW_V2:
            buf.sprintf("The V2 chart");
            break;
        }
    }

    msgBar_->showMessage(buf);

    if (selectingZoom_) {
        saveAnchorMap();
        zoomAnchor2_ = e->pos();
        loadAnchorMap();
        markSelection(zoomAnchor1_.x(), zoomAnchor1_.y(), zoomAnchor2_.x(),
                      zoomAnchor2_.y(), 0);
    }

    if (selectingLCSection_) {
        saveAnchorMap();
        lcAnchor2_ = e->pos();
        loadAnchorMap();
        markSelection(lcAnchor1_.x(), lcAnchor1_.y(), lcAnchor2_.x(),
                      lcAnchor2_.y(), 1);
    }
}

double P4WinSphere::coWorldX(int x)
{
    double wx;

    wx = (double)x;
    wx /= (w_ - 1);
    return (wx * dx_ + x0_);
}

double P4WinSphere::coWorldY(int y)
{
    double wy;

    wy = (double)(h_ - 1 - y);
    wy /= (h_ - 1);
    return (wy * dy_ + y0_);
}

int P4WinSphere::coWinX(double x)
{
    double wx;
    int iwx;

    wx = (x - x0_) / dx_;
    wx *= w_ - 1;

    iwx = (int)(wx + 0.5);  // +0.5 to round upwards
    if (iwx >= w_)
        iwx = w_ - 1;

    return iwx;
}

int P4WinSphere::coWinH(double deltax)
{
    double wx;

    wx = deltax / dx_;
    wx *= w_ - 1;
    return (int)(wx + 0.5);
}

int P4WinSphere::coWinV(double deltay)
{
    double wy;

    wy = deltay / dy_;
    wy *= h_ - 1;
    return (int)(wy + 0.5);
}

int P4WinSphere::coWinY(double y)
{
    double wy;
    int iwy;

    wy = (y - y0_) / dy_;
    wy *= h_ - 1;

    iwy = (int)(wy + 0.5);  // +0.5 to round upwards
    if (iwy >= h_)
        iwy = h_ - 1;

    return (reverseYAxis_)
               ? iwy
               : h_ - 1 -
                     iwy;  // on screen: vertical axis orientation is reversed
}

void P4WinSphere::mousePressEvent(QMouseEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier &&
        e->button() == Qt::LeftButton) {
        if (!selectingZoom_) {
            selectingZoom_ = true;
            zoomAnchor1_ = e->pos();
            zoomAnchor2_ = zoomAnchor1_;
            loadAnchorMap();
            markSelection(zoomAnchor1_.x(), zoomAnchor1_.y(), zoomAnchor2_.x(),
                          zoomAnchor2_.y(), 0);
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if (e->modifiers() == Qt::ShiftModifier && e->button() == Qt::LeftButton) {
        // select nearest singularity having separatrices, and open the
        // separatrices window.

        selectNearestSingularity(e->pos());

        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::LeftButton &&
        (g_LCWindowIsUp || e->modifiers() == Qt::AltModifier)) {
        if (selectingLCSection_ == false) {
            selectingLCSection_ = true;
            lcAnchor1_ = e->pos();
            lcAnchor2_ = lcAnchor1_;

            loadAnchorMap();
            markSelection(lcAnchor1_.x(), lcAnchor1_.y(), lcAnchor2_.x(),
                          lcAnchor2_.y(), 0);
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::LeftButton) {
        // normally, start integrating new orbit at the chosen point.
        //
        // However, when the limit cycle window is open, select the first
        // and second point of a transverse section.

        struct DOUBLEPOINT *data1 = new DOUBLEPOINT;
        data1->x = coWorldX(e->x());
        data1->y = coWorldY(e->y());
        //*((void **)(data1 + 1)) = this;

        double pcoord[3];
        if (MATHFUNC(is_valid_viewcoord)(data1->x, data1->y, pcoord)) {
            P4Event *e1;
            e1 = new P4Event((QEvent::Type)TYPE_SELECT_ORBIT, data1);
            g_p4app->postEvent(parentWnd_, e1);
        } else {
            free(data1);
        }

        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::RightButton) {
        // cancel zoom window with right mouse button

        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;
        }

        if (selectingLCSection_) {
            saveAnchorMap();
            selectingLCSection_ = false;
        }

        QWidget::mousePressEvent(e);
        return;
    }

    QWidget::mousePressEvent(e);
}

void P4WinSphere::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // finish zoom window between zoomAnchor1_ and zoomAnchor2_

        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;

            double *data1 = new double[4];
            data1[0] = coWorldX(zoomAnchor1_.x());
            data1[1] = coWorldY(zoomAnchor1_.y());
            data1[2] = coWorldX(zoomAnchor2_.x());
            data1[3] = coWorldY(zoomAnchor2_.y());
            P4Event *e1 =
                new P4Event((QEvent::Type)TYPE_OPENZOOMWINDOW, data1);
            g_p4app->postEvent(parentWnd_, e1);
        }

        if (selectingLCSection_) {
            saveAnchorMap();
            selectingLCSection_ = false;

            double *data1 = new double[4];
            data1[0] = coWorldX(lcAnchor1_.x());
            data1[1] = coWorldY(lcAnchor1_.y());
            data1[2] = coWorldX(lcAnchor2_.x());
            data1[3] = coWorldY(lcAnchor2_.y());
            P4Event *e1 =
                new P4Event((QEvent::Type)TYPE_SELECT_LCSECTION, data1);
            g_p4app->postEvent(parentWnd_, e1);
        }

        QWidget::mouseReleaseEvent(e);
        return;
    }

    QWidget::mouseReleaseEvent(e);
}

bool P4WinSphere::getChartPos(int chart, double x0_, double y0_, double *pos)
{
    double pcoord[3];

    switch (chart) {
    case CHART_R2:
        MATHFUNC(finite_to_viewcoord)(x0_, y0_, pos);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    }

    return true;
}

void P4WinSphere::updatePointSelection()
{
    if (selectingPointStep_ == 0) {
        selectingPointRadius_ = 0;
        update(selectingX_ - SELECTINGPOINTSTEPS * 4,
               selectingY_ - SELECTINGPOINTSTEPS * 4,
               SELECTINGPOINTSTEPS * 4 * 2 + 1,
               SELECTINGPOINTSTEPS * 4 * 2 + 1);
        return;
    }

    selectingPointStep_--;
    selectingPointRadius_ = (SELECTINGPOINTSTEPS - selectingPointStep_) * 4;
    update(selectingX_ - selectingPointRadius_,
           selectingY_ - selectingPointRadius_,
           selectingPointRadius_ + selectingPointRadius_ + 1,
           selectingPointRadius_ + selectingPointRadius_ + 1);
}

void P4WinSphere::selectNearestSingularity(QPoint winpos)
{
    bool result;
    int x, y;
    int px, py;

    x = winpos.x();
    y = winpos.y();

    (*sm_SphereList)->prepareDrawing();
    result = find_critical_point(*sm_SphereList, coWorldX(x), coWorldY(y));
    (*sm_SphereList)->finishDrawing();

    if (result == false) {
        msgBar_->showMessage(
            "Search nearest critical point: None with separatrices found.");
    } else {
        px = coWinX(g_VFResults.selected_ucoord_[0]);
        py = coWinY(g_VFResults.selected_ucoord_[1]);

        if (selectingTimer_ != nullptr) {
            delete selectingTimer_;
            selectingTimer_ = nullptr;
            selectingPointStep_ = 0;
            updatePointSelection();
        }

        selectingPointStep_ = SELECTINGPOINTSTEPS - 1;
        selectingX_ = px;
        selectingY_ = py;

        selectingTimer_ = new QTimer();
        connect(selectingTimer_, &QTimer::timeout, this,
                &P4WinSphere::updatePointSelection);
        selectingTimer_->start(SELECTINGPOINTSPEED);
        msgBar_->showMessage("Search nearest critical point: Found");

        int *data1;
        data1 = new int;
        *data1 = -1;
        P4Event *e1 = new P4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        g_p4app->postEvent(parentWnd_, e1);
    }
}

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void P4WinSphere::plotPoint(p4singularity::saddle p)
{
    double pos[2];
    int x, y;

    getChartPos(p.chart, p.x0, p.y0, pos);
    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    x = coWinX(pos[0]);
    y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p.position) {
    case POSITION_VIRTUAL:
        win_plot_virtualsaddle(staticPainter_, x, y);
        break;
    case POSITION_COINCIDING:
        break;
    case POSITION_COINCIDING_VIRTUAL:
        break;
    case POSITION_COINCIDING_MAIN:
        win_plot_coinciding(staticPainter_, x, y);
        break;
    default:
        win_plot_saddle(staticPainter_, x, y);
        break;
    }
}

void P4WinSphere::plotPoint(p4singularity::node p)
{
    double pos[2];
    int x, y;

    getChartPos(p.chart, p.x0, p.y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    x = coWinX(pos[0]);
    y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    if (p.stable == -1) {
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualstablenode(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_stablenode(staticPainter_, x, y);
        }
    } else {
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualunstablenode(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_unstablenode(staticPainter_, x, y);
        }
    }
}

void P4WinSphere::plotPoint(p4singularity::weak_focus p)
{
    double pos[2];
    int x, y;

    getChartPos(p.chart, p.x0, p.y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    x = coWinX(pos[0]);
    y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p.type) {
    case FOCUSTYPE_STABLE:
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualstableweakfocus(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_stableweakfocus(staticPainter_, x, y);
        }
        break;
    case FOCUSTYPE_UNSTABLE:
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualunstableweakfocus(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_unstableweakfocus(staticPainter_, x, y);
        }
        break;
    case FOCUSTYPE_CENTER:
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualcenter(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_center(staticPainter_, x, y);
        }
        break;
    default:
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualweakfocus(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_weakfocus(staticPainter_, x, y);
        }
        break;
    }
}

void P4WinSphere::plotPoint(p4singularity::strong_focus p)
{
    double pos[2];
    int x, y;

    getChartPos(p.chart, p.x0, p.y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    x = coWinX(pos[0]);
    y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    if (p.stable == -1) {
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualstablestrongfocus(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_stablestrongfocus(staticPainter_, x, y);
        }
    } else {
        switch (p.position) {
        case POSITION_VIRTUAL:
            win_plot_virtualunstablestrongfocus(staticPainter_, x, y);
            break;
        case POSITION_COINCIDING:
            break;
        case POSITION_COINCIDING_VIRTUAL:
            break;
        case POSITION_COINCIDING_MAIN:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_unstablestrongfocus(staticPainter_, x, y);
        }
    }
}

void P4WinSphere::plotPoint(p4singularity::degenerate p)
{
    double pos[2];
    int x, y;

    getChartPos(p.chart, p.x0, p.y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    x = coWinX(pos[0]);
    y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p.position) {
    case POSITION_VIRTUAL:
        win_plot_virtualdegen(staticPainter_, x, y);
        break;
    case POSITION_COINCIDING:
        break;
    case POSITION_COINCIDING_VIRTUAL:
        break;
    case POSITION_COINCIDING_MAIN:
        win_plot_coinciding(staticPainter_, x, y);
        break;
    default:
        win_plot_degen(staticPainter_, x, y);
    }
}

void P4WinSphere::plotPoint(p4singularity::semi_elementary p)
{
    double pos[2];
    int x, y;

    getChartPos(p.chart, p.x0, p.y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    x = coWinX(pos[0]);
    y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p.position) {
    case POSITION_VIRTUAL:
        switch (p->type) {
        case 1:
            win_plot_virtualsesaddlenode(staticPainter_, x, y);
            break;
        case 2:
            win_plot_virtualsesaddlenode(staticPainter_, x, y);
            break;
        case 3:
            win_plot_virtualsesaddlenode(staticPainter_, x, y);
            break;
        case 4:
            win_plot_virtualsesaddlenode(staticPainter_, x, y);
            break;
        case 5:
            win_plot_virtualseunstablenode(staticPainter_, x, y);
            break;
        case 6:
            win_plot_virtualsesaddle(staticPainter_, x, y);
            break;
        case 7:
            win_plot_virtualsesaddle(staticPainter_, x, y);
            break;
        case 8:
            win_plot_virtualsestablenode(staticPainter_, x, y);
            break;
        }
        break;
    case POSITION_COINCIDING:
        break;
    case POSITION_COINCIDING_VIRTUAL:
        break;
    case POSITION_COINCIDING_MAIN:
        win_plot_coinciding(staticPainter_, x, y);
        break;
    default:
        switch (p->type) {
        case 1:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 2:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 3:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 4:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 5:
            win_plot_seunstablenode(staticPainter_, x, y);
            break;
        case 6:
            win_plot_sesaddle(staticPainter_, x, y);
            break;
        case 7:
            win_plot_sesaddle(staticPainter_, x, y);
            break;
        case 8:
            win_plot_sestablenode(staticPainter_, x, y);
            break;
        }
    }
}

void P4WinSphere::plotPoints()
{
    if (!g_VFResults.vf_.empty()) {
        for (int i = 0; i < g_ThisVF->numVF_; i++) {
            for (auto sp : g_VFResults.vf_[i].saddlePoints_)
                plotPoint(sp);
            for (auto np : g_VFResults.vf_[i].nodePoints_)
                plotPoint(np);
            for (auto wfp : g_VFResults.vf_[i].wfPoints_)
                plotPoint(wfp);
            for (auto sfp : g_VFResults.vf_[i].sfPoints_)
                plotPoint(sfp);
            for (auto sep : g_VFResults.vf_[i].sePoints_)
                plotPoint(sep);
            for (auto dp : g_VFResults.vf_[i].dePoints_)
                plotPoint(dp);
        }
    }
}

void P4WinSphere::plotPointSeparatrices(
    std::vector<p4singularity::semi_elementary> p)
{
    for (auto separatrice : p)
        draw_sep(this, separatrice.sep_points);
}

void P4WinSphere::plotPointSeparatrices(std::vector<p4singularity::saddle> p)
{
    for (auto separatrice : p)
        draw_sep(this, separatrice.sep_points);
}

void P4WinSphere::plotPointSeparatrices(
    std::vector<p4singularity::degenerate> p)
{
    for (auto blow_up : p) {
        draw_sep(this, blow_up.sep_points);
    }
}

void P4WinSphere::plotSeparatrices()
{
    if (!g_VFResults.vf_.empty()) {
        for (int i = 0; i < g_ThisVF->numVF; i++) {
            for (auto sp : g_VFResults.vf_[i].saddlePoints_)
                plotPointSeparatrices(sp);
            for (auto sep : g_VFResults.vf_[i].sePoints_)
                plotPointSeparatrices(sep);
            for (auto dp : g_VFResults.vf_[i].dePoints_)
                plotPointSeparatrices(dp);
        }
    }
}

void P4WinSphere::plotGcf()
{
    for (auto it : g_VFResults.vf_)
        draw_gcf(this, it.gcf_points_, CSING, 1);
}

void P4WinSphere::plotCurve()
{
    std::vector<curves>::const_iterator it;
    for (it = g_VFResults.curve_vector_.begin();
         it != g_VFResults.curve_vector_.end(); it++) {
        draw_curve(this, it->points, CCURV, 1);
    }
}

void P4WinSphere::drawIsoclines()
{
    std::vector<isoclines>::const_iterator it;
    for (it = g_VFResults.isocline_vector_.begin();
         it != g_VFResults.isocline_vector_.end(); it++) {
        draw_isoclines(this, it->points, it->color, 1);
    }
}

// -----------------------------------------------------------------------
//                          PLOT TOOLS
// -----------------------------------------------------------------------

std::vector<P4POLYLINES> P4WinSphere::produceEllipse(double cx, double cy,
                                                     double a, double b,
                                                     bool dotted, double resa,
                                                     double resb)
{
    // this is an exact copy of the plotEllipse routine, except that output is
    // stored in a list of points that is dynamically allocated.

    double theta{0};
    double t1, t2;
    double e;
    double R{(resa < resb) ? resa : resb};
    double x, y, c;
    double prevx{0.}, prevy{0.};
    bool d{false}, doton{true};
    int dotcount{0};
    std::vector<P4POLYLINES> result;

    if (R < 1.0)
        R = 1.0;  // protection
    e = 2 * acos(1.0 - 0.5 / R);
    if (R * sin(e) > 1.0)
        e = asin(1.0 / R);

    while (theta < TWOPI) {
        c = (x0_ - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta >= t1 && theta < t2) {
                theta = t2 + e / 4;
                d = false;
                continue;
            }
        }
        c = (x1_ - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta < t1) {
                theta = t1 + e / 4;
                d = false;
                continue;
            }
            if (theta >= t2) {
                theta = TWOPI + e / 4;
                d = false;
                break;
            }
        }
        c = (y0_ - cy) / b;
        if (c > -1.0 && c < 1.0) {
            t1 = asin(c);
            t2 = PI - t1;
            if (t1 < 0) {
                t2 = t1 + TWOPI;
                t1 = PI - t1;
                if (theta >= t1 && theta < t2) {
                    theta = t2 + e / 4;
                    d = false;
                    continue;
                }
            } else {
                if (theta < t1) {
                    theta = t1 + e / 4;
                    d = false;
                    continue;
                }
                if (theta >= t2) {
                    theta = TWOPI + e / 4;
                    d = false;
                    break;
                }
            }
        }
        c = (y1_ - cy) / b;
        if (c > -1.0 && c < 1.0) {
            t1 = asin(c);
            t2 = PI - t1;
            if (t1 < 0) {
                t2 = t1 + TWOPI;
                t1 = PI - t1;
                if (theta < t1) {
                    theta = t1 + e / 4;
                    d = false;
                    continue;
                }
                if (theta >= t2) {
                    theta = TWOPI;
                    d = false;
                    break;
                }
            } else {
                if (theta >= t1 && theta < t2) {
                    theta = t2 + e / 4;
                    d = false;
                    continue;
                }
            }
        }

        x = cx + a * cos(theta);
        y = cy + b * sin(theta);
        theta += e;

        // (x,y) is inside view

        if (!d) {
            if (doton) {
                d = true;
                prevx = x;
                prevy = y;
                dotcount = 0;
                doton = true;
            } else {
                if (++dotcount > 7 && dotted) {
                    d = false;
                    doton = (doton) ? false : true;
                    dotcount = 0;
                }
            }
        } else {
            if (doton) {
                result.push_back(P4POLYLINES(x1, y1, x2, y2));
                prevx = x;
                prevy = y;
            }
            if (++dotcount > 7 && dotted) {
                d = false;
                doton = (doton) ? false : true;
                dotcount = 0;
            }
        }
    }
    return result;
}

void P4WinSphere::plotPoincareSphere()
{
    std::vector<P4POLYLINES> &p{circleAtInfinity_};
    int color{g_VFResults.singinf_ ? CSING : CLINEATINFINITY};

    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto it : p) {
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
    }
}

void P4WinSphere::plotPoincareLyapunovSphere()
{
    std::vector<P4POLYLINES> &p{circleAtInfinity_};
    int color{g_VFResults.singinf_ ? CSING : CLINEATINFINITY};

    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto it : p) {
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
    }

    std::vector<P4POLYLINES> &p2{plCircle_};
    color = CLINEATINFINITY;

    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto it : p2) {
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
    }
}

void P4WinSphere::plotLineAtInfinity()
{
    switch (g_VFResults.typeofview_) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0_ < 0.0 && x1_ > 0.0) {
            staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter_->drawLine(coWinX(0.0), 0, coWinX(0.0), h_ - 1);
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0_ < 0.0 && y1_ > 0.0) {
            staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter_->drawLine(0, coWinY(0.0), w_ - 1, coWinY(0.0));
        }

        break;
    default:
        // other cases should not appear
        break;
    }
}

void P4WinSphere::drawLine(double x1, double y1, double x2, double y2,
                           int color)
{
    int wx1, wy1, wx2, wy2;

    if (staticPainter_ != nullptr) {
        if (x1 >= x0_ && x1 <= x1_ && y1 >= y0_ && y1 <= y1_) {
            wx1 = coWinX(x1);
            wy1 = coWinY(y1);

            if (x2 >= x0_ && x2 <= x1_ && y2 >= y0_ && y2 <= y1_) {
                // both points are visible in the window

                wx2 = coWinX(x2);
                wy2 = coWinY(y2);

                if (paintedXMin_ > wx1)
                    paintedXMin_ = wx1;
                if (paintedXMax_ < wx1)
                    paintedXMax_ = wx1;
                if (paintedYMin_ > wy1)
                    paintedYMin_ = wy1;
                if (paintedYMax_ < wy1)
                    paintedYMax_ = wy1;
                if (paintedXMin_ > wx2)
                    paintedXMin_ = wx2;
                if (paintedXMax_ < wx2)
                    paintedXMax_ = wx2;
                if (paintedYMin_ > wy2)
                    paintedYMin_ = wy2;
                if (paintedYMax_ < wy2)
                    paintedYMax_ = wy2;

                staticPainter_->setPen(QXFIGCOLOR(color));
                staticPainter_->drawLine(wx1, wy1, wx2, wy2);
            } else {
                // only (x2,y2) is not visible

                if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(x1);
                    wy1 = coWinY(y1);
                    wx2 = coWinX(x2);
                    wy2 = coWinY(y2);
                    staticPainter_->setPen(QXFIGCOLOR(color));

                    if (paintedXMin_ > wx1)
                        paintedXMin_ = wx1;
                    if (paintedXMax_ < wx1)
                        paintedXMax_ = wx1;
                    if (paintedYMin_ > wy1)
                        paintedYMin_ = wy1;
                    if (paintedYMax_ < wy1)
                        paintedYMax_ = wy1;
                    if (paintedXMin_ > wx2)
                        paintedXMin_ = wx2;
                    if (paintedXMax_ < wx2)
                        paintedXMax_ = wx2;
                    if (paintedYMin_ > wy2)
                        paintedYMin_ = wy2;
                    if (paintedYMax_ < wy2)
                        paintedYMax_ = wy2;

                    staticPainter_->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        } else {
            if (x2 >= x0_ && x2 <= x1_ && y2 >= y0_ && y2 <= y1_) {
                // only (x2,y2) is visible

                if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(x1);
                    wy1 = coWinY(y1);
                    wx2 = coWinX(x2);
                    wy2 = coWinY(y2);
                    staticPainter_->setPen(QXFIGCOLOR(color));

                    if (paintedXMin_ > wx1)
                        paintedXMin_ = wx1;
                    if (paintedXMax_ < wx1)
                        paintedXMax_ = wx1;
                    if (paintedYMin_ > wy1)
                        paintedYMin_ = wy1;
                    if (paintedYMax_ < wy1)
                        paintedYMax_ = wy1;
                    if (paintedXMin_ > wx2)
                        paintedXMin_ = wx2;
                    if (paintedXMax_ < wx2)
                        paintedXMax_ = wx2;
                    if (paintedYMin_ > wy2)
                        paintedYMin_ = wy2;
                    if (paintedYMax_ < wy2)
                        paintedYMax_ = wy2;

                    staticPainter_->drawLine(wx1, wy1, wx2, wy2);
                }
            } else {
                // both end points are invisible

                if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(x1);
                    wy1 = coWinY(y1);
                    wx2 = coWinX(x2);
                    wy2 = coWinY(y2);
                    staticPainter_->setPen(QXFIGCOLOR(color));

                    if (paintedXMin_ > wx1)
                        paintedXMin_ = wx1;
                    if (paintedXMax_ < wx1)
                        paintedXMax_ = wx1;
                    if (paintedYMin_ > wy1)
                        paintedYMin_ = wy1;
                    if (paintedYMax_ < wy1)
                        paintedYMax_ = wy1;
                    if (paintedXMin_ > wx2)
                        paintedXMin_ = wx2;
                    if (paintedXMax_ < wx2)
                        paintedXMax_ = wx2;
                    if (paintedYMin_ > wy2)
                        paintedYMin_ = wy2;
                    if (paintedYMax_ < wy2)
                        paintedYMax_ = wy2;

                    staticPainter_->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        }
    }
}

void P4WinSphere::drawPoint(double x, double y, int color)
{
    int _x, _y;
    if (staticPainter_ != nullptr) {
        if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
            return;
        staticPainter_->setPen(QXFIGCOLOR(color));
        _x = coWinX(x);
        _y = coWinY(y);

        if (paintedXMin_ > _x)
            paintedXMin_ = _x;
        if (paintedXMax_ < _x)
            paintedXMax_ = _x;
        if (paintedYMin_ > _y)
            paintedYMin_ = _y;
        if (paintedYMax_ < _y)
            paintedYMax_ = _y;

        staticPainter_->drawPoint(_x, _y);
    }
}

//---------------------------------------------------------------------
//                  PRINTING METHODS
//---------------------------------------------------------------------

void P4WinSphere::printPoint(struct saddle *p)
{
    double pos[2];
    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);
        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;
        print_saddle(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void P4WinSphere::printPoint(struct node *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        if (p->stable == -1)
            print_stablenode(coWinX(pos[0]), coWinY(pos[1]));
        else
            print_unstablenode(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void P4WinSphere::printPoint(struct weak_focus *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        switch (p->type) {
        case FOCUSTYPE_STABLE:
            print_stableweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case FOCUSTYPE_UNSTABLE:
            print_unstableweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case FOCUSTYPE_CENTER:
            print_center(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_weakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
    }
}

void P4WinSphere::printPoint(struct strong_focus *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        if (p->stable == -1)
            print_stablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
        else
            print_unstablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void P4WinSphere::printPoint(struct degenerate *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        print_degen(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void P4WinSphere::printPoint(struct semi_elementary *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        switch (p->type) {
        case 1:
            print_sesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 2:
            print_sesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 3:
            print_sesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 4:
            print_sesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 5:
            print_seunstablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 6:
            print_sesaddle(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 7:
            print_sesaddle(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 8:
            print_sestablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
    }
}

void P4WinSphere::printPoints()
{
    struct saddle *sp;
    struct node *np;
    struct weak_focus *wfp;
    struct strong_focus *sfp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    print_comment("Printing symbols at all singular points:");

    for (sp = g_VFResults.first_saddle_point_; sp != nullptr;
         sp = sp->next_saddle)
        printPoint(sp);
    for (np = g_VFResults.first_node_point_; np != nullptr; np = np->next_node)
        printPoint(np);
    for (wfp = g_VFResults.first_wf_point_; wfp != nullptr; wfp = wfp->next_wf)
        printPoint(wfp);
    for (sfp = g_VFResults.first_sf_point_; sfp != nullptr; sfp = sfp->next_sf)
        printPoint(sfp);
    for (sep = g_VFResults.first_se_point_; sep != nullptr; sep = sep->next_se)
        printPoint(sep);
    for (dp = g_VFResults.first_de_point_; dp != nullptr; dp = dp->next_de)
        printPoint(dp);
}

void P4WinSphere::printPointSeparatrices(struct semi_elementary *p)
{
    struct sep *separatrice;

    for (auto separatrice : p) {
        draw_sep(this, separatrice->first_sep_point);
        if (separatrice->next_sep != nullptr) {
            print_comment("Next separatrix of degenerate point:");
        }
    }
}

void P4WinSphere::printPointSeparatrices(struct saddle *p)
{
    struct sep *separatrice;

    for (separatrice = p->separatrices; separatrice != nullptr;
         separatrice = separatrice->next_sep) {
        draw_sep(this, separatrice->first_sep_point);
        if (separatrice->next_sep != nullptr) {
            print_comment("Next separatrix of saddle point:");
        }
    }
}

void P4WinSphere::printPointSeparatrices(struct degenerate *p)
{
    struct blow_up_points *blow_up;

    for (blow_up = p->blow_up; blow_up != nullptr;
         blow_up = blow_up->next_blow_up_point) {
        draw_sep(this, blow_up->first_sep_point);
        if (blow_up->next_blow_up_point != nullptr) {
            print_comment("Next separatrix of degenerate point:");
        }
    }
}

void P4WinSphere::printSeparatrices()
{
    QString comment;
    struct saddle *sp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    for (sp = g_VFResults.first_saddle_point_; sp != nullptr;
         sp = sp->next_saddle) {
        comment = "Printing separatrice for saddle singularity:";
        print_comment(comment);
        printPointSeparatrices(sp);
    }
    for (sep = g_VFResults.first_se_point_; sep != nullptr;
         sep = sep->next_se) {
        comment = "Printing separatrices for semi-hyperbolic singularity:";
        print_comment(comment);
        printPointSeparatrices(sep);
    }
    for (dp = g_VFResults.first_de_point_; dp != nullptr; dp = dp->next_de) {
        comment = "Printing separatrices for degenerate singularity:";
        print_comment(comment);
        printPointSeparatrices(dp);
    }
}

void P4WinSphere::printGcf()
{
    QString comment;

    if (g_VFResults.gcf_points_ != nullptr) {
        comment = "Printing Greatest common factor:";
        print_comment(comment);
        draw_gcf(this, g_VFResults.gcf_points_, CSING, 1);
    }
}

void P4WinSphere::printCurve()
{
    QString comment;
    std::vector<curves>::const_iterator it;
    int i;
    for (it = g_VFResults.curve_vector_.begin(), i = 0;
         it != g_VFResults.curve_vector_.end(); it++, i++) {
        if (it->points != nullptr) {
            comment.sprintf("Printing curve %d:", i);
            print_comment(comment);
            draw_curve(this, it->points, CCURV, 1);
        }
    }
}

void P4WinSphere::printIsoclines()
{
    QString comment;

    std::vector<isoclines>::const_iterator it;
    int i;
    for (it = g_VFResults.isocline_vector_.begin(), i = 0;
         it != g_VFResults.isocline_vector_.end(); it++, i++) {
        if (it->points != nullptr) {
            comment.sprintf("Printing isocline %d:", i);
            print_comment(comment);
            draw_isoclines(this, it->points, it->color, 1);
        }
    }
}

void P4WinSphere::printPoincareSphere()
{
    QString comment;
    struct P4POLYLINES *p;
    struct P4POLYLINES *q;

    comment = "Printing Poincare Sphere:";
    print_comment(comment);
    p = produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
    for (q = p; q != nullptr; q = q->next) {
        q->x1 = coWinX(q->x1);
        q->y1 = coWinY(q->y1);
        q->x2 = coWinX(q->x2);
        q->y2 = coWinY(q->y2);
    }
    // FIXME p needs to be a P4POLYLINE vector
    print_elips(coWinX(0), coWinY(0), coWinH(1), coWinV(1),
                g_VFResults.singinf_ ? CSING : CLINEATINFINITY, false, p);

    while (p != nullptr) {
        q = p;
        p = p->next;
        delete q;
        q = nullptr;
    }
}

void P4WinSphere::printPoincareLyapunovSphere()
{
    QString comment;
    struct P4POLYLINES *p;
    struct P4POLYLINES *q;

    comment = "Printing Poincare Lyapunov-Sphere (circle at infinity):";
    print_comment(comment);

    p = produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
    for (q = p; q != nullptr; q = q->next) {
        q->x1 = coWinX(q->x1);
        q->y1 = coWinY(q->y1);
        q->x2 = coWinX(q->x2);
        q->y2 = coWinY(q->y2);
    }

    print_elips(coWinX(0.0), coWinY(0.0), coWinH(1.0), coWinV(1.0),
                CLINEATINFINITY, false, p);

    while (p != nullptr) {
        q = p;
        p = p->next;
        delete q;
        q = nullptr;
    }

    comment = "Printing Poincare Lyapunov-Sphere (circle of finite radius):";
    print_comment(comment);

    p = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS),
                       coWinV(RADIUS));
    for (q = p; q != nullptr; q = q->next) {
        q->x1 = coWinX(q->x1);
        q->y1 = coWinY(q->y1);
        q->x2 = coWinX(q->x2);
        q->y2 = coWinY(q->y2);
    }

    print_elips(coWinX(0.0), coWinY(0.0), coWinH(RADIUS), coWinV(RADIUS),
                CLINEATINFINITY, true, p);

    while (p != nullptr) {
        q = p;
        p = p->next;
        delete q;
        q = nullptr;
    }
}

void P4WinSphere::printLineAtInfinity()
{
    switch (g_VFResults.typeofview_) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0_ < 0.0 && x1_ > 0.0)
            print_line(coWinX(0.0), coWinY(y0_), coWinX(0.0), coWinY(y1_),
                       CLINEATINFINITY);
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0_ < 0.0 && y1_ > 0.0)
            print_line(coWinX(x0_), coWinY(0.0), coWinX(x1_), coWinY(0.0),
                       CLINEATINFINITY);
        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}

void P4WinSphere::printOrbits()
{
    // inspired by DrawOrbits, except that we put comments between

    struct orbits *orbit;
    QString s;
    int i;
    i = 1;

    for (orbit = g_VFResults.first_orbit_; orbit != nullptr;
         orbit = orbit->next_orbit) {
        s.sprintf("Starting orbit %d", i++);
        print_comment(s);
        drawOrbit(this, orbit->pcoord, orbit->f_orbits, orbit->color);
    }
}

void P4WinSphere::printLimitCycles()
{
    // inspired by DrawOrbits, except that we put comments between

    struct orbits *orbit;
    QString s;
    int i;
    i = 1;

    for (orbit = g_VFResults.first_lim_cycle_; orbit != nullptr;
         orbit = orbit->next_orbit) {
        s.sprintf("Starting Limit Cycle %d", i++);
        print_comment(s);
        drawOrbit(this, orbit->pcoord, orbit->f_orbits, orbit->color);
    }
}

void P4WinSphere::printLine(double x1, double y1, double x2, double y2,
                            int color)
{
    int wx1, wy1, wx2, wy2;

    if (x1 >= x0_ && x1 <= x1_ && y1 >= y0_ && y1 <= y1_) {
        wx1 = coWinX(x1);
        wy1 = coWinY(y1);

        if (x2 >= x0_ && x2 <= x1_ && y2 >= y0_ && y2 <= y1_) {
            // both points are visible in the window

            wx2 = coWinX(x2);
            wy2 = coWinY(y2);

            print_line(wx1, wy1, wx2, wy2, color);
        } else {
            // only (x2,y2) is not visible

            if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_, y1_)) {
                wx1 = coWinX(x1);
                wy1 = coWinY(y1);
                wx2 = coWinX(x2);
                wy2 = coWinY(y2);
                print_line(wx1, wy1, wx2, wy2, color);
            }
        }
    } else {
        if (x2 >= x0_ && x2 <= x1_ && y2 >= y0_ && y2 <= y1_) {
            // only (x2,y2) is visible

            if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_, y1_)) {
                wx1 = coWinX(x1);
                wy1 = coWinY(y1);
                wx2 = coWinX(x2);
                wy2 = coWinY(y2);
                print_line(wx1, wy1, wx2, wy2, color);
            }
        } else {
            // both end points are invisible

            if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_, y1_)) {
                wx1 = coWinX(x1);
                wy1 = coWinY(y1);
                wx2 = coWinX(x2);
                wy2 = coWinY(y2);
                print_line(wx1, wy1, wx2, wy2, color);
            }
        }
    }
}

void P4WinSphere::printPoint(double x, double y, int color)
{
    if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
        return;

    print_point(coWinX(x), coWinY(y), color);
}

void P4WinSphere::refresh()
{
    isPainterCacheDirty_ = true;
    update();
}

void P4WinSphere::calculateHeightFromWidth(int *width, int *height,
                                           int maxheight = -1,
                                           double aspectratio = 1)
{
    // given an optimal width in *width, this procedure calculates the
    // corresponding height in order to maintain the given aspectratio
    // If however the maximum height is violated, then we choose to
    // have the maximum height and calculate the corresponding width.

    // aspect ratio is

    double w_, h_;

    w_ = (double)(*width);
    h_ = w_ * dy_ / dx_;
    h_ *= aspectratio;

    if ((int)(h_ + 0.5) <= maxheight || maxheight == -1) {
        *height = (int)(h_ + 0.5);
    } else {
        *height = maxheight;

        h_ = (double)maxheight;
        w_ = h_ * dx_ / dy_;
        w_ /= aspectratio;

        *width = (int)(w_ + 0.5);
    }
}

void P4WinSphere::preparePrinting(int printmethod, bool isblackwhite,
                                  int myresolution, double mylinewidth,
                                  double mysymbolsize)
{
    double aspectratio, lw, ss, hpixels, maxvpixels, pagewidth, pageheight, tx,
        ty;

    printMethod_ = printmethod;

    aspectratio = 1;  // assume aspect ratio 1

    if (printmethod == P4PRINT_DEFAULT) {
        // g_p4printer->setResolution(myresolution); we have moved this.
        // according to documentation: set of resolution must be done BEFORE
        // setup!

        //  aspectratio = metrics.logicalDpiX()/metrics.logicalDpiY();
        aspectratio = 1;

        pagewidth = g_p4printer->width();
        pageheight = g_p4printer->height();
    } else
        pagewidth = pageheight = -1;  // will be redefined in a minut

    s_p4pixmap_dpm = lw = ss = hpixels = myresolution;
    s_p4pixmap_dpm /= 2.54;
    hpixels *= 15;
    hpixels /= 2.54;

    maxvpixels = myresolution;
    maxvpixels *= aspectratio;
    maxvpixels *= 20;
    maxvpixels /= 2.54;
    maxvpixels += 0.5;

    oldw_ = w_;
    oldh_ = h_;
    w_ = (int)(hpixels + 0.5);

    switch (printmethod) {
    case P4PRINT_DEFAULT: /* pagewidth and height already set */
        break;
    case P4PRINT_JPEGIMAGE:
        pagewidth = -1;
        pageheight = -1;
        break;
    case P4PRINT_XFIGIMAGE:
        pagewidth = -1;
        pageheight = -1;
        break;
    case P4PRINT_EPSIMAGE:
        pagewidth = POSTSCRIPTPAGEWIDTH;  // see custom.cpp
        pageheight = POSTSCRIPTPAGEHEIGHT;
        pagewidth *= myresolution;
        pageheight *= myresolution;
        break;
    }

    if (w_ > pagewidth && pagewidth != -1)
        w_ = (int)pagewidth;
    if (maxvpixels > pageheight && pageheight != -1)
        maxvpixels = pageheight;

    calculateHeightFromWidth(&w_, &h_, (int)maxvpixels, aspectratio);

    lw /= 25.4;         // dots per mm
    lw *= mylinewidth;  // linewidth in pixels
    lw += 0.5;          // prepare round above
    if (lw < 1.0)
        lw = 1.0;

    ss /= 25.4;          // dots per mm
    ss *= mysymbolsize;  // symbolsize in pixels
    ss += 0.5;           // prepare round above for units of 2
    ss /= 2.0;
    if (ss < 1.0)
        ss = 1.0;

    if (pagewidth == -1 || pageheight == -1) {
        tx = 0;
        ty = 0;
    } else {
        tx = pagewidth;
        tx -= w_;
        tx /= 2;
        ty = pageheight;
        ty -= h_;
        ty /= 2;
    }

    switch (printmethod) {
    case P4PRINT_EPSIMAGE:
        reverseYAxis_ = true;
        preparePostscriptPrinting((int)(tx + 0.5), (int)(ty + 0.5), w_, h_,
                                  iszoom_, isblackwhite, myresolution, (int)lw,
                                  2 * (int)ss);
        break;
    case P4PRINT_XFIGIMAGE:
        reverseYAxis_ = false;
        prepareXFigPrinting(w_, h_, iszoom_, isblackwhite, myresolution,
                            (int)lw, 2 * (int)ss);
        break;
    case P4PRINT_DEFAULT:
        staticPainter_ = new QPainter();

        if (!staticPainter_->begin(g_p4printer)) {
            delete staticPainter_;
            staticPainter_ = nullptr;
            return;
        }

        staticPainter_->translate(tx, ty);
        if (iszoom_ || g_VFResults.typeofview_ == TYPEOFVIEW_PLANE) {
            QPen p = QPen(QXFIGCOLOR(printColorTable(bgColours::CFOREGROUND)),
                          (int)lw);
            staticPainter_->setPen(p);
            staticPainter_->drawRect(0, 0, w_, h_);
        }
        //      staticPainter_->setClipRect( (int)tx, (int)ty, w_, h_ );
        reverseYAxis_ = false;  // no need for reversing axes in this case
        prepareP4Printing(w_, h_, isblackwhite, staticPainter_, (int)lw,
                          2 * (int)ss);
        break;

    case P4PRINT_JPEGIMAGE:
        staticPainter_ = new QPainter();
        s_p4pixmap = new QPixmap(w_, h_);
        reverseYAxis_ = false;  // no need for reversing axes in this case
        if (s_p4pixmap->isNull()) {
            msgBar_->showMessage(
                "Print failure (try to choose a lower resolution).");
            delete s_p4pixmap;
            s_p4pixmap = nullptr;
            delete staticPainter_;
            staticPainter_ = nullptr;
            return;
        }
        if (!staticPainter_->begin(s_p4pixmap)) {
            delete s_p4pixmap;
            s_p4pixmap = nullptr;
            delete staticPainter_;
            staticPainter_ = nullptr;
            return;
        }

        prepareP4Printing(w_, h_, isblackwhite, staticPainter_, (int)lw,
                          2 * (int)ss);
        break;
    }

    msgBar_->showMessage("Printing ...");
}

void P4WinSphere::finishPrinting()
{
    if (printMethod_ == P4PRINT_EPSIMAGE) {
        finishPostscriptPrinting();
        reverseYAxis_ = false;
        w_ = oldw_;
        h_ = oldh_;
    } else if (printMethod_ == P4PRINT_XFIGIMAGE) {
        finishXFigPrinting();
        reverseYAxis_ = false;
        w_ = oldw_;
        h_ = oldh_;
    } else if (printMethod_ == P4PRINT_DEFAULT) {
        finishP4Printing();
        staticPainter_->end();
        delete staticPainter_;
        staticPainter_ = nullptr;
        w_ = oldw_;
        h_ = oldh_;
        reverseYAxis_ = false;
    } else if (printMethod_ == P4PRINT_JPEGIMAGE) {
        if (s_p4pixmap == nullptr) {
            finishP4Printing();
            w_ = oldw_;
            h_ = oldh_;
            reverseYAxis_ = false;
            return;
        }

        finishP4Printing();
        staticPainter_->end();
        delete staticPainter_;
        staticPainter_ = nullptr;

        if (s_p4pixmap->save(g_ThisVF->getbarefilename() + ".jpg", "JPEG",
                             100) == false) {
            QMessageBox::critical(this, "P4",
                                  "For some reason, P4 is unable "
                                  "to save the resulting JPEG "
                                  "image to disc.");
        }

        delete s_p4pixmap;
        s_p4pixmap = nullptr;
        reverseYAxis_ = false;
        w_ = oldw_;
        h_ = oldh_;
    }
    msgBar_->showMessage("Printing has finished.");
}

void P4WinSphere::print()
{
    if (printMethod_ == P4PRINT_JPEGIMAGE && s_p4pixmap == nullptr)
        return;

    if (g_VFResults.typeofview_ != TYPEOFVIEW_PLANE) {
        if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
            if (g_VFResults.plweights_)
                printPoincareLyapunovSphere();
            else
                printPoincareSphere();
        } else
            printLineAtInfinity();
    }
    printOrbits();
    printSeparatrices();
    printGcf();
    printCurve();
    printIsoclines();
    printLimitCycles();
    printPoints();
}

void P4WinSphere::prepareDrawing()
{
    if (painterCache_ == nullptr) {
        isPainterCacheDirty_ = true;
        painterCache_ = new QPixmap(size());
    }
    staticPainter_ = new QPainter(painterCache_);

    paintedXMin_ = width() - 1;
    paintedYMin_ = height() - 1;
    paintedXMax_ = 0;
    paintedYMax_ = 0;

    if (next_ != nullptr)
        next_->prepareDrawing();
}

void P4WinSphere::finishDrawing()
{
    if (next_ != nullptr)
        next_->finishDrawing();

    if (staticPainter_ != nullptr) {
        staticPainter_->end();
        delete staticPainter_;
        staticPainter_ = nullptr;

        if (paintedXMin_ < 0)
            paintedXMin_ = 0;
        if (paintedXMax_ >= width())
            paintedXMax_ = width() - 1;
        if (paintedYMin_ < 0)
            paintedYMin_ = 0;
        if (paintedYMax_ >= height())
            paintedYMax_ = height() - 1;

        if (paintedYMax_ >= paintedYMin_ && paintedXMax_ >= paintedXMin_)
            update(paintedXMin_, paintedYMin_, paintedXMax_ - paintedXMin_ + 1,
                   paintedYMax_ - paintedYMin_ + 1);
    }
}

void P4WinSphere::signalEvaluating()
{
    /*
        QPalette palette;
        palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor_ =
       bgColours::CBACKGROUND) );
        setPalette(palette);
    */
}

void P4WinSphere::signalChanged()
{
    /*
        QPalette palette;
        palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor_ = DARKGRAY)
       );
        setPalette(palette);
    */
}

void P4WinSphere::plotSeparatringCurves()
{
    bool dashes;
    double pcoord[3];
    p4orbits::orbits_points sep;

    if (g_VFResults.curves_result_.empty())
        return;

    for (int k = 0; k < g_ThisVF->numCurves_; k++) {
        dashes = true;
        sep = g_VFResults.curves_result_[k].sep_points;
        for (auto it = sep.begin(); it != sep.eng(); ++it) {
            if (it->color == CSEPCURVE) {
                if (it->dashes && dashes)
                    (*plot_l)(this, pcoord, it->pcoord, it->color);
                else {
                    if (it + 1 == sep.end())
                        (*plot_p)(this, it->pcoord, it->color);
                    else if ((it + 1)->dashes || (it + 1)->color == CSEPCURVE ||
                             dashes)
                        // draw nothing when the next point is a dash
                        (*plot_p)(this, it->pcoord, it->color);
                }
                dashes = true;
            } else {
                dashes = false;
            }
            copy_x_into_y(it->pcoord, pcoord);
        }
    }
}