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

#include "P4Application.h"

#include "custom.h"
#include "P4InputVF.h"
#include "main.h"
#include "P4Event.h"
#include "P4FindDlg.h"
#include "P4StartDlg.h"

#include <QFont>

std::unique_ptr<P4Application> g_p4app{nullptr};

P4Application::P4Application(int &argc, char **argv) : QApplication(argc, argv)
{
    standardFont_.reset(new QFont());
    standardFont_->setPointSize(standardFont_->pointSize() + FONTSIZE);
    setFont(*standardFont_);

    boldFont_.reset(new QFont());
    boldFont_->setWeight(QFont::Bold);

    titleFont_.reset(new QFont());
    titleFont_->setPointSize(titleFont_->pointSize() + TITLEFONTSIZE);
    titleFont_->setWeight(QFont::Bold);

    courierFont_.reset(new QFont);
    courierFont_->setFamily("Courier");
    courierFont_->setFixedPitch(true);
    courierFont_->setPointSize(courierFont_->pointSize() + FONTSIZE);

    boldCourierFont_.reset(new QFont);
    boldCourierFont_->setFamily("Courier");
    boldCourierFont_->setFixedPitch(true);
    boldCourierFont_->setWeight(QFont::Bold);
    boldCourierFont_->setPointSize(boldCourierFont_->pointSize() + FONTSIZE);

    legendFont_.reset(new QFont);
    legendFont_->setFamily("Courier");
    legendFont_->setFixedPitch(true);
    legendFont_->setPointSize(legendFont_->pointSize() + LEGENDFONTSIZE);
}

// -----------------------------------------------------------------------
//                          SIGNALS
// -----------------------------------------------------------------------
//
// Following signals may be called from another thread

void P4Application::signalChanged(void)
{
    g_ThisVF->evaluated_ = false;
    g_ThisVF->changed_ = true;

    std::unique_ptr<P4Event> e{
        new P4Event((QEvent::Type)TYPE_SIGNAL_CHANGED, nullptr)};
    g_p4app->postEvent(g_p4StartDlg, e.get());
}

void P4Application::signalEvaluated(int exitCode)
{
    g_ThisVF->evaluated_ = true;
    g_ThisVF->evaluating_ = false;

    g_ThisVF->finishEvaluation(exitCode);

    std::unique_ptr<P4Event> e{
        new P4Event((QEvent::Type)TYPE_SIGNAL_EVALUATED, nullptr)};
    g_p4app->postEvent(g_p4StartDlg, e.get());

    if (g_cmdLine_AutoExit) {
        g_cmdLine_AutoPlot = false;
        g_p4StartDlg->onQuit();
        return;
    }

    if (g_cmdLine_AutoPlot) {
        g_cmdLine_AutoPlot = false;
        g_p4StartDlg->onPlot();
    }
}

void P4Application::signalGcfEvaluated(int exitCode)
{
    g_ThisVF->evaluated_ = true;
    g_ThisVF->evaluating_ = false;
    g_ThisVF->finishEvaluation(exitCode);
}

void P4Application::signalSeparatingCurvesEvaluated(int exitCode)
{
    g_ThisVF->evaluating_ = false;
    g_ThisVF->finishEvaluation(exitCode);
}

void P4Application::signalCurveEvaluated(int exitCode)
{
    g_ThisVF->evaluated_ = true;
    g_ThisVF->evaluating_ = false;

    g_ThisVF->finishEvaluation(exitCode);
}

void P4Application::catchProcessError(QProcess::ProcessError qperr)
{
    g_ThisVF->catchProcessError(qperr);
}

void P4Application::signalLoaded(void)
{
    std::unique_ptr<P4Event> e{
        new P4Event((QEvent::Type)TYPE_SIGNAL_LOADED, nullptr)};
    g_p4app->postEvent(g_p4StartDlg, e.get());

    if (g_cmdLine_AutoEvaluate) {
        g_cmdLine_AutoEvaluate = false;
        g_p4StartDlg->findWindow_->onBtnEval();
        return;
    }
    if (g_cmdLine_AutoPlot) {
        g_cmdLine_AutoPlot = false;
        g_p4StartDlg->onPlot();
        return;
    }
}

void P4Application::signalSaved(void)
{
    std::unique_ptr<P4Event> e{
        new P4Event((QEvent::Type)TYPE_SIGNAL_SAVED, nullptr)};
    g_p4app->postEvent(g_p4StartDlg, e.get());
}
