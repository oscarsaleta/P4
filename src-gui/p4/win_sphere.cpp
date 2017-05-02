/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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
#include "math_findpoint.h"
#include "math_gcf.h"
#include "math_curve.h"
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
#include "win_event.h"
#include "win_print.h"

#include <QMessageBox>

#include <cmath>

static QPixmap *p4pixmap = nullptr;
static double p4pixmap_dpm = 0;

int QWinSphere::numSpheres = 0;
QWinSphere **QWinSphere::SphereList = nullptr;

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

// parameters _x1,... are irrelevant if isZoom is false

QWinSphere::QWinSphere(QWidget *parent, QStatusBar *bar, bool isZoom,
                       double _x1, double _y1, double _x2, double _y2)
    : QWidget(parent)
{
    ReverseYaxis = false;
    PainterCache = nullptr;
    isPainterCacheDirty = true;
    AnchorMap = nullptr;
    refreshTimeout = nullptr;
    SelectingTimer = nullptr;

    //    setAttribute( Qt::WA_PaintOnScreen );

    SphereList = (QWinSphere **)realloc(SphereList, sizeof(QWinSphere *) *
                                                        (numSpheres + 1));
    SphereList[numSpheres++] = this;
    if (numSpheres > 1) {
        SphereList[numSpheres - 2]->next = this;
    }

    parentWnd = parent;
    setMinimumSize(MINWIDTHPLOTWINDOW,
                   MINHEIGHTPLOTWINDOW); // THIS IS THE MINIMUM SIZE
    w = width();
    h = height();
    idealh = w;
    SelectingPointStep = 0;

    horPixelsPerMM = ((double)w) / ((double)widthMM());
    verPixelsPerMM = ((double)h) / ((double)heightMM());

    setMouseTracking(true);
    msgBar = bar;
    selectingZoom = false;
    selectingLCSection = false;
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(windowFlags());
    next = nullptr;

    iszoom = isZoom;
    if (isZoom) {
        x0 = _x1;
        y0 = _y1;

        x1 = _x2;
        y1 = _y2;
    }

    CircleAtInfinity = nullptr;
    PLCircle = nullptr;
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

void QWinSphere::keyPressEvent(QKeyEvent *e)
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
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }
    if (id == Qt::Key_C && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // C: continue integrate orbit

        data1 = new int;
        *data1 = 0;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }
    if (id == Qt::Key_B && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // B: integrate orbit backwards in time

        data1 = new int;
        *data1 = -1;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }
    if (id == Qt::Key_D && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // D: delete orbit

        data1 = new int;
        *data1 = 2;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }

    if (id == Qt::Key_A && (bs == Qt::NoModifier || bs == Qt::AltModifier)) {
        // A: delete all orbits

        data1 = new int;
        *data1 = 3;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_ORBIT_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }

    if (id == Qt::Key_C && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+C:  continue integrating separatrice

        data1 = new int;
        *data1 = 0;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }

    if (id == Qt::Key_N && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+N: select next separatrice

        data1 = new int;
        *data1 = 3;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }

    if (id == Qt::Key_I && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+I: integrate next separatrice

        data1 = new int;
        *data1 = 2;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }

    if (id == Qt::Key_S && (bs == Qt::ShiftModifier ||
                            bs == Qt::AltModifier + Qt::ShiftModifier)) {
        // SHIFT+S: start integrate separatrice

        data1 = new int;
        *data1 = 1;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
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

void QWinSphere::SetupPlot(void)
{
    struct P4POLYLINES *t;
    QPalette palette;

    spherebgcolor = CBACKGROUND;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor));
    setPalette(palette);

    while (CircleAtInfinity != nullptr) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t; // free( t );
        t = nullptr;
    }
    while (PLCircle != nullptr) {
        t = PLCircle;
        PLCircle = t->next;
        delete t; // free( t );
        t = nullptr;
    }

    if (!iszoom) {
        switch (VFResults.typeofview) {
        case TYPEOFVIEW_PLANE:
        case TYPEOFVIEW_U1:
        case TYPEOFVIEW_U2:
        case TYPEOFVIEW_V1:
        case TYPEOFVIEW_V2:
            x0 = VFResults.xmin;
            y0 = VFResults.ymin;
            x1 = VFResults.xmax;
            y1 = VFResults.ymax;
            break;
        case TYPEOFVIEW_SPHERE:
            x0 = -1.1;
            y0 = -1.1;
            x1 = 1.1;
            y1 = 1.1;
            break;
        }
    }

    dx = x1 - x0;
    dy = y1 - y0;

    double idealhd;

    idealhd = w;
    idealhd = (idealhd / dx) * dy;

    idealh = (int)(idealhd + .5);

    switch (VFResults.typeofview) {
    case TYPEOFVIEW_PLANE:
        chartstring = "";
        break;
    case TYPEOFVIEW_SPHERE:
        chartstring = "";
        break;
    case TYPEOFVIEW_U1:
        chartstring = makechartstring(VFResults.p, VFResults.q, true, false);
        break;
    case TYPEOFVIEW_U2:
        chartstring = makechartstring(VFResults.p, VFResults.q, false, false);
        break;
    case TYPEOFVIEW_V1:
        chartstring = makechartstring(VFResults.p, VFResults.q, true, true);
        break;
    case TYPEOFVIEW_V2:
        chartstring = makechartstring(VFResults.p, VFResults.q, false, true);
        break;
    }

    if (VFResults.typeofview == TYPEOFVIEW_SPHERE) {
        CircleAtInfinity =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (VFResults.plweights)
            PLCircle = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                      coWinH(RADIUS), coWinV(RADIUS));
    }

    isPainterCacheDirty = true;
}

QWinSphere::~QWinSphere()
{
    int i;

    struct P4POLYLINES *t;
    while (CircleAtInfinity != nullptr) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t; // free( t );
        t = nullptr;
    }
    while (PLCircle != nullptr) {
        t = PLCircle;
        PLCircle = t->next;
        delete t; // free( t );
        t = nullptr;
    }

    for (i = 0; i < numSpheres; i++) {
        if (SphereList[i] == this)
            break;
    }
    if (i == numSpheres)
        return; // error: sphere not found?

    if (i > 0)
        SphereList[i - 1]->next = next;

    if (i < numSpheres - 1)
        memmove(SphereList + i, SphereList + i + 1,
                sizeof(QWinSphere *) * (numSpheres - i - 1));

    numSpheres--;

    if (PainterCache != nullptr) {
        delete PainterCache;
        PainterCache = nullptr;
    }

    if (AnchorMap != nullptr) {
        delete AnchorMap;
        AnchorMap = nullptr;
    }

    if (refreshTimeout != nullptr) {
        delete refreshTimeout;
        refreshTimeout = nullptr;
    }
    if (SelectingTimer != nullptr) {
        delete SelectingTimer;
        SelectingTimer = nullptr;
    }
}

void QWinSphere::LoadAnchorMap(void)
{
    int x1, y1;
    int x2, y2;
    int aw, ah;
    int s;
    if (selectingZoom) {
        x1 = zoomAnchor1.x();
        y1 = zoomAnchor1.y();
        x2 = zoomAnchor2.x();
        y2 = zoomAnchor2.y();
    } else if (selectingLCSection) {
        x1 = lcAnchor1.x();
        y1 = lcAnchor1.y();
        x2 = lcAnchor2.x();
        y2 = lcAnchor2.y();
    } else
        return;

    if (x1 > x2) {
        s = x1;
        x1 = x2;
        x2 = s;
    }
    if (y1 > y2) {
        s = y1;
        y1 = y2;
        y2 = s;
    }
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 >= width())
        x2 = width() - 1;
    if (y2 >= height())
        y2 = height() - 1;

    aw = x2 - x1 + 1;
    ah = y2 - y1 + 1;

    /*     QString ms = msgBar->currentMessage();
         QString as;
         as.sprintf( " Load: (%d,%d,%d,%d)", x1,y1,aw,ah );
         msgBar->showMessage(as+ms);
    */
    if (AnchorMap != nullptr) {
        if (AnchorMap->width() < aw || AnchorMap->height() < ah) {
            delete AnchorMap;
            AnchorMap = nullptr;
            AnchorMap = new QPixmap(aw, ah);
        }
    } else {
        AnchorMap = new QPixmap(aw, ah);
    }

    if (PainterCache == nullptr) {
        delete AnchorMap;
        AnchorMap = nullptr;
        return;
    }

    QPainter paint(AnchorMap);
    if (selectingZoom) {
        // only copy rectangular edges, not inside
        //   paint.drawPixmap (    0,    0, aw,  1, *PainterCache, x1, y1, aw, 1
        //   );
        //     paint.drawPixmap (    0, ah-1, aw,  1, *PainterCache, x1, y2, aw,
        //     1 );
        //       paint.drawPixmap (    0,    0,  1, ah, *PainterCache, x1, y1,
        //       1, ah );
        //         paint.drawPixmap ( aw-1, 0,     1, ah, *PainterCache, x2, y1,
        //         1, ah );

        paint.drawPixmap(0, 0, aw, ah, *PainterCache, x1, y1, aw, ah);

    } else {
        paint.drawPixmap(0, 0, aw, ah, *PainterCache, x1, y1, aw, ah);
    }
}

void QWinSphere::SaveAnchorMap(void)
{
    int x1, y1;
    int x2, y2;
    int aw, ah;
    int s;

    if (AnchorMap == nullptr || PainterCache == nullptr ||
        (!selectingZoom && !selectingLCSection))
        return;

    if (selectingZoom) {
        x1 = zoomAnchor1.x();
        y1 = zoomAnchor1.y();
        x2 = zoomAnchor2.x();
        y2 = zoomAnchor2.y();
    }
    if (selectingLCSection) {
        x1 = lcAnchor1.x();
        y1 = lcAnchor1.y();
        x2 = lcAnchor2.x();
        y2 = lcAnchor2.y();
    }

    if (x1 > x2) {
        s = x1;
        x1 = x2;
        x2 = s;
    }
    if (y1 > y2) {
        s = y1;
        y1 = y2;
        y2 = s;
    }
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 >= width())
        x2 = width() - 1;
    if (y2 >= height())
        y2 = height() - 1;

    aw = x2 - x1 + 1;
    ah = y2 - y1 + 1;
    /*
         QString ms = msgBar->currentMessage();
         QString as;
         as.sprintf( " Save: (%d,%d,%d,%d)", x1,y1,aw,ah );
         msgBar->showMessage(as+ms);
    */
    QPainter paint(PainterCache);

    if (selectingZoom) {
        // only copy rectangular edges, not inside
        //         paint.drawPixmap ( x1, y1, aw,  1, *AnchorMap,    0,    0,
        //         aw, 1 );
        //       paint.drawPixmap ( x1, y2, aw,  1, *AnchorMap,    0, ah-1, aw,
        //       1 );
        //     paint.drawPixmap ( x1, y1,  1, ah, *AnchorMap,    0,    0, 1, ah
        //     );
        //   paint.drawPixmap ( x2, y1,  1, ah, *AnchorMap, aw-1,    0, 1, ah );
        paint.drawPixmap(x1, y1, aw, ah, *AnchorMap, 0, 0, aw, ah);
    } else {
        paint.drawPixmap(x1, y1, aw, ah, *AnchorMap, 0, 0, aw, ah);
    }

    update(x1, y1, aw, ah);
}

void QWinSphere::adjustToNewSize(void)
{
    double idealhd;
    double reqratio;
    double ratio;
    QString buf;
    struct P4POLYLINES *t;

    w = width();
    h = height();

    idealhd = w;
    idealhd = (idealhd / dx) * dy;

    idealh = (int)(idealhd + .5);

    reqratio = (((double)w) / horPixelsPerMM) / (idealh / verPixelsPerMM);
    ratio = (((double)w) / horPixelsPerMM) / (((double)h) / verPixelsPerMM);

    buf.sprintf("Aspect Ratio = %f\n", (float)(ratio / reqratio));
    msgBar->showMessage(buf);

    while (CircleAtInfinity != nullptr) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t; // free( t );
        t = nullptr;
    }
    while (PLCircle != nullptr) {
        t = PLCircle;
        PLCircle = t->next;
        delete t; // free( t );
        t = nullptr;
    }
    if (VFResults.typeofview == TYPEOFVIEW_SPHERE) {
        CircleAtInfinity =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (VFResults.plweights)
            PLCircle = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                      coWinH(RADIUS), coWinV(RADIUS));
    }

    if (PainterCache != nullptr) {
        delete PainterCache;
        PainterCache = nullptr;
        PainterCache = new QPixmap(size());
        isPainterCacheDirty = false;

        QPainter paint(PainterCache);
        paint.fillRect(0, 0, width(), height(),
                       QColor(QXFIGCOLOR(CBACKGROUND)));

        if (VFResults.singinf)
            paint.setPen(QXFIGCOLOR(CSING));
        else
            paint.setPen(QXFIGCOLOR(CLINEATINFINITY));

        staticPainter = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if (VFResults.typeofview != TYPEOFVIEW_PLANE) {
            if (VFResults.typeofview == TYPEOFVIEW_SPHERE) {
                if (VFResults.plweights)
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

        staticPainter = nullptr;

        if (refreshTimeout != nullptr)
            refreshTimeout->stop();
        else {
            refreshTimeout = new QTimer();
            connect(refreshTimeout, SIGNAL(timeout()), this,
                    SLOT(refreshAfterResize()));
        }
        refreshTimeout->start(500);
    }
}

void QWinSphere::refreshAfterResize(void)
{
    if (refreshTimeout != nullptr) {
        delete refreshTimeout;
        refreshTimeout = nullptr;
    }
    refresh();
}

void QWinSphere::resizeEvent(QResizeEvent *e)
{
    if (e->size() == e->oldSize())
        return;

    adjustToNewSize();
}

void QWinSphere::paintEvent(QPaintEvent *p)
{
    UNUSED(p);
    if (ThisVF->evaluating)
        return;

    if (PainterCache == nullptr || isPainterCacheDirty) {
        if (PainterCache == nullptr)
            PainterCache = new QPixmap(size());
        isPainterCacheDirty = false;

        QPainter paint(PainterCache);
        paint.fillRect(0, 0, width(), height(),
                       QColor(QXFIGCOLOR(CBACKGROUND)));

        if (VFResults.singinf)
            paint.setPen(QXFIGCOLOR(CSING));
        else
            paint.setPen(QXFIGCOLOR(CLINEATINFINITY));

        staticPainter = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if (VFResults.typeofview != TYPEOFVIEW_PLANE) {
            if (VFResults.typeofview == TYPEOFVIEW_SPHERE) {
                if (VFResults.plweights)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }
        plotSeparatrices();
        plotGcf();
        drawOrbits(this);
        drawLimitCycles(this);
        plotPoints();
        staticPainter = nullptr;
    }

    QPainter widgetpaint(this);
    widgetpaint.drawPixmap(0, 0, *PainterCache);

    if (SelectingPointStep != 0) {
        widgetpaint.setPen(QXFIGCOLOR(WHITE));
        widgetpaint.setBrush(Qt::NoBrush);
        widgetpaint.drawEllipse(SelectingX - SelectingPointRadius,
                                SelectingY - SelectingPointRadius,
                                SelectingPointRadius + SelectingPointRadius,
                                SelectingPointRadius + SelectingPointRadius);
    }
}

void QWinSphere::MarkSelection(int x1, int y1, int x2, int y2,
                               int selectiontype)
{
    int bx1, by1, bx2, by2;

    if (PainterCache == nullptr)
        return;

    bx1 = (x1 < x2) ? x1 : x2;
    bx2 = (x1 < x2) ? x2 : x1;
    by1 = (y1 < y2) ? y1 : y2;
    by2 = (y1 < y2) ? y2 : y1;

    if (bx1 < 0)
        bx1 = 0;
    if (by1 < 0)
        by1 = 0;
    if (bx2 >= width())
        bx2 = width() - 1;
    if (by2 >= height())
        by2 = height() - 1;

    QPainter p(PainterCache);
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
        p.drawLine(x1, y1, x2, y2);
        break;
    }
    update(bx1, by1, bx2 - bx1 + 1, by2 - by1 + 1);
}

void QWinSphere::mouseMoveEvent(QMouseEvent *e)
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
        ratio = ( ((double)w)/horPixelsPerMM ) / ( ((double)h)/verPixelsPerMM );
        sprintf( buf, "Aspect Ratio = %f\n", (float)ratio );
        msgBar->showMessage(buf);
        return;
    */
    double pcoord[3];
    if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord)) {
        switch (VFResults.typeofview) {
        case TYPEOFVIEW_PLANE:
            if (VFResults.typeofstudy == TYPEOFSTUDY_ONE)
                buf.sprintf("Local study   (x,y) = (%f,%f)", (float)wx,
                            (float)wy);
            else
                buf.sprintf("Planar view   (x,y) = (%f,%f)", (float)wx,
                            (float)wy);
            break;
        case TYPEOFVIEW_SPHERE:
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

            if (VFResults.p == 1 && VFResults.q == 1)
                buf.sprintf("The Poincare sphere  (x,y) = (%f,%f)",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The P-L sphere of type (%d,%d)  (x,y) = (%f,%f)",
                            VFResults.p, VFResults.q, (float)ucoord[0],
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
            buf.append(chartstring);
            break;
        case TYPEOFVIEW_V1:
            MATHFUNC(sphere_to_V1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!VFResults.plweights) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V1 chart  (z2,z1) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The U1' chart (z2,z1) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            buf.append(chartstring);
            break;
        case TYPEOFVIEW_U2:
            MATHFUNC(sphere_to_U2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U2 chart  (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The V2' chart (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            buf.append(chartstring);
            break;
        case TYPEOFVIEW_V2:
            MATHFUNC(sphere_to_V2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!VFResults.plweights) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V2 chart  (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The U2' chart (z1,z2) = (%f,%f) ",
                            (float)ucoord[0], (float)ucoord[1]);
            buf.append(chartstring);
            break;
        }
    } else {
        switch (VFResults.typeofview) {
        case TYPEOFVIEW_PLANE:
            if (VFResults.typeofstudy == TYPEOFSTUDY_ONE)
                buf.sprintf("Local study");
            else
                buf.sprintf("Planar view");
            break;
        case TYPEOFVIEW_SPHERE:
            if (VFResults.p == 1 && VFResults.q == 1)
                buf.sprintf("The Poincare sphere");
            else
                buf.sprintf("The P-L sphere of type (%d,%d)", VFResults.p,
                            VFResults.q);
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

    msgBar->showMessage(buf);

    if (selectingZoom) {
        SaveAnchorMap();
        zoomAnchor2 = e->pos();
        LoadAnchorMap();
        MarkSelection(zoomAnchor1.x(), zoomAnchor1.y(), zoomAnchor2.x(),
                      zoomAnchor2.y(), 0);
    }

    if (selectingLCSection) {
        SaveAnchorMap();
        lcAnchor2 = e->pos();
        LoadAnchorMap();
        MarkSelection(lcAnchor1.x(), lcAnchor1.y(), lcAnchor2.x(),
                      lcAnchor2.y(), 1);
    }
}

double QWinSphere::coWorldX(int x)
{
    double wx;

    wx = (double)x;
    wx /= (w - 1);
    return (wx * dx + x0);
}

double QWinSphere::coWorldY(int y)
{
    double wy;

    wy = (double)(h - 1 - y);
    wy /= (h - 1);
    return (wy * dy + y0);
}

int QWinSphere::coWinX(double x)
{
    double wx;
    int iwx;

    wx = (x - x0) / dx;
    wx *= w - 1;

    iwx = (int)(wx + 0.5); // +0.5 to round upwards
    if (iwx >= w)
        iwx = w - 1;

    return iwx;
}

int QWinSphere::coWinH(double deltax)
{
    double wx;

    wx = deltax / dx;
    wx *= w - 1;
    return (int)(wx + 0.5);
}

int QWinSphere::coWinV(double deltay)
{
    double wy;

    wy = deltay / dy;
    wy *= h - 1;
    return (int)(wy + 0.5);
}

int QWinSphere::coWinY(double y)
{
    double wy;
    int iwy;

    wy = (y - y0) / dy;
    wy *= h - 1;

    iwy = (int)(wy + 0.5); // +0.5 to round upwards
    if (iwy >= h)
        iwy = h - 1;

    return (ReverseYaxis)
               ? iwy
               : h - 1 -
                     iwy; // on screen: vertical axis orientation is reversed
}

void QWinSphere::mousePressEvent(QMouseEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier &&
        e->button() == Qt::LeftButton) {
        if (!selectingZoom) {
            selectingZoom = true;
            zoomAnchor1 = e->pos();
            zoomAnchor2 = zoomAnchor1;
            LoadAnchorMap();
            MarkSelection(zoomAnchor1.x(), zoomAnchor1.y(), zoomAnchor2.x(),
                          zoomAnchor2.y(), 0);
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if (e->modifiers() == Qt::ShiftModifier && e->button() == Qt::LeftButton) {
        // select nearest singularity having separatrices, and open the
        // separatrices window.

        SelectNearestSingularity(e->pos());

        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::LeftButton &&
        (lcWindowIsUp || e->modifiers() == Qt::AltModifier)) {
        if (selectingLCSection == false) {
            selectingLCSection = true;
            lcAnchor1 = e->pos();
            lcAnchor2 = lcAnchor1;

            LoadAnchorMap();
            MarkSelection(lcAnchor1.x(), lcAnchor1.y(), lcAnchor2.x(),
                          lcAnchor2.y(), 0);
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::LeftButton) {
        // normally, start integrating new orbit at the chosen point.
        //
        // However, when the limit cycle window is open, select the first
        // and second point of a transverse section.

        struct DOUBLEPOINT *data1 = new DOUBLEPOINT;//(struct DOUBLEPOINT *)malloc(sizeof(struct DOUBLEPOINT) + sizeof(void *));
        data1->x = coWorldX(e->x());
        data1->y = coWorldY(e->y());
        *((void **)(data1 + 1)) = this;

        double pcoord[3];
        if (MATHFUNC(is_valid_viewcoord)(data1->x, data1->y, pcoord)) {
            QP4Event *e1;
            e1 = new QP4Event((QEvent::Type)TYPE_SELECT_ORBIT, data1);
            p4app->postEvent(parentWnd, e1);
        } else {
            free(data1);
        }

        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::RightButton) {
        // cancel zoom window with right mouse button

        if (selectingZoom) {
            SaveAnchorMap();
            selectingZoom = false;
        }

        if (selectingLCSection) {
            SaveAnchorMap();
            selectingLCSection = false;
        }

        QWidget::mousePressEvent(e);
        return;
    }

    QWidget::mousePressEvent(e);
}

void QWinSphere::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // finish zoom window between zoomAnchor1 and zoomAnchor2

        if (selectingZoom) {
            SaveAnchorMap();
            selectingZoom = false;

            double *data1 =
                new double[4];
            data1[0] = coWorldX(zoomAnchor1.x());
            data1[1] = coWorldY(zoomAnchor1.y());
            data1[2] = coWorldX(zoomAnchor2.x());
            data1[3] = coWorldY(zoomAnchor2.y());
            QP4Event *e1 =
                new QP4Event((QEvent::Type)TYPE_OPENZOOMWINDOW, data1);
            p4app->postEvent(parentWnd, e1);
        }

        if (selectingLCSection) {
            SaveAnchorMap();
            selectingLCSection = false;

            double *data1 =
                new double[4];
            data1[0] = coWorldX(lcAnchor1.x());
            data1[1] = coWorldY(lcAnchor1.y());
            data1[2] = coWorldX(lcAnchor2.x());
            data1[3] = coWorldY(lcAnchor2.y());
            QP4Event *e1 =
                new QP4Event((QEvent::Type)TYPE_SELECT_LCSECTION, data1);
            p4app->postEvent(parentWnd, e1);
        }

        QWidget::mouseReleaseEvent(e);
        return;
    }

    QWidget::mouseReleaseEvent(e);
}

bool QWinSphere::getChartPos(int chart, double x0, double y0, double *pos)
{
    double pcoord[3];

    switch (chart) {
    case CHART_R2:
        MATHFUNC(finite_to_viewcoord)(x0, y0, pos);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    }

    return true;
}

void QWinSphere::updatePointSelection(void)
{
    if (SelectingPointStep == 0) {
        SelectingPointRadius = 0;
        update(SelectingX - SELECTINGPOINTSTEPS * 4,
               SelectingY - SELECTINGPOINTSTEPS * 4,
               SELECTINGPOINTSTEPS * 4 * 2 + 1,
               SELECTINGPOINTSTEPS * 4 * 2 + 1);
        return;
    }

    SelectingPointStep--;
    SelectingPointRadius = (SELECTINGPOINTSTEPS - SelectingPointStep) * 4;
    update(SelectingX - SelectingPointRadius, SelectingY - SelectingPointRadius,
           SelectingPointRadius + SelectingPointRadius + 1,
           SelectingPointRadius + SelectingPointRadius + 1);
}

void QWinSphere::SelectNearestSingularity(QPoint winpos)
{
    bool result;
    int x, y;
    int px, py;

    x = winpos.x();
    y = winpos.y();

    (*SphereList)->prepareDrawing();
    result = find_critical_point(*SphereList, coWorldX(x), coWorldY(y));
    (*SphereList)->finishDrawing();

    if (result == false) {
        msgBar->showMessage(
            "Search nearest critical point: None with separatrices found.");
    } else {
        px = coWinX(VFResults.selected_ucoord[0]);
        py = coWinY(VFResults.selected_ucoord[1]);

        if (SelectingTimer != nullptr) {
            delete SelectingTimer;
            SelectingTimer = nullptr;
            SelectingPointStep = 0;
            updatePointSelection();
        }

        SelectingPointStep = SELECTINGPOINTSTEPS - 1;
        SelectingX = px;
        SelectingY = py;

        SelectingTimer = new QTimer();
        connect(SelectingTimer, SIGNAL(timeout()), this,
                SLOT(updatePointSelection()));
        SelectingTimer->start(SELECTINGPOINTSPEED);
        msgBar->showMessage("Search nearest critical point: Found");

        int *data1;
        data1 = new int;
        *data1 = -1;
        QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_SEP_EVENT, data1);
        p4app->postEvent(parentWnd, e1);
    }
}

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void QWinSphere::plotPoint(struct saddle *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);
        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        win_plot_saddle(staticPainter, x, y);
    }
}

void QWinSphere::plotPoint(struct node *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        if (p->stable == -1)
            win_plot_stablenode(staticPainter, x, y);
        else
            win_plot_unstablenode(staticPainter, x, y);
    }
}

void QWinSphere::plotPoint(struct weak_focus *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        switch (p->type) {
        case FOCUSTYPE_STABLE:
            win_plot_stableweakfocus(staticPainter, x, y);
            break;
        case FOCUSTYPE_UNSTABLE:
            win_plot_unstableweakfocus(staticPainter, x, y);
            break;
        case FOCUSTYPE_CENTER:
            win_plot_center(staticPainter, x, y);
            break;
        default:
            win_plot_weakfocus(staticPainter, x, y);
            break;
        }
    }
}

void QWinSphere::plotPoint(struct strong_focus *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        if (p->stable == -1)
            win_plot_stablestrongfocus(staticPainter, x, y);
        else
            win_plot_unstablestrongfocus(staticPainter, x, y);
    }
}

void QWinSphere::plotPoint(struct degenerate *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        win_plot_degen(staticPainter, x, y);
    }
}

void QWinSphere::plotPoint(struct semi_elementary *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        switch (p->type) {
        case 1:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 2:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 3:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 4:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 5:
            win_plot_seunstablenode(staticPainter, x, y);
            break;
        case 6:
            win_plot_sesaddle(staticPainter, x, y);
            break;
        case 7:
            win_plot_sesaddle(staticPainter, x, y);
            break;
        case 8:
            win_plot_sestablenode(staticPainter, x, y);
            break;
        }
    }
}

void QWinSphere::plotPoints(void)
{
    struct saddle *sp;
    struct node *np;
    struct weak_focus *wfp;
    struct strong_focus *sfp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    for (sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle)
        plotPoint(sp);
    for (np = VFResults.first_node_point; np != nullptr; np = np->next_node)
        plotPoint(np);
    for (wfp = VFResults.first_wf_point; wfp != nullptr; wfp = wfp->next_wf)
        plotPoint(wfp);
    for (sfp = VFResults.first_sf_point; sfp != nullptr; sfp = sfp->next_sf)
        plotPoint(sfp);
    for (sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se)
        plotPoint(sep);
    for (dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de)
        plotPoint(dp);
}

void QWinSphere::plotPointSeparatrices(struct semi_elementary *p)
{
    struct sep *separatrice;

    for (separatrice = p->separatrices; separatrice != nullptr;
         separatrice = separatrice->next_sep)
        draw_sep(this, separatrice->first_sep_point);
}

void QWinSphere::plotPointSeparatrices(struct saddle *p)
{
    struct sep *separatrice;

    for (separatrice = p->separatrices; separatrice != nullptr;
         separatrice = separatrice->next_sep)
        draw_sep(this, separatrice->first_sep_point);
}

void QWinSphere::plotPointSeparatrices(struct degenerate *p)
{
    struct blow_up_points *blow_up;

    for (blow_up = p->blow_up; blow_up != nullptr;
         blow_up = blow_up->next_blow_up_point) {
        draw_sep(this, blow_up->first_sep_point);
    }
}

void QWinSphere::plotSeparatrices(void)
{
    struct saddle *sp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    for (sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle)
        plotPointSeparatrices(sp);
    for (sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se)
        plotPointSeparatrices(sep);
    for (dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de)
        plotPointSeparatrices(dp);
}

void QWinSphere::plotGcf(void)
{
    draw_gcf(this, VFResults.gcf_points, CSING, 1);
}

void QWinSphere::plotCurve(void)
{
    draw_curve(this, VFResults.curve_points, CSING, 1);
}

// -----------------------------------------------------------------------
//                          PLOT TOOLS
// -----------------------------------------------------------------------

P4POLYLINES *QWinSphere::produceEllipse(double cx, double cy, double a,
                                        double b, bool dotted, double resa,
                                        double resb)
{
    // this is an exact copy of the plotEllipse routine, except that output is
    // stored
    // in a list of points that is dynamically allocated.

    double theta, t1, t2, e, R, x, y, c, prevx, prevy;
    bool d;
    bool doton;
    int dotcount;
    P4POLYLINES *first;
    P4POLYLINES *last;

    prevx = prevy = 0;
    dotcount = 0;
    first = nullptr;
    last = nullptr;

    R = (resa < resb) ? resa : resb;
    if (R < 1.0)
        R = 1.0; // protection
    e = 2 * acos(1.0 - 0.5 / R);
    if (R * sin(e) > 1.0)
        e = asin(1.0 / R);

    theta = 0;

    d = false;
    doton = true;

    //  FILE * fp;
    //  fp = fopen( "C:\\test.txt", "wt" );

    while (theta < TWOPI) {
        //        fprintf( fp, "%f\n", (float)theta );
        //        fflush(fp);
        c = (x0 - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta >= t1 && theta < t2) {
                //                fprintf( fp, "A EXCL [%f %f]\n", (float)t1,
                //                (float)t2 );
                theta = t2 + e / 4;
                d = false;
                continue;
            }
        }
        c = (x1 - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta < t1) {
                //                fprintf( fp, "B EXCL [-infinity %f]\n",
                //                (float)t1 );
                theta = t1 + e / 4;
                d = false;
                continue;
            }
            if (theta >= t2) {
                //                fprintf( fp, "C EXCL [%f, infinity]\n",
                //                (float)t2 );
                theta = TWOPI + e / 4;
                d = false;
                break;
            }
        }
        c = (y0 - cy) / b;
        if (c > -1.0 && c < 1.0) {
            t1 = asin(c);
            t2 = PI - t1;
            if (t1 < 0) {
                t2 = t1 + TWOPI;
                t1 = PI - t1;
                if (theta >= t1 && theta < t2) {
                    //                    fprintf( fp, "D EXCL [%f %f]\n",
                    //                    (float)t1, (float)t2 );
                    theta = t2 + e / 4;
                    d = false;
                    continue;
                }
            } else {
                if (theta < t1) {
                    //                    fprintf( fp, "E EXCL [-infinity
                    //                    %f]\n", (float)t1 );
                    theta = t1 + e / 4;
                    d = false;
                    continue;
                }
                if (theta >= t2) {
                    //                    fprintf( fp, "F EXCL [%f,
                    //                    infinity]\n",  (float)t2 );
                    theta = TWOPI + e / 4;
                    d = false;
                    break;
                }
            }
        }
        c = (y1 - cy) / b;
        if (c > -1.0 && c < 1.0) {
            t1 = asin(c);
            t2 = PI - t1;
            if (t1 < 0) {
                t2 = t1 + TWOPI;
                t1 = PI - t1;
                if (theta < t1) {
                    //                    fprintf( fp, "G EXCL [-infinity
                    //                    %f]\n", (float)t1 );
                    theta = t1 + e / 4;
                    d = false;
                    continue;
                }
                if (theta >= t2) {
                    //                    fprintf( fp, "H EXCL [%f,
                    //                    infinity]\n",  (float)t2 );
                    theta = TWOPI;
                    d = false;
                    break;
                }
            } else {
                if (theta >= t1 && theta < t2) {
                    //                    fprintf( fp, "I EXCL [%f %f]\n",
                    //                    (float)t1, (float)t2 );
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
                if (first == nullptr) {
                    last = first = new P4POLYLINES;
                    last->next = nullptr;
                } else {
                    last->next = new P4POLYLINES;
                    last = last->next;
                    last->next = nullptr;
                }

                last->x1 = prevx;
                last->y1 = prevy;
                last->x2 = x;
                last->y2 = y;

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
    //  fclose(fp);
    return first;
}

void QWinSphere::plotPoincareSphere(void)
{
    int color;
    P4POLYLINES *p;

    p = CircleAtInfinity;
    color = VFResults.singinf ? CSING : CLINEATINFINITY;

    staticPainter->setPen(QXFIGCOLOR(color));
    while (p != nullptr) {
        staticPainter->drawLine(coWinX(p->x1), coWinY(p->y1), coWinX(p->x2),
                                coWinY(p->y2));
        p = p->next;
    }
}

void QWinSphere::plotPoincareLyapunovSphere(void)
{
    int color;
    P4POLYLINES *p;

    p = CircleAtInfinity;
    color = VFResults.singinf ? CSING : CLINEATINFINITY;

    staticPainter->setPen(QXFIGCOLOR(color));
    while (p != nullptr) {
        staticPainter->drawLine(coWinX(p->x1), coWinY(p->y1), coWinX(p->x2),
                                coWinY(p->y2));
        p = p->next;
    }

    p = PLCircle;
    color = CLINEATINFINITY;

    staticPainter->setPen(QXFIGCOLOR(color));
    while (p != nullptr) {
        staticPainter->drawLine(coWinX(p->x1), coWinY(p->y1), coWinX(p->x2),
                                coWinY(p->y2));
        p = p->next;
    }
    return;
}

void QWinSphere::plotLineAtInfinity(void)
{
    switch (VFResults.typeofview) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0 < 0.0 && x1 > 0.0) {
            staticPainter->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter->drawLine(coWinX(0.0), 0, coWinX(0.0), h - 1);
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0 < 0.0 && y1 > 0.0) {
            staticPainter->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter->drawLine(0, coWinY(0.0), w - 1, coWinY(0.0));
        }

        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}

void QWinSphere::drawLine(double _x1, double _y1, double _x2, double _y2,
                          int color)
{
    int wx1, wy1, wx2, wy2;

    if (staticPainter != nullptr) {
        if (_x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1) {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);

            if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
                // both points are visible in the window

                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);

                if (paintedXMin > wx1)
                    paintedXMin = wx1;
                if (paintedXMax < wx1)
                    paintedXMax = wx1;
                if (paintedYMin > wy1)
                    paintedYMin = wy1;
                if (paintedYMax < wy1)
                    paintedYMax = wy1;
                if (paintedXMin > wx2)
                    paintedXMin = wx2;
                if (paintedXMax < wx2)
                    paintedXMax = wx2;
                if (paintedYMin > wy2)
                    paintedYMin = wy2;
                if (paintedYMax < wy2)
                    paintedYMax = wy2;

                staticPainter->setPen(QXFIGCOLOR(color));
                staticPainter->drawLine(wx1, wy1, wx2, wy2);
            } else {
                // only (_x2,_y2) is not visible

                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0,
                                           y1)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen(QXFIGCOLOR(color));

                    if (paintedXMin > wx1)
                        paintedXMin = wx1;
                    if (paintedXMax < wx1)
                        paintedXMax = wx1;
                    if (paintedYMin > wy1)
                        paintedYMin = wy1;
                    if (paintedYMax < wy1)
                        paintedYMax = wy1;
                    if (paintedXMin > wx2)
                        paintedXMin = wx2;
                    if (paintedXMax < wx2)
                        paintedXMax = wx2;
                    if (paintedYMin > wy2)
                        paintedYMin = wy2;
                    if (paintedYMax < wy2)
                        paintedYMax = wy2;

                    staticPainter->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        } else {
            if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
                // only (_x2,_y2) is visible

                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0,
                                           y1)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen(QXFIGCOLOR(color));

                    if (paintedXMin > wx1)
                        paintedXMin = wx1;
                    if (paintedXMax < wx1)
                        paintedXMax = wx1;
                    if (paintedYMin > wy1)
                        paintedYMin = wy1;
                    if (paintedYMax < wy1)
                        paintedYMax = wy1;
                    if (paintedXMin > wx2)
                        paintedXMin = wx2;
                    if (paintedXMax < wx2)
                        paintedXMax = wx2;
                    if (paintedYMin > wy2)
                        paintedYMin = wy2;
                    if (paintedYMax < wy2)
                        paintedYMax = wy2;

                    staticPainter->drawLine(wx1, wy1, wx2, wy2);
                }
            } else {
                // both end points are invisible

                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0,
                                           y1)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen(QXFIGCOLOR(color));

                    if (paintedXMin > wx1)
                        paintedXMin = wx1;
                    if (paintedXMax < wx1)
                        paintedXMax = wx1;
                    if (paintedYMin > wy1)
                        paintedYMin = wy1;
                    if (paintedYMax < wy1)
                        paintedYMax = wy1;
                    if (paintedXMin > wx2)
                        paintedXMin = wx2;
                    if (paintedXMax < wx2)
                        paintedXMax = wx2;
                    if (paintedYMin > wy2)
                        paintedYMin = wy2;
                    if (paintedYMax < wy2)
                        paintedYMax = wy2;

                    staticPainter->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        }
    }
}

void QWinSphere::drawPoint(double x, double y, int color)
{
    int _x, _y;
    if (staticPainter != nullptr) {
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return;
        staticPainter->setPen(QXFIGCOLOR(color));
        _x = coWinX(x);
        _y = coWinY(y);

        if (paintedXMin > _x)
            paintedXMin = _x;
        if (paintedXMax < _x)
            paintedXMax = _x;
        if (paintedYMin > _y)
            paintedYMin = _y;
        if (paintedYMax < _y)
            paintedYMax = _y;

        staticPainter->drawPoint(_x, _y);
    }
}

//---------------------------------------------------------------------
//                  PRINTING METHODS
//---------------------------------------------------------------------

void QWinSphere::printPoint(struct saddle *p)
{
    double pos[2];
    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);
        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;
        print_saddle(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void QWinSphere::printPoint(struct node *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        if (p->stable == -1)
            print_stablenode(coWinX(pos[0]), coWinY(pos[1]));
        else
            print_unstablenode(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void QWinSphere::printPoint(struct weak_focus *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
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

void QWinSphere::printPoint(struct strong_focus *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        if (p->stable == -1)
            print_stablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
        else
            print_unstablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void QWinSphere::printPoint(struct degenerate *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        print_degen(coWinX(pos[0]), coWinY(pos[1]));
    }
}

void QWinSphere::printPoint(struct semi_elementary *p)
{
    double pos[2];

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
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

void QWinSphere::printPoints(void)
{
    struct saddle *sp;
    struct node *np;
    struct weak_focus *wfp;
    struct strong_focus *sfp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    print_comment("Printing symbols at all singular points:");

    for (sp = VFResults.first_saddle_point; sp != nullptr; sp = sp->next_saddle)
        printPoint(sp);
    for (np = VFResults.first_node_point; np != nullptr; np = np->next_node)
        printPoint(np);
    for (wfp = VFResults.first_wf_point; wfp != nullptr; wfp = wfp->next_wf)
        printPoint(wfp);
    for (sfp = VFResults.first_sf_point; sfp != nullptr; sfp = sfp->next_sf)
        printPoint(sfp);
    for (sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se)
        printPoint(sep);
    for (dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de)
        printPoint(dp);
}

void QWinSphere::printPointSeparatrices(struct semi_elementary *p)
{
    struct sep *separatrice;

    for (separatrice = p->separatrices; separatrice != nullptr;
         separatrice = separatrice->next_sep) {
        draw_sep(this, separatrice->first_sep_point);
        if (separatrice->next_sep != nullptr) {
            print_comment("Next separatrix of degenerate point:");
        }
    }
}

void QWinSphere::printPointSeparatrices(struct saddle *p)
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

void QWinSphere::printPointSeparatrices(struct degenerate *p)
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

void QWinSphere::printSeparatrices(void)
{
    QString comment;
    struct saddle *sp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    for (sp = VFResults.first_saddle_point; sp != nullptr;
         sp = sp->next_saddle) {
        comment = "Printing separatrice for saddle singularity:";
        print_comment(comment);
        printPointSeparatrices(sp);
    }
    for (sep = VFResults.first_se_point; sep != nullptr; sep = sep->next_se) {
        comment = "Printing separatrices for semi-hyperbolic singularity:";
        print_comment(comment);
        printPointSeparatrices(sep);
    }
    for (dp = VFResults.first_de_point; dp != nullptr; dp = dp->next_de) {
        comment = "Printing separatrices for degenerate singularity:";
        print_comment(comment);
        printPointSeparatrices(dp);
    }
}

void QWinSphere::printGcf(void)
{
    QString comment;

    if (VFResults.gcf_points != nullptr) {
        comment = "Printing Greatest common factor:";
        print_comment(comment);
        draw_gcf(this, VFResults.gcf_points, CSING, 1);
    }
}

void QWinSphere::printCurve(void)
{
    QString comment;

    if (VFResults.curve_points != nullptr) {
        comment = "Printing Greatest common factor:";
        print_comment(comment);
        draw_curve(this, VFResults.curve_points, CSING, 1);
    }
}

void QWinSphere::printPoincareSphere(void)
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
    print_elips(coWinX(0), coWinY(0), coWinH(1), coWinV(1),
                VFResults.singinf ? CSING : CLINEATINFINITY, false, p);

    while (p != nullptr) {
        q = p;
        p = p->next;
        delete q; // free( q );
        q = nullptr;
    }
}

void QWinSphere::printPoincareLyapunovSphere(void)
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
        delete q; // free( q );
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
        delete q; // free( q );
        q = nullptr;
    }
}

void QWinSphere::printLineAtInfinity(void)
{
    switch (VFResults.typeofview) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0 < 0.0 && x1 > 0.0)
            print_line(coWinX(0.0), coWinY(y0), coWinX(0.0), coWinY(y1),
                       CLINEATINFINITY);
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0 < 0.0 && y1 > 0.0)
            print_line(coWinX(x0), coWinY(0.0), coWinX(x1), coWinY(0.0),
                       CLINEATINFINITY);
        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}

void QWinSphere::printOrbits(void)
{
    // inspired by DrawOrbits, except that we put comments between

    struct orbits *orbit;
    QString s;
    int i;
    i = 1;

    for (orbit = VFResults.first_orbit; orbit != nullptr;
         orbit = orbit->next_orbit) {
        s.sprintf("Starting orbit %d", i++);
        print_comment(s);
        drawOrbit(this, orbit->pcoord, orbit->f_orbits, orbit->color);
    }
}

void QWinSphere::printLimitCycles(void)
{
    // inspired by DrawOrbits, except that we put comments between

    struct orbits *orbit;
    QString s;
    int i;
    i = 1;

    for (orbit = VFResults.first_lim_cycle; orbit != nullptr;
         orbit = orbit->next_orbit) {
        s.sprintf("Starting Limit Cycle %d", i++);
        print_comment(s);
        drawOrbit(this, orbit->pcoord, orbit->f_orbits, orbit->color);
    }
}

void QWinSphere::printLine(double _x1, double _y1, double _x2, double _y2,
                           int color)
{
    int wx1, wy1, wx2, wy2;

    if (_x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1) {
        wx1 = coWinX(_x1);
        wy1 = coWinY(_y1);

        if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
            // both points are visible in the window

            wx2 = coWinX(_x2);
            wy2 = coWinY(_y2);

            print_line(wx1, wy1, wx2, wy2, color);
        } else {
            // only (_x2,_y2) is not visible

            if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0, y1)) {
                wx1 = coWinX(_x1);
                wy1 = coWinY(_y1);
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
                print_line(wx1, wy1, wx2, wy2, color);
            }
        }
    } else {
        if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
            // only (_x2,_y2) is visible

            if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0, y1)) {
                wx1 = coWinX(_x1);
                wy1 = coWinY(_y1);
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
                print_line(wx1, wy1, wx2, wy2, color);
            }
        } else {
            // both end points are invisible

            if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0, y1)) {
                wx1 = coWinX(_x1);
                wy1 = coWinY(_y1);
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
                print_line(wx1, wy1, wx2, wy2, color);
            }
        }
    }
}

void QWinSphere::printPoint(double x, double y, int color)
{
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return;

    print_point(coWinX(x), coWinY(y), color);
}

void QWinSphere::refresh(void)
{
    isPainterCacheDirty = true;
    update();
}

void QWinSphere::CalculateHeightFromWidth(int *width, int *height,
                                          int maxheight = -1,
                                          double aspectratio = 1)
{
    // given an optimal width in *width, this procedure calculates the
    // corresponding height in order to maintain the given aspectratio
    // If however the maximum height is violated, then we choose to
    // have the maximum height and calculate the corresponding width.

    // aspect ratio is

    double w, h;

    w = (double)(*width);
    h = w * dy / dx;
    h *= aspectratio;

    if ((int)(h + 0.5) <= maxheight || maxheight == -1) {
        *height = (int)(h + 0.5);
    } else {
        *height = maxheight;

        h = (double)maxheight;
        w = h * dx / dy;
        w /= aspectratio;

        *width = (int)(w + 0.5);
    }
}

void QWinSphere::preparePrinting(int printmethod, bool isblackwhite,
                                 int myresolution, double mylinewidth,
                                 double mysymbolsize)
{
    double aspectratio, lw, ss, hpixels, maxvpixels, pagewidth, pageheight, tx,
        ty;

    PrintMethod = printmethod;

    aspectratio = 1; // assume aspect ratio 1

    if (printmethod == P4PRINT_DEFAULT) {
        // p4printer->setResolution(myresolution); we have moved this.
        // according to documentation: set of resolution must be done BEFORE
        // setup!

        //  aspectratio = metrics.logicalDpiX()/metrics.logicalDpiY();
        aspectratio = 1;

        pagewidth = p4printer->width();
        pageheight = p4printer->height();
    } else
        pagewidth = pageheight = -1; // will be redefined in a minut

    p4pixmap_dpm = lw = ss = hpixels = myresolution;
    p4pixmap_dpm /= 2.54;
    hpixels *= 15;
    hpixels /= 2.54;

    maxvpixels = myresolution;
    maxvpixels *= aspectratio;
    maxvpixels *= 20;
    maxvpixels /= 2.54;
    maxvpixels += 0.5;

    oldw = w;
    oldh = h;
    w = (int)(hpixels + 0.5);

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
        pagewidth = POSTSCRIPTPAGEWIDTH; // see custom.cpp
        pageheight = POSTSCRIPTPAGEHEIGHT;
        pagewidth *= myresolution;
        pageheight *= myresolution;
        break;
    }

    if (w > pagewidth && pagewidth != -1)
        w = (int)pagewidth;
    if (maxvpixels > pageheight && pageheight != -1)
        maxvpixels = pageheight;

    CalculateHeightFromWidth(&w, &h, (int)maxvpixels, aspectratio);

    lw /= 25.4;        // dots per mm
    lw *= mylinewidth; // linewidth in pixels
    lw += 0.5;         // prepare round above
    if (lw < 1.0)
        lw = 1.0;

    ss /= 25.4;         // dots per mm
    ss *= mysymbolsize; // symbolsize in pixels
    ss += 0.5;          // prepare round above for units of 2
    ss /= 2.0;
    if (ss < 1.0)
        ss = 1.0;

    if (pagewidth == -1 || pageheight == -1) {
        tx = 0;
        ty = 0;
    } else {
        tx = pagewidth;
        tx -= w;
        tx /= 2;
        ty = pageheight;
        ty -= h;
        ty /= 2;
    }

    switch (printmethod) {
    case P4PRINT_EPSIMAGE:
        ReverseYaxis = true;
        preparePostscriptPrinting((int)(tx + 0.5), (int)(ty + 0.5), w, h,
                                  iszoom, isblackwhite, myresolution, (int)lw,
                                  2 * (int)ss);
        break;
    case P4PRINT_XFIGIMAGE:
        ReverseYaxis = false;
        prepareXFigPrinting(w, h, iszoom, isblackwhite, myresolution, (int)lw,
                            2 * (int)ss);
        break;
    case P4PRINT_DEFAULT:
        staticPainter = new QPainter();

        if (!staticPainter->begin(p4printer)) {
            delete staticPainter;
            staticPainter = nullptr;
            return;
        }

        staticPainter->translate(tx, ty);
        if (iszoom || VFResults.typeofview == TYPEOFVIEW_PLANE) {
            QPen p = QPen(QXFIGCOLOR(printColorTable[CFOREGROUND]), (int)lw);
            staticPainter->setPen(p);
            staticPainter->drawRect(0, 0, w, h);
        }
        //      staticPainter->setClipRect( (int)tx, (int)ty, w, h );
        ReverseYaxis = false; // no need for reversing axes in this case
        prepareP4Printing(w, h, isblackwhite, staticPainter, (int)lw,
                          2 * (int)ss);
        break;

    case P4PRINT_JPEGIMAGE:
        staticPainter = new QPainter();
        p4pixmap = new QPixmap(w, h);
        ReverseYaxis = false; // no need for reversing axes in this case
        if (p4pixmap->isNull()) {
            msgBar->showMessage(
                "Print failure (try to choose a lower resolution).");
            delete p4pixmap;
            p4pixmap = nullptr;
            delete staticPainter;
            staticPainter = nullptr;
            return;
        }
        if (!staticPainter->begin(p4pixmap)) {
            delete p4pixmap;
            p4pixmap = nullptr;
            delete staticPainter;
            staticPainter = nullptr;
            return;
        }

        prepareP4Printing(w, h, isblackwhite, staticPainter, (int)lw,
                          2 * (int)ss);
        break;
    }

    msgBar->showMessage("Printing ...");
}

void QWinSphere::finishPrinting(void)
{
    if (PrintMethod == P4PRINT_EPSIMAGE) {
        finishPostscriptPrinting();
        ReverseYaxis = false;
        w = oldw;
        h = oldh;
    } else if (PrintMethod == P4PRINT_XFIGIMAGE) {
        finishXFigPrinting();
        ReverseYaxis = false;
        w = oldw;
        h = oldh;
    } else if (PrintMethod == P4PRINT_DEFAULT) {
        finishP4Printing();
        staticPainter->end();
        delete staticPainter;
        staticPainter = nullptr;
        w = oldw;
        h = oldh;
        ReverseYaxis = false;
    } else if (PrintMethod == P4PRINT_JPEGIMAGE) {
        if (p4pixmap == nullptr) {
            finishP4Printing();
            w = oldw;
            h = oldh;
            ReverseYaxis = false;
            return;
        }

        finishP4Printing();
        staticPainter->end();
        delete staticPainter;
        staticPainter = nullptr;

        if (p4pixmap->save(ThisVF->getbarefilename() + ".jpg", "JPEG", 100) ==
            false) {
            QMessageBox::critical(this, "P4", "For some reason, P4 is unable "
                                              "to save the resulting JPEG "
                                              "image to disc.");
        }

        delete p4pixmap;
        p4pixmap = nullptr;
        ReverseYaxis = false;
        w = oldw;
        h = oldh;
    }
    msgBar->showMessage("Printing has finished.");
}

void QWinSphere::print(void)
{
    if (PrintMethod == P4PRINT_JPEGIMAGE && p4pixmap == nullptr)
        return;

    if (VFResults.typeofview != TYPEOFVIEW_PLANE) {
        if (VFResults.typeofview == TYPEOFVIEW_SPHERE) {
            if (VFResults.plweights)
                printPoincareLyapunovSphere();
            else
                printPoincareSphere();
        } else
            printLineAtInfinity();
    }
    printOrbits();
    printSeparatrices();
    printGcf();
    printLimitCycles();
    printPoints();
}

void QWinSphere::prepareDrawing()
{
    if (PainterCache == nullptr) {
        isPainterCacheDirty = true;
        PainterCache = new QPixmap(size());
    }
    staticPainter = new QPainter(PainterCache);

    paintedXMin = width() - 1;
    paintedYMin = height() - 1;
    paintedXMax = 0;
    paintedYMax = 0;

    if (next != nullptr)
        next->prepareDrawing();
}

void QWinSphere::finishDrawing()
{
    if (next != nullptr)
        next->finishDrawing();

    if (staticPainter != nullptr) {
        staticPainter->end();
        delete staticPainter;
        staticPainter = nullptr;

        if (paintedXMin < 0)
            paintedXMin = 0;
        if (paintedXMax >= width())
            paintedXMax = width() - 1;
        if (paintedYMin < 0)
            paintedYMin = 0;
        if (paintedYMax >= height())
            paintedYMax = height() - 1;

        if (paintedYMax >= paintedYMin && paintedXMax >= paintedXMin)
            update(paintedXMin, paintedYMin, paintedXMax - paintedXMin + 1,
                   paintedYMax - paintedYMin + 1);
    }
}

void QWinSphere::Signal_Evaluating(void)
{
    /*
        QPalette palette;
        palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor =
       CBACKGROUND) );
        setPalette(palette);
    */
}

void QWinSphere::Signal_Changed(void)
{
    /*
        QPalette palette;
        palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor = DARKGRAY)
       );
        setPalette(palette);
    */
}
