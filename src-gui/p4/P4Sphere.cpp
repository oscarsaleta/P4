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

#include "P4Sphere.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QResizeEvent>
#include <QStatusBar>
#include <QTimer>

#include <cmath>
#include <utility>

#include "P4Application.hpp"
#include "P4Event.hpp"
#include "P4ParentStudy.hpp"
#include "P4PrintDlg.hpp"
#include "P4StartDlg.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "math_arbitrarycurve.hpp"
#include "math_charts.hpp"
#include "math_findpoint.hpp"
#include "math_gcf.hpp"
#include "math_isoclines.hpp"
#include "math_limitcycles.hpp"
#include "math_orbits.hpp"
#include "math_p4.hpp"
#include "math_separatrice.hpp"
#include "plot_points.hpp"
#include "plot_tools.hpp"
#include "print_bitmap.hpp"
#include "print_points.hpp"
#include "print_postscript.hpp"
#include "print_xfig.hpp"

QPixmap *sP4pixmap;
static double sP4pixmapDPM{0};

int P4Sphere::sM_numSpheres{0};
QVector<P4Sphere *> P4Sphere::sM_sphereList;

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

P4Sphere::P4Sphere(QStatusBar *bar, bool isZoom, double x1, double y1,
                   double x2, double y2, QWidget *parent)
    : QWidget{parent}, parentWnd_{parent}, msgBar_{bar}, iszoom_{isZoom}
{
    // qDebug() << "called constructor";
    // setAttribute(Qt::WA_PaintOnScreen);

    // FIXME: is next needed?
    if (!sM_sphereList.empty())
        sM_sphereList.back()->next_ = this;
    sM_sphereList.push_back(this);
    sM_numSpheres++;

    // THIS IS THE MINIMUM SIZE
    setMinimumSize(MINWIDTHPLOTWINDOW, MINHEIGHTPLOTWINDOW);
    w_ = width();
    h_ = height();
    idealh_ = w_;

    horPixelsPerMM_ = static_cast<double>(w_) / widthMM();
    verPixelsPerMM_ = static_cast<double>(h_) / heightMM();

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(windowFlags());

    if (iszoom_) {
        x0_ = x1;
        y0_ = y1;
        x1_ = x2;
        y1_ = y2;
    }

    paintedXMax_ = w_;
    paintedYMax_ = h_;
}

P4Sphere::~P4Sphere()
{
    int i;

    for (i = 0; i < sM_numSpheres; i++) {
        if (sM_sphereList[i] == this)
            break;
    }
    if (i == sM_numSpheres)
        return; // error: sphere not found?
    if (i > 0)
        sM_sphereList[i - 1]->next_ = next_;
    sM_sphereList.erase(std::begin(sM_sphereList) + i);

    sM_numSpheres--;
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

void P4Sphere::keyPressEvent(QKeyEvent *e)
{
    int id;
    int *data1;
    Qt::KeyboardModifiers bs;

    id = e->key();
    bs = e->modifiers();
    switch (id) {
    case Qt::Key_F:
        if (bs == Qt::NoModifier || bs == Qt::AltModifier) {
            // F: integrate orbit forwards in time
            data1 = new int{1};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_ORBIT_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_C:
        if (bs == Qt::NoModifier || bs == Qt::AltModifier) {
            // C: continue integrate orbit
            data1 = new int{0};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_ORBIT_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        } else if (bs == Qt::ShiftModifier ||
                   bs == Qt::AltModifier + Qt::ShiftModifier) {
            // SHIFT+C:  continue integrating separatrice
            data1 = new int{0};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_SEP_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_B:
        if (bs == Qt::NoModifier || bs == Qt::AltModifier) {
            // B: integrate orbit backwards in time
            data1 = new int{-1};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_ORBIT_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_D:
        if (bs == Qt::NoModifier || bs == Qt::AltModifier) {
            // D: delete orbit
            data1 = new int{2};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_ORBIT_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_A:
        if (bs == Qt::NoModifier || bs == Qt::AltModifier) {
            // A: delete all orbits
            data1 = new int{3};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_ORBIT_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_N:
        if (bs == Qt::ShiftModifier ||
            bs == Qt::AltModifier + Qt::ShiftModifier) {
            // SHIFT+N: select next separatrice
            data1 = new int{3};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_SEP_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_I:
        if (bs == Qt::ShiftModifier ||
            bs == Qt::AltModifier + Qt::ShiftModifier) {
            // SHIFT+I: integrate next separatrice
            data1 = new int{2};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_SEP_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    case Qt::Key_S:
        if (bs == Qt::ShiftModifier ||
            bs == Qt::AltModifier + Qt::ShiftModifier) {
            // SHIFT+S: start integrate separatrice
            data1 = new int{1};
            auto e1 =
                new P4Event{static_cast<QEvent::Type>(TYPE_SEP_EVENT), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        break;
    }

    e->ignore();
}

static QString makechartstring(int p, int q, bool isu1v1chart, bool negchart)
{
    QString buf;

    if (isu1v1chart) {
        // make { x = +/- 1/z2^p, y = z1/z2^q }

        if (p != 1 && q != 1)
            buf.sprintf("{x=%d/z2^%d,y=z1/z2^%d}", (negchart ? -1 : 1), p, q);
        else if (p == 1 && q != 1)
            buf.sprintf("{x=%d/z2,y=z1/z2^%d}", (negchart ? -1 : 1), q);
        else if (p != 1 && q == 1)
            buf.sprintf("{x=%d/z2^%d,y=z1/z2}", (negchart ? -1 : 1), p);
        else
            buf.sprintf("{x=%d/z2,y=z1/z2}", (negchart ? -1 : 1));
    } else {
        // make { x = 1/z2^p, y = +/- z1/z2^q }

        if (p != 1 && q != 1)
            buf.sprintf("{x=z1/z2^%d,y=%d/z2^%d}", p, (negchart ? -1 : 1), q);
        else if (p == 1 && q != 1)
            buf.sprintf("{x=z1/z2,y=%d/z2^%d}", (negchart ? -1 : 1), q);
        else if (p != 1 && q == 1)
            buf.sprintf("{x=z1/z2^%d,y=%d/z2}", p, (negchart ? -1 : 1));
        else
            buf.sprintf("{x=z1/z2,y=%d/z2}", (negchart ? -1 : 1));
    }

    return buf;
}

void P4Sphere::setupPlot()
{
    QPalette palette;

    spherebgcolor_ = P4ColourSettings::colour_background;
    palette.setColor(backgroundRole(), P4Colours::p4XfigColour(spherebgcolor_));
    setPalette(palette);

    circleAtInfinity_.clear();
    plCircle_.clear();

    if (!iszoom_) {
        switch (gVFResults.typeofview_) {
        case P4TypeOfView::typeofview_plane:
        case P4TypeOfView::typeofview_U1:
        case P4TypeOfView::typeofview_U2:
        case P4TypeOfView::typeofview_V1:
        case P4TypeOfView::typeofview_V2:
            x0_ = gVFResults.xmin_;
            y0_ = gVFResults.ymin_;
            x1_ = gVFResults.xmax_;
            y1_ = gVFResults.ymax_;
            break;
        case P4TypeOfView::typeofview_sphere:
            x0_ = -1.1;
            y0_ = -1.1;
            x1_ = 1.1;
            y1_ = 1.1;
            break;
        }
    }

    dx_ = x1_ - x0_;
    dy_ = y1_ - y0_;

    // double idealhd{std::round(w_ / dx_ * dy_)};

    switch (gVFResults.typeofview_) {
    case P4TypeOfView::typeofview_plane:
        chartstring_ = "";
        break;
    case P4TypeOfView::typeofview_sphere:
        chartstring_ = "";
        break;
    case P4TypeOfView::typeofview_U1:
        chartstring_ =
            makechartstring(gVFResults.p_, gVFResults.q_, true, false);
        break;
    case P4TypeOfView::typeofview_U2:
        chartstring_ =
            makechartstring(gVFResults.p_, gVFResults.q_, false, false);
        break;
    case P4TypeOfView::typeofview_V1:
        chartstring_ =
            makechartstring(gVFResults.p_, gVFResults.q_, true, true);
        break;
    case P4TypeOfView::typeofview_V2:
        chartstring_ =
            makechartstring(gVFResults.p_, gVFResults.q_, false, true);
        break;
    }

    if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
        circleAtInfinity_ =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (gVFResults.plweights_)
            plCircle_ = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }

    isPainterCacheDirty_ = true;
}

void P4Sphere::loadAnchorMap()
{
    int x1, y1;
    int x2, y2;
    int aw, ah;

    if (selectingZoom_) {
        x1 = zoomAnchor1_.x();
        y1 = zoomAnchor1_.y();
        x2 = zoomAnchor2_.x();
        y2 = zoomAnchor2_.y();
    } else if (selectingLCSection_) {
        x1 = lcAnchor1_.x();
        y1 = lcAnchor1_.y();
        x2 = lcAnchor2_.x();
        y2 = lcAnchor2_.y();
    } else
        return;

    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);
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

    if (anchorMap_ != nullptr) {
        if (anchorMap_->width() < aw || anchorMap_->height() < ah) {
            delete anchorMap_;
            anchorMap_ = new QPixmap{aw, ah};
        }
    } else {
        anchorMap_ = new QPixmap{aw, ah};
    }

    if (painterCache_ == nullptr) {
        delete anchorMap_;
        anchorMap_ = nullptr;
        return;
    }

    QPainter paint{anchorMap_};
    if (selectingZoom_) {
        // only copy rectangular edges, not inside
        /*paint.drawPixmap(0, 0, aw, 1, *painterCache_, x1, y1, aw, 1);
        paint.drawPixmap(0, ah - 1, aw, 1, *painterCache_, x1, y2, aw, 1);
        paint.drawPixmap(0, 0, 1, ah, *painterCache_, x1, y1, 1, ah);
        paint.drawPixmap(aw - 1, 0, 1, ah, *painterCache_, x2, y1, 1, ah);*/
        paint.drawPixmap(0, 0, aw, ah, *painterCache_, x1, y1, aw, ah);
    } else {
        paint.drawPixmap(0, 0, aw, ah, *painterCache_, x1, y1, aw, ah);
    }
}

void P4Sphere::saveAnchorMap()
{
    int x1, y1;
    int x2, y2;
    int aw, ah;

    if (anchorMap_ == nullptr || painterCache_ == nullptr ||
        (!selectingZoom_ && !selectingLCSection_))
        return;

    if (selectingZoom_) {
        x1 = zoomAnchor1_.x();
        y1 = zoomAnchor1_.y();
        x2 = zoomAnchor2_.x();
        y2 = zoomAnchor2_.y();
    }
    if (selectingLCSection_) {
        x1 = lcAnchor1_.x();
        y1 = lcAnchor1_.y();
        x2 = lcAnchor2_.x();
        y2 = lcAnchor2_.y();
    }

    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);
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

    QPainter paint{painterCache_};

    if (selectingZoom_) {
        // only copy rectangular edges, not inside
        /*paint.drawPixmap(x1, y1, aw, 1, *anchorMap_, 0, 0, aw, 1);
        paint.drawPixmap(x1, y2, aw, 1, *anchorMap_, 0, ah - 1, aw, 1);
        paint.drawPixmap(x1, y1, 1, ah, *anchorMap_, 0, 0, 1, ah);
        paint.drawPixmap(x2, y1, 1, ah, *anchorMap_, aw - 1, 0, 1, ah);*/
        paint.drawPixmap(x1, y1, aw, ah, *anchorMap_, 0, 0, aw, ah);
    } else {
        paint.drawPixmap(x1, y1, aw, ah, *anchorMap_, 0, 0, aw, ah);
    }

    update(x1, y1, aw, ah);
}

void P4Sphere::adjustToNewSize()
{
    QString buf;

    w_ = width();
    h_ = height();

    auto idealhd = static_cast<double>(std::round(w_ / dx_ * dy_));
    idealh_ = static_cast<int>(std::round(idealhd));

    auto reqratio = (static_cast<double>(w_) / horPixelsPerMM_) /
                    (idealh_ / verPixelsPerMM_);
    auto ratio = (static_cast<double>(w_) / horPixelsPerMM_) /
                 (static_cast<double>(h_) / verPixelsPerMM_);

    buf.sprintf("Aspect Ratio = %8.5g\n", ratio / reqratio);
    msgBar_->showMessage(buf);

    circleAtInfinity_.clear();
    plCircle_.clear();

    if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
        circleAtInfinity_ =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (gVFResults.plweights_)
            plCircle_ = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }

    if (painterCache_ != nullptr) {
        delete painterCache_;
        painterCache_ = new QPixmap{size()};
        isPainterCacheDirty_ = false;

        QPainter paint{painterCache_};
        paint.fillRect(0, 0, width(), height(),
                       QColor(P4Colours::p4XfigColour(
                           P4ColourSettings::colour_background)));

        paint.setPen(
            P4Colours::p4XfigColour(P4ColourSettings::colour_line_at_infinity));
        staticPainter_ = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if (gVFResults.typeofview_ != P4TypeOfView::typeofview_plane) {
            if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
                if (gVFResults.plweights_)
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

        QColor c{P4Colours::p4XfigColour(P4Colours::white)};
        c.setAlpha(128);
        paint.setPen(c);
        paint.drawText(0, 0, width(), height(),
                       Qt::AlignHCenter | Qt::AlignVCenter, "Resizing ...  ");

        staticPainter_ = nullptr;

        if (refreshTimeout_ != nullptr)
            refreshTimeout_->stop();
        else {
            refreshTimeout_ = new QTimer{};
            QObject::connect(refreshTimeout_, &QTimer::timeout, this,
                             &P4Sphere::refreshAfterResize);
        }
        refreshTimeout_->start(500);
    }
}

void P4Sphere::refreshAfterResize()
{
    if (refreshTimeout_ != nullptr) {
        delete refreshTimeout_;
        refreshTimeout_ = nullptr;
    }
    refresh();
}

void P4Sphere::resizeEvent(QResizeEvent *e)
{
    if (e->size() == e->oldSize())
        return;

    adjustToNewSize();
}

void P4Sphere::paintEvent(QPaintEvent *p)
{
    // qDebug() << "paint event";
    if (gThisVF->evaluating_)
        return;

    if (painterCache_ == nullptr || isPainterCacheDirty_) {
        if (painterCache_ == nullptr) {
            // qDebug() << "  creating new QPixmap painterCache_...";
            painterCache_ = new QPixmap{size()};
        }
        isPainterCacheDirty_ = false;

        // qDebug() << "creating new QPainter from QPixmap painterCache_...";
        QPainter paint{painterCache_};
        paint.fillRect(0, 0, width(), height(),
                       QColor(P4Colours::p4XfigColour(
                           P4ColourSettings::colour_background)));

        paint.setPen(
            P4Colours::p4XfigColour(P4ColourSettings::colour_line_at_infinity));

        staticPainter_ = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if (gVFResults.typeofview_ != P4TypeOfView::typeofview_plane) {
            if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
                if (gVFResults.plweights_)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }
        plotSeparatingCurves();
        plotSeparatrices();
        plotGcf();
        plotArbitraryCurves();
        plotIsoclines();
        drawOrbits(this);
        drawLimitCycles(this);
        plotPoints();

        staticPainter_ = nullptr;
    }

    QPainter widgetpaint{this};
    widgetpaint.drawPixmap(0, 0, *painterCache_);

    if (selectingPointStep_ != 0) {
        widgetpaint.setPen(P4Colours::p4XfigColour(P4Colours::white));
        widgetpaint.setBrush(Qt::NoBrush);
        widgetpaint.drawEllipse(selectingX_ - selectingPointRadius_,
                                selectingY_ - selectingPointRadius_,
                                selectingPointRadius_ + selectingPointRadius_,
                                selectingPointRadius_ + selectingPointRadius_);
    }
}

void P4Sphere::markSelection(int x1, int y1, int x2, int y2, int selectiontype)
{
    if (painterCache_ == nullptr)
        return;

    int bx1{(x1 < x2) ? x1 : x2};
    int bx2{(x1 < x2) ? x2 : x1};
    int by1{(y1 < y2) ? y1 : y2};
    int by2{(y1 < y2) ? y2 : y1};

    if (bx1 < 0)
        bx1 = 0;
    if (by1 < 0)
        by1 = 0;
    if (bx2 >= width())
        bx2 = width() - 1;
    if (by2 >= height())
        by2 = height() - 1;

    QPainter p{painterCache_};
    QColor c;

    switch (selectiontype) {
    case 0:
        c = P4Colours::p4XfigColour(P4Colours::white);
        c.setAlpha(32);
        p.setPen(P4Colours::p4XfigColour(P4Colours::white));
        p.setBrush(c);
        if (bx1 == bx2 || by1 == by2)
            p.drawLine(bx1, by1, bx2, by2);
        else
            p.drawRect(bx1, by1, bx2 - bx1, by2 - by1);
        break;

    case 1:
        p.setPen(P4Colours::p4XfigColour(P4Colours::white));
        p.drawLine(x1, y1, x2, y2);
        break;
    }
    update(bx1, by1, bx2 - bx1 + 1, by2 - by1 + 1);
}

void P4Sphere::mouseMoveEvent(QMouseEvent *e)
{
    QString buf;

    int x{e->x()};
    int y{e->y()};

    double wx{coWorldX(x)};
    double wy{coWorldY(y)};

    double ucoord[2];
    double pcoord[3];

    if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord)) {
        switch (gVFResults.typeofview_) {
        case P4TypeOfView::typeofview_plane:
            if (gVFResults.typeofstudy_ == P4TypeOfStudy::typeofstudy_one)
                buf.sprintf("Local study   (x,y) = (%8.5g,%8.5g)", wx, wy);
            else
                buf.sprintf("Planar view   (x,y) = (%8.5g,%8.5g)", wx, wy);
            break;
        case P4TypeOfView::typeofview_sphere:
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

            if (gVFResults.p_ == 1 && gVFResults.q_ == 1)
                buf.sprintf("The Poincare sphere  (x,y) = (%8.5g,%8.5g)",
                            ucoord[0], ucoord[1]);
            else
                buf.sprintf(
                    "The P-L sphere of type (%d,%d)  (x,y) = (%8.5g,%8.5g)",
                    gVFResults.p_, gVFResults.q_, ucoord[0], ucoord[1]);
            break;
        case P4TypeOfView::typeofview_U1:
            MATHFUNC(sphere_to_U1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U1 chart  (z2,z1) = (%8.5g,%8.5g) ", ucoord[1],
                            ucoord[0]);
            else
                buf.sprintf("The V1' chart (z2,z1) = (%8.5g,%8.5g) ", ucoord[1],
                            ucoord[0]);
            buf.append(chartstring_);
            break;
        case P4TypeOfView::typeofview_V1:
            MATHFUNC(sphere_to_V1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!gVFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V1 chart  (z2,z1) = (%8.5g,%8.5g) ", ucoord[1],
                            ucoord[0]);
            else
                buf.sprintf("The U1' chart (z2,z1) = (%8.5g,%8.5g) ", ucoord[1],
                            ucoord[0]);
            buf.append(chartstring_);
            break;
        case P4TypeOfView::typeofview_U2:
            MATHFUNC(sphere_to_U2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U2 chart  (z1,z2) = (%8.5g,%8.5g) ", ucoord[0],
                            ucoord[1]);
            else
                buf.sprintf("The V2' chart (z1,z2) = (%8.5g,%8.5g) ", ucoord[0],
                            ucoord[1]);
            buf.append(chartstring_);
            break;
        case P4TypeOfView::typeofview_V2:
            MATHFUNC(sphere_to_V2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!gVFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V2 chart  (z1,z2) = (%8.5g,%8.5g) ", ucoord[0],
                            ucoord[1]);
            else
                buf.sprintf("The U2' chart (z1,z2) = (%8.5g,%8.5g) ", ucoord[0],
                            ucoord[1]);
            buf.append(chartstring_);
            break;
        }

        if (!gVFResults.separatingCurves_.empty()) {
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            int index{gThisVF->getVFIndex_R2(ucoord)};
            if (index < 0) {
                buf.append("  VF: NONE");
            } else {
                QString s;
                s.sprintf("  VF: #%d", index + 1);
                buf.append(s);
            }
        }
    } else {
        switch (gVFResults.typeofview_) {
        case P4TypeOfView::typeofview_plane:
            if (gVFResults.typeofstudy_ == P4TypeOfStudy::typeofstudy_one)
                buf.sprintf("Local study");
            else
                buf.sprintf("Planar view");
            break;
        case P4TypeOfView::typeofview_sphere:
            if (gVFResults.p_ == 1 && gVFResults.q_ == 1)
                buf.sprintf("The Poincare sphere");
            else
                buf.sprintf("The P-L sphere of type (%d,%d)", gVFResults.p_,
                            gVFResults.q_);
            break;
        case P4TypeOfView::typeofview_U1:
            buf.sprintf("The U1 chart");
            break;
        case P4TypeOfView::typeofview_V1:
            buf.sprintf("The V1 chart");
            break;
        case P4TypeOfView::typeofview_U2:
            buf.sprintf("The U2 chart");
            break;
        case P4TypeOfView::typeofview_V2:
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

double P4Sphere::coWorldX(int x)
{
    double wx{static_cast<double>(x) / (w_ - 1)};
    return (wx * dx_ + x0_);
}

double P4Sphere::coWorldY(int y)
{
    double wy{static_cast<double>(h_ - 1 - y) / (h_ - 1)};

    return (wy * dy_ + y0_);
}

int P4Sphere::coWinH(double deltax)
{
    double wx{deltax / dx_ * (w_ - 1)};

    return static_cast<int>(std::round(wx));
}

int P4Sphere::coWinV(double deltay)
{
    double wy{deltay / dy_ * (h_ - 1)};

    return static_cast<int>(std::round(wy));
}

int P4Sphere::coWinX(double x)
{
    double wx{(x - x0_) / dx_ * (w_ - 1)};
    int iwx{static_cast<int>(std::round(wx))};

    if (iwx >= w_)
        iwx = w_ - 1;

    return iwx;
}

int P4Sphere::coWinY(double y)
{
    double wy{(y - y0_) / dy_ * (h_ - 1)};
    int iwy{static_cast<int>(std::round(wy))};

    if (iwy >= h_)
        iwy = h_ - 1;

    // on screen: vertical axis orientation is reversed
    return (reverseYAxis_) ? iwy : h_ - 1 - iwy;
}

void P4Sphere::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (e->modifiers() == Qt::ControlModifier) {
            if (!selectingZoom_) {
                selectingZoom_ = true;
                zoomAnchor1_ = e->pos();
                zoomAnchor2_ = zoomAnchor1_;
                loadAnchorMap();
                markSelection(zoomAnchor1_.x(), zoomAnchor1_.y(),
                              zoomAnchor2_.x(), zoomAnchor2_.y(), 0);
            }
        } else if (e->modifiers() == Qt::ShiftModifier) {
            // select nearest singularity having separatrices, and open the
            // separatrices window.
            selectNearestSingularity(e->pos());
        } else if (e->modifiers() == Qt::AltModifier) {
            if (!selectingLCSection_) {
                selectingLCSection_ = true;
                lcAnchor1_ = e->pos();
                lcAnchor2_ = lcAnchor1_;
                loadAnchorMap();
                markSelection(lcAnchor1_.x(), lcAnchor1_.y(), lcAnchor2_.x(),
                              lcAnchor2_.y(), 0);
            }
        } else {
            // normally, start integrating new orbit at the chosen point.
            // However, when the limit cycle window is open, select the first
            // and second point of a transverse section.
            auto data1 = std::make_unique<DOUBLEPOINT>(coWorldX(e->x()),
                                                       coWorldY(e->y()));
            double pcoord[3];
            if (MATHFUNC(is_valid_viewcoord)(data1->x, data1->y, pcoord)) {
                auto e1 =
                    new P4Event{static_cast<QEvent::Type>(TYPE_SELECT_ORBIT),
                                data1.release()};
                gP4app->postEvent(parentWnd_, e1);
            }
        }
    } else if (e->button() == Qt::RightButton) {
        // cancel zoom window with right mouse button
        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;
        }
        if (selectingLCSection_) {
            saveAnchorMap();
            selectingLCSection_ = false;
        }
    }
    QWidget::mousePressEvent(e);
}

void P4Sphere::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // finish zoom window between zoomAnchor1_ and zoomAnchor2_
        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;
            auto data1 = new double[4];
            data1[0] = coWorldX(zoomAnchor1_.x());
            data1[1] = coWorldY(zoomAnchor1_.y());
            data1[2] = coWorldX(zoomAnchor2_.x());
            data1[3] = coWorldY(zoomAnchor2_.y());
            auto e1 = new P4Event{
                static_cast<QEvent::Type>(TYPE_OPENZOOMWINDOW), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
        if (selectingLCSection_) {
            saveAnchorMap();
            selectingLCSection_ = false;

            auto data1 = new double[4];
            data1[0] = coWorldX(lcAnchor1_.x());
            data1[1] = coWorldY(lcAnchor1_.y());
            data1[2] = coWorldX(lcAnchor2_.x());
            data1[3] = coWorldY(lcAnchor2_.y());
            auto e1 = new P4Event{
                static_cast<QEvent::Type>(TYPE_SELECT_LCSECTION), data1};
            gP4app->postEvent(parentWnd_, e1);
        }
    }
    QWidget::mouseReleaseEvent(e);
}

bool P4Sphere::getChartPos(int chart, double x0_, double y0_, double *pos)
{
    double pcoord[3];

    switch (chart) {
    case P4Charts::chart_R2:
        MATHFUNC(finite_to_viewcoord)(x0_, y0_, pos);
        break;
    case P4Charts::chart_U1:
        MATHFUNC(U1_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case P4Charts::chart_U2:
        MATHFUNC(U2_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case P4Charts::chart_V1:
        MATHFUNC(V1_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    case P4Charts::chart_V2:
        MATHFUNC(V2_to_sphere)(x0_, 0, pcoord);
        MATHFUNC(sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], pos);
        break;
    }
    return true;
}

void P4Sphere::updatePointSelection()
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

void P4Sphere::selectNearestSingularity(const QPoint &winpos)
{
    int x{winpos.x()}, y{winpos.y()};

    sM_sphereList.back()->prepareDrawing();
    auto result =
        find_critical_point(sM_sphereList.back(), coWorldX(x), coWorldY(y));
    sM_sphereList.back()->finishDrawing();

    if (!result) {
        msgBar_->showMessage(
            "Search nearest critical point: None with separatrices found.");
    } else {
        auto px = coWinX(gVFResults.selected_ucoord_[0]);
        auto py = coWinY(gVFResults.selected_ucoord_[1]);

        if (selectingTimer_ != nullptr) {
            delete selectingTimer_;
            selectingTimer_ = nullptr;
            selectingPointStep_ = 0;
            updatePointSelection();
        }

        selectingPointStep_ = SELECTINGPOINTSTEPS - 1;
        selectingX_ = px;
        selectingY_ = py;

        selectingTimer_ = new QTimer{};
        QObject::connect(selectingTimer_, &QTimer::timeout, this,
                         &P4Sphere::updatePointSelection);
        selectingTimer_->start(SELECTINGPOINTSPEED);
        msgBar_->showMessage("Search nearest critical point: Found");

        auto data1 = -1;
        auto e1 =
            new P4Event{static_cast<QEvent::Type>(TYPE_SEP_EVENT), &data1};
        gP4app->postEvent(parentWnd_, e1);
    }
}

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void P4Sphere::plotPoint(const P4Singularities::saddle *p)
{
    qDebug() << "plot point saddle";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);
    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    auto x = coWinX(pos[0]);
    auto y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p->position) {
    case P4Singularities::position_virtual:
        win_plot_virtualsaddle(staticPainter_, x, y);
        break;
    case P4Singularities::position_coinciding:
        break;
    case P4Singularities::position_coinciding_virtual:
        break;
    case P4Singularities::position_coinciding_main:
        win_plot_coinciding(staticPainter_, x, y);
        break;
    default:
        win_plot_saddle(staticPainter_, x, y);
        break;
    }
}

void P4Sphere::plotPoint(const P4Singularities::node *p)
{
    qDebug() << "plot point node";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    auto x = coWinX(pos[0]);
    auto y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    if (p->stable == -1) {
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualstablenode(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_stablenode(staticPainter_, x, y);
            break;
        }
    } else {
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualunstablenode(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_unstablenode(staticPainter_, x, y);
            break;
        }
    }
}

void P4Sphere::plotPoint(const P4Singularities::weak_focus *p)
{
    qDebug() << "plot point weak focus";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    auto x = coWinX(pos[0]);
    auto y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p->type) {
    case P4SingularityStability::stable:
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualstableweakfocus(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_stableweakfocus(staticPainter_, x, y);
            break;
        }
        break;
    case P4SingularityStability::unstable:
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualunstableweakfocus(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_unstableweakfocus(staticPainter_, x, y);
            break;
        }
        break;
    case P4SingularityStability::center:
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualcenter(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_center(staticPainter_, x, y);
            break;
        }
        break;
    default:
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualweakfocus(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_weakfocus(staticPainter_, x, y);
            break;
        }
        break;
    }
}

void P4Sphere::plotPoint(const P4Singularities::strong_focus *p)
{
    qDebug() << "plot point strong focus";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    auto x = coWinX(pos[0]);
    auto y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    if (p->stable == -1) {
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualstablestrongfocus(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_stablestrongfocus(staticPainter_, x, y);
            break;
        }
    } else {
        switch (p->position) {
        case P4Singularities::position_virtual:
            win_plot_virtualunstablestrongfocus(staticPainter_, x, y);
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            win_plot_coinciding(staticPainter_, x, y);
            break;
        default:
            win_plot_unstablestrongfocus(staticPainter_, x, y);
            break;
        }
    }
}

void P4Sphere::plotPoint(const P4Singularities::degenerate *p)
{
    qDebug() << "plot point degenerate";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    auto x = coWinX(pos[0]);
    auto y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p->position) {
    case P4Singularities::position_virtual:
        win_plot_virtualdegen(staticPainter_, x, y);
        break;
    case P4Singularities::position_coinciding:
        break;
    case P4Singularities::position_coinciding_virtual:
        break;
    case P4Singularities::position_coinciding_main:
        win_plot_coinciding(staticPainter_, x, y);
        break;
    default:
        win_plot_degen(staticPainter_, x, y);
        break;
    }
}

void P4Sphere::plotPoint(const P4Singularities::semi_elementary *p)
{
    qDebug() << "plot point semi elementary";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    auto x = coWinX(pos[0]);
    auto y = coWinY(pos[1]);

    if (paintedXMin_ > x - SYMBOLWIDTH / 2)
        paintedXMin_ = x - SYMBOLWIDTH / 2;
    if (paintedXMax_ < x + SYMBOLWIDTH / 2)
        paintedXMax_ = x - SYMBOLWIDTH / 2;
    if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
        paintedYMin_ = y - SYMBOLHEIGHT / 2;
    if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
        paintedYMax_ = y - SYMBOLHEIGHT / 2;

    switch (p->position) {
    case P4Singularities::position_virtual:
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
    case P4Singularities::position_coinciding:
        break;
    case P4Singularities::position_coinciding_virtual:
        break;
    case P4Singularities::position_coinciding_main:
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
        break;
    }
}

void P4Sphere::plotPoints()
{
    qDebug() << "plot points";
    for (auto const &vf : gVFResults.vf_) {
        for (auto sp = vf->firstSaddlePoint_; sp != nullptr;
             sp = sp->next_saddle)
            plotPoint(sp);
        for (auto np = vf->firstNodePoint_; np != nullptr; np = np->next_node)
            plotPoint(np);
        for (auto wfp = vf->firstWfPoint_; wfp != nullptr; wfp = wfp->next_wf)
            plotPoint(wfp);
        for (auto sfp = vf->firstSfPoint_; sfp != nullptr; sfp = sfp->next_sf)
            plotPoint(sfp);
        for (auto sep = vf->firstSePoint_; sep != nullptr; sep = sep->next_se)
            plotPoint(sep);
        for (auto dp = vf->firstDePoint_; dp != nullptr; dp = dp->next_de)
            plotPoint(dp);
    }
}

void P4Sphere::plotPointSeparatrices(const P4Singularities::semi_elementary *p)
{
    // qDebug() << "plot point separatrices (semi elementary)";
    for (auto sep = p->separatrices; sep != nullptr; sep = sep->next_sep)
        draw_sep(this, sep->first_sep_point);
}

void P4Sphere::plotPointSeparatrices(const P4Singularities::saddle *p)
{
    // qDebug() << "plot point separatrices (saddle)";
    for (auto sep = p->separatrices; sep != nullptr; sep = sep->next_sep)
        draw_sep(this, sep->first_sep_point);
}

void P4Sphere::plotPointSeparatrices(const P4Singularities::degenerate *p)
{
    // qDebug() << "plot point separatrices (degenerate)";
    for (auto b = p->blow_up; b != nullptr; b = b->next_blow_up_point)
        draw_sep(this, b->first_sep_point);
}

void P4Sphere::plotSeparatrices()
{
    // qDebug() << "plot separatrices";
    for (auto const &vf : gVFResults.vf_) {
        for (auto sp = vf->firstSaddlePoint_; sp != nullptr;
             sp = sp->next_saddle)
            plotPointSeparatrices(sp);
        for (auto sep = vf->firstSePoint_; sep != nullptr; sep = sep->next_se)
            plotPointSeparatrices(sep);
        for (auto dp = vf->firstDePoint_; dp != nullptr; dp = dp->next_de)
            plotPointSeparatrices(dp);
    }
}

void P4Sphere::plotGcf()
{
    // qDebug() << "plot gcf";
    for (auto const &vf : gVFResults.vf_) {
        draw_gcf(this, vf->gcf_points_,
                 P4ColourSettings::colour_curve_singularities, 1);
    }
}

void P4Sphere::plotSeparatingCurves()
{
    // qDebug() << "plot separating curves";
    bool dashes;
    double pcoord[3];

    if (gVFResults.separatingCurves_.empty())
        return;

    for (unsigned int k = 0; k < gThisVF->numSeparatingCurves_; k++) {
        dashes = true;
        auto sep = gVFResults.separatingCurves_[k].points;
        while (sep != nullptr) {
            if (sep->color == P4ColourSettings::colour_separating_curve) {
                if (sep->dashes && dashes)
                    (*plot_l)(this, pcoord, sep->pcoord, sep->color);
                else {
                    if (sep->nextpt == nullptr)
                        (*plot_p)(this, sep->pcoord, sep->color);
                    else if (!(sep->nextpt->dashes &&
                               sep->nextpt->color ==
                                   P4ColourSettings::colour_separating_curve &&
                               dashes))
                        (*plot_p)(this, sep->pcoord, sep->color);
                    // draw nothing when the next point is a dash
                }
                dashes = true;
            } else {
                dashes = false;
            }
            copy_x_into_y(sep->pcoord, pcoord);
            sep = sep->nextpt;
        }
    }
}

void P4Sphere::plotArbitraryCurves()
{
    // qDebug() << "plot arbitrary curves";
    for (auto const &it : gVFResults.arbitraryCurves_) {
        drawArbitraryCurve(this, it.points,
                           P4ColourSettings::colour_arbitrary_curve, 1);
    }
}

void P4Sphere::plotIsoclines()
{
    // qDebug() << "plot isoclines";
    for (auto const &vf : gVFResults.vf_) {
        for (auto const &it : vf->isocline_vector_) {
            draw_isoclines(this, it.points, it.color, 1);
        }
    }
}

// -----------------------------------------------------------------------
//                          PLOT TOOLS
// -----------------------------------------------------------------------

std::vector<P4POLYLINES> P4Sphere::produceEllipse(double cx, double cy,
                                                  double a, double b,
                                                  bool dotted, double resa,
                                                  double resb)
{
    // this is an exact copy of the plotEllipse routine, except that output
    // is stored in a list of points that is dynamically allocated.

    double theta{0}, t1, t2, e, R, x, y, c, prevx{0}, prevy{0};
    bool d{false};
    bool doton{true};
    int dotcount{0};
    std::vector<P4POLYLINES> result;

    R = (resa < resb) ? resa : resb;
    if (R < 1.0)
        R = 1.0; // protection
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
                    doton = !doton;
                    dotcount = 0;
                }
            }
        } else {
            if (doton) {
                result.emplace_back(prevx, prevy, x, y);
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

void P4Sphere::plotPoincareSphere()
{
    // qDebug() << "plot poincare sphere";
    int color{P4ColourSettings::colour_line_at_infinity};

    staticPainter_->setPen(P4Colours::p4XfigColour(color));
    for (auto const &it : circleAtInfinity_) {
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
    }
}

void P4Sphere::plotPoincareLyapunovSphere()
{
    // qDebug() << "plot poincare-lyapunov sphere";
    int color{P4ColourSettings::colour_line_at_infinity};

    staticPainter_->setPen(P4Colours::p4XfigColour(color));
    for (auto const &it : circleAtInfinity_) {
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
    }

    for (auto const &it : plCircle_) {
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
    }
    return;
}

void P4Sphere::plotLineAtInfinity()
{
    // qDebug() << "plot line at infinity";
    switch (gVFResults.typeofview_) {
    case P4TypeOfView::typeofview_U1:
    case P4TypeOfView::typeofview_V1:
        if (x0_ < 0.0 && x1_ > 0.0) {
            staticPainter_->setPen(P4Colours::p4XfigColour(
                P4ColourSettings::colour_line_at_infinity));
            staticPainter_->drawLine(coWinX(0.0), 0, coWinX(0.0), h_ - 1);
        }
        break;
    case P4TypeOfView::typeofview_U2:
    case P4TypeOfView::typeofview_V2:
        if (y0_ < 0.0 && y1_ > 0.0) {
            staticPainter_->setPen(P4Colours::p4XfigColour(
                P4ColourSettings::colour_line_at_infinity));
            staticPainter_->drawLine(0, coWinY(0.0), w_ - 1, coWinY(0.0));
        }

        break;
    case P4TypeOfView::typeofview_plane:
    case P4TypeOfView::typeofview_sphere:
        // should not appear
        break;
    }
}

void P4Sphere::drawLine(double x1, double y1, double x2, double y2, int color)
{
    // qDebug() << "draw line";
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

                staticPainter_->setPen(P4Colours::p4XfigColour(color));
                staticPainter_->drawLine(wx1, wy1, wx2, wy2);
            } else {
                // only (x2,y2) is not visible

                if (lineRectangleIntersect(x1, y1, x2, y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(x1);
                    wy1 = coWinY(y1);
                    wx2 = coWinX(x2);
                    wy2 = coWinY(y2);
                    staticPainter_->setPen(P4Colours::p4XfigColour(color));

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
                    staticPainter_->setPen(P4Colours::p4XfigColour(color));

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
                    staticPainter_->setPen(P4Colours::p4XfigColour(color));

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

void P4Sphere::drawPoint(double x, double y, int color)
{
    // qDebug() << "draw point";
    if (staticPainter_ != nullptr) {
        if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
            return;
        staticPainter_->setPen(P4Colours::p4XfigColour(color));
        auto _x = coWinX(x);
        auto _y = coWinY(y);

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
void P4Sphere::printPoint(const P4Singularities::saddle *p)
{
    // qDebug() << "print point saddle";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);
    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    switch (p->position) {
    case P4Singularities::position_virtual:
        print_virtualsaddle(coWinX(pos[0]), coWinY(pos[1]));
        break;
    case P4Singularities::position_coinciding:
        break;
    case P4Singularities::position_coinciding_virtual:
        break;
    case P4Singularities::position_coinciding_main:
        print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
        break;
    default:
        print_saddle(coWinX(pos[0]), coWinY(pos[1]));
        break;
    }
}

void P4Sphere::printPoint(const P4Singularities::node *p)
{
    // qDebug() << "print point node";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    if (p->stable == -1) {
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualstablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_stablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
    } else {
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualunstablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_unstablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
    }
}

void P4Sphere::printPoint(const P4Singularities::weak_focus *p)
{
    // qDebug() << "print point weak focus";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    switch (p->type) {
    case P4SingularityStability::stable:
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualstableweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_stableweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
        break;
    case P4SingularityStability::unstable:
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualunstableweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_unstableweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
        break;
    case P4SingularityStability::center:
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualcenter(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_center(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
        break;
    default:
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualweakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_weakfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
        break;
    }
}

void P4Sphere::printPoint(const P4Singularities::strong_focus *p)
{
    // qDebug() << "print point strong focus";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    if (p->stable == -1) {
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualstablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_stablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
    } else {
        switch (p->position) {
        case P4Singularities::position_virtual:
            print_virtualunstablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case P4Singularities::position_coinciding:
            break;
        case P4Singularities::position_coinciding_virtual:
            break;
        case P4Singularities::position_coinciding_main:
            print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
            break;
        default:
            print_unstablestrongfocus(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
    }
}

void P4Sphere::printPoint(const P4Singularities::degenerate *p)
{
    // qDebug() << "print point degenerate";
    double pos[2];

    getChartPos(p->chart, p->x0, p->y0, pos);

    if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
        return;

    switch (p->position) {
    case P4Singularities::position_virtual:
        print_virtualdegen(coWinX(pos[0]), coWinY(pos[1]));
        break;
    case P4Singularities::position_coinciding:
        break;
    case P4Singularities::position_coinciding_virtual:
        break;
    case P4Singularities::position_coinciding_main:
        print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
        break;
    default:
        print_degen(coWinX(pos[0]), coWinY(pos[1]));
        break;
    }
}

void P4Sphere::printPoint(const P4Singularities::semi_elementary *p)
{
    // qDebug() << "print point semi elementary";
    double pos[2];

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

    switch (p->position) {
    case P4Singularities::position_virtual:
        switch (p->type) {
        case 1:
            print_virtualsesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 2:
            print_virtualsesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 3:
            print_virtualsesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 4:
            print_virtualsesaddlenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 5:
            print_virtualseunstablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 6:
            print_virtualsesaddle(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 7:
            print_virtualsesaddle(coWinX(pos[0]), coWinY(pos[1]));
            break;
        case 8:
            print_virtualsestablenode(coWinX(pos[0]), coWinY(pos[1]));
            break;
        }
        break;
    case P4Singularities::position_coinciding:
        break;
    case P4Singularities::position_coinciding_virtual:
        break;
    case P4Singularities::position_coinciding_main:
        print_coinciding(coWinX(pos[0]), coWinY(pos[1]));
        break;
    default:
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
        break;
    }
}

void P4Sphere::printPoints()
{
    // qDebug() << "print points";
    print_comment("Printing symbols at all singular points:");

    for (auto const &vf : gVFResults.vf_) {
        for (auto s = vf->firstSaddlePoint_; s != nullptr; s = s->next_saddle)
            printPoint(s);
        for (auto np = vf->firstNodePoint_; np != nullptr; np = np->next_node)
            printPoint(np);
        for (auto wfp = vf->firstWfPoint_; wfp != nullptr; wfp = wfp->next_wf)
            printPoint(wfp);
        for (auto sfp = vf->firstSfPoint_; sfp != nullptr; sfp = sfp->next_sf)
            printPoint(sfp);
        for (auto sep = vf->firstSePoint_; sep != nullptr; sep = sep->next_se)
            printPoint(sep);
        for (auto dp = vf->firstDePoint_; dp != nullptr; dp = dp->next_de)
            printPoint(dp);
    }
}

void P4Sphere::printPointSeparatrices(const P4Singularities::semi_elementary *p)
{
    // qDebug() << "print point separatrices (semi elementary)";
    for (auto s = p->separatrices; s != nullptr; s = s->next_sep) {
        print_comment("Next separatrix of degenerate point:");
        draw_sep(this, s->first_sep_point);
    }
}

void P4Sphere::printPointSeparatrices(const P4Singularities::saddle *p)
{
    // qDebug() << "print point separatrices (saddle)";
    for (auto s = p->separatrices; s != nullptr; s = s->next_sep) {
        print_comment("Next separatrix of saddle point:");
        draw_sep(this, s->first_sep_point);
    }
}

void P4Sphere::printPointSeparatrices(const P4Singularities::degenerate *p)
{
    // qDebug() << "print point separatrices (degenerate)";
    for (auto b = p->blow_up; b != nullptr; b = b->next_blow_up_point) {
        print_comment("Next separatrix of degenerate point:");
        draw_sep(this, b->first_sep_point);
    }
}

void P4Sphere::printSeparatrices()
{
    // qDebug() << "print separatrices";
    for (auto const &vf : gVFResults.vf_) {
        for (auto s = vf->firstSaddlePoint_; s != nullptr; s = s->next_saddle) {
            print_comment("Printing separatrice for saddle singularity:");
            printPointSeparatrices(s);
        }
        for (auto sep = vf->firstSePoint_; sep != nullptr; sep = sep->next_se) {
            print_comment(
                "Printing separatrices for semi-hyperbolic singularity:");
            printPointSeparatrices(sep);
        }
        for (auto dp = vf->firstDePoint_; dp != nullptr; dp = dp->next_de) {
            print_comment("Printing separatrices for degenerate singularity:");
            printPointSeparatrices(dp);
        }
    }
}

void P4Sphere::printGcf()
{
    // qDebug() << "print gcf";
    bool isagcf{false};
    for (auto const &vf : gVFResults.vf_) {
        if (vf->gcf_points_ != nullptr) {
            isagcf = true;
            break;
        }
    }
    if (isagcf) {
        for (auto const &vf : gVFResults.vf_) {
            print_comment("Printing greatest common factor:");
            draw_gcf(this, vf->gcf_points_,
                     P4ColourSettings::colour_curve_singularities, 1);
        }
    }
}

void P4Sphere::printSeparatingCurves()
{
    // qDebug() << "print separating curve";
    QString comment;
    bool dashes;
    double pcoord[3];

    if (gThisVF->numSeparatingCurves_ > 0 &&
        !gVFResults.separatingCurves_.empty()) {
        print_comment("Printing separating curves:");
        for (unsigned int i = 0; i < gThisVF->numSeparatingCurves_; i++) {
            comment.sprintf("Curve #%d:", i + 1);
            print_comment(comment);
            dashes = true;
            auto sep = gVFResults.separatingCurves_[i].points;
            while (sep != nullptr) {
                if (sep->color == P4ColourSettings::colour_separating_curve) {
                    if (sep->dashes && dashes)
                        (*plot_l)(this, pcoord, sep->pcoord, sep->color);
                    else {
                        if (sep->nextpt == nullptr)
                            (*plot_p)(this, sep->pcoord, sep->color);
                        else if (!sep->nextpt->dashes ||
                                 sep->nextpt->color !=
                                     P4ColourSettings::
                                         colour_separating_curve ||
                                 !dashes)
                            (*plot_p)(this, sep->pcoord, sep->color);
                        // draw nothing when the next point is a dash
                    }
                    dashes = true;
                } else {
                    dashes = false;
                }
                copy_x_into_y(sep->pcoord, pcoord);
                sep = sep->nextpt;
            }
        }
    }
}

void P4Sphere::printArbitraryCurves()
{
    // qDebug() << "print arbitrary curves";
    QString comment;
    int i{1};
    for (auto const &it : gVFResults.arbitraryCurves_) {
        comment.sprintf("Printing curve %d:", i++);
        print_comment(comment);
        drawArbitraryCurve(this, it.points,
                           P4ColourSettings::colour_arbitrary_curve, 1);
    }
}

void P4Sphere::printIsoclines()
{
    // qDebug() << "print isoclines";
    QString comment;
    int i{1};
    for (auto const &vf : gVFResults.vf_) {
        for (auto const &it : vf->isocline_vector_) {
            comment.sprintf("Printing isocline %d:", i++);
            print_comment(comment);
            draw_isoclines(this, it.points, it.color, 1);
        }
    }
}

void P4Sphere::printPoincareSphere()
{
    // qDebug() << "print poincare sphere";
    print_comment("Printing Poincare Sphere:");
    auto p =
        produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
    for (auto &q : p) {
        q.x1 = coWinX(q.x1);
        q.y1 = coWinY(q.y1);
        q.x2 = coWinX(q.x2);
        q.y2 = coWinY(q.y2);
    }
    print_elips(coWinX(0), coWinY(0), coWinH(1), coWinV(1),
                P4ColourSettings::colour_line_at_infinity, false, p);
}

void P4Sphere::printPoincareLyapunovSphere()
{
    // qDebug() << "print poincare-lyapunov sphere";
    print_comment("Printing Poincare Lyapunov-Sphere (circle at infinity):");

    auto p =
        produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
    for (auto &q : p) {
        q.x1 = coWinX(q.x1);
        q.y1 = coWinY(q.y1);
        q.x2 = coWinX(q.x2);
        q.y2 = coWinY(q.y2);
    }
    print_elips(coWinX(0.0), coWinY(0.0), coWinH(1.0), coWinV(1.0),
                P4ColourSettings::colour_line_at_infinity, false, p);

    p.clear();

    print_comment(
        "Printing Poincare Lyapunov-Sphere (circle of finite radius):");

    p = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS),
                       coWinV(RADIUS));
    for (auto &q : p) {
        q.x1 = coWinX(q.x1);
        q.y1 = coWinY(q.y1);
        q.x2 = coWinX(q.x2);
        q.y2 = coWinY(q.y2);
    }
    print_elips(coWinX(0.0), coWinY(0.0), coWinH(RADIUS), coWinV(RADIUS),
                P4ColourSettings::colour_line_at_infinity, true, p);
}

void P4Sphere::printLineAtInfinity()
{
    // qDebug() << "print line at infinity";
    switch (gVFResults.typeofview_) {
    case P4TypeOfView::typeofview_U1:
    case P4TypeOfView::typeofview_V1:
        if (x0_ < 0.0 && x1_ > 0.0)
            print_line(coWinX(0.0), coWinY(y0_), coWinX(0.0), coWinY(y1_),
                       P4ColourSettings::colour_line_at_infinity);
        break;
    case P4TypeOfView::typeofview_U2:
    case P4TypeOfView::typeofview_V2:
        if (y0_ < 0.0 && y1_ > 0.0)
            print_line(coWinX(x0_), coWinY(0.0), coWinX(x1_), coWinY(0.0),
                       P4ColourSettings::colour_line_at_infinity);
        break;
    }
}

void P4Sphere::printOrbits()
{
    // qDebug() << "print orbits";
    // inspired by DrawOrbits, except that we put comments between
    QString s;
    int i{1};
    for (auto o = gVFResults.firstOrbit_; o != nullptr; o = o->next) {
        s.sprintf("Starting orbit %d", i++);
        print_comment(s);
        drawOrbit(this, o->pcoord, o->firstpt, o->color);
    }
}

void P4Sphere::printLimitCycles()
{
    // qDebug() << "print limit cycles";
    // inspired by DrawOrbits, except that we put comments between
    QString s;
    int i{1};
    for (auto o = gVFResults.firstLimCycle_; o != nullptr; o = o->next) {
        s.sprintf("Starting limit cycle %d", i++);
        print_comment(s);
        drawOrbit(this, o->pcoord, o->firstpt, o->color);
    }
}

void P4Sphere::printLine(double x1, double y1, double x2, double y2, int color)
{
    // qDebug() << "print line";
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

void P4Sphere::printPoint(double x, double y, int color)
{
    // qDebug() << "print point";
    if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
        return;

    print_point(coWinX(x), coWinY(y), color);
}

void P4Sphere::refresh()
{
    isPainterCacheDirty_ = true;
    update();
}

void P4Sphere::calculateHeightFromWidth(int &width, int &height,
                                        int maxheight = -1,
                                        double aspectratio = 1)
{
    // given an optimal width in width, this procedure calculates the
    // corresponding height in order to maintain the given aspectratio. If
    // however the maximum height is violated, then we choose to have the
    // maximum height and calculate the corresponding width.

    double w{static_cast<double>(width)};
    double h{w * dy_ / dx_ * aspectratio};

    if (std::round(h) <= maxheight || maxheight == -1) {
        height = static_cast<int>(std::round(h));
    } else {
        height = maxheight;
        h = static_cast<double>(maxheight);
        w = h * dx_ / dy_;
        w /= aspectratio;
        width = static_cast<int>(std::round(w));
    }
}

void P4Sphere::preparePrinting(int printmethod, bool isblackwhite,
                               int myresolution, double mylinewidth,
                               double mysymbolsize)
{
    // qDebug() << "prepare printing";
    double pagewidth, pageheight;
    double aspectratio{1}; // assume aspect ratio 1

    printMethod_ = printmethod;

    if (printmethod == P4PRINT_DEFAULT) {
        pagewidth = gP4printer->width();
        pageheight = gP4printer->height();
    } else
        pagewidth = pageheight = -1; // will be redefined in a minute

    sP4pixmapDPM = myresolution / 2.54;

    double hpixels{myresolution * 15 / 2.54};
    double maxvpixels{(myresolution * aspectratio * 20) / 2.54 + 0.5};

    oldw_ = w_;
    oldh_ = h_;
    w_ = static_cast<int>(std::round(hpixels));

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
        pagewidth = myresolution * POSTSCRIPTPAGEWIDTH; // see custom.cpp
        pageheight = myresolution * POSTSCRIPTPAGEHEIGHT;
        break;
    }

    if (w_ > pagewidth && pagewidth != -1)
        w_ = static_cast<int>(std::round(pagewidth));
    if (maxvpixels > pageheight && pageheight != -1)
        maxvpixels = pageheight;

    calculateHeightFromWidth(w_, h_, std::round(maxvpixels), aspectratio);

    // 25.4 dots per mm
    double lw{myresolution * mylinewidth / 25.4};
    if (lw < 1.0)
        lw = 1.0;

    // 25.4 dots per mm
    double ss{(myresolution * mysymbolsize / 25.4 + 0.5) / 2.0};
    if (ss < 1.0)
        ss = 1.0;

    double tx, ty;
    if (pagewidth == -1 || pageheight == -1) {
        tx = 0;
        ty = 0;
    } else {
        tx = (pagewidth - w_) / 2.0;
        ty = (pageheight - h_) / 2.0;
    }

    switch (printmethod) {
    case P4PRINT_EPSIMAGE:
        reverseYAxis_ = true;
        preparePostscriptPrinting(std::round(tx), std::round(ty), w_, h_,
                                  iszoom_, isblackwhite, myresolution,
                                  std::round(lw), 2 * std::round(ss));
        break;
    case P4PRINT_XFIGIMAGE:
        reverseYAxis_ = false;
        prepareXFigPrinting(w_, h_, iszoom_, isblackwhite, myresolution,
                            std::round(lw), 2 * std::round(ss));
        break;
    case P4PRINT_DEFAULT:
        staticPainter_ = new QPainter{};

        if (!staticPainter_->begin(gP4printer)) {
            delete staticPainter_;
            staticPainter_ = nullptr;
            return;
        }

        staticPainter_->translate(tx, ty);
        if (iszoom_ ||
            gVFResults.typeofview_ == P4TypeOfView::typeofview_plane) {
            QPen p{P4Colours::p4XfigColour(
                       printColorTable(P4ColourSettings::colour_foreground)),
                   std::round(lw)};
            staticPainter_->setPen(p);
            staticPainter_->drawRect(0, 0, w_, h_);
        }
        reverseYAxis_ = false; // no need for reversing axes in this case
        prepareP4Printing(w_, h_, isblackwhite, staticPainter_, std::round(lw),
                          2 * std::round(ss));
        break;

    case P4PRINT_JPEGIMAGE:
        staticPainter_ = new QPainter{};
        sP4pixmap = new QPixmap{w_, h_};
        reverseYAxis_ = false; // no need for reversing axes in this case
        if (sP4pixmap->isNull()) {
            msgBar_->showMessage(
                "Print failure (try to choose a lower resolution).");
            delete sP4pixmap;
            sP4pixmap = nullptr;
            delete staticPainter_;
            staticPainter_ = nullptr;
            return;
        }
        if (!staticPainter_->begin(sP4pixmap)) {
            delete sP4pixmap;
            sP4pixmap = nullptr;
            delete staticPainter_;
            staticPainter_ = nullptr;
            return;
        }

        prepareP4Printing(w_, h_, isblackwhite, staticPainter_, std::round(lw),
                          2 * std::round(ss));
        break;
    }
    msgBar_->showMessage("Printing ...");
}

void P4Sphere::finishPrinting()
{
    // qDebug() << "finish printing";
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
        if (sP4pixmap == nullptr) {
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

        if (sP4pixmap->save(gThisVF->getbarefilename() + ".jpg", "JPEG", 100) ==
            false) {
            QMessageBox::critical(this, "P4",
                                  "For some reason, P4 is unable to save the "
                                  "resulting JPEG image to disc.");
        }

        delete sP4pixmap;
        sP4pixmap = nullptr;
        reverseYAxis_ = false;
        w_ = oldw_;
        h_ = oldh_;
    }
    msgBar_->showMessage("Printing has finished.");
}

// TODO call from paintEvent with a printFlag or something
void P4Sphere::print()
{
    // qDebug() << "print";
    if (printMethod_ == P4PRINT_JPEGIMAGE && sP4pixmap == nullptr)
        return;

    if (gVFResults.typeofview_ != P4TypeOfView::typeofview_plane) {
        if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
            if (gVFResults.plweights_)
                printPoincareLyapunovSphere();
            else
                printPoincareSphere();
        } else
            printLineAtInfinity();
    }
    printSeparatingCurves();
    printOrbits();
    printSeparatrices();
    printGcf();
    printArbitraryCurves(); // TODO
    printIsoclines();
    printLimitCycles();
    printPoints();
}

void P4Sphere::prepareDrawing()
{
    // qDebug() << "prepare drawing";
    if (painterCache_ == nullptr) {
        isPainterCacheDirty_ = true;
        painterCache_ = new QPixmap{size()};
    }
    staticPainter_ = new QPainter{painterCache_};

    paintedXMin_ = width() - 1;
    paintedYMin_ = height() - 1;
    paintedXMax_ = 0;
    paintedYMax_ = 0;

    if (next_ != nullptr)
        next_->prepareDrawing();
}

void P4Sphere::finishDrawing()
{
    // qDebug() << "finish drawing";
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

/*void P4Sphere::signalEvaluating()
{
    //qDebug() << "signal evaluating";
        QPalette palette;
        palette.setColor(backgroundRole(),
P4Colours::p4XfigColour(spherebgcolor_ = P4ColourSettings::colour_background) );
        setPalette(palette);
}*/

void P4Sphere::signalChanged()
{
    // qDebug() << "signal changed";
    /*
        QPalette palette;
        palette.setColor(backgroundRole(),
       P4Colours::p4XfigColour(spherebgcolor_ = DARKGRAY)
       );
        setPalette(palette);
    */
}
