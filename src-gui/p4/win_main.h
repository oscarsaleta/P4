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

#ifndef WIN_MAIN_H
#define WIN_MAIN_H

#include <QBoxLayout>
#include <QCloseEvent>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QPalette>
#include <QPushButton>
#include <QString>
#include <QWidget>

#define TYPE_SIGNAL_EVALUATING (QEvent::User + 1)
#define TYPE_SIGNAL_EVALUATED (QEvent::User + 2)
#define TYPE_SIGNAL_CHANGED (QEvent::User + 3)
#define TYPE_SIGNAL_LOADED (QEvent::User + 4)
#define TYPE_SIGNAL_SAVED (QEvent::User + 5)

#define TYPE_SELECT_ORBIT (QEvent::User + 6)
#define TYPE_CLOSE_PLOTWINDOW (QEvent::User + 7)
#define TYPE_ORBIT_EVENT (QEvent::User + 8)
#define TYPE_SEP_EVENT (QEvent::User + 9)
#define TYPE_CLOSE_ZOOMWINDOW (QEvent::User + 10)
#define TYPE_OPENZOOMWINDOW (QEvent::User + 11)
#define TYPE_SELECT_LCSECTION (QEvent::User + 12)

/* Forward-declarations to solve cross-include problems */
class QPlotWnd; // in win_plot.h
class QFindDlg; // in win_find.h

struct DOUBLEPOINT {
    double x;
    double y;
};

// class defined in win_main.cpp
class QStartDlg : public QWidget
{
    Q_OBJECT

  public:
    QStartDlg(const QString &);

    QFindDlg *findWindow_;
    QPlotWnd *plotWindow_;

    void closeEvent(QCloseEvent *ce);

    // following functions are called by other P4 widgets when something
    // happened:
    void signalEvaluating(void);
    void signalEvaluated(void);
    void signalChanged(void);
    void signalLoaded(void);
    void signalSaved(void);

    void customEvent(QEvent *e);

    void saveSettings();
    void readSettings();

  public slots:
    // following slots are called by QT when a button is pressed or a file name
    // is changed:
    void onQuit();
    void onPlot();
    void onViewFinite();
    void onViewInfinite();
    void onHelp();
    void onFind();
    void onBrowse();
    void onAbout();
    void onFilenameChange(const QString &);
    QWidget *showText(QWidget *win, QString caption, QString fname);

  private:
    QBoxLayout *mainLayout_;
    QPushButton *btn_quit_;
    QPushButton *btn_find_;
    QPushButton *btn_view_;
    QPushButton *btn_plot_;
    QPushButton *btn_help_;
    QPushButton *btn_about_;
    QPushButton *btn_browse_;
    QLineEdit *edt_name_;
    QMenu *viewMenu_;

    QWidget *helpWindow_;
    QWidget *viewInfiniteWindow_;
    QWidget *viewFiniteWindow_;
};

extern QStartDlg *g_p4stardlg;

#endif /* WIN_MAIN_H */
