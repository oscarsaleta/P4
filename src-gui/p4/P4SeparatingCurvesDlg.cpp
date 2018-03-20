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

#include "P4SeparatingCurvesDlg.hpp"

#include <QMessageBox>

#include "P4Application.hpp"
#include "P4FindDlg.hpp"
#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "P4WinInputSphere.hpp"
#include "main.hpp"

P4SeparatingCurvesDlg::P4SeparatingCurvesDlg(P4FindDlg *parent)
    : QWidget{}, parent_{parent}
{
    auto p4title = std::make_unique<QLabel>("Region Separating Curves:", this);
    p4title->setFont(gP4app->getTitleFont());

    auto p4title2 = std::make_unique<QLabel>("Vector Field List:", this);
    p4title2->setFont(gP4app->getTitleFont());

    btn_add_ = std::make_unique<QPushButton>("&Add", this);
    btn_del_ = std::make_unique<QPushButton>("&Del", this);
    btn_edit_ = std::make_unique<QPushButton>("E&dit", this);
    btn_zoomout_ = std::make_unique<QPushButton>("&Zoom out", this);
    btn_refresh_ = std::make_unique<QPushButton>("&Refresh", this);
    btn_eval_ = std::make_unique<QPushButton>("&Eval", this);
    btn_mark_ = std::make_unique<QPushButton>("&Mark", this);
    btn_mark_->setCheckable(true);
    btn_mark_->setChecked(marking_ = true);
    btn_unmark_ = std::make_unique<QPushButton>("&Unmark", this);
    btn_unmark_->setCheckable(true);
    btn_resetmarks_ = std::make_unique<QPushButton>("&Reset Marks", this);
    btn_view_ = std::make_unique<QPushButton>("&View", this);

    markingvf_ = true;

    lbl_vf_or_curves_ = std::make_unique<QLabel>("Curves:", this);
    lbl_vf_or_curves_->setFont(gP4app->getTitleFont());
    lbl_vf_or_curves_->setMinimumSize(lbl_vf_or_curves_->sizeHint());
    lbl_vf_or_curves_->setMaximumSize(lbl_vf_or_curves_->sizeHint());
    lbl_vf_or_curves_->setText("VF:");
    lbl_numpoints_ = std::make_unique<QLabel>("# curve points:", this);
    lbl_status_ = std::make_unique<QLabel>(
        "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM", this);
    lbl_status_->setMaximumSize(lbl_status_->sizeHint());
    lbl_status_->setText("Ready");
    lbl_info_ = std::make_unique<QLabel>(
        "Click on a region to assign the selected vector field", this);

    lst_curves_ = std::make_unique<QListWidget>(this);
    lst_vfs_ = std::make_unique<QListWidget>(this);

    isphere_ = std::make_unique<P4WinInputSphere>(this, lbl_status_);
    isphere_->setupPlot();

    viewParamsWindow_ = std::make_unique<P4ViewDlg>(false);
    viewParamsWindow_->updateDlgData();

    edt_numpoints_ = std::make_unique<QLineEdit>("", this);

    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

    // layout
    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::LeftToRight, this);
    auto layoutA = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);
    auto layoutB = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);
    layoutA->addWidget(p4title.get());

    auto layout1 = std::make_unique<QHBoxLayout>();
    layout1->addWidget(btn_add_.get());
    layout1->addWidget(btn_del_.get());
    auto layout1a = std::make_unique<QHBoxLayout>();
    layout1a->addWidget(lbl_numpoints_.get());
    layout1a->addWidget(edt_numpoints_.get());

    layout1->addWidget(btn_edit_.get());
    layoutA->addLayout(layout1.get());
    layoutA->addWidget(lst_curves_.get());
    layoutA->addLayout(layout1a.get());
    layoutA->addWidget(p4title2.get());
    layoutA->addWidget(lst_vfs_.get());

    auto layout2 = std::make_unique<QHBoxLayout>();
    layout2->addWidget(lbl_vf_or_curves_.get());
    layout2->addWidget(btn_mark_.get());
    layout2->addWidget(btn_unmark_.get());
    layout2->addWidget(btn_resetmarks_.get());
    layout2->addStretch(1);

    auto layout3 = std::make_unique<QHBoxLayout>();
    layout3->addWidget(btn_refresh_.get());
    layout3->addWidget(btn_eval_.get());
    layout3->addWidget(btn_view_.get());
    layout3->addWidget(btn_zoomout_.get());
    layout3->addStretch(1);

    layoutB->addLayout(layout2.get());
    layoutB->addWidget(lbl_info_.get());
    layoutB->addLayout(layout3.get());
    layoutB->addWidget(isphere_.get());
    layoutB->addWidget(lbl_status_.get());

    layoutB->setStretchFactor(isphere_.get(), 1);
    layoutB->setStretchFactor(lbl_status_.get(), 0);

    mainLayout_->addLayout(layoutA.get());
    mainLayout_->addLayout(layoutB.get());
    mainLayout_->setStretchFactor(layoutA.get(), 0);
    mainLayout_->setStretchFactor(layoutB.get(), 2);
    setLayout(mainLayout_.get());

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
    QObject::connect(btn_refresh_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnRefresh);
    QObject::connect(btn_eval_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnEval);
    QObject::connect(btn_view_.get(), &QPushButton::clicked, this,
                     &P4SeparatingCurvesDlg::onBtnView);

    QObject::connect(lst_curves_.get(), &QListWidget::itemChanged, this,
                     &P4SeparatingCurvesDlg::onCurveChanged);
    QObject::connect(lst_curves_.get(), &QListWidget::itemSelectionChanged,
                     this, &P4SeparatingCurvesDlg::onCurvesSelectionChanged);
    QObject::connect(lst_curves_.get(), &QListWidget::itemActivated, this,
                     &P4SeparatingCurvesDlg::onCurvesItemActivated);
    QObject::connect(lst_vfs_.get(), &QListWidget::itemSelectionChanged, this,
                     &P4SeparatingCurvesDlg::onVfsSelectionChanged);
    QObject::connect(lst_vfs_.get(), &QListWidget::itemActivated, this,
                     &P4SeparatingCurvesDlg::onVfsItemActivated);
    QObject::connect(lst_curves_.get(), &QListWidget::itemClicked, this,
                     &P4SeparatingCurvesDlg::onCurvesClicked);
    QObject::connect(lst_vfs_.get(), &QListWidget::itemClicked, this,
                     &P4SeparatingCurvesDlg::onVfsClicked);

    QObject::connect(edt_numpoints_.get(), &QLineEdit::editingFinished, this,
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
    if (gThisVF->numVFRegions_ > 0 || gThisVF->numCurveRegions_ > 0) {
        gThisVF->clearVFMarks();
        gThisVF->clearCurveMarks();
    }

    auto itm = std::make_unique<QListWidgetItem>("polynomial");
    itm->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                  Qt::ItemIsEnabled);
    lst_curves_->addItem(itm.get());
    lst_curves_->scrollToItem(itm.get());
    lst_curves_->setCurrentItem(itm.get());
    lst_curves_->editItem(itm.get());
    gThisVF->addSeparatingCurve();
    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onBtnDel()
{
    if (lst_curves_->selectedItems().isEmpty())
        return;
    if (gThisVF->numVFRegions_ > 0 || gThisVF->numCurveRegions_ > 0) {
        gThisVF->clearVFMarks();
        gThisVF->clearCurveMarks();
    }

    int r{lst_curves_->currentRow()};
    std::unique_ptr<QListWidgetItem> itm;
    if (r >= 0 && r < gThisVF->numSeparatingCurves_) {
        itm.reset(lst_curves_->takeItem(r));
        // lst_curves_.removeItemWidget(itm.get()); TODO necessary?
    }
    gThisVF->deleteSeparatingCurve(r);
    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onBtnEdit()
{
    lst_curves_->editItem(lst_curves_->currentItem());
}

void P4SeparatingCurvesDlg::updateDlgData()
{
    std::unique_ptr<QListWidgetItem> itm;
    int i, v;
    QString s;

    lst_curves_->clear();

    for (i = 0; i < gThisVF->numSeparatingCurves_; i++) {
        if (gThisVF->separatingCurves_[i].isEmpty())
            itm = std::make_unique<QListWidgetItem>("polynomial", lst_curves_);
        else
            itm = std::make_unique<QListWidgetItem>(
                gThisVF->separatingCurves_[i], lst_curves_);
    }

    if (!lst_curves_->selectedItems().isEmpty()) {
        int curveindex{lst_curves_->row(lst_curves_->selectedItems()[0])};
        if (curveindex < 0 || curveindex >= gThisVF->numSeparatingCurves_ ||
            gThisVF->numPointsSeparatingCurve_.empty())
            v = -1;
        else
            v = gThisVF->numPointsSeparatingCurve_[curveindex];
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

    for (i = 0; i < gThisVF->numVF_; i++) {
        s = "x' = ";
        s = s.append(gThisVF->xdot_[i]);
        s = s.append("\ny' = ");
        s = s.append(gThisVF->ydot_[i]);
        s = s.append("\n");
        itm = std::make_unique<QListWidgetItem>(s, lst_vfs_);
        itm->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    isphere_->setupCoordinateTransformations();
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onCurveChanged(QListWidgetItem *itm)
{
    int r{lst_curves_->row(itm)};
    if (r < 0 || r >= gThisVF->numSeparatingCurves_)
        return;
    QString s{itm->text().trimmed()};
    if (gThisVF->separatingCurves_[r].compare(s)) {
        gThisVF->separatingCurves_[r] = s;
        if (gThisVF->changed_ == false) {
            gThisVF->changed_ = true;
            gP4app->signalChanged();
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
        if (curveindex < 0 || curveindex >= gThisVF->numSeparatingCurves_ ||
            gThisVF->numPointsSeparatingCurve_.empty())
            v = -1;
        else
            v = gThisVF->numPointsSeparatingCurve_[curveindex];
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
        if (gThisVF->numVFRegions_ > 0) {
            gThisVF->clearVFMarks();
            if (gThisVF->changed_ == false) {
                gThisVF->changed_ = true;
                gP4app->signalChanged();
            }
            isphere_->refresh();
        }
        lbl_info_->setText(
            "All vector field marks have been erased. Please, reassign vector "
            "fields from scratch.");
    } else {
        if (gThisVF->numCurveRegions_ > 0) {
            gThisVF->clearCurveMarks();
            if (gThisVF->changed_ == false) {
                gThisVF->changed_ = true;
                gP4app->signalChanged();
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
    isphere_->isZoom_ = false;
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
    if (gThisVF->evaluating_)
        return;
    parent_->getDataFromDlg();
    gThisVF->evaluateSeparatingCurves();
    if (gThisVF->evaluating_)
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
        gVFResults.setupCoordinateTransformations();
        isphere_->setupPlot();
    }
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::signalSeparatingCurvesEvaluated()
{
    btn_eval_->setEnabled(true);
    if (!gVFResults.readTables(gThisVF->getbarefilename(), true, false)) {
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
    isphere_->isZoom_ = true;
    isphere_->setupPlot();
    isphere_->refresh();
}

void P4SeparatingCurvesDlg::onMouseClickLeft(double x0, double y0, double z0)
{
    int vfindex, curveindex, oldindex;
    double pcoord[3] = {x0, y0, z0};
    QString s;

    if (gVFResults.separatingCurves_.empty()) {
        QMessageBox::critical(this, "P4",
                              "Please evaluate the piecewise configuration "
                              "first,\nusing the Eval button.\n");
        return;
    }

    if (markingvf_) {
        if (lst_vfs_->selectedItems().isEmpty())
            return;
        vfindex = lst_vfs_->row(lst_vfs_->selectedItems()[0]);
        oldindex = gThisVF->getVFIndex_sphere(pcoord);
        if (marking_) {
            if (oldindex == -1) {
                s.sprintf("Marked region with VF #%d", vfindex + 1);
                lbl_status_->setText(s);
                gThisVF->markVFRegion(vfindex, pcoord);
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                isphere_->refresh();
            } else if (oldindex == vfindex) {
                s.sprintf("Region is already marked with VF #%d", vfindex + 1);
                lbl_status_->setText(s);
                return;
            } else {
                s.sprintf("Marked region with VF #%d", vfindex + 1);
                lbl_status_->setText(s);
                gThisVF->markVFRegion(vfindex, pcoord);
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
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
                gThisVF->unmarkVFRegion(vfindex, pcoord);
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                isphere_->refresh();
            } else {
                s.sprintf("Region is marked by a different VF (#%d)!",
                          oldindex + 1);
                lbl_status_->setText(s);
                return;
            }
        }
    } else {
        if (lst_curves_->selectedItems().isEmpty())
            return;
        curveindex = lst_curves_->row(lst_curves_->selectedItems()[0]);
        if (marking_) {
            if (!gThisVF->isCurvePointDrawn(curveindex, pcoord)) {
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
                gThisVF->markCurveRegion(curveindex, pcoord);
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
                }
                isphere_->refresh();
            }
        } else {
            if (gThisVF->isCurvePointDrawn(curveindex, pcoord)) {
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
                gThisVF->unmarkCurveRegion(curveindex, pcoord);
                if (gThisVF->changed_ == false) {
                    gThisVF->changed_ = true;
                    gP4app->signalChanged();
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
    if (curveindex < 0 || curveindex >= gThisVF->numSeparatingCurves_)
        return;

    QString s{edt_numpoints_->text()};
    bool ok;
    int v{s.toInt(&ok)};
    if (!ok || v <= 0)
        return;

    if (v == gThisVF->numPointsSeparatingCurve_[curveindex])
        return;

    gThisVF->numPointsSeparatingCurve_[curveindex] = v;
    if (gThisVF->changed_ == false) {
        gThisVF->changed_ = true;
        gP4app->signalChanged();
    }
}