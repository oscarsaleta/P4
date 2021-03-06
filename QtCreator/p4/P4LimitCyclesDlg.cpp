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

#include "P4LimitCyclesDlg.hpp"

#include <QBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QSpinBox>

#include <cmath>

#include "P4Application.hpp"
#include "P4ParentStudy.hpp"
#include "P4PlotWnd.hpp"
#include "P4Sphere.hpp"
#include "main.hpp"
#include "math_limitcycles.hpp"

bool gLCWindowIsUp{false}; // see definition in main.h

static QProgressDialog *sLCProgressDlg{nullptr};
static int sLCProgressCount{1};
static int sLCMaxProgressCount{0};

P4LimitCyclesDlg::P4LimitCyclesDlg(P4PlotWnd *plt, P4Sphere *sp)
    : QWidget(plt, Qt::Tool | Qt::WindowStaysOnTopHint), plotwnd_{plt},
      mainSphere_{sp}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    edt_x0_ = new QLineEdit{"", this};
    auto lbl1 = new QLabel{"x0 = ", this};
    edt_y0_ = new QLineEdit{"", this};
    auto lbl2 = new QLabel{"y0 = ", this};
    edt_x1_ = new QLineEdit{"", this};
    auto lbl3 = new QLabel{"x1 = ", this};
    edt_y1_ = new QLineEdit{"", this};
    auto lbl4 = new QLabel{"y1 = ", this};
    edt_grid_ = new QLineEdit{"", this};
    auto lbl5 = new QLabel{"Grid: ", this};

    spin_numpoints_ = new QSpinBox{this};
    spin_numpoints_->setMinimum(MIN_LCPOINTS);
    spin_numpoints_->setMaximum(MAX_LCPOINTS);
    auto lbl6 = new QLabel{"# Points: ", this};

    btn_start_ = new QPushButton{"&Start", this};
    btn_cancel_ = new QPushButton{"&Reset setpoints", this};
    btn_dellast_ = new QPushButton{"&Delete Last LC", this};
    btn_delall_ = new QPushButton{"Delete &All LC", this};

#ifdef TOOLTIPS
    edt_x0_->setToolTip("Start point of the transverse section.\n"
                        "You can also Alt+click and drag on the plot\n"
                        "window to fill this field.");
    edt_y0_->setToolTip("Start point of the transverse section.\n"
                        "You can also Alt+click and drag on the plot\n"
                        "window to fill this field.");
    edt_x1_->setToolTip("End point of the transverse section.\n"
                        "You can also Alt+click and drag on the plot\n"
                        "window to fill this field.");
    edt_y1_->setToolTip("Start point of the transverse section.\n"
                        "You can also Alt+click and drag on the plot\n"
                        "window to fill this field.");
    spin_numpoints_->setToolTip("Number of points to integrate for each orbit");
    edt_grid_->setToolTip(
        "Diameter of intervals dividing the transverse section");
    btn_start_->setToolTip("Start integrating from the transverse section,\n"
                           "looking for the presence of a limit cycle");
    btn_dellast_->setToolTip("Delete last limit cycle");
    btn_delall_->setToolTip("Delete all limit cycles");
    btn_cancel_->setToolTip("Reset set points");
#endif

    // layout

    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    auto lay00 = new QGridLayout{};
    lay00->addWidget(lbl1, 0, 0);
    lay00->addWidget(edt_x0_, 0, 1);
    lay00->addWidget(lbl2, 0, 2);
    lay00->addWidget(edt_y0_, 0, 3);
    lay00->addWidget(lbl3, 1, 0);
    lay00->addWidget(edt_x1_, 1, 1);
    lay00->addWidget(lbl4, 1, 2);
    lay00->addWidget(edt_y1_, 1, 3);

    auto layout1 = new QHBoxLayout{};
    layout1->addWidget(lbl5);
    layout1->addWidget(edt_grid_);

    auto layout3 = new QHBoxLayout{};
    layout3->addWidget(btn_start_);
    layout3->addWidget(btn_cancel_);

    auto layout2 = new QHBoxLayout{};
    layout2->addWidget(lbl6);
    layout2->addWidget(spin_numpoints_);

    auto layout4 = new QHBoxLayout{};
    layout4->addWidget(btn_dellast_);
    layout4->addWidget(btn_delall_);

    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections
    QObject::connect(btn_start_, &QPushButton::clicked, this,
                     &P4LimitCyclesDlg::onbtn_start);
    QObject::connect(btn_cancel_, &QPushButton::clicked, this,
                     &P4LimitCyclesDlg::onbtn_cancel);
    QObject::connect(btn_delall_, &QPushButton::clicked, this,
                     &P4LimitCyclesDlg::onbtn_delall);
    QObject::connect(btn_dellast_, &QPushButton::clicked, this,
                     &P4LimitCyclesDlg::onbtn_dellast);

    // finishing
    spin_numpoints_->setValue(selected_numpoints_);
    QString buf;
    buf.sprintf("%g", selected_grid_);
    edt_grid_->setText(buf);

    if (gVFResults.firstLimCycle_ == nullptr) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Limit Cycles");
}

void P4LimitCyclesDlg::setSection(double x0, double y0, double x1, double y1)
{
    QString buf, bufx, bufy;
    bufx.sprintf("%g", x0);
    bufy.sprintf("%g", y0);
    edt_x0_->setText(bufx);
    edt_y0_->setText(bufy);

    bufx.sprintf("%g", x1);
    bufy.sprintf("%g", y1);
    edt_x1_->setText(bufx);
    edt_y1_->setText(bufy);
}

void P4LimitCyclesDlg::onbtn_start()
{
    plotwnd_->getDlgData();

    QString bufx{edt_x0_->text()};
    QString bufy{edt_y0_->text()};
    bool empty{false};
    if (bufx.length() == 0 || bufy.length() == 0) {
        empty = true;
    }

    selected_x0_ = bufx.toDouble();
    selected_y0_ = bufy.toDouble();

    bufx = edt_x1_->text();
    bufy = edt_y1_->text();
    if (bufx.length() == 0 || bufy.length() == 0 || empty) {
        QMessageBox::critical(
            this, "P4",
            "Please enter setpoint coordinates for the transverse "
            "section.\nYou can also click on the poincare sphere "
            "with the left mouse button\nwhile the limit cycle "
            "window is opened.\n");
        return;
    }

    selected_x1_ = bufx.toDouble();
    selected_y1_ = bufy.toDouble();

    QString buf{edt_grid_->text()};
    selected_grid_ = buf.toDouble();

    selected_numpoints_ = spin_numpoints_->value();

    double d{(selected_x0_ - selected_x1_) * (selected_x0_ - selected_x1_)};
    d += (selected_y0_ - selected_y1_) * (selected_y0_ - selected_y1_);
    d = sqrt(d);
    if (selected_grid_ > d || selected_grid_ < MIN_LCGRID ||
        selected_grid_ > MAX_LCGRID) {
        QMessageBox::critical(this, "P4",
                              "Grid size is either too big or too "
                              "small for this transverse section.");
        return;
    }

    d /= selected_grid_;
    if (d < MIN_LCORBITS || d > MAX_LCORBITS) {
        QMessageBox::critical(this, "P4",
                              "Grid size is either too big or too "
                              "small for this transverse section.");
        return;
    }

    // SEARCH FOR LIMIT CYCLES:
    sLCMaxProgressCount = (int)(d + 0.5);
    if (sLCProgressDlg != nullptr) {
        delete sLCProgressDlg;
        sLCProgressDlg = nullptr;
    }
    sLCProgressDlg = new QProgressDialog{"Searching for limit cycles...",
                                         "Stop search",
                                         0,
                                         sLCMaxProgressCount,
                                         this,
                                         static_cast<Qt::WindowFlags>(0)};
    sLCProgressDlg->setAutoReset(false);
    sLCProgressDlg->setAutoClose(false);
    sLCProgressDlg->setMinimumDuration(0);
    sLCProgressDlg->setValue(sLCProgressCount = 0);
    setP4WindowTitle(sLCProgressDlg, "Searching for limit cycles...");

    searchLimitCycle(mainSphere_, selected_x0_, selected_y0_, selected_x1_,
                     selected_y1_, selected_grid_);

    // update buttons
    if (gVFResults.firstLimCycle_ == nullptr) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    } else {
        btn_delall_->setEnabled(true);
        btn_dellast_->setEnabled(true);
    }

    gP4app->processEvents();

    delete sLCProgressDlg;
    sLCProgressDlg = nullptr;
}

void P4LimitCyclesDlg::onbtn_cancel()
{
    edt_x0_->setText("");
    edt_y0_->setText("");
    edt_x1_->setText("");
    edt_y1_->setText("");
}

void P4LimitCyclesDlg::reset()
{
    // finishing
    selected_x0_ = 0;
    selected_y0_ = 0;
    selected_x1_ = 0;
    selected_y1_ = 0;
    selected_grid_ = DEFAULT_LCGRID;
    selected_numpoints_ = DEFAULT_LCPOINTS;

    edt_x0_->setText("");
    edt_y0_->setText("");
    edt_x1_->setText("");
    edt_y1_->setText("");

    QString buf;
    buf.sprintf("%g", selected_grid_);
    edt_grid_->setText(buf);
    spin_numpoints_->setValue(selected_numpoints_);

    if (gVFResults.firstLimCycle_ == nullptr) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    } else {
        btn_delall_->setEnabled(true);
        btn_dellast_->setEnabled(true);
    }
}

void P4LimitCyclesDlg::showEvent(QShowEvent *se)
{
    QWidget::showEvent(se);
    gLCWindowIsUp = true;
}

void P4LimitCyclesDlg::hideEvent(QHideEvent *he)
{
    QWidget::hideEvent(he);
    gLCWindowIsUp = false;
}

void P4LimitCyclesDlg::onbtn_delall()
{
    plotwnd_->getDlgData();

    btn_delall_->setEnabled(false);
    btn_dellast_->setEnabled(false);

    delete gVFResults.firstLimCycle_;
    gVFResults.firstLimCycle_ = nullptr;
    gVFResults.currentLimCycle_ = nullptr;

    mainSphere_->refresh();
}

void P4LimitCyclesDlg::onbtn_dellast()
{
    plotwnd_->getDlgData();

    mainSphere_->prepareDrawing();
    deleteLastLimitCycle(mainSphere_);
    mainSphere_->finishDrawing();

    if (gVFResults.firstLimCycle_ == nullptr) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    }
}

bool stop_search_limit()
{
    gP4app->processEvents();
    if (sLCProgressDlg->wasCanceled())
        return true;

    return false;
}

void write_to_limit_window()
{
    sLCProgressCount++;

    if (!(sLCProgressDlg->wasCanceled()))
        sLCProgressDlg->setValue(sLCProgressCount);
}
