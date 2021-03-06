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

#include "P4InputSphere.hpp"

#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>
#include <QTimer>

#include <cmath>
#include <utility>

#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4SeparatingCurvesDlg.hpp"
#include "math_charts.hpp"
#include "math_p4.hpp"
#include "math_regions.hpp"
#include "plot_tools.hpp"

P4InputSphere::P4InputSphere(P4SeparatingCurvesDlg *wnd, QLabel *lbl_status,
                             QWidget *parent)
    : QWidget{parent}, status_{lbl_status}, parentWnd_{wnd}
{
    //    setAttribute(Qt::WA_PaintOnScreen);

    setMinimumSize(MINWIDTHPLOTWINDOW, MINHEIGHTPLOTWINDOW); // minimum size
    w_ = width();
    h_ = height();
    idealh_ = w_;

    horPixelsPerMM_ = (static_cast<double>(w_)) / widthMM();
    verPixelsPerMM_ = (static_cast<double>(h_)) / heightMM();

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(windowFlags());

    setupPlot();
}

void P4InputSphere::keyPressEvent(QKeyEvent *e) { e->ignore(); }

void P4InputSphere::setupPlot()
{
    QPalette palette;

    setupCoordinateTransformations();

    spherebgcolor_ = P4ColourSettings::colour_background;
    palette.setColor(backgroundRole(), P4Colours::p4XfigColour(spherebgcolor_));
    setPalette(palette);

    // clear circle vectors
    circleAtInfinity_.clear();
    plCircle_.clear();

    if (!isZoom_) {
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
    double idealhd{w_ / dx_ * dy_};
    idealh_ = std::round(idealhd);

    if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
        circleAtInfinity_ =
            produceEllipse(0., 0., 1., 1., false, coWinH(1.), coWinV(1.));
        if (gThisVF->p_ != 1 || gThisVF->q_ != 1)
            plCircle_ = produceEllipse(0., 0., RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }
    isPainterCacheDirty_ = true;
}

void P4InputSphere::loadAnchorMap()
{
    if (!selectingZoom_)
        return;

    int x1{zoomAnchor1_.x()}, y1{zoomAnchor1_.y()};
    int x2{zoomAnchor2_.x()}, y2{zoomAnchor2_.y()};

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

    int aw{x2 - x1 + 1};
    int ah{y2 - y1 + 1};

    if (anchorMap_ != nullptr) {
        if (anchorMap_->width() < aw || anchorMap_->height() < ah) {
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
    paint.drawPixmap(0, 0, aw, ah, *painterCache_, x1, y1, aw, ah);
}

void P4InputSphere::saveAnchorMap()
{
    if (!anchorMap_ || !painterCache_ || !selectingZoom_)
        return;

    int x1{zoomAnchor1_.x()};
    int y1{zoomAnchor1_.y()};
    int x2{zoomAnchor2_.x()};
    int y2{zoomAnchor2_.y()};

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

    int aw{x2 - x1 + 1};
    int ah{y2 - y1 + 1};

    QPainter paint{painterCache_};
    paint.drawPixmap(x1, y1, aw, ah, *anchorMap_, 0, 0, aw, ah);
    update(x1, y1, aw, ah);
}

void P4InputSphere::adjustToNewSize()
{
    w_ = width();
    h_ = height();

    double idealhd{w_ / dx_ * dy_};
    idealh_ = std::round(idealhd);

    double reqratio{((static_cast<double>(w_)) / horPixelsPerMM_) /
                    (idealh_ / verPixelsPerMM_)};
    double ratio{((static_cast<double>(w_)) / horPixelsPerMM_) /
                 ((static_cast<double>(h_)) / verPixelsPerMM_)};

    QString buf;
    buf.sprintf("Aspect ratio = %8.5g\n", ratio / reqratio);
    status_->setText(buf);
    status_->setMaximumWidth(w_);

    circleAtInfinity_.clear();
    plCircle_.clear();

    if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
        circleAtInfinity_ =
            produceEllipse(0., 0., 1., 1., false, coWinH(1.), coWinV(1.));
        if (gThisVF->p_ != 1 || gThisVF->q_ != 1)
            plCircle_ = produceEllipse(0., 0., RADIUS, RADIUS, true,
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

        if (gVFResults.typeofview_ != P4TypeOfView::typeofview_plane) {
            if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
                if (gThisVF->p_ != 1 || gThisVF->q_ != 1)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }

        // during resizing: only plot essential information
        QColor c = P4Colours::p4XfigColour(P4Colours::white);
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
                             &P4InputSphere::refreshAfterResize);
        }
        refreshTimeout_->start(500);
    }
}

void P4InputSphere::refreshAfterResize()
{
    if (refreshTimeout_ != nullptr) {
        delete refreshTimeout_;
        refreshTimeout_ = nullptr;
    }
    refresh();
}

void P4InputSphere::resizeEvent(QResizeEvent *e)
{
    if (e->size() == e->oldSize())
        return;
    adjustToNewSize();
}

void P4InputSphere::paintEvent(QPaintEvent *p)
{
    if (gThisVF->evaluating_)
        return;

    if (painterCache_ == nullptr || isPainterCacheDirty_) {
        if (painterCache_ == nullptr)
            painterCache_ = new QPixmap{size()};
        isPainterCacheDirty_ = false;

        QPainter paint{painterCache_};
        paint.fillRect(0, 0, width(), height(),
                       QColor(P4Colours::p4XfigColour(
                           P4ColourSettings::colour_background)));
        paint.setPen(
            P4Colours::p4XfigColour(P4ColourSettings::colour_line_at_infinity));

        staticPainter_ = &paint;

        if (gVFResults.typeofview_ != P4TypeOfView::typeofview_plane) {
            if (gVFResults.typeofview_ == P4TypeOfView::typeofview_sphere) {
                if (gThisVF->p_ != 1 || gThisVF->q_ != 1)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }

        plotSeparatingCurves();

        staticPainter_ = nullptr;
    }

    QPainter widgetpaint{this};
    widgetpaint.drawPixmap(0, 0, *painterCache_);
}

void P4InputSphere::markSelection(int x1, int y1, int x2, int y2)
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
    QColor c{P4Colours::p4XfigColour(P4Colours::white)};
    c.setAlpha(32);
    p.setPen(P4Colours::p4XfigColour(P4Colours::white));
    p.setBrush(c);

    if (bx1 == bx2 || by1 == by2)
        p.drawLine(bx1, by1, bx2, by2);
    else
        p.drawRect(bx1, by1, bx2 - bx1, by2 - by1);

    update(bx1, by1, bx2 - bx1 + 1, by2 - by1 + 1);
}

void P4InputSphere::mouseMoveEvent(QMouseEvent *e)
{
    int x{e->x()}, y{e->y()};
    double wx{coWorldX(x)}, wy{coWorldY(y)};
    double ucoord[2], pcoord[3];
    QString buf;
    int index;

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
                buf.sprintf("The Poincare sphere   (x,y) = (%8.5g,%8.5g)",
                            ucoord[0], ucoord[1]);
            else
                buf.sprintf(
                    "The P-L sphere of type (%d,%d)   (x,y) = (%8.5g,%8.5g)",
                    gVFResults.p_, gVFResults.q_, ucoord[0], ucoord[1]);
            break;
        case P4TypeOfView::typeofview_U1:
            MATHFUNC(sphere_to_U1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U1 chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            else
                buf.sprintf("The V1' chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            break;
        case P4TypeOfView::typeofview_V1:
            MATHFUNC(sphere_to_V1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!gVFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V1 chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            else
                buf.sprintf("The U1' chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            break;
        case P4TypeOfView::typeofview_U2:
            MATHFUNC(sphere_to_U2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U2 chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            else
                buf.sprintf("The V2' chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            break;
        case P4TypeOfView::typeofview_V2:
            MATHFUNC(sphere_to_V2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!gVFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V2 chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            else
                buf.sprintf("The U2' chart   (z1,z2) = (%8.5g,%8.5g) ",
                            ucoord[1], ucoord[0]);
            break;
        }

        if (!gVFResults.separatingCurves_.empty()) {
            QString s;
            index = gThisVF->getVFIndex_sphere(pcoord);
            if (index < 0) {
                buf.append("   VF: NONE");
            } else {
                s.sprintf("   VF: #%d", index + 1);
                buf.append(s);
            }
            s = describeRegion(pcoord);
            buf += " (" + s + ")";
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

    status_->setText(buf);

    if (selectingZoom_) {
        saveAnchorMap();
        zoomAnchor2_ = e->pos();
        loadAnchorMap();
        markSelection(zoomAnchor1_.x(), zoomAnchor1_.y(), zoomAnchor2_.x(),
                      zoomAnchor2_.y());
    }
}

double P4InputSphere::coWorldX(int x)
{
    double wx{(static_cast<double>(x)) / (w_ - 1)};
    return wx * dx_ + x0_;
}

double P4InputSphere::coWorldY(int y)
{
    double wy{(static_cast<double>(h_ - 1 - y)) / (h_ - 1)};
    return wy * dy_ + y0_;
}

int P4InputSphere::coWinX(double x)
{
    double wx{((x - x0_) / dx_) * (w_ - 1)};
    int iwx{static_cast<int>(wx + 0.5)};
    if (iwx >= w_)
        iwx = w_ - 1;
    return iwx;
}

int P4InputSphere::coWinY(double y)
{
    double wy{((y - y0_) / dy_) * (h_ - 1)};
    int iwy{static_cast<int>(wy + 0.5)};
    if (iwy >= h_)
        iwy = h_ - 1;
    return h_ - 1 - iwy;
}

int P4InputSphere::coWinH(double deltax)
{
    double wx{(deltax / dx_) * (w_ - 1)};
    return static_cast<int>(std::round(wx));
}

int P4InputSphere::coWinV(double deltay)
{
    double wy{(deltay / dy_) * (h_ - 1)};
    return static_cast<int>(std::round(wy));
}

void P4InputSphere::mousePressEvent(QMouseEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier &&
        e->button() == Qt::LeftButton) {
        if (!selectingZoom_) {
            selectingZoom_ = true;
            zoomAnchor1_ = e->pos();
            zoomAnchor2_ = zoomAnchor1_;
            loadAnchorMap();
            markSelection(zoomAnchor1_.x(), zoomAnchor1_.y(), zoomAnchor2_.x(),
                          zoomAnchor2_.y());
        }
        QWidget::mousePressEvent(e);
        return;
    }

    if (e->button() == Qt::LeftButton) {
        int x{e->x()};
        int y{e->y()};
        double wx{coWorldX(x)};
        double wy{coWorldY(y)};
        double pcoord[3];

        if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord))
            parentWnd_->onMouseClickLeft(pcoord[0], pcoord[1], pcoord[2]);
        return;
    }

    //    double pcoord[3];

    if (e->button() == Qt::RightButton) {
        // cancel zoom window with right mouse button
        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;
        } else {
            //            int x{e->x()};
            //            int y{e->y()};
            //            double wx{coWorldX(x)};
            //            double wy{coWorldY(y)};
            //            if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord)) {
            //                parentWnd_->onMouseClickRight(pcoord[0],
            //                pcoord[1], pcoord[2]);
            //            }
            return;
        }
        QWidget::mousePressEvent(e);
        return;
    }
    QWidget::mousePressEvent(e);
}

void P4InputSphere::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // finish zoom window between zoomAnchor1 and zoomAnchor2
        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;

            int _x1{zoomAnchor1_.x()};
            int _y1{zoomAnchor1_.y()};
            int _x2{zoomAnchor2_.x()};
            int _y2{zoomAnchor2_.y()};

            parentWnd_->makeZoom(coWorldX(_x1), coWorldY(_y1), coWorldX(_x2),
                                 coWorldY(_y2));
        }
        QWidget::mouseReleaseEvent(e);
        return;
    }
    QWidget::mouseReleaseEvent(e);
}

void P4InputSphere::drawLine(double x1, double y1, double x2, double y2,
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

                staticPainter_->setPen(P4Colours::p4XfigColour(color));
                staticPainter_->drawLine(wx1, wy1, wx2, wy2);
            } else {
                // only (x2, y2) is not visible
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

void P4InputSphere::drawPoint(double x, double y, int color)
{
    if (staticPainter_ != nullptr) {
        if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
            return;

        staticPainter_->setPen(P4Colours::p4XfigColour(color));

        int _x{coWinX(x)}, _y{coWinY(y)};
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

void P4InputSphere::prepareDrawing()
{
    if (painterCache_ == nullptr) {
        isPainterCacheDirty_ = true;
        painterCache_ = new QPixmap{size()};
    }
    staticPainter_ = new QPainter{painterCache_};

    paintedXMin_ = width() - 1;
    paintedYMin_ = height() - 1;
    paintedXMax_ = 0;
    paintedYMax_ = 0;
}

void P4InputSphere::finishDrawing()
{
    if (staticPainter_) {
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

void P4InputSphere::refresh()
{
    isPainterCacheDirty_ = true;
    update();
}

void P4InputSphere::plotPoincareSphere()
{
    int color{P4ColourSettings::colour_line_at_infinity};

    staticPainter_->setPen(P4Colours::p4XfigColour(color));
    for (auto const &it : circleAtInfinity_)
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
}

void P4InputSphere::plotPoincareLyapunovSphere()
{
    staticPainter_->setPen(
        P4Colours::p4XfigColour(P4ColourSettings::colour_line_at_infinity));

    for (auto const &it : circleAtInfinity_)
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));

    for (auto const &it : plCircle_)
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
}

void P4InputSphere::plotLineAtInfinity()
{
    switch (gVFResults.typeofview_) {
    case P4TypeOfView::typeofview_U1:
    case P4TypeOfView::typeofview_V1:
        if (x0_ < 0.0 && x1_ > 0.0) {
            staticPainter_->setPen(P4Colours::p4XfigColour(
                P4ColourSettings::colour_line_at_infinity));
            staticPainter_->drawLine(coWinX(0.0), 0, coWinY(0.0), h_ - 1);
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
    }
}

std::vector<P4POLYLINES> P4InputSphere::produceEllipse(double cx, double cy,
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
    double prevx{0.0}, prevy{0.0};
    bool d{false}, doton{true};
    int dotcount{0};
    std::vector<P4POLYLINES> result;

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
                    doton = (doton) ? false : true;
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

void P4InputSphere::plotSeparatingCurves()
{
    if (!gVFResults.separatingCurves_.empty())
        for (unsigned int r = 0; r < gThisVF->numSeparatingCurves_; r++)
            plotSeparatingCurve(gVFResults.separatingCurves_[r], r);
}

void P4InputSphere::plotSeparatingCurve(const P4Curves::curves &crv, int index)
{
    double pcoord[3];
    auto const &sep = crv.points;
    for (auto const &it : sep) {
        if (it.dashes)
            plotLine(pcoord, it.pcoord, it.color);
        else
            plotPoint(it.pcoord, it.color);
        copy_x_into_y(it.pcoord, pcoord);
    }
}

void P4InputSphere::plotLine(const double *p1, const double *p2, int color)
{
    double ucoord1[2];
    double ucoord2[2];
    double ucoord3[2];
    double ucoord4[2];

    if (MATHFUNC(sphere_to_viewcoordpair)(p1, p2, ucoord1, ucoord2, ucoord3,
                                          ucoord4)) {
        drawLine(ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color);
    } else {
        drawLine(ucoord1[0], ucoord1[1], ucoord2[0], ucoord2[1], color);
        drawLine(ucoord3[0], ucoord3[1], ucoord4[0], ucoord4[1], color);
    }
}

void P4InputSphere::plotPoint(const double *p, int color)
{
    double ucoord[2];
    MATHFUNC(sphere_to_viewcoord)(p[0], p[1], p[2], ucoord);
    drawPoint(ucoord[0], ucoord[1], color);
}

void P4InputSphere::setupCoordinateTransformations()
{
    gVFResults.setupCoordinateTransformations();
}
