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

#ifndef WIN_SPHERE_H
#define WIN_SPHERE_H

#include "file_tab.h"
#include "main.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QResizeEvent>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QWidget>

#define SELECTINGPOINTSTEPS 5
#define SELECTINGPOINTSPEED 150

class QWinSphere : public QWidget
{
    Q_OBJECT

    static int sm_numSpheres;
    // this could be implemented as a c++ vector
    static QWinSphere **sm_SphereList;

  public:
    /* Constructor and destructor */
    QWinSphere(QWidget *, QStatusBar *, bool, double, double, double, double);
    ~QWinSphere();

    /* Member variables */
    double horPixelsPerMM_;
    double verPixelsPerMM_;

    double x0_, y0_; // world-coordinates of upper-left corner
    double x1_, y1_; // world-coordinates of upper-right corner
    double dx_;       // x1-x0
    double dy_;       // y1-y0

    QPixmap *painterCache_;
    bool isPainterCacheDirty_;
    int paintedXMin_; // to know the update rectangle after painting
    int paintedXMax_; // we keep to smallest rectangle enclosing
    int paintedYMin_; // all painted objects.
    int paintedYMax_;

    QString chartstring_;

    int spherebgcolor_;
    QWinSphere *next_; // visible to PlotWnd
    int selectingX_, selectingY_, selectingPointStep_, selectingPointRadius_;
    QTimer *selectingTimer_;

    int oldw_; // used while printing
    int oldh_;
    int w_;      // width of window
    int h_;      // height of window
    int idealh_; // ideal height of window to get good aspect ratio

    /* Member functions */

    void paintEvent(QPaintEvent *);

    bool getChartPos(int, double, double, double *);
    void adjustToNewSize(void);

    void signalEvaluating(void);
    void signalChanged(void);
    void plotPoint(struct saddle *);
    void plotPoint(struct node *);
    void plotPoint(struct semi_elementary *);
    void plotPoint(struct weak_focus *);
    void plotPoint(struct strong_focus *);
    void plotPoint(struct degenerate *);
    void plotPointSeparatrices(struct semi_elementary *p);
    void plotPointSeparatrices(struct saddle *p);
    void plotPointSeparatrices(struct degenerate *p);
    void plotPoints(void);
    void plotSeparatrices(void);
    void plotGcf(void);
    void plotCurve(void);
    void plotPoincareSphere(void);
    void plotPoincareLyapunovSphere(void);
    void plotLineAtInfinity(void);
    void markSelection(int x1, int y1, int x2, int y2, int selectiontype);

    void printPoint(struct saddle *);
    void printPoint(struct node *);
    void printPoint(struct semi_elementary *);
    void printPoint(struct weak_focus *);
    void printPoint(struct strong_focus *);
    void printPoint(struct degenerate *);
    void printPointSeparatrices(struct semi_elementary *p);
    void printPointSeparatrices(struct saddle *p);
    void printPointSeparatrices(struct degenerate *p);
    void printPoints(void);
    void printSeparatrices(void);
    void printGcf(void);
    void printCurve(void);
    void printPoincareSphere(void);
    void printPoincareLyapunovSphere(void);
    void printLineAtInfinity(void);

    struct P4POLYLINES *produceEllipse(double cx, double cy, double a, double b,
                                       bool dotted, double resa, double resb);

    void selectNearestSingularity(QPoint winpos);

    void prepareDrawing(void);
    void drawPoint(double x, double y, int color);
    void drawLine(double x1, double y1, double x2, double y2, int color);
    void finishDrawing(void);
    void printOrbits(void);
    void printLimitCycles(void);

    void preparePrinting(int, bool, int, double, double);
    void printPoint(double x, double y, int color);
    void print(void);
    void printLine(double x1, double y1, double x2, double y2, int color);
    void finishPrinting(void);

    void saveAnchorMap(void);
    void loadAnchorMap(void);

    // coordinate changes: from world to windows coordinates
    int coWinX(double x);
    int coWinY(double y);
    // from windows to world coordinates
    double coWorldX(int x);
    double coWorldY(int y);
    int coWinV(double);
    int coWinH(double);

  public slots:
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void setupPlot(void);
    void refresh(void);
    void keyPressEvent(QKeyEvent *e);
    void calculateHeightFromWidth(int *width, int *height, int maxheight,
                                  double aspectratio);
    void refreshAfterResize(void);
    void updatePointSelection(void);

  private:
    QPainter *staticPainter_;
    QWidget *parentWnd_;
    bool iszoom_;
    bool reverseYAxis_; // when calculating coordinates: this determines
                       // orientation
    // of horizontal axis.  Normally false, only true when printing.

    P4POLYLINES *circleAtInfinity_;
    P4POLYLINES *plCircle_;
    QTimer *refreshTimeout_;

    bool selectingZoom_;
    bool selectingLCSection_;
    QPoint zoomAnchor1_;
    QPoint zoomAnchor2_;
    QPoint lcAnchor1_;
    QPoint lcAnchor2_;
    QPixmap *anchorMap_;

    QStatusBar *msgBar_;
    int printMethod_;
};

#endif /* WIN_SPHERE_H */
