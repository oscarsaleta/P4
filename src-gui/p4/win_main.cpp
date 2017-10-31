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

#include "win_main.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "p4settings.h"
#include "win_about.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextBrowser>


QStartDlg *g_p4stardlg = nullptr;

QStartDlg::QStartDlg(const QString &autofilename) : QWidget()
{
    // general initialization

    QString cap;
    cap = "Planar Polynomial Phase Portraits";
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    btn_quit_ = new QPushButton("&Quit", this);
    btn_view_ = new QPushButton("Vie&w", this);
    btn_plot_ = new QPushButton("&Plot", this);
    btn_help_ = new QPushButton("&Help", this);
    btn_about_ = new QPushButton("Ab&out P4...", this);

    if (autofilename.length() == 0)
        edt_name_ = new QLineEdit(DEFAULTFILENAME, this);
    else
        edt_name_ = new QLineEdit(g_ThisVF->filename_ = autofilename, this);
    QLabel *p4name = new QLabel(" &Name: ", this);
    p4name->setBuddy(edt_name_);
    p4name->setFont(*(g_p4app->boldFont_));

    edt_name_->setSelection(0, strlen(DEFAULTFILENAME));
    edt_name_->setCursorPosition(strlen(DEFAULTFILENAME));

    btn_browse_ = new QPushButton("&Browse", this);

#ifdef TOOLTIPS
    btn_quit_->setToolTip("Quit P4");
    btn_view_->setToolTip(
        "View results of the symbolic manipulator after evaluation");
    btn_plot_->setToolTip("Draw singular points, orbits and separatrices");
    btn_help_->setToolTip("Shows extensive help on the use of P4");
    edt_name_->setToolTip("Enter the filename of the vector field here.\n"
                          "You do not need to add the extension (.inp).\n");
    btn_browse_->setToolTip("Search for vector field files on your system");
    btn_about_->setToolTip("Displays information about the program P4, its "
                           "version and main settings");
#endif

    // define placement of controls

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(btn_quit_);
    buttons->addWidget(btn_view_);
    buttons->addWidget(btn_plot_);
    buttons->addWidget(btn_help_);

    mainLayout_->addLayout(buttons);

    QHBoxLayout *names = new QHBoxLayout();
    names->addWidget(p4name);
    names->addWidget(edt_name_);
    names->addWidget(btn_browse_);
    names->addWidget(btn_about_);
    mainLayout_->addLayout(names);

    setLayout(mainLayout_);
    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);

    // connections

    viewMenu_ = new QMenu(this);

    QAction *ActFin = new QAction("Fini&te", this);
    ActFin->setShortcut(Qt::ALT + Qt::Key_T);
    connect(ActFin, SIGNAL(triggered()), this, SLOT(onViewFinite()));
    viewMenu_->addAction(ActFin);

    QAction *ActInf = new QAction("&Infinite", this);
    ActInf->setShortcut(Qt::ALT + Qt::Key_I);
    connect(ActInf, SIGNAL(triggered()), this, SLOT(onViewInfinite()));
    viewMenu_->addAction(ActInf);

    btn_view_->setMenu(viewMenu_);

    QObject::connect(btn_quit_, SIGNAL(clicked()), this, SLOT(onQuit()));
    QObject::connect(btn_plot_, SIGNAL(clicked()), this, SLOT(onPlot()));
    QObject::connect(btn_help_, SIGNAL(clicked()), this, SLOT(onHelp()));
    QObject::connect(btn_about_, SIGNAL(clicked()), this, SLOT(onAbout()));
    QObject::connect(btn_browse_, SIGNAL(clicked()), this, SLOT(onBrowse()));
    QObject::connect(edt_name_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(onFilenameChange(const QString &)));

    // setting focus

    edt_name_->setFocus();

    // finishing

    //  btn_plot_->setDisabled( true );
    //  viewMenu_->setItemEnabled( 1, false );
    //  viewMenu_->setItemEnabled( 2, false );

    helpWindow_ = nullptr;
    findWindow_ = nullptr;
    viewInfiniteWindow_ = nullptr;
    viewFiniteWindow_ = nullptr;
    plotWindow_ = nullptr;
    
    // show find dialog
    if (findWindow_ == nullptr) {
        findWindow_ = new QFindDlg(this);
        findWindow_->show();
        findWindow_->raise();
        mainLayout_->addWidget(findWindow_);
    } else {
        delete findWindow_;
        findWindow_ = nullptr;
        // connect(findWindow_, &QFindDlg::saveStateSignal, this,
        //        &QStartDlg::saveSettings);
    }

    setP4WindowTitle(this, cap);
}


void QStartDlg::onHelp(void)
{
    // display help
    QTextBrowser *hlp;
    QString helpname;

    helpname = getP4HelpPath();
    if (helpname.isNull()) {
        QMessageBox::critical(this, "P4", "Cannot determine P4 install "
                                          "location!\nPlease re-check "
                                          "installation.\n");
        return;
    }

    helpname += QDir::separator();
    helpname += P4HELPFILE;

    if (QFile(helpname).exists() == false) {
        QMessageBox::critical(this, "P4", "Cannot find P4 help files in "
                                          "install location!\nPlease re-check "
                                          "installation.\n");
        return;
    }

    hlp = (QTextBrowser *)helpWindow_;

    if (hlp == nullptr) {
        hlp = new QTextBrowser();
    }

    hlp->setSource(QUrl::fromLocalFile(helpname));
    hlp->resize(640, 480);
    if (g_p4smallicon != nullptr)
        hlp->setWindowIcon(*g_p4smallicon);

    setP4WindowTitle(hlp, "P4 Help");
    hlp->show();
    hlp->raise();

    helpWindow_ = hlp;
}

void QStartDlg::onPlot(void)
{
    // show plot window

    /*if (findWindow_ != nullptr) {
        findWindow_->onBtnLoad();
        findWindow_->getDataFromDlg();
    }*/

    g_VFResults.deleteVF(); // delete any previous result object
    if (!g_VFResults.readTables(
            g_ThisVF->getbarefilename())) // read maple/reduce results
    {
        delete plotWindow_;
        plotWindow_ = nullptr;

        QMessageBox::critical(
            this, "P4",
            "Cannot read computation results.\n"
            "Please check the input-vector field and parameters!\n");

        return;
    }
    if (!g_VFResults.readCurve(g_ThisVF->getbarefilename())) {
        // nothing, we simply don't have a curve for plotting
    }

    g_VFResults.setupCoordinateTransformations();

    if (plotWindow_ == nullptr) {
        plotWindow_ = new QPlotWnd(this);
    }

    plotWindow_->configure(); // configure plot window
    plotWindow_->show();
    plotWindow_->raise();
    plotWindow_->adjustHeight();
}

void QStartDlg::onQuit(void)
{
    if (plotWindow_ != nullptr) {
        delete plotWindow_;
        plotWindow_ = nullptr;
    }
    if (findWindow_ != nullptr) {
        delete findWindow_;
        findWindow_ = nullptr;
    }
    if (helpWindow_ != nullptr) {
        delete helpWindow_;
        helpWindow_ = nullptr;
    }
    if (viewFiniteWindow_ != nullptr) {
        delete viewFiniteWindow_;
        viewFiniteWindow_ = nullptr;
    }
    if (viewInfiniteWindow_ != nullptr) {
        delete viewInfiniteWindow_;
        viewInfiniteWindow_ = nullptr;
    }

    if (g_ThisVF != nullptr) {
        delete g_ThisVF;
        g_ThisVF = nullptr;
    }

    close();
}

void QStartDlg::onFilenameChange(const QString &fname)
{
    g_ThisVF->filename_ = fname;
}

void QStartDlg::signalEvaluating(void)
{
    // disable view button, disable plot button:

    btn_view_->setEnabled(false);
    btn_plot_->setEnabled(false);

    // Transfer signal to findWindow_:

    if (findWindow_ != nullptr) {
        findWindow_->signalEvaluating();
    }

    // Transfer signal to plotWindow_:

    if (plotWindow_ != nullptr)
        plotWindow_->signalEvaluating();
}

void QStartDlg::signalEvaluated(void)
{
    // enable view button, disable plot button:

    btn_view_->setEnabled(true);
    btn_plot_->setEnabled(true);

    // freshen view finite/infinite windows if they are open:

    if (viewFiniteWindow_ != nullptr) {
        QString fname;

        if (findWindow_ != nullptr)
            findWindow_->getDataFromDlg();

        fname = g_ThisVF->getfilename_finresults();

        if (g_ThisVF->fileExists(fname)) {
            viewFiniteWindow_ = showText(
                viewFiniteWindow_, "View results at the finite region", fname);
        } else {
            if (viewFiniteWindow_ != nullptr) {
                ((QTextEdit *)viewFiniteWindow_)->clear();
                ((QTextEdit *)viewFiniteWindow_)
                    ->setCurrentFont(*(g_p4app->boldCourierFont_));
                ((QTextEdit *)viewFiniteWindow_)
                    ->insertPlainText(
                        "\nA study at the finite region is not available!");
            }
        }
    }

    if (viewInfiniteWindow_ != nullptr) {
        QString fname;

        if (findWindow_ != nullptr)
            findWindow_->getDataFromDlg();

        fname = g_ThisVF->getfilename_infresults();
        if (g_ThisVF->fileExists(fname)) {
            viewInfiniteWindow_ = showText(viewInfiniteWindow_,
                                           "View results at infinity", fname);
            if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
                g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
                // mark: data invalid according to vf information

                viewInfiniteWindow_->setFont(*(g_p4app->courierFont_));
            }
        } else {
            if (viewInfiniteWindow_ != nullptr) {
                ((QTextEdit *)viewInfiniteWindow_)->clear();
                ((QTextEdit *)viewInfiniteWindow_)
                    ->setCurrentFont(*(g_p4app->boldCourierFont_));
                ((QTextEdit *)viewInfiniteWindow_)
                    ->insertPlainText(
                        "\nA study at infinity is not available!");
            }
        }
    }

    // Transfer signal to findWindow_:

    if (findWindow_ != nullptr) {
        findWindow_->signalEvaluated();
    }

    // Transfer signal to plotWindow_:

    if (plotWindow_ != nullptr) {
        g_VFResults.deleteVF(); // delete any previous result object
        if (!g_VFResults.readTables(
                g_ThisVF->getbarefilename())) // read maple/reduce results
        {
            QMessageBox::critical(
                this, "P4",
                "Cannot read computation results.\n"
                "Please check the input-vector field and parameters!\n");
        }
        g_VFResults.setupCoordinateTransformations();
        plotWindow_->signalEvaluated();
    }

    // the vector field may be changed during evaluation.  In that
    // case, the flag g_ThisVF->changed_ is set, so the newly evaluated context
    // is immediately marked as "old".

    if (g_ThisVF->changed_)
        signalChanged();
}

void QStartDlg::signalSaved(void)
{
    //
}

void QStartDlg::signalLoaded(void)
{
    //
}

void QStartDlg::signalChanged(void)
{
    if (viewFiniteWindow_ != nullptr) {
        viewFiniteWindow_->setFont(*(g_p4app->courierFont_));
    }

    if (viewInfiniteWindow_ != nullptr) {
        viewInfiniteWindow_->setFont(*(g_p4app->courierFont_));
    }
    if (plotWindow_ != nullptr) {
        plotWindow_->signalChanged();
    }
}

void QStartDlg::onBrowse(void)
{
    QString result;

    result = QFileDialog::getOpenFileName(
        this, "Select vector field configuration file", edt_name_->text(),
        "Vector Field files (*.inp);;All Files (*.*)", 0,
        QFileDialog::DontConfirmOverwrite);

    if (!(result.isNull())) {
        edt_name_->setText(result);
    }
}

void QStartDlg::onAbout(void)
{
    QP4AboutDlg *pdlg;
    pdlg = new QP4AboutDlg(this, 0);
    pdlg->exec();
    delete pdlg;
    pdlg = nullptr;
}

void QStartDlg::onViewFinite()
{
    // display help

    QString fname;

    if (findWindow_ != nullptr)
        findWindow_->getDataFromDlg();

    fname = g_ThisVF->getfilename_finresults();

    if (g_ThisVF->fileExists(fname) == false) {
        if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_INF) {
            QMessageBox::critical(
                this, "P4",
                "A study at the finite region was not requested!\n");
            return;
        }
        QMessageBox::critical(this, "P4", "Cannot open the result "
                                          "file.\nPlease re-evaluate, or check "
                                          "filename.\n");
        return;
    }

    viewFiniteWindow_ =
        showText(viewFiniteWindow_, "View results at the finite region", fname);
    viewFiniteWindow_->show();
    viewFiniteWindow_->raise();

    if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_INF) {
        // mark: data invalid according to vf information
        viewFiniteWindow_->setFont(*(g_p4app->courierFont_));
        return;
    }
}

void QStartDlg::onViewInfinite()
{
    QString fname;

    if (findWindow_ != nullptr)
        findWindow_->getDataFromDlg();

    fname = g_ThisVF->getfilename_infresults();

    if (g_ThisVF->fileExists(fname) == false) {
        if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
            g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
            QMessageBox::critical(this, "P4",
                                  "A study at infinity was not requested!\n");
            return;
        }
        QMessageBox::critical(this, "P4", "Cannot open the result "
                                          "file.\nPlease re-evaluate, or check "
                                          "filename.\n");
        return;
    }

    viewInfiniteWindow_ =
        showText(viewInfiniteWindow_, "View results at infinity", fname);
    viewInfiniteWindow_->show();
    viewInfiniteWindow_->raise();
    if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
        g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
        // mark: data invalid according to vf information
        viewInfiniteWindow_->setFont(*(g_p4app->courierFont_));
        return;
    }
}

QWidget *QStartDlg::showText(QWidget *win, QString caption, QString fname)
{
    bool shown;
    QTextEdit *result;

    result = (QTextEdit *)win;
    if (result == nullptr)
        result = new QTextEdit();
    else
        result->clear();

    if (g_p4smallicon != nullptr)
        result->setWindowIcon(*g_p4smallicon);

    QFile f(fname);
    if (!f.open(QIODevice::ReadOnly)) {
        delete result;
        return nullptr;
    }

    shown = false;
    if (result->isHidden() == false) {
        shown = true;
        result->hide();
    }
    result->setFont(*(g_p4app->courierFont_));
    result->insertPlainText("");
    result->setReadOnly(true);

    QTextStream t(&f);

    while (!t.atEnd()) {
        QString s = t.readLine();
        result->append(s);
    }
    f.close();

    result->resize(640, 480);

    //  result->setCaption( caption );
    setP4WindowTitle(result, caption);

    if (g_ThisVF->evaluated_ == false)
        result->setFont(*(g_p4app->courierFont_));

    if (shown) {
        result->show();
        result->raise();
    }

    return result;
}

void QStartDlg::closeEvent(QCloseEvent *ce)
{
    int result;

    // This event get to process window-system close events.
    // A close event is subtly different from a hide event:
    // hide may often mean "iconify" but close means that the window is going
    // away for good.

    if (g_ThisVF == nullptr) {
        ce->accept();
        return;
    }

    if (findWindow_ != nullptr)
        findWindow_->getDataFromDlg();

    if (g_ThisVF->changed_ == false) {
        ce->accept();
        return;
    }

    result = QMessageBox::information(
        this, "P4", "The vector field has been changed since "
                    "the last save.",
        "&Save Now", "&Cancel", "&Leave Anyway", 0, 1);

    if (result == 2)
        ce->accept();
    else {
        if (result == 0) {
            if (g_ThisVF->save()) {
                ce->accept();
                return;
            }
            QMessageBox::critical(
                this, "P4",
                "Unable to save the input vector field.\n"
                "Please check permissions on the write location.\n");
        }
        ce->ignore();
    }
}

void QStartDlg::customEvent(QEvent *e)
{
    switch ((int)(e->type())) {
    case TYPE_SIGNAL_EVALUATING:
        signalEvaluating();
        break;
    case TYPE_SIGNAL_EVALUATED:
        signalEvaluated();
        break;
    case TYPE_SIGNAL_CHANGED:
        signalChanged();
        break;
    case TYPE_SIGNAL_LOADED:
        signalLoaded();
        break;
    case TYPE_SIGNAL_SAVED:
        signalSaved();
        break;
    case TYPE_CLOSE_PLOTWINDOW:
        if (plotWindow_ != nullptr) {
            delete plotWindow_;
            plotWindow_ = nullptr;
        }
        break;
    default:
        QWidget::customEvent(e);
    }
}


/*void QStartDlg::saveSettings()
{
    QString settingsName = g_ThisVF->getbarefilename().append(".conf");
    QSettings settings(settingsName, QSettings::NativeFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
}*/

/*void QStartDlg::readSettings()
{
    QString settingsName = g_ThisVF->getbarefilename().append(".conf");
    QSettings settings(settingsName, QSettings::NativeFormat);
    restoreGeometry();
    restoreState()
}*/
