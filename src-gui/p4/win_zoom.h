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

#ifndef WIN_ZOOM_H
#define WIN_ZOOM_H

#include "win_plot.h"
#include "win_sphere.h"

#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QHideEvent>
#include <QMainWindow>

class QZoomWnd : public QMainWindow
{
    Q_OBJECT

  public:
    QZoomWnd(QPlotWnd *, int, double x1, double y1, double x2, double y2);
    ~QZoomWnd();

    int zoomid;

  private:
    QPlotWnd *parent;
    QBoxLayout *mainLayout_;
    QAction *ActClose;
    QAction *ActRefresh;
    QAction *ActPrint;

    QWinSphere *sphere; // main sphere

  public slots:
    void signalEvaluating(void);
    void signalEvaluated(void);
    void signalChanged(void);

    void OnBtnClose(void);
    void OnBtnRefresh(void);
    void OnBtnPrint(void);
    bool close(void);

    void configure(void);
    void customEvent(QEvent *e);
    void hideEvent(QHideEvent *h);
    void AdjustHeight(void);
};

#endif /* WIN_ZOOM_H */
