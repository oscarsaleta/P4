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

#ifndef WIN_ORBITS_H
#define WIN_ORBITS_H

#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class QOrbitsDlg : public QWidget
{
    Q_OBJECT

  public:
    QOrbitsDlg(QPlotWnd *, QWinSphere *);
    void reset(void);

  private:
    QWinSphere *mainSphere_;
    QPlotWnd *plotwnd_;

    QPushButton *btnForwards_;
    QPushButton *btnContinue_;
    QPushButton *btnBackwards_;
    QPushButton *btnDelAll_;
    QPushButton *btnDelLast_;
    QPushButton *btnSelect_;

    QLineEdit *edt_x0_;
    QLineEdit *edt_y0_;

    QBoxLayout *mainLayout_;

    double selected_x0;
    double selected_y0;

    bool orbit_started;
    bool orbit_selected;

  public slots:
    void OrbitEvent(int);
    void onbtnSelect_(void);
    void onbtnBackwards_(void);
    void onbtnContinue_(void);
    void onbtnForwards_(void);
    void onbtnDelAll_(void);
    void onbtnDelLast_(void);

    void setInitialPoint(double, double);
};

#endif /* WIN_ORBITS_H */
