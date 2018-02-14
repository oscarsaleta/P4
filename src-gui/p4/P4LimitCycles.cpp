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

#include "P4LimitCyclesDlg.h"

#include "custom.h"
#include "file_tab.h"
#include "main.h"
#include "math_limitcycles.h"
#include "p4application.h"
#include "win_plot.h"
#include "win_sphere.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QSpinBox>

#include <cmath>

bool g_LCWindowIsUp{false}; // see definition in main.h

static std::unique_ptr<QProgressDialog> s_LCprogressDlg{};
static int s_LCprogressCount{1};
static int s_LCmaxProgressCount{0};

P4LimitCyclesDlg::P4LimitCyclesDlg(std::unique_ptr<QPlotWnd> plt,
                                   std::unique_ptr<QWinSphere> sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint),
      plotwnd_{plt},
      mainSphere_{sp}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    edt_x0_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl1{new QLabel("x0 = ", this)};
    edt_y0_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl2{new QLabel("y0 = ", this)};
    edt_x1_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl3{new QLabel("x1 = ", this)};
    edt_y1_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl4{new QLabel("y1 = ", this)};
    edt_grid_.reset(new QLineEdit("", this));
    std::unique_ptr<QLabel> lbl5{new QLabel("Grid: ", this)};

    spin_numpoints_.reset(new QSpinBox(this));
    spin_numpoints_->setMinimum(MIN_LCPOINTS);
    spin_numpoints_->setMaximum(MAX_LCPOINTS);
    std::unique_ptr<QLabel> lbl6{new QLabel("# Points: ", this)};

    btn_start_.reset(new QPushButton("&Start", this));
    btn_cancel_.reset(new QPushButton("&Reset setpoints", this));
    btn_dellast_.reset(new QPushButton("&Delete Last LC", this));
    btn_delall_.reset(new QPushButton("Delete &All LC", this));

#ifdef TOOLTIPS
    edt_x0_->setToolTip(
        "Start point of the transverse section.\n"
        "You can also click on the plot window to fill this field.");
    edt_y0_->setToolTip(
        "Start point of the transverse section.\n"
        "You can also click on the plot window to fill this field.");
    edt_x1_->setToolTip(
        "End point of the transverse section.\n"
        "You can also click on the plot window to fill this field.");
    edt_y1_->setToolTip(
        "End point of the transverse section.\n"
        "You_ can also click on the plot window to fill this field.");
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

    mainLayout_.reset(new QBoxLayout(QBoxLayout::TopToBottom, this));

    std::unique_ptr<QGridLayout> lay00{new QGridLayout()};
    lay00->addWidget(lbl1, 0, 0);
    lay00->addWidget(edt_x0_, 0, 1);
    lay00->addWidget(lbl2, 0, 2);
    lay00->addWidget(edt_y0_, 0, 3);
    lay00->addWidget(lbl3, 1, 0);
    lay00->addWidget(edt_x1_, 1, 1);
    lay00->addWidget(lbl4, 1, 2);
    lay00->addWidget(edt_y1_, 1, 3);

    std::unique_ptr<QHBoxLayout> layout1{new QHBoxLayout()};
    layout1->addWidget(lbl5);
    layout1->addWidget(edt_grid_);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addWidget(btn_start_);
    layout2->addWidget(btn_cancel_);

    std::unique_ptr<QHBoxLayout> layout3{new QHBoxLayout()};
    layout3->addWidget(lbl6);
    layout3->addWidget(spin_numpoints_);

    std::unique_ptr<QHBoxLayout> layout4{new QHBoxLayout()};
    layout4->addWidget(btn_dellast_);
    layout4->addWidget(btn_delall_);

    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout4);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections
    connect(btn_start_, &QPushButton::clicked, this,
            &P4LimitCyclesDlg::onbtn_start);
    connect(btn_cancel_, &QPushButton::clicked, this,
            &P4LimitCyclesDlg::onbtn_cancel);
    connect(btn_delall_, &QPushButton::clicked, this,
            &P4LimitCyclesDlg::onbtn_delall);
    connect(btn_dellast_, &QPushButton::clicked, this,
            &P4LimitCyclesDlg::onbtn_dellast);

    // finishing
    selected_x0_ = 0;
    selected_y0_ = 0;
    selected_x1_ = 0;
    selected_y1_ = 0;
    selected_grid_ = DEFAULT_LCGRID;
    selected_numpoints_ = DEFAULT_LCPOINTS;

    spin_numpoints_->setValue(selected_numpoints_);
    QString buf;
    buf.sprintf("%g", (float)selected_grid_);
    edt_grid_->setText(buf);

    if (g_VFResults.limCycles_.empty()) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Limit Cycles");
}

void P4LimitCyclesDlg::setSection(double x0, double y0, double x1, double y1)
{
    QString buf;
    QString bufx;
    QString bufy;
    bufx.sprintf("%g", (float)x0);
    bufy.sprintf("%g", (float)y0);
    edt_x0_->setText(bufx);
    edt_y0_->setText(bufy);

    bufx.sprintf("%g", (float)x1);
    bufy.sprintf("%g", (float)y1);
    edt_x1_->setText(bufx);
    edt_y1_->setText(bufy);
}

void P4LimitCyclesDlg::onbtn_start(void)
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
            this, "P4", "Please enter setpoint coordinates for the transverse "
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
        QMessageBox::critical(this, "P4", "Grid size is either too big or too "
                                          "small for this transverse section.");
        return;
    }

    d /= selected_grid_;
    if (d < MIN_LCORBITS || d > MAX_LCORBITS) {
        QMessageBox::critical(this, "P4", "Grid size is either too big or too "
                                          "small for this transverse section.");
        return;
    }

    // SEARCH FOR LIMIT CYCLES:
    s_LCmaxProgressCount = (int)(d + 0.5);
    s_LCprogressDlg.reset(new QProgressDialog("Searching for limit cycles...",
                                              "Stop search", 0,
                                              s_LCmaxProgressCount, this, 0));
    s_LCprogressDlg->setAutoReset(false);
    s_LCprogressDlg->setAutoClose(false);
    s_LCprogressDlg->setMinimumDuration(0);
    s_LCprogressDlg->setValue(s_LCprogressCount = 0);
    setP4WindowTitle(s_LCprogressDlg, "Searching for limit cycles...");

    searchLimitCycle(mainSphere_, selected_x0_, selected_y0_, selected_x1_,
                     selected_y1_, selected_grid_);

    // update buttons
    if (g_VFResults.limCycles_.empty()) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    } else {
        btn_delall_->setEnabled(true);
        btn_dellast_->setEnabled(true);
    }

    g_p4app->processEvents();
    delete s_LCprogressDlg;
    s_LCprogressDlg = nullptr;
}

void P4LimitCyclesDlg::onbtn_cancel(void)
{
    edt_x0_->setText("");
    edt_y0_->setText("");
    edt_x1_->setText("");
    edt_y1_->setText("");
}

void P4LimitCyclesDlg::reset(void)
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
    buf.sprintf("%g", (float)selected_grid_);
    edt_grid_->setText(buf);
    spin_numpoints_->setValue(selected_numpoints_);

    if (g_VFResults.limCycles_.empty()) {
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
    g_LCWindowIsUp = true;
}

void P4LimitCyclesDlg::hideEvent(QHideEvent *he)
{
    QWidget::hideEvent(he);
    g_LCWindowIsUp = false;
}

void P4LimitCyclesDlg::onbtn_delall(void)
{
    plotwnd_->getDlgData();

    btn_delall_->setEnabled(false);
    btn_dellast_->setEnabled(false);

    g_VFResults.limCycles_.clear();
    //g_VFResults.currentLimCycleIndex_ = -1;

    mainSphere_->refresh();
}

void P4LimitCyclesDlg::onbtn_dellast(void)
{
    plotwnd_->getDlgData();

    mainSphere_->prepareDrawing();
    deleteLastLimitCycle(mainSphere_);
    mainSphere_->finishDrawing();

    if (g_VFResults.limCycles_.empty()) {
        btn_delall_->setEnabled(false);
        btn_dellast_->setEnabled(false);
    }
}

bool stop_search_limit(void)
{
    g_p4app->processEvents();
    if (s_LCprogressDlg->wasCanceled())
        return true;

    return false;
}

void write_to_limit_window(double x, double y)
{
    s_LCprogressCount++;

    if (!(s_LCprogressDlg->wasCanceled()))
        s_LCprogressDlg->setValue(s_LCprogressCount);
}
