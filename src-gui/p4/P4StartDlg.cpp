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

#include "P4StartDlg.hpp"

#include "P4AboutDlg.h"
#include "P4Application.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "P4PlotWnd.hpp"
#include "custom.hpp"
#include "file_tab.hpp"
#include "main.hpp"
#include "p4settings.hpp"

#include <QBoxLayout>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QTextBrowser>
#include <QTextStream>

std::unique_ptr<P4StartDlg> gP4startDlg();

// initialise background colors
int bgColours::CBACKGROUND = BLACK;
int bgColours::CFOREGROUND = WHITE;
int bgColours::CORBIT = YELLOW;
bool bgColours::PRINT_WHITE_BG = true;

P4StartDlg::P4StartDlg(const QString &autofilename)
    : QWidget{nullptr, Qt::WA_DeleteOnClose}
{
    // general initialization

    QString cap{"Planar Polynomial Phase Portraits"};

    // define controls

    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

    btn_quit_ = std::make_unique<QPushButton>("&Quit", this);
    btn_view_ = std::make_unique<QPushButton>("Vie&w", this);
    btn_plot_ = std::make_unique<QPushButton>("&Plot", this);
    btn_help_ = std::make_unique<QPushButton>("&Help", this);
    btn_about_ = std::make_unique<QPushButton>("Ab&out P4...", this);

    if (autofilename.length() == 0)
        edt_name_ = std::make_unique<QLineEdit>(DEFAULTFILENAME, this);
    else
        edt_name_ = std::make_unique<QLineEdit>(
            gThisVF->filename_ = autofilename, this);
    auto p4name = std::make_unique<QLabel>(" &Name: ", this);
    p4name->setBuddy(edt_name_.get());
    p4name->setFont(gP4app->getBoldFont());

    edt_name_->setSelection(0, strlen(DEFAULTFILENAME));
    edt_name_->setCursorPosition(strlen(DEFAULTFILENAME));

    btn_browse_ = std::make_unique<QPushButton>("&Browse", this);

#ifdef TOOLTIPS
    btn_quit_->setToolTip("Quit P4");
    btn_view_->setToolTip(
        "View results of the symbolic manipulator after evaluation");
    btn_plot_->setToolTip("Draw singular points, orbits and separatrices");
    btn_help_->setToolTip("Shows extensive help on the use of P4");
    edt_name_->setToolTip(
        "Enter the filename of the vector field here.\n"
        "You do not need to add the extension (.inp).\n");
    btn_browse_->setToolTip("Search for vector field files on your system");
    btn_about_->setToolTip(
        "Displays information about the program P4, its "
        "version and main settings");
#endif

    // define placement of controls
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    auto buttons = std::make_unique<QHBoxLayout>();
    buttons->addWidget(btn_quit_.get());
    buttons->addWidget(btn_view_.get());
    buttons->addWidget(btn_plot_.get());
    buttons->addWidget(btn_help_.get());

    mainLayout_->addLayout(buttons.get());

    auto names = std::make_unique<QHBoxLayout>();
    names->addWidget(p4name.get());
    names->addWidget(edt_name_.get());
    names->addWidget(btn_browse_.get());
    names->addWidget(btn_about_.get());
    mainLayout_->addLayout(names.get());

    setLayout(mainLayout_.get());
    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);

    // connections

    viewMenu_ = std::make_unique<QMenu>(this);

    auto actFin = std::make_unique<QAction>("Fini&te", this);
    actFin->setShortcut(Qt::ALT + Qt::Key_T);
    QObject::connect(actFin.get(), &QAction::triggered, this,
                     &P4StartDlg::onViewFinite);
    viewMenu_->addAction(actFin.get());

    auto actInf = std::make_unique<QAction>("&Infinite", this);
    actInf->setShortcut(Qt::ALT + Qt::Key_I);
    QObject::connect(actInf.get(), &QAction::triggered, this,
                     &P4StartDlg::onViewInfinite);
    viewMenu_->addAction(actInf.get());

    btn_view_->setMenu(viewMenu_);

    QObject::connect(btn_quit_.get(), &QPushButton::clicked, this,
                     &P4StartDlg::onQuit);
    QObject::connect(btn_plot_.get(), &QPushButton::clicked, this,
                     &P4StartDlg::onPlot);
    QObject::connect(btn_help_.get(), &QPushButton::clicked, this,
                     &P4StartDlg::onHelp);
    QObject::connect(btn_about_.get(), &QPushButton::clicked, this,
                     &P4StartDlg::onAbout);
    QObject::connect(btn_browse_.get(), &QPushButton::clicked, this,
                     &P4StartDlg::onBrowse);
    QObject::connect(edt_name_.get(), &QLineEdit::textChanged, this,
                     &P4StartDlg::onFilenameChange);
    QObject::connect(gThisVF.get(), &QInputVF::saveSignal, this,
                     &P4StartDlg::onSaveSignal);
    QObject::connect(gThisVF.get(), &QInputVF::loadSignal, this,
                     &P4StartDlg::onLoadSignal);

    // setting focus

    edt_name_->setFocus();

    // show find dialog
    if (!findWindow_) {
        findWindow_ = std::make_unique<P4FindDlg>(this);
        findWindow_->show();
        findWindow_->raise();
        mainLayout_->addWidget(findWindow_);
    }

    setP4WindowTitle(this, cap);
}

void P4StartDlg::onSaveSignal()
{
    QSettings settings(gThisVF->getbarefilename().append(".conf"),
                       QSettings::NativeFormat);
    settings.beginGroup("P4StartDlg");

    settings.setValue("pos", pos());
    settings.setValue("size", size());

    if (viewInfiniteWindow_) {
        settings.setValue("viewInfiniteWindow", true);
        settings.setValue("viewInfiniteWindow-pos", viewInfiniteWindow_->pos());
        settings.setValue("viewInfiniteWindow-size",
                          viewInfiniteWindow_->size());
    }
    if (viewFiniteWindow_) {
        settings.setValue("viewFiniteWindow", true);
        settings.setValue("viewFiniteWindow-pos", viewFiniteWindow_->pos());
        settings.setValue("viewFiniteWindow-size", viewFiniteWindow_->size());
    }
    if (plotWindow_)
        settings.setValue("plotWindow", true);
    else
        settings.setValue("plotWindow", false);
    if (gThisVF) {
        if (gThisVF->outputWindow_ != nullptr) {
            settings.setValue("outputWindow", true);
            settings.setValue("outputWindow-size",
                              gThisVF->outputWindow_->size());
            settings.setValue("outputWindow-pos",
                              gThisVF->outputWindow_->pos());
        }
        if (gThisVF->processText_) {
            settings.setValue("processText", true);
            settings.setValue("processText-contents",
                              gThisVF->processText_->toPlainText());
        }
    }
    settings.endGroup();
}

void P4StartDlg::onLoadSignal()
{
    QSettings settings(gThisVF->getbarefilename().append(".conf"),
                       QSettings::NativeFormat);
    settings.beginGroup("P4StartDlg");
    // check if there is configuration in this file
    if (settings.contains("size")) {
        resize(settings.value("size").toSize());
        move(settings.value("pos").toPoint());
        if (settings.value("plotWindow").toBool()) {
            if (plotWindow_)
                plotWindow_->show();
            else {
                onPlot();
                plotWindow_->onLoadSignal();
            }
        }
        if (settings.value("viewInfiniteWindow").toBool()) {
            if (viewInfiniteWindow_)
                viewInfiniteWindow_.reset();
            onViewInfinite();
            viewInfiniteWindow_->resize(
                settings.value("viewInfiniteWindow-size").toSize());
            viewInfiniteWindow_->move(
                settings.value("viewInfiniteWindow-pos").toPoint());
        }
        if (settings.value("viewFiniteWindow").toBool()) {
            if (viewFiniteWindow_)
                viewFiniteWindow_.reset();
            onViewFinite();
            viewFiniteWindow_->resize(
                settings.value("viewFiniteWindow-size").toSize());
            viewFiniteWindow_->move(
                settings.value("viewFiniteWindow-pos").toPoint());
        }
        if (settings.value("outputWindow").toBool() &&
            settings.value("processText").toBool()) {
            if (gThisVF) {
                gThisVF->createProcessWindow();
                gThisVF->terminateProcessButton_->setDisabled(true);
                gThisVF->outputWindow_->resize(
                    settings.value("outputWindow-size").toSize());
                gThisVF->outputWindow_->move(
                    settings.value("outputWindow-pos").toPoint());
                gThisVF->processText_->setPlainText(
                    settings.value("processText-contents").toString());
            }
        }
    }
    settings.endGroup();
}

void P4StartDlg::onHelp()
{
    // display help
    QString helpname{getP4HelpPath()};
    if (helpname.isNull()) {
        QMessageBox::critical(this, "P4",
                              "Cannot determine P4 install location!\nPlease "
                              "re-check installation.\n");
        return;
    }

    helpname += QDir::separator();
    helpname += P4HELPFILE;

    if (!QFile(helpname).exists()) {
        QMessageBox::critical(this, "P4",
                              "Cannot find P4 help files in install "
                              "location!\nPlease re-check installation.\n");
        return;
    }

    if (!helpWindow_)
        helpWindow_ = std::make_unique<QTextBrowser>();

    helpWindow_->setSource(QUrl::fromLocalFile(helpname));
    helpWindow_->resize(640, 480);
    if (gP4smallIcon)
        helpWindow_->setWindowIcon(*gP4smallIcon);

    setP4WindowTitle(helpWindow_, "P4 Help");
    helpWindow_->show();
    helpWindow_->raise();
}

void P4StartDlg::onPlot()
{
    // show plot window
    if (!canOpenPlot()) {
        QMessageBox::critical(
            this, "P4", "Please close the piecewise-config window first\n");
        return
    }

    if (findWindow_)
        findWindow_->getDataFromDlg();

    // read maple/reduce results
    if (!gVFResults.readTables(gThisVF->getbarefilename(), false, false)) {
        QMessageBox::critical(this, "P4",
                              "Cannot read computation results.\nPlease check "
                              "the input-vector field and parameters!\n");
        gVFResults.setupCoordinateTransformations();
        if (plotWindow_)
            plotWindow_->signalEvaluated();

        return;
    }
    if (!gVFResults.readCurve(gThisVF->getbarefilename())) {
        // nothing, we simply don't have a curve for plotting
    }

    gVFResults.setupCoordinateTransformations();

    if (!plotWindow_) {
        plotWindow_ = std::make_unique<P4PlotWnd>(this);
    }

    plotWindow_->configure();  // configure plot window
    plotWindow_->show();
    plotWindow_->raise();
    plotWindow_->adjustHeight();
}

void P4StartDlg::onQuit() { close(); }

void P4StartDlg::onFilenameChange(const QString &fname)
{
    gThisVF->filename_ = fname;
}

void P4StartDlg::signalEvaluating()
{
    // disable view button, disable plot button:
    btn_view_->setEnabled(false);
    btn_plot_->setEnabled(false);

    // Transfer signal to findWindow_:
    /*if (findWindow_)
        findWindow_->signalEvaluating();*/

    // Transfer signal to plotWindow_:
    /*if (plotWindow_)
        plotWindow_->signalEvaluating();*/
}

void P4StartDlg::signalEvaluated()
{
    // enable view button, disable plot button:
    btn_view_->setEnabled(true);
    btn_plot_->setEnabled(true);

    // freshen view finite/infinite windows if they are open:
    if (viewFiniteWindow_) {
        QString fname;

        if (findWindow_)
            findWindow_->getDataFromDlg();

        fname = gThisVF->getfilename_finresults();

        if (P4InputVF::fileExists(fname)) {
            viewFiniteWindow_ = std::make_unique<QTextEdit>();
            showText(*viewFiniteWindow_, "View results at the finite region",
                     fname);
        }
    } else {
        if (viewFiniteWindow_) {
            viewFiniteWindow_->clear();
            viewFiniteWindow_->setCurrentFont(gP4app->getBoldCourierFont());
            viewFiniteWindow_->insertPlainText(
                "\nA study at the finite region is not available!");
        }
    }
}

if (viewInfiniteWindow_) {
    QString fname;

    if (findWindow_)
        findWindow_->getDataFromDlg();

    fname = gThisVF->getfilename_infresults();
    if (P4InputVF::fileExists(fname)) {
        viewInfiniteWindow_ = std::make_unique<QTextEdit>();
        showText(*viewInfiniteWindow_, "View results at infinity", fname);
        if (gThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
            gThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
            // mark: data invalid according to vf information
            viewInfiniteWindow_->setFont(gP4app->getCourierFont());
        }
    } else {
        if (viewInfiniteWindow_) {
            viewInfiniteWindow_->clear();
            viewInfiniteWindow_->setCurrentFont(gP4app->getBoldCourierFont());
            viewInfiniteWindow_->insertPlainText(
                "\nA study at infinity is not available!");
        }
    }
}

// Transfer signal to findWindow_:
if (findWindow_) {
    findWindow_->signalEvaluated();
}

// Transfer signal to plotWindow_:
if (plotWindow_) {
    // read maple/reduce results
    if (!gVFResults.readTables(gThisVF->getbarefilename(),
                               gThisVF->evaluatingPiecewiseConfig_, false)) {
        if (gThisVF->evaluatingPiecewiseConfig_) {
            // ...
        } else {
            QMessageBox::critical(
                this, "P4",
                "Cannot read computation results.\n"
                "Please check the input-vector field and parameters!\n");
            gVFResults.setupCoordinateTransformations();
            plotWindow_->signalEvaluated();
        }
    } else {
        if (gThisVF->evaluatingPiecewiseConfig_) {
            // ...
        } else {
            gVFResults.setupCoordinateTransformations();
            plotWindow_->signalEvaluated();
        }
    }
}

// the vector field may be changed during evaluation.  In that
// case, the flag gThisVF->changed_ is set, so the newly evaluated context
// is immediately marked as "old".
if (gThisVF->changed_)
    signalChanged();
}

void P4StartDlg::signalSaved()
{
    //
}

void P4StartDlg::signalLoaded()
{
    //
}

void P4StartDlg::signalChanged()
{
    if (viewFiniteWindow_) {
        viewFiniteWindow_->setFont(gP4app->getCourierFont());
    }
    if (viewInfiniteWindow_) {
        viewInfiniteWindow_->setFont(gP4app->getCourierFont());
    }
    if (plotWindow_) {
        plotWindow_->signalChanged();
    }
}

void P4StartDlg::onBrowse()
{
    auto result = QFileDialog::getOpenFileName(
        this, "Select vector field configuration file", edt_name_->text(),
        "Vector Field files (*.inp);;All Files (*.*)", 0,
        QFileDialog::DontConfirmOverwrite);

    if (!(result.isNull())) {
        edt_name_->setText(result);
    }
}

void P4StartDlg::onAbout()
{
    std::unique_ptr<P4AboutDlg> pdlg{std::make_unique<P4AboutDlg>(this, 0)};
    pdlg->exec();
}

void P4StartDlg::onViewFinite()
{
    // display help

    QString fname;

    if (findWindow_)
        findWindow_->getDataFromDlg();

    fname = gThisVF->getfilename_finresults();

    if (!P4InputVF::fileExists(fname)) {
        if (gThisVF->typeofstudy_ == TYPEOFSTUDY_INF) {
            QMessageBox::critical(
                this, "P4",
                "A study at the finite region was not requested!\n");
            return;
        }
        QMessageBox::critical(this, "P4",
                              "Cannot open the result file.\nPlease "
                              "re-evaluate, or check filename.\n");
        return;
    }

    viewFiniteWindow_ = std::make_unique<QTextEdit>();
    showText(*viewFiniteWindow_, "View results at the finite region", fname);
    viewFiniteWindow_->show();
    viewFiniteWindow_->raise();

    if (gThisVF->typeofstudy_ == TYPEOFSTUDY_INF) {
        // mark: data invalid according to vf information
        viewFiniteWindow_->setFont(gP4app->getCourierFont());
        return;
    }
}

void P4StartDlg::onViewInfinite()
{
    QString fname;

    if (findWindow_ != nullptr)
        findWindow_->getDataFromDlg();

    fname = gThisVF->getfilename_infresults();

    if (P4InputVF::fileExists(fname) == false) {
        if (gThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
            gThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
            QMessageBox::critical(this, "P4",
                                  "A study at infinity was not requested!\n");
            return;
        }
        QMessageBox::critical(this, "P4",
                              "Cannot open the result "
                              "file.\nPlease re-evaluate, or check "
                              "filename.\n");
        return;
    }

    viewInfiniteWindow_ = std::make_unique<QTextEdit>();
    showText(*viewInfiniteWindow_, "View results at infinity", fname);
    viewInfiniteWindow_->show();
    viewInfiniteWindow_->raise();
    if (gThisVF->typeofstudy_ == TYPEOFSTUDY_FIN ||
        gThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
        // mark: data invalid according to vf information
        viewInfiniteWindow_->setFont(gP4app->getCourierFont());
        return;
    }
}

void P4StartDlg::showText(QTextEdit &win, const QString &caption,
                          const QString &fname)
{
    win.clear();

    if (gP4smallIcon)
        win.setWindowIcon(*gP4smallIcon);

    QFile f{fname};
    if (!f.open(QIODevice::ReadOnly))
        return;

    bool shown{false};
    if (win.isHidden() == false) {
        shown = true;
        win.hide();
    }
    win.setFont(gP4app->getCourierFont());
    win.insertPlainText("");
    win.setReadOnly(true);

    QTextStream t{&f};
    while (!t.atEnd()) {
        auto s = t.readLine();
        win.append(s);
    }
    f.close();

    win.resize(640, 480);

    setP4WindowTitle(&win, caption);

    if (!gThisVF->evaluated_)
        win.setFont(gP4app->getCourierFont());

    if (shown) {
        win.show();
        win.raise();
    }

    return;
}

void P4StartDlg::closeEvent(QCloseEvent *ce)
{
    int result;

    // This event get to process window-system close events.
    // A close event is subtly different from a hide event:
    // hide may often mean "iconify" but close means that the window is going
    // away for good.

    if (!gThisVF) {
        ce->accept();
        return;
    }

    if (findWindow_)
        findWindow_->getDataFromDlg();

    if (!gThisVF->changed_) {
        ce->accept();
        return;
    }

    result = QMessageBox::information(
        this, "P4", "The vector field has been changed since the last save.",
        "&Save Now", "&Cancel", "&Leave Anyway", 0, 1);

    if (result == 2)
        ce->accept();
    else {
        if (result == 0) {
            if (gThisVF->save()) {
                ce->accept();
                return;
            }
            QMessageBox::critical(this, "P4",
                                  "Unable to save the input vector "
                                  "field.\nPlease check permissions on the "
                                  "write location.\n");
        }
        ce->ignore();
    }
}

void P4StartDlg::customEvent(QEvent *e)
{
    switch (e->type()) {
    /*case TYPE_SIGNAL_EVALUATING:
        signalEvaluating();
        break;*/
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
        if (plotWindow_)
            plotWindow_.reset();
        break;
    default:
        QWidget::customEvent(e);
    }
}

/*void P4StartDlg::saveSettings()
{
    QString settingsName = gThisVF->getbarefilename().append(".conf");
    QSettings settings(settingsName, QSettings::NativeFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
}*/

/*void P4StartDlg::readSettings()
{
    QString settingsName = gThisVF->getbarefilename().append(".conf");
    QSettings settings(settingsName, QSettings::NativeFormat);
    restoreGeometry();
    restoreState()
}*/

bool P4StartDlg::canOpenPlot()
{
    if (!findWindow_) {
        return true;
    } else {
        auto *p = findWindow_->getVFSelectWindowPtr();
        if (p == nullptr) {
            return true;
        } else {
            auto *q = p->getWinCurvesPtr();
            if (q != nullptr)
                q->closeConfigWindow();
        }
    }
    return true;
}

void P4StartDlg::signalSeparatingCurvesEvaluated()
{
    if (findWindow_)
        findWindow_->signalSeparatingCurvesEvaluated();
}

void P4StartDlg::closePlotWindow()
{
    if (plotWindow_)
        plotWindow_.reset();
}

P4FindDlg *P4StartDlg::getFindWindowPtr()
{
    if (findWindow_)
        return findWindow_.get();
    return nullptr;
}

void P4StartDlg::setFindWindow(P4FindDlg *newdlg)
{
    if (newdlg != nullptr && newdlg != findWindow_.get()) {
        findWindow_ = std::make_unique<P4FindDlg>(newdlg);
    } else {
        findWindow_.reset();
    }
}

P4PlotWnd *P4StartDlg::getPlotWindowPtr()
{
    if (plotWindow_)
        return plotWindow_.get();
    return nullptr;
}

void P4StartDlg::setPlotWindow(P4PlotWnd *newdlg)
{
    if (newdlg != nullptr && newdlg != plotWindow_.get()) {
        plotWindow_ = std::make_unique<P4PlotWnd>(newdlg);
    } else {
        plotWindow_.reset();
    }
}