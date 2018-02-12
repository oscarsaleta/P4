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

#include "P4SeparatingCurvesDlg.h"

#include "P4Application.h"
#include "P4ViewDlg.h"
#include "QWinInputSphere.h"

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

P4SeparatingCurvesDlg::P4SeparatingCurvesDlg(std::unique_ptr<P4ViewDlg> _parent)
    : QWidget(), parent{_parent}
{
    std::unique_ptr<QLabel> p4title{
        new QLabel("Region Separating Curves:", this)};
    p4title->setFont(*(g_p4app->titleFont_));

    std::unique_ptr<QLabel> p4title2{new QLabel("Vector Field List:", this)};
    p4title2->setFont(*(g_p4app->titleFont_));

    // TODO: check if i have to redeclare?
    btn_add_.reset(new QPushButton("&Add"), this);
    btn_del_.reset(new QPushButton("&Del", this));
    btn_edit_.reset(new QPushButton("E&dit", this));
    btn_zoomout_.reset(new QPushButton("&Zoom out", this));
    btn_refresh_.reset(new QPushButton("&Refresh", this));
    btn_eval_.reset(new QPushButton("&Eval", this));
    btn_mark_.reset(new QPushButton("&Mark", this));
    btn_mark_->setCheckable(true);
    btn_mark_->setChecked(marking_ = true);
    btn_unmark_.reset(new QPushButton("&Unmark", this));
    btn_unmark_->setCheckable(true);
    btn_resetmarks_.reset(new QPushButton("&Reset Marks", this));
    btn_view_.reset(new QPushButton("&View", this));

    markingvf_ = true;

    lbl_vf_or_curves_.reset(new QLabel("Curves:", this));
    lbl_vf_or_curves_->setFont(*(p4app->titleFont_));
    lbl_vf_or_curves_->setMinimumSize(lbl_vf_or_curves_->sizeHint());
    lbl_vf_or_curves_->setMaximumSize(lbl_vf_or_curves_->sizeHint());
    lbl_vf_or_curves_->setText("VF:");
    lbl_numpoints.reset(new QLabel("# curve points:", this));
    lbl_status_.reset(
        new QLabel("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM", this));
    lbl_status_->setMaximumSize(lbl_status_->sizeHint());
    lbl_status_->setText("Ready");
    lbl_info_.reset(new QLabel(
        "Click on a region to assign the selected vector field", this));

    lst_curves_.reset(new QListWidget(this));
    lst_vfs_.reset(new QListWidget(this));

    isphere_.reset(new QWinInputSphere(this, lbl_status_));
    isphere_->setupPlot();

    viewParamsWindow_.reset(new P4ViewDlg(this, false));
    viewParamsWindow_->updateDlgData();

    edt_numpoints_.reset(new QLineEdit("", this));

    if (g_p4smallicon)
        setWindowIcon(*g_p4smallicon);

    // layout
    mainLayout_.reset(new QBoxLayout(QBoxLayout::LeftToRight, this));
    std::unique_ptr<QBoxLayout> layoutA{
        new QBoxLayout(QBoxLayout::TopToBottom, this)};
    std::unique_ptr<QBoxLayout> layoutB{
        new QBoxLayout(QBoxLayout::TopToBottom, this)};
    layoutA->addWidget(p4title);
    std::unique_ptr<QHBoxLayout> layout1{new QHBoxLayout()};
    layout1->addWidget(btn_add_);
    layout1->addWidget(btn_del_);
    std::unique_ptr<QHBoxLayout> layout1a{new QHBoxLayout()};
    layout1a->addWidget(lbl_numpoints_);
    layout1a->addWidget(edt_numpoints_);

    layout1->addWidget(btn_edit_);
    layoutA->addLayout(layout1);
    layoutA->addWidget(lst_curves_);
    layoutA->addLayout(layout1a);
    layoutA->addWidget(p4title2);
    layoutA->addWidget(lst_vfs_);

    std::unique_ptr<QHBoxLayout> layout2{new QHBoxLayout()};
    layout2->addWidget(lbl_vf_or_curves_);
    layout2->addWidget(btn_mark_);
    layout2->addWidget(btn_unmark_);
    layout2->addWidget(btn_resetmarks_);
    layout2->addStretch(1);

    std::unique_ptr<QHBoxLayout> layout3{new QHBoxLayout()};
    layout3->addWidget(btn_refresh_);
    layout3->addWidget(btn_eval_);
    layout3->addWidget(btn_view_);
    layout3->addWidget(btn_zoomout_);
    layout3->addStretch(1);

    layoutB->addLayout(layout2);
    layoutB->addWidget(lbl_info_);
    layoutB->addLayout(layout3);
    layoutB->addWidget(isphere_);
    layoutB->addWidget(lbl_status_);

    layoutB->setStretchFactor(isphere_, 1);
    layoutB->setStretchFactor(lbl_status_, 0);

    mainLayout_->addLayout(layoutA);
    mainLayout_->addLayout(layoutB);
    mainLayout_->setStretchFactor(layoutA, 0);
    mainLayout_->setStretchFactor(layoutB, 2);
    setLayout(mainLayout_);

    // connections
    QObject::connect(btn_add_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnAdd);
    QObject::connect(btn_del_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnDel);
    QObject::connect(btn_edit_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnEdit);
    QObject::connect(btn_mark_.get(), &QPushButton::toggled, this,
                     &P4SeparatingCurvesDlg::onBtnMarkToggled);
    QObject::connect(btn_unmark_.get(), &QPushButton::toggled, this,
                     &P4SeparatingCurvesDlg::onBtnUnmarkToggled);
    QObject::connect(btn_resetmarks_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnResetMarks);
    QObject::connect(btn_zoomout_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnZoomOut);
    QObject::connect(btn_refresh_.get(), &QPushBut ton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnRefresh);
    QObject::connect(btn_eval_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnEval);
    QObject::connect(btn_view_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnView);

    QObject::connect(lst_curves_, &QListWidget::itemChanged, this,
                     &P4SeparatingCurvesDlg::onCurveChanged);
    QObject::connect(lst_curves_, &QListWidget::itemSelectionChanged, this,
                     &P4SeparatingCurvesDlg::onCurvesSelectionChanged);
    QObject::connect(lst_curves_, &QListWidget::itemActivated, this,
                     &P4SeparatingCurvesDlg::onCurvesItemActivated);
    QObject::connect(lst_vfs_, &QListWidget::itemSelectionChanged, this,
                     &P4SeparatingCurvesDlg::onVfsSelectionChanged);
    QObject::connect(lst_vfs_, &QListWidget::itemActivated, this,
                     &P4SeparatingCurvesDlg::onVfsItemActivated);
    QObject::connect(lst_curves_, &QListWidget::itemClicked, this,
                     &P4SeparatingCurvesDlg::onCurvesClicked);
    QObject::connect(lst_vfs_, &QListWidget::itemClicked, this,
                     &P4SeparatingCurvesDlg::onVfsClicked);

    QObject::connect(edt_numpoints_, &QLineEdit::editingFinished, this,
                     &P4SeparatingCurvesDlg::onNumpointsEditingFinished);

#ifdef TOOLTIPS
    btn_add_->setToolTip(
        "Adds a new separating curve to the list.\nEnter a Maple expression "
        "after pressing the button.");
    btn_del_->setToolTip(
        "Deletes the selected separating curve from the list.");
    btn_edit_->setToolTip("Edits the separating curve Maple expression.");
    btn_mark_->setToolTip(
        "If selected, clicking in the configuration window adds a marks to the "
        "clicked region.");
    btn_unmark_->setToolTip(
        "If selected, clicking in the configuration window removes a marks to "
        "the clicked region.");
    btn_resetmarks_->setToolTip("Reset all region marks.");
    btn_zoomout_->setToolTip(
        "Zoom out.  You can zoom in by pressing CTRL, clicking in the window "
        "upper-left corner\nand keeping the mouse pressed while going to the "
        "lower-right corner.");
    btn_refresh_->setToolTip("Refresh the configuration window.");
    btn_eval_->setToolTip("Evaluate the configuration using Maple.");
    btn_view_->setToolTip("Changes the view on the configuration window.");
    lst_curves_->setToolTip(
        "A list of all separating curves.\nThe combination of the signs define "
        "the different regions.");
    lst_vfs_->setToolTip("A list of vector fields, numbered #1, #2, ...");
    edt_numpoints_->setToolTip(
        "Number of points to calculate this separating curve while "
        "evaluating.");
//    isphere->setToolTip( "Configuration window.  Click to mark/unmark a region
//    with the selected vector field (or curve)" );
#endif

    // finishing

    setP4WindowTitle(this, "P4 Piecewise Configuration");

    updateDlgData();
    lst_vfs_->setCurrentRow(0);
    lst_vfs_->setFocus();
}

void P4SeparatingCurvesDlg::onBtnAdd()
{
    if (g_ThisVF->numVFRegions_ > 0 || g_ThisVF->numCurveRegions_ > 0) {
        g_ThisVF->clearVfMarks();
        g_ThisVF->clearCurveMarks();
    }

    std::unique_ptr<QListWidgetItem> itm{new QListWidgetItem("polynomial")};
    itm->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                  Qt::ItemIsEnabled);
    lst_curves_->addItem(itm);
    lst_curves_->scrollToItem(itm);
    lst_curves_->setCurrentItem(itm);
    lst_curves_->editItem(itm);
    g_ThisVF->addSeparatingCurve();
    if (g_ThisVF->changed_ == false) {
        g_ThisVF->changed_ = true;
        g_p4app->signalChanged();
    }
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::OnBtnDel()
{
    if (lst_curves_->selectedItems().isEmpty())
        reeturn;
    if (g_ThisVF->numVFRegions_ > 0 || g_ThisVF->numCurveRegions_ > 0) {
        g_ThisVF->clearVfMarks();
        g_ThisVF->clearCurveMarks();
    }

    int r{lst_curves_->currentRow()};
    std::unique_ptr<QListWidgetItem> itm;
    if (r >= 0 && r < g_ThisVF->numCurves_) {
        itm.reset(lst_curves_->takeItem(r));
        // lst_curves_.removeItemWidget(itm.get()); TODO necessary?
    }
    g_ThisVF->deleteSeparatingCurve(r);
    if (g_ThisVF->changed_ == false) {
        g_ThisVF->changed_ = true;
        p4app->signalChanged();
    }
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onBtnEdit()
{
    lst_curves_->editItem(lst_curves_->currentItem());
}

void P4SeparatingCurvesDlg::getDataFromDlg() {}

void P4SeparatingCurvesDlg::updateDlgData()
{
    std::unique_ptr<QListWidgetItem> itm;
    int i, v;
    QString s;

    lst_curves_->clear();

    for (i = 0; i < g_ThisVF->numCurves_; i++) {
        if (g_ThisVF->curves_[i].isEmpty())
            itm.reset(new QListWidgetItem("polynomial", lst_curves_));
        else
            itm.reset(
                new QListWidgetItem(*(g_ThisVF->curves_[i]), lst_curves_));
    }

    if (!lst_curves_->selectedItems().isEmpty()) {
        int curveindex { lst_curves_->row(lst_curves_->selectedItems()[0]) }
        if (curveindex < 0 || curveindex >= g_ThisVF->numCurves_ ||
            g_ThisVF->numPointsCurve_.empty())
            v = -1;
        else
            v = g_ThisVF->numPointsCurve_[curveindex];
    } else {
        v = -1;
    }

    if (v < 0) {
        edt_numpoints_->setText("");
    } else {
        s.setNum(v);
        edt_numpoints_->setText(s);
    }

    lst_vfs_->clear();

    for (i = 0; i < g_ThisVF->numVF_; i++) {
        s = "x' = ";
        s = s.append(*(g_ThisVF->xdot_[i]));
        s = s.append("\ny' = ");
        s = s.append(*(g_ThisVF->ydot_[i]));
        s = s.append("\n");
        itm.reset(new QListWidgetItem(s, lst_vfs_));
        itm->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    isphere_->setupCoordinateTransformations();
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onCurveChanged(QListWidgetItem *itm)
{
    int r{lst_curves_->row(itm)};
    if (r < 0 || r >= g_ThisVF->numCurves_)
        return QString s{itm->text().trimmed()};
    if (g_ThisVF->curves_[r].compare(s)) {
        g_ThisVF->curves_[r] = s;
        if (g_ThisVF->changed_ == false) {
            g_ThisVF->changed_ = true;
            g_p4app->signalChanged();
        }
    }
}

void P4SeparatingCurvesDlg::onCurvesSelectionChanged()
{
    int v;
    QString s;

    if (!lst_curves_->selectedItems().isEmpty()) {
        if (markingvf_) {
            lbl_vf_or_curves_->setText("Curves:");
            if (marking_)
                lbl_info_->setText(
                    "Click on a region where the selected curve is not to be "
                    "drawn");
            else
                lbl_info_->setText(
                    "Click on a region were the selected curve is to be drawn");
            markingvf_ = false;
        }
        int curveindex{lst_curves_->row(lst_curves_->selectedItems()[0])};
        if (curveindex < 0 || curveindex >= g_ThisVF->numCurves_ ||
            g_ThisVF->numPointsCurve_.empty())
            v = -1;
        else
            v = g_ThisVF->numPointsCurve_[curveindex];
    } else
        v = -1;

    if (v < 0)
        s = "";
    else
        s.setNum(v);

    edt_numpoints_->setText(s);
}

void P4SeparatingCurvesDlg::onCurvesItemActivated(QListWidgetItem *itm)
{
    onCurvesSelectionChanged();
}

void P4SeparatingCurvesDlg::onVfsSelectionChanged()
{
    if (!lst_vfs_->selectedItems().isEmpty()) {
        if (!markingvf_) {
            lbl_vf_or_curves_->setText("VF:");
            if (marking_)
                lbl_info_->setText(
                    "Click on a region to assign the selected vector field");
            else
                lbl_info_->setText(
                    "Click on a region to unassign the selected vector field");
            markingvf_ = true;
        }
    }
}

void P4SeparatingCurvesDlg::onVfsItemActivated(QListWidgetItem *itm)
{
    onVfsSelectionChanged();
}

void P4SeparatingCurvesDlg::onCurvesClicked(QListWidgetItem *itm)
{
    onCurvesSelectionChanged();
}

void P4SeparatingCurvesDlg::onVfsClicked(QListWidgetItem *itm)
{
    onVfsSelectionChanged();
}

void P4SeparatingCurvesDlg::onBtnUnmarkToggled(bool c)
{
    if (c) {
        btn_mark_->setChecked(false);
        marking_ = false;
        if (markingvf_)
            lbl_info_->setText(
                "Click on a region to unassing the selected vector field");
        else
            lbl_info_->setText(
                "Click on a region where the selected curve is to be drawn");
    } else {
        marking_ = true;
        if (!btn_mark_->isChecked())
            btn_mark_->setChecked(true);
        if (markingvf_)
            lbl_info_->setText(
                "Click on a region to assign the selected vector field");
        else
            lbl_info_->setText(
                "Click on a region where the selected curve is not to be "
                "drawn");
    }
}

void P4SeparatingCurvesDlg::onBtnMarkToggled(bool c)
{
    if (c) {
        btn_unmark_->setChecked(false);
        marking_ = true;
        if (markingvf_)
            lbl_info_->setText(
                "Click on a regionm to assign the selected vector field");
        else
            lbl_info_->setText(
                "Click on a region where the selected curve is not to be "
                "drawn");
    } else {
        marking_ = false;
        if (!btn_unmark_->isChecked())
            btn_unmark_->setChecked(true);
        if (markingvf_)
            lbl_info_->setText(
                "Click on a region to unassign the selected vector field");
        else
            lbl_info_->setText(
                "Click on a region where the selected curve is to be drawn");
    }
}

void P4SeparatingCurvesDlg::onBtnResetMarks()
{
    if (markingvf_) {
        if (g_ThisVF->numVFRegions_ > 0) {
            g_ThisVF->clearVfMarks();
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
            isphere_->refresh();
        }
        lbl_info_->setText(
            "All vector field marks have been erased. Please, reassign vector "
            "fields from scratch.");
    } else {
        if (g_ThisVF->numCurveRegions_ > 0) {
            g_ThisVF->clearCurveMarks();
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
            isphere_->refresh();
        }
        lbl_info_->setText(
            "All curve marks have been erased. Now separating curves are drawn "
            "completely.");
    }
}

void P4SeparatingCurvesDlg::onBtnZoomOut()
{
    isphere_->iszoom_ = false;
    isphere_->setupPlot();
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onBtnView()
{
    viewParamsWindow_->show();
    viewParamsWindow_->raise();
}

void P4SeparatingCurvesDlg::onBtnEval()
{
    if (g_ThisVF->evaluating_)
        return;
    parent_->getDataFromDlg();
    g_ThisVF->evaluateSeparatingCurves();  // TODO: a p5 es EvaluateCurves()
    if (g_ThisVF->evaluating_)
        btn_eval_->setEnabled(false);
    else
        QMessageBox::critical(
            this, "P4",
            "An error occured while plotting the separating curves\n");
}

void P4SeparatingCurvesDlg::onBtnRefresh()
{
    if (viewParamsWindow_->getDataFromDlg()) {
        // true when a big change occured in the view
        g_VFResults.setupCoordinateTransformations();
        isphere_->setupPlot();
    }
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::signalCurvesEvaluated()
{
    btn_eval_->setEnabled(true);
    if (!g_VFResults.readTables(g_ThisVF->getbarefilename(), true, false)) {
        QMessageBox::critical(
            this, "P4",
            "Cannot read computation results.\n"
            "Please check the input vector field, curves and parameters");
    }
    isphere_->setupPlot();
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::makeZoom(double x1, double y1, double x2, double y2)
{
    // chart a zoom window
    double swap;
    if (x1 > x2) {
        swap = x1;
        x1 = x2;
        x2 = swap;
    }
    if (y1 > y2) {
        swap = y1;
        y1 = y2;
        y2 = swap;
    }
    if (x1 == x2 || y1 == y2)
        return;

    isphere_->x0_ = x1;
    isphere_->y0_ = y1;
    isphere_->x1_ = x2;
    isphere_->y1_ = y2;
    isphere_->iszoom_ = true;
    isphere_->setupPlot();
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onMouseClickLeft(double x0, double y0, double z0)
{
    int vfindex, curveindex, oldindex;
    double pcoord[3] = {x0, y0, z0};
    QString s;

    if (g_VFResults.curves_result_.empty()) {
        QMessageBox::critical(this, "P4",
                              "Please evaluate the piecewise configuration "
                              "first,\nusing the Eval button.\n");
        return;
    }

    if (markingvf_) {
        if (lst_vfs_->selectedItems().isEmpty())
            return;
        vfindex = lst_vfs_->row(lst_vfs_->selectedItems()[0]);
        oldindex = g_ThisVF->getVFIndex_sphere(pcoord);
        if (marking_) {
            if (oldindex == -1) {
                s.sprintf("Marked region with VF #%d", vfindex + 1);
                lbl_status_->setText(s);
                g_ThisVF->markVFRegion(vfindex, pcoord);
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                isphere_->refresh();
            } else if (oldindex == vfindex) {
                s.sprintf("Region is already marked with VF #%d", vfindex + 1);
                lbl_status_->setText(s);
                return;
            } else {
                s.sprintf("Marked region with VF #%d", vfindex + 1);
                lbl_status_->setText(s);
                g_ThisVF->markVFRegion(vfindex, pcoord);
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                isphere_->refresh();
            }
        } else {
            if (oldindex == -1) {
                s.sprintf("The region is not marked yet: no VF is assigned.");
                lbl_status_->setText(s);
                return;
            } else if (oldindex == vfindex) {
                s.sprintf("Region has been unmarked: no VF is assigned");
                lbl_status_->setText(s);
                g_ThisVF->unmarkVFRegion(vfindex, pcoord);
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed = true;
                    g_p4app->signalChanged();
                }
                isphere_->refresh();
            } else {
                s.sprintf("Region is marked by a different VF (#%d)!",
                          oldindex + 1);
                lbl_status_->setText(s);
                return
            }
        }
    } else {
        if (lst_curves_->selectedItems().isEmpty())
            return;
        curveindex = lst_curves_->row(lst_curves_->selectedItems()[0]);
        if (marking_) {
            if (!g_ThisVF->isCurvePointDrawn(curveindex, pcoord)) {
                s.sprintf(
                    "In this region, the plotting of the curve #%d was already "
                    "disabled.",
                    curveindex + 1);
                lbl_status_->setText(s);
            } else {
                s.sprintf(
                    "The plotting of the curve #%d is now disabled in this "
                    "region.",
                    curveindex + 1);
                lbl_status_->setText(s);
                g_ThisVF->markCurveRegion(curveindex, pcoord);
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                isphere_->refresh();
            }
        } else {
            if (g_ThisVF->isCurvePointDrawn(curveindex, pcoord)) {
                s.sprintf(
                    "In this region, the plotting of curve #%d was already "
                    "enabled.",
                    curveindex + 1);
                lbl_status_->setText(s);
                return;
            } else {
                s.sprintf(
                    "The plotting of curve #%d is now enabled in this region.",
                    curveindex + 1);
                lbl_status_->setText(s);
                g_ThisVF->unmarkCurveRegion(curveindex, pcoord);
                if (g_ThisVF->changed_ == false) {
                    g_ThisVF->changed_ = true;
                    g_p4app->signalChanged();
                }
                isphere_->refresh();
            }
        }
    }
}

void P4SeparatingCurvesDlg::onMouseClickRight(double x0, double y0, double z0)
{
    // unused
}

void P4SeparatingCurvesDlg::onNumpointsEditingFinished()
{
    if (lst_curves_->selectedItems().isEmpty())
        return;
    int curveindex{lst_curves_->row(lst_curves_->selectedItems()[0])};
    if (curveindex < 0 || curveindex >= g_ThisVF->numCurves_)
        return;

    QString s{edt_numpoints_->text()};
    bool ok;
    int v{s.toInt(&ok)};
    if (!ok || v<=0) return;

    if (v == g_ThisVF->numPointsCurve_[curveindex]) return;

    g_ThisVF->numPointsCurve_[curveindex]=v;
    if(g_ThisVF->changed_==false){
        g_ThisVF->changed_=true;
        g_p4app->signalChanged();
    }
}