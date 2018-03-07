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

#include "P4Application.hpp"

#include "P4Event.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "P4StartDlg.hpp"
#include "custom.hpp"
#include "main.hpp"

#include <QFont>

P4Application::P4Application(int &argc, char **argv) : QApplication{argc, argv}
{
    standardFont_ = std::make_unique<QFont>();
    standardFont_->setPointSize(standardFont_->pointSize() + FONTSIZE);
    setFont(*standardFont_);

    boldFont_ = std::make_unique<QFont>();
    boldFont_->setWeight(QFont::Bold);

    titleFont_ = std::make_unique<QFont>();
    titleFont_->setPointSize(titleFont_->pointSize() + TITLEFONTSIZE);
    titleFont_->setWeight(QFont::Bold);

    courierFont_ = std::make_unique<QFont>();
    courierFont_->setFamily("Courier");
    courierFont_->setFixedPitch(true);
    courierFont_->setPointSize(courierFont_->pointSize() + FONTSIZE);

    boldCourierFont_ = std::make_unique<QFont>();
    boldCourierFont_->setFamily("Courier");
    boldCourierFont_->setFixedPitch(true);
    boldCourierFont_->setWeight(QFont::Bold);
    boldCourierFont_->setPointSize(boldCourierFont_->pointSize() + FONTSIZE);

    legendFont_ = std::make_unique<QFont>();
    legendFont_->setFamily("Courier");
    legendFont_->setFixedPitch(true);
    legendFont_->setPointSize(legendFont_->pointSize() + LEGENDFONTSIZE);
}

// -----------------------------------------------------------------------
//                          SIGNALS
// -----------------------------------------------------------------------
//
// Following signals may be called from another thread

void P4Application::signalChanged()
{
    gThisVF.evaluated_ = false;
    gThisVF.changed_ = true;

    auto e = std::make_unique<P4Event>(
        static_cast<QEvent::Type>(TYPE_SIGNAL_CHANGED), nullptr);
    gP4app->postEvent(gP4startDlg.get(), e.release());
}

void P4Application::signalEvaluated(int exitCode)
{
    gThisVF.evaluated_ = true;
    gThisVF.evaluating_ = false;

    gThisVF.finishEvaluation(exitCode);

    auto e = std::make_unique<P4Event>(
        static_cast<QEvent::Type>(TYPE_SIGNAL_EVALUATED), nullptr);
    gP4app->postEvent(gP4startDlg.get(), e.release());

    if (gCmdLineAutoExit) {
        gCmdLineAutoPlot = false;
        gP4startDlg->onQuit();
        return;
    }

    if (gCmdLineAutoPlot) {
        gCmdLineAutoPlot = false;
        gP4startDlg->onPlot();
    }
}

void P4Application::signalGcfEvaluated(int exitCode)
{
    gThisVF.evaluated_ = true;
    gThisVF.evaluating_ = false;
    gThisVF.finishEvaluation(exitCode);
}

void P4Application::signalSeparatingCurvesEvaluated(int exitCode)
{
    gThisVF.evaluating_ = false;
    gThisVF.finishEvaluation(exitCode);
}

void P4Application::signalCurveEvaluated(int exitCode)
{
    gThisVF.evaluated_ = true;
    gThisVF.evaluating_ = false;

    gThisVF.finishEvaluation(exitCode);
}

void P4Application::catchProcessError(QProcess::ProcessError qperr)
{
    gThisVF.catchProcessError(qperr);
}

void P4Application::signalLoaded()
{
    auto e = std::make_unique<P4Event>(
        static_cast<QEvent::Type>(TYPE_SIGNAL_LOADED), nullptr);
    gP4app->postEvent(gP4startDlg.get(), e.release());

    if (gCmdLineAutoEvaluate) {
        gCmdLineAutoEvaluate = false;
        gP4startDlg->getFindWindowPtr()->onBtnEval();
        return;
    }
    if (gCmdLineAutoPlot) {
        gCmdLineAutoPlot = false;
        gP4startDlg->onPlot();
        return;
    }
}

void P4Application::signalSaved()
{
    auto e = std::make_unique<P4Event>(
        static_cast<QEvent::Type>(TYPE_SIGNAL_SAVED), nullptr);
    gP4app->postEvent(gP4startDlg.get(), e.release());
}

QFont P4Application::getStandardFont() const { return *standardFont_; }

QFont P4Application::getBoldFont() const { return *boldFont_; }

QFont P4Application::getTitleFont() const { return *titleFont_; }

QFont P4Application::getCourierFont() const { return *courierFont_; }

QFont P4Application::getBoldCourierFont() const { return *boldCourierFont_; }

QFont P4Application::getLegendFont() const { return *legendFont_; }