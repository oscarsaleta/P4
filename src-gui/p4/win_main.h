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

#include <memory>

#include <QWidget>

class QPlotWnd;
class P4FindDlg;

class QCloseEvent;
class QEvent;
class QString;
class QBoxLayout;
class QPushButton;
class QLineEdit;
class QMenu;

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

struct DOUBLEPOINT {
    double x;
    double y;
};

// class defined in win_main.cpp
class P4StartDlg : public QWidget
{
    Q_OBJECT

   public:
    P4StartDlg(const QString &);

    std::unique_ptr<P4FindDlg> findWindow_;
    std::unique_ptr<QPlotWnd> plotWindow_;

    void closeEvent(QCloseEvent *ce);

    // following functions are called by other P4 widgets when something
    // happened:
    void signalEvaluating(void);
    void signalEvaluated(void);
    void signalChanged(void);
    void signalLoaded(void);
    void signalSaved(void);

    void customEvent(QEvent *e);

   public slots:
    // following slots are called by QT when a button is pressed or a file name
    // is changed:
    void onQuit();
    void onPlot();
    void onViewFinite();
    void onViewInfinite();
    void onHelp();
    void onBrowse();
    void onAbout();
    void onFilenameChange(const QString &);
    QWidget *showText(QWidget *win, QString caption, QString fname);
    void onSaveSignal();
    void onLoadSignal();

   private:
    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<QPushButton> btn_quit_;
    std::unique_ptr<QPushButton> btn_find_;
    std::unique_ptr<QPushButton> btn_view_;
    std::unique_ptr<QPushButton> btn_plot_;
    std::unique_ptr<QPushButton> btn_help_;
    std::unique_ptr<QPushButton> btn_about_;
    std::unique_ptr<QPushButton> btn_browse_;
    std::unique_ptr<QLineEdit> edt_name_;
    std::unique_ptr<QMenu> viewMenu_;

    std::unique_ptr<QWidget> helpWindow_;
    std::unique_ptr<QWidget> viewInfiniteWindow_;
    std::unique_ptr<QWidget> viewFiniteWindow_;
};

extern std::unique_ptr<P4StartDlg> g_p4StartDlg;
