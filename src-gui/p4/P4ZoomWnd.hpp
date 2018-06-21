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

#include <QMainWindow>

#include "P4Sphere.hpp"

class QBoxLayout;
class QEvent;
class QHideEvent;

class P4PlotWnd;

class P4ZoomWnd : public QMainWindow
{
    Q_OBJECT

  public:
    P4ZoomWnd(P4PlotWnd *, int, double x1, double y1, double x2, double y2);

    int zoomid_;

    void signalEvaluated();
    void signalChanged();

  private:
    // non-owning raw pointer to parent
    P4PlotWnd *parent_;

    double x1_, x2_, y1_, y2_;

    P4Sphere *sphere_; // main sphere

  public slots:
    // void signalEvaluating();

    void onBtnClose();
    void onBtnRefresh();
    void onBtnPrint();
    bool close();

    void configure();
    void customEvent(QEvent *e);
    void hideEvent(QHideEvent *h);
    void adjustHeight();

    void onSaveSignal();
};
