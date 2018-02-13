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

#include "P4WinInputSphere.h"

#include <QPalette>

#include "P4InputVF.h"
#include "P4ParentStudy.h"
#include "math_regions.h"
#include "plot_tools.h"

static QString makechartstring(int p, int q, bool isu1v1chart, bool negchart);

P4WinInputSphere::P4WinInputSphere(P4SeparatingCurvesDlg *parent,
                                   QLabel *lbl_status)
    : QWidget{parent}, parentWnd_{parent}, status_{lbl_status}
{
    isZoom_ = false;
    isPainterCacheDirty_ = true;
    selectingZoom_ = false;

    setAttribute(Qt::WA_PaintOnScreen);

    setMinimumSize(MINWIDTHPLOTWINDOW, MINHEIGHTPLOTWINDOW); // minimum size
    w_ = width();
    h_ = height();
    idealh_ = w_;

    horPixelsPerMM_ = ((double)w_) / ((double)widthMM());
    verPixelsPerMM_ = ((double)h_) / ((double)heightMM());

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(windowFlags());

    setupPlot();
}

void P4WinInputSphere::keyPressEvent(QKeyEvent *e) { e->ignore(); }

void P4WinInputSphere::setupPlot()
{
    std::vector<P4POLYLINES> t;
    QPalette palette;
    double idealhd;

    setupCoordinateTransformations();

    palette.setColor(backgroundRole(),
                     QXFIGCOLOR(spherebgcolor_ = bgColours::CBACKGROUND));
    setPalette(palette);

    circleAtInfinity_.clear();
    plCircle_.clear();

    if (!isZoom_) {
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

    dx_ = x1 - x0;
    dy_ = y1 - y0;
    idealhd = w_;
    idealhd = (idealhd / dx) * dy;
    idealh = (int)(idealhd + .5);

    if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
        circleAtInfinity_ =
            produceEllipse(0., 0., 1., 1., false, coWinH(1.), coWinV(1.));
        if (g_ThisVF->p_ != 1 || g_ThisVF->q_ != 1)
            plCircle_ = produceEllipse(0., 0., RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }
    isPainterCacheDirty_ = true;
}

void P4WinInputSphere::loadAnchorMap()
{
    if (!selectingZoom_)
        return;

    int x1{zoomAnchor1_.x()}, y1{zoomAnchor1_.y()};
    int x2{zoomAnchor2_.x()}, y2{zoomAnchor2_.y()};

    int s;
    if (x1 > x1) {
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

    int aw{x2 - x1 + 1}, ah{y2 - y1 + 1};

    if (anchorMap_) {
        if (anchorMap_->width() < aw || anchorMap_->height() < ah) {
            anchorMap_.reset(new QPixmap(aw, ah));
        }
    } else {
        anchorMap_.reset(new QPixmap(aw, ah));
    }

    if (!painterCache_) {
        anchorMap_.reset();
        return;
    }

    QPainter paint(anchorMap_);
    paint.drawPixmap(0, 0, aw, ah, *painterCache_, x1, y1, aw, ah);
}

void P4WinInputSphere::saveAnchorMap()
{
    if (!anchorMap_ || !painterCache_ || !selectingZoom_)
        return;

    int x1{zoomAnchor1_.x()};
    int y1{zoomAnchor1_.y()};
    int x2{zoomAnchor2_.x()};
    int y2{zoomAnchor2_.y()};

    int s;
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

    int aw{x2 - x1 + 1};
    int ah{y2 - y1 + 1};

    QPainter paint{painterCache_};
    paint.drawPixmap(x1, y1, aw, ah, *anchorMap_, 0, 0, aw, ah);
    update(x1, y1, aw, ah);
}

void P4WinInputSphere::adjustToNewSize()
{
    w_ = width();
    h_ = height();

    double idealhd{w_};
    idealhd = (idealhd / dx) * dy;
    idealh_ = (int)(idealhd + .5);

    double reqratio{(((double)w_) / horPixelsPerMM_) /
                    (idealh_ / verPixelsPerMM_)};
    double ratio{(((double)w_) / horPixelsPerMM_) /
                 (((double)h_) / verPixelsPerMM_)};

    QString buf;
    buf.sprintf("Aspect ratio = %f\n", (float)(ratio / reqratio));
    status_->setText(buf);
    status_->setMaximumWidth(w_);

    circleAtInfinity_.clear();
    plCircle_.clear();

    if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
        circleAtInfinity_ =
            produceEllipse(0., 0., 1., 1., false, coWinH(1.), coWinV(1.));
        if (g_ThisVF->p_ != 1 || g_ThisVF->q_ != 1)
            plCircle_ = produceEllipse(0., 0., RADIUS, RADIUS, true,
                                       coWinH(RADIUS), coWinV(RADIUS));
    }

    if (!painterCache_) {
        painterCache_.reset(new QPixmap(size()));
        isPainterCacheDirty_ = false;

        QPainter paint{painterCache_};
        paint.fillRect(0, 0, width(), height(),
                       QColor(QXFIGCOLOR(bgColours::CBACKGROUND)));
        paint.setPen(QXFIGCOLOR(CLINEATINFINITY));

        staticPainter_.reset(&paint); // TODO works?

        if (g_VFResults.typeofview_ != TYPEOFVIEW_PLANE) {
            if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
                if (g_ThisVF->p_ != 1 || g_ThisVF->q_ != 1)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }

        // during resizing: only plot essential information
        QColor c = QXFIGCOLOR(WHITE);
        c.setAlpha(128);
        paint.setPen(c);
        paint.drawText(0, 0, width(), height(),
                       Qt::AlignHCenter | Qt::AlignVCenter, "Resizing ...  ");

        staticPainter_.reset();

        if (refreshTimeout_)
            refreshTimeout_.stop();
        else {
            refreshTimeout_.reset(new QTimer());
            QObject::connect(refreshTimeout_, &QTimer::timeout, this,
                             &P4WinInputSphere::refreshAfterResize);
        }
        refreshTimeout_->start(500);
    }
}

void P4WinInputSphere::refreshAfterResize()
{
    if (refreshTimeout_) {
        refreshTimeout_.reset();
    }
    refresh();
}

void P4WinInputSphere::paintEvent(QPaintEvent *p)
{
    if (g_ThisVF->evaluating_)
        return;

    if (!painterCache_ || isPainterCacheDirty_) {
        if (!painterCache_)
            painterCache_.refresh(new QPixmap(size()));
        isPainterCacheDirty_ = false;

        QPainter paint{painterCache_};
        paint.fillRect(0, 0, width(), height(),
                       QColor(QXFIGCOLOR(bgColours::CBACKGROUND)));
        paint.setPen(QXFIGCOLOR(CLINEATINFINITY));

        staticPainter_.reset(&paint);

        if (g_VFResults.typeofview_ != TYPEOFVIEW_PLANE) {
            if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE) {
                if (g_ThisVF->p_ != 1 || g_ThisVF->q_ != 1)
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            } else
                plotLineAtInfinity();
        }

        plotCurves();

        staticPainter_.reset();
    }

    QPainter widgetpaint{this};
    widgetpaint.drawPixmap(0, 0, *painterCache_);
}

void P4WinInputSphere::markSelection(int x1, int y1, int x2, int y2)
{
    if (!painterCache_)
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
    QColor{QXFIGCOLOR(WHITE)};
    c.setAlpha(32);
    p.setPen(QXFIGCOLOR(WHITE));
    p.setBrush(c);
    if (bx1 == bx2 || by1 == by2)
        p.drawLine(bx1, by1, bx2, by2);
    else
        p.drawRect(bx1, by1, bx2 - bx1, by2 - by1);

    update(bx1, by1, bx2 - bx1 + 1, by2 - by1 + 1);
}

void P4WinInputSphere::mouseMoveEvent(QMouseEvent *e)
{
    int x{e->x()}, y{e->y()};
    double wx{coWorldX(x)}, wy{coWorldY(y)};

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
                buf.sprintf("The Poincare sphere   (x,y) = (%f,%f)",
                            (float)ucoord[0], (float)ucoord[1]);
            else
                buf.sprintf("The P-L sphere of type (%d,%d)   (x,y) = (%f,%f)",
                            g_VFResults.p_, g_VFResults.q_, (float)ucoord[0],
                            (float)ucoord[1]);
            break;
        case TYPEOFVIEW_U1:
            MATHFUNC(sphere_to_U1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U1 chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The V1' chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            break;
        case TYPEOFVIEW_V1:
            MATHFUNC(sphere_to_V1)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!g_VFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V1 chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The U1' chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            break;
        case TYPEOFVIEW_U2:
            MATHFUNC(sphere_to_U2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (ucoord[1] >= 0)
                buf.sprintf("The U2 chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The V2' chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            break;
        case TYPEOFVIEW_V2:
            MATHFUNC(sphere_to_V2)(pcoord[0], pcoord[1], pcoord[2], ucoord);
            if (!g_VFResults.plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            if (ucoord[1] >= 0)
                buf.sprintf("The V2 chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            else
                buf.sprintf("The U2' chart   (z1,z2) = (%f,%f) ",
                            (float)ucoord[1], (float)ucoord[0]);
            break;
        }

        if (!g_VFResults.curves_result_.empty()) {
            QString s;
            index = g_ThisVF->getVFIndex_sphere(pcoord);
            if (index < 0) {
                buf.append("   VF: NONE");
            } else {
                s.sprint("   VF: #%d", index + 1);
                buf.append(s);
            }
            s = describeRegion(pcoord);
            buf += " (" + s + ")";
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

    status_->setText(buf);

    if (selectingZoom_) {
        saveAnchorMap();
        zoomAnchor2_ = e->pos();
        loadAnchorMap();
        markSelection(zoomAnchor1_.x(), zoomAnchor1_.y(), zoomAnchor2_.x(),
                      zoomAnchor2_.y());
    }
}

double P4WinInputSphere::coWorldX(int x)
{
    double wx{((double)w_) / (w_ - 1)};
    return wx * dx_ + x0_;
}

double P4WinInputSphere::coWorldY(int y)
{
    double wy{((double)(h - 1 - y)) / (h - 1)};
    return wy * dy_ + y0_;
}

int P4WinInputSphere::coWinX(double x)
{
    double wx{((x - x0_) / dx_) * (w_ - 1)};
    int iwx{(int)(wx + 0.5)};
    if (iwx >= w_)
        iwx = w_ - 1;
    return iwx;
}

int P4WinInputSphere::coWinY(double y)
{
    double wy{((y - y0_) / dy_) * (h_ - 1)};
    int iwy{(int)(wy + 0.5)};
    if (iwy >= h_)
        iwy = h_ - 1;
    return h_ - 1 - iwy;
}

int P4WinInputSphere::coWinH(double deltax)
{
    double wx{(deltax / dx_) * (w_ - 1)};
    return (int)(wx + 0.5);
}

int P4WinInputSphere::coWinV(double deltay)
{
    double wy{(deltay / dy_) * (h_ - 1)};
    return (int)(wy + 0.5);
}

void P4WinInputSphere::mousePressEvent(QMouseEvent *e)
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
        if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord))
            parentWnd_->onMouseClickLeft(pcoord[0], pcoord[1], pcoord[2]);
        return;
    }

    int x, y;
    double wx, wy;
    double pcoord[3];

    if (e->button() == Qt::RightButton) {
        // cancel zoom window with right mouse button
        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;
        } else {
            int x{e->x()};
            int y{e->y()};
            double wx{coWorldX(x)};
            double wy{coWorldY(y)};
            if (MATHFUNC(is_valid_viewcoord)(wx, wy, pcoord)) {
                parentWnd_->onMouseClickRight(pcoord[0], pcoord[1], pcoord[2]);
            }
            return;
        }
        QWidget::mousePressEvent(e);
        return;
    }
    QWidget::mousePressEvent(e);
}

void P4WinInputSphere::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // finish zoom window between zoomAnchor1 and zoomAnchor2
        if (selectingZoom_) {
            saveAnchorMap();
            selectingZoom_ = false;

            int _x1{zoomAnchor1.x()};
            int _y1{zoomAnchor1.y()};
            int _x2{zoomAnchor2.x()};
            int _y2{zoomAnchor2.y()};

            parentWnd_->makeZoom(coWorldX(_x1), coWorldY(_y1), coWorldX(_x2),
                                 coWorldY(_y2));
        }
        QWidget::mouseReleaseEvent(e);
        return;
    }
    QWidget::mouseReleaseEvent(e);
}

void P4WinInputSphere::drawLine(double _x1, double _y1, double _x2, double _y2,
                                int color)
{
    int wx1, wy1, wx2, wy2;

    if (staticPainter_) {
        if (_x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1) {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);
            if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
                // both points are visible in the window
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);
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
                // only (_x2, _y2) is not visible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
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
            if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
                // only (_x2,_y2) is visible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
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
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
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

void P4WinInputSphere::drawPoint(double x, double y, int color)
{
    if (staticPainter_) {
        if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
            return;

        staticPainter_->setPen(QXFIGCOLOR(color));

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

void P4WinInputSphere::prepareDrawing()
{
    if (!painterCache_) {
        isPainterCacheDirty_ = true;
        painterCache_.reset(new QPixmap(size()));
    }
    staticPainter_.reset(new QPainter(painterCache_));

    paintedXMin_ = width() - 1;
    paintedYMin_ = height() - 1;
    paintedXMax_ = 0;
    paintedYMax_ = 0;
}

void P4WinInputSphere::finishDrawing()
{
    if (staticPainter_) {
        staticPainter_->end();
        staticPainter_.reset();

        if (paintedXMin_ < 0)
            paintedXMin_ = 0;
        if (paintedXMax_ >= width())
            paintedXMax_ = width() - 1;
        if (paintedYMin_ < 0)
            paintedYMin = 0;
        if (paintedYMax_ >= height())
            paintedYMax_ = height() - 1;

        if (paintedYMax_ >= paintedYMin_ && paintedXMax_ >= paintedXMin_)
            update(paintedXMin_, paintedYMin_, paintedXMax_ - paintedXMin_ + 1,
                   paintedYMax_ - paintedYMin_ + 1);
    }
}

void P4WinInputSphere::refresh()
{
    isPainterCacheDirty_ = true;
    update();
}

void P4WinInputSphere::plotPoincareSphere()
{
    int color{CLINEATINFINITY};

    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto it : circleAtInfinity_)
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
}

void P4WinInputSphere::plotPoincareLyapunovSphere()
{
    staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));

    for (auto it : circleAtInfinity_)
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));

    for (auto it : plCircle_)
        staticPainter_->drawLine(coWinX(it.x1), coWinY(it.y1), coWinX(it.x2),
                                 coWinY(it.y2));
}

void P4WinInputSphere::plotLineAtInfinity()
{
    switch (g_VFResults.typeofview_) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0_ < 0.0 && x1_ > 0.0) {
            staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter_->drawLine(coWinX(0.0), 0, coWinY(0.0), h - 1);
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0_ < 0.0 && y1_ > 0.0) {
            staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter_->drawLine(0, coWinY(0.0), w - 1, coWinY(0.0));
        }
        break;
    }
}

std::vector<P4POLYLINES>
P4WinInputSphere::produceEllipse(double cx, double cy, double a, double b,
                                 bool dotted, double resa, double resb)
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

void P4WinInputSphere::plotCurves()
{
    if (!g_VFResults.curves_result_.empty())
        for (r = 0; r < g_ThisVF->numCurveS_; r++)
            plotCurve(g_VFResults.curves_result_[r], r);
}

void P4WinInputSphere::plotCurve(p4curves::curves crv, int index)
{
    double pcoord[3];
    std::vector<p4orbits::orbits_points> &sep{crv.points};

    for (auto it : sep) {
        if (it.dashes)
            plotLine(pcoord, it.pcoord, it.color);
        else
            plotPoint(it.pcoord, it.color);

        copy_x_into_y(it.pcoord, pcoord);
    }
}

void QP4WinInputSphere::plotLine(double *p1, double *p2, int color)
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

void P4WinInputSphere::plotPoint(double *p, int color)
{
    double ucoord[2];
    MATHFUNC(sphere_to_viecoord)(p[0], p[1], p[2], ucoord);
    drawPoint(ucoord[0], ucoord[1], color);
}

void P4WinInputSphere::setupCoordinateTransformations()
{
    g_VFResults.setupCoordinateTransformations();
}
