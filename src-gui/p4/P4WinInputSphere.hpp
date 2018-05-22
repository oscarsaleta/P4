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

#pragma once

#include <QWidget>

#include <QMainWindow>
#include <QPoint>

#include <vector>

#include "main.hpp"
#include "tables.hpp"

class QKeyEvent;
class QLabel;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class QPixmap;
class QResizeEvent;

class P4SeparatingCurvesDlg;

class P4WinInputSphere : public QWidget
{
    Q_OBJECT

  public:
    P4WinInputSphere(P4SeparatingCurvesDlg *wnd, QLabel *lbl_status,
                     QWidget *parent);

    //////////////////////
    // MEMBER VARIABLES //
    //////////////////////

    double x0_, y0_; // world coordinates of upper-left corner
    double x1_, y1_; // world coordinates of upper-right corner
    double dx_;      // x1-x0
    double dy_;      // y1-y0;

    int w_;      // width of window
    int h_;      // height of window
    int idealh_; // ideal height of window to get good aspect ratio

    bool isZoom_{false};

    double horPixelsPerMM_;
    double verPixelsPerMM_;

    QPixmap *painterCache_;
    QLabel *status_;
    bool isPainterCacheDirty_{true};
    // to know the update rectangle after painting we keep to smallest rectangle
    // enclosing all painted objects
    int paintedXMin_;
    int paintedXMax_;
    int paintedYMin_;
    int paintedYMax_;

    int spherebgcolor_;

    /////////////
    // METHODS //
    /////////////

    void paintEvent(QPaintEvent *);

    // coordinate changes: from world to window coordinates
    int coWinX(double x);
    int coWinY(double y);
    // coordinate changes: from window to world coordinates
    double coWorldX(int x);
    double coWorldY(int y);
    int coWinH(double);
    int coWinV(double);

    void adjustToNewSize();

    void plotPoincareSphere();
    void plotPoincareLyapunovSphere();
    void plotLineAtInfinity();

    std::vector<P4POLYLINES> produceEllipse(double cx, double cy, double a,
                                            double b, bool dotted, double resa,
                                            double resb);

    void prepareDrawing();
    void drawPoint(double x, double y, int color);
    void drawLine(double x1, double y1, double x2, double y2, int color);
    void finishDrawing();

    void saveAnchorMap();
    void loadAnchorMap();

    void setupPlot();
    void refresh();
    // void calculateHeightFromWidth(int *width, int *height, int maxheight,
    //                              double aspectratio);
    // void updatePointSelection();
    void markSelection(int x1, int y1, int x2, int y2);

    void plotSeparatingCurves();
    void plotSeparatingCurve(const p4curves::curves &, int);
    void plotLine(const double *, const double *, int);
    void plotPoint(const double *, int);
    void setupCoordinateTransformations();

  public slots:
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void refreshAfterResize();

  private:
    P4SeparatingCurvesDlg *parentWnd_;
    QPainter *staticPainter_;
    std::vector<P4POLYLINES> circleAtInfinity_;
    std::vector<P4POLYLINES> plCircle_;
    QTimer *refreshTimeout_;

    bool selectingZoom_{false};
    QPoint zoomAnchor1_;
    QPoint zoomAnchor2_;
    QPixmap *anchorMap_;
};
