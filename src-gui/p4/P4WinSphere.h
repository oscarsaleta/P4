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

#ifndef WIN_SPHERE_H
#define WIN_SPHERE_H

#include <QPoint>
#include <QString>
#include <QWidget>

#define SELECTINGPOINTSTEPS 5
#define SELECTINGPOINTSPEED 150

class QKeyEvent;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class QPixmap;
class QResizeEvent;
class QStatusBar;
class QTimer;

struct p4singularities::saddle;
struct p4singularities::node;
struct p4singularities::semi_elementary;
struct p4singularities::weak_focus;
struct p4singularities::strong_focus;
struct p4singularities::degenerate;
struct P4POLYLINES;

class P4WinSphere : public QWidget
{
    Q_OBJECT

    static int sm_numSpheres;
    // this could be implemented as a c++ vector
    static std::vector<P4WinSphere> sm_SphereList;

  public:
    /* Constructor and destructor */
    P4WinSphere(QWidget *, QStatusBar *, bool, double, double, double, double);
    ~P4WinSphere();

    /* Member variables */
    double horPixelsPerMM_;
    double verPixelsPerMM_;

    double x0_, y0_; // world-coordinates of upper-left corner
    double x1_, y1_; // world-coordinates of upper-right corner
    double dx_;      // x1-x0
    double dy_;      // y1-y0

    std::unique_ptr<QPixmap> painterCache_;
    bool isPainterCacheDirty_;
    int paintedXMin_; // to know the update rectangle after painting
    int paintedXMax_; // we keep to smallest rectangle enclosing
    int paintedYMin_; // all painted objects.
    int paintedYMax_;

    QString chartstring_;

    int spherebgcolor_;
    P4WinSphere *next_; // visible to PlotWnd // FIXME needed??????
    int selectingX_, selectingY_, selectingPointStep_, selectingPointRadius_;
    std::unique_ptr<QTimer> selectingTimer_;

    int oldw_; // used while printing
    int oldh_;
    int w_;      // width of window
    int h_;      // height of window
    int idealh_; // ideal height of window to get good aspect ratio

    /* Member functions */
    void paintEvent(QPaintEvent *); // FIXME not in p5

    bool getChartPos(int, double, double, double *);
    void adjustToNewSize();

    void signalEvaluating(); // FIXME not in p5
    void signalChanged();
    void plotPoint(std::vector<p4singularities::saddle>);
    void plotPoint(std::vector<p4singularities::node>);
    void plotPoint(std::vector<p4singularities::semi_elementary>);
    void plotPoint(std::vector<p4singularities::weak_focus>);
    void plotPoint(std::vector<p4singularities::strong_focus>);
    void plotPoint(std::vector<p4singularities::degenerate>);
    void plotPointSeparatrices(std::vector<p4singularities::semi_elementary> p);
    void plotPointSeparatrices(std::vector<p4singularities::saddle> p);
    void plotPointSeparatrices(std::vector<p4singularities::degenerate> p);
    void plotPoints();
    void plotSeparatrices();
    void plotSeparatingCurves();
    void plotGcf();
    void plotCurve();
    void drawIsoclines();
    void plotPoincareSphere();
    void plotPoincareLyapunovSphere();
    void plotLineAtInfinity();
    void markSelection(int x1, int y1, int x2, int y2, int selectiontype);

    void printPoint(std::vector<p4singularities::saddle>);
    void printPoint(std::vector<p4singularities::node>);
    void printPoint(std::vector<p4singularities::semi_elementary>);
    void printPoint(std::vector<p4singularities::weak_focus>);
    void printPoint(std::vector<p4singularities::strong_focus>);
    void printPoint(std::vector<p4singularities::degenerate>);
    void printPointSeparatrices(std::vector<p4singularities::semi_elementary>);
    void printPointSeparatrices(std::vector<p4singularities::saddle>);
    void printPointSeparatrices(std::vector<p4singularities::degenerate>);
    void printPoints();
    void printSeparatrices();
    void printSeparatingCurves(); // FIXME new in p5
    void printGcf();
    void printCurve();
    void printIsoclines();
    void printPoincareSphere();
    void printPoincareLyapunovSphere();
    void printLineAtInfinity();

    // FIXME si ha de generar un vector de p4polylines s'ha d'arreglar
    P4POLYLINES *produceEllipse(double cx, double cy, double a, double b,
                                bool dotted, double resa, double resb);

    void selectNearestSingularity(QPoint winpos);

    void prepareDrawing();
    void drawPoint(double x, double y, int color);
    void drawLine(double x1, double y1, double x2, double y2, int color);
    void finishDrawing();
    void printOrbits();
    void printLimitCycles();

    void preparePrinting(int, bool, int, double, double);
    void printPoint(double x, double y, int color);
    void print();
    void printLine(double x1, double y1, double x2, double y2, int color);
    void finishPrinting();

    void saveAnchorMap();
    void loadAnchorMap();

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
    void setupPlot();
    void refresh();
    void keyPressEvent(QKeyEvent *e);
    void calculateHeightFromWidth(int *width, int *height, int maxheight,
                                  double aspectratio);
    void refreshAfterResize();
    void updatePointSelection();

  private:
    std::unique_ptr<QPainter> staticPainter_;
    std::unique_ptr<QWidget> parentWnd_;
    bool iszoom_;
    bool reverseYAxis_; // when calculating coordinates: this determines
                        // orientation
    // of horizontal axis.  Normally false, only true when printing.

    std::vector<P4POLYLINES> circleAtInfinity_;
    std::vector<P4POLYLINES> plCircle_;

    std::unique_ptr<QTimer> refreshTimeout_;

    bool selectingZoom_;
    bool selectingLCSection_;
    QPoint zoomAnchor1_;
    QPoint zoomAnchor2_;
    QPoint lcAnchor1_;
    QPoint lcAnchor2_;
    std::unique_ptr<QPixmap> anchorMap_;

    std::unique_ptr<QStatusBar> msgBar_;
    int printMethod_;
};

#endif /* WIN_SPHERE_H */
