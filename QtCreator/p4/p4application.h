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

#ifndef P4APPLICATION_H
#define P4APPLICATION_H

#include <QApplication>
#include <QFont>
#include <QProcess>

#ifdef Q_OS_WIN
#define USERPLATFORM "WINDOWS"
#else
#define USERPLATFORM "LINUX"
#endif

class QP4Application : public QApplication
{
    Q_OBJECT

  public:
    QP4Application(int &argc, char **argv);

    QFont *StandardFont;
    QFont *BoldFont;
    QFont *CourierFont;
    QFont *BoldCourierFont;
    QFont *TitleFont;
    QFont *LegendFont;

  public slots:
    void Signal_Evaluating(void);
    void Signal_Evaluated(int);
    void Signal_GcfEvaluated(int);
    void Signal_CurveEvaluated(int);
    void Signal_Changed(void);
    void Signal_Loaded(void);
    void Signal_Saved(void);
    void cathProcessError(QProcess::ProcessError);
};

extern QP4Application *p4app;

#endif /* P4APPLICATION_H */
