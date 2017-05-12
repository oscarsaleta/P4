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

    QFont *standardFont_;
    QFont *boldFont_;
    QFont *courierFont_;
    QFont *boldCourierFont_;
    QFont *titleFont_;
    QFont *legendFont_;

  public slots:
    void signalEvaluating(void);
    void signalEvaluated(int);
    void signalGcfEvaluated(int);
    void signalCurveEvaluated(int);
    void signalChanged(void);
    void signalLoaded(void);
    void signalSaved(void);
    void catchProcessError(QProcess::ProcessError);
};

extern QP4Application *g_p4app;

#endif /* P4APPLICATION_H */
