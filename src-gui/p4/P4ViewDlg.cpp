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

#include "P4ViewDlg.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>

#include "P4Application.hpp"
#include "P4InputVF.hpp"
#include "P4ParentStudy.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "math_p4.hpp"
#include "tables.hpp"

P4ViewDlg::~P4ViewDlg() { getDataFromDlg(); }

P4ViewDlg::P4ViewDlg(bool virtualchk, QWidget *parent)
    : QWidget{parent, Qt::Tool | Qt::WindowStaysOnTopHint},
      haveVirtualCheckBox_{virtualchk}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    auto p4title = new QLabel{"View Parameters", this};
    p4title->setFont(gP4app->getTitleFont());

    auto kindlabel = new QLabel{"Type of view: ", this};
    kindlabel->setFont(gP4app->getBoldFont());

    auto btngrp = new QButtonGroup{this};
    btn_sphere_ = new QRadioButton{"Spherical", this};
    btn_plane_ = new QRadioButton{"Planar", this};
    btn_U1_ = new QRadioButton{"U1", this};
    btn_U2_ = new QRadioButton{"U2", this};
    btn_V1_ = new QRadioButton{"V1", this};
    btn_V2_ = new QRadioButton{"V2", this};
    btngrp->addButton(btn_sphere_);
    btngrp->addButton(btn_plane_);
    btngrp->addButton(btn_U1_);
    btngrp->addButton(btn_U2_);
    btngrp->addButton(btn_V1_);
    btngrp->addButton(btn_V2_);

    auto lbl_projection = new QLabel{"Projection:", this};
    lbl_projection->setFont(gP4app->getBoldFont());
    edt_projection_ = new QLineEdit{"-1", this};

    auto lbl_x0 = new QLabel{"Min. x:", this};
    lbl_x0->setFont(gP4app->getBoldFont());
    edt_x0_ = new QLineEdit{"-1", this};

    btn_square_ = new QPushButton{"&Square", this};

    auto lbl_y0 = new QLabel{"Min. y:", this};
    lbl_y0->setFont(gP4app->getBoldFont());
    edt_y0_ = new QLineEdit{"-1", this};

    auto lbl_x1 = new QLabel{"Max. x", this};
    lbl_x1->setFont(gP4app->getBoldFont());
    edt_x1_ = new QLineEdit{"1", this};

    auto lbl_y1 = new QLabel{"Max. y", this};
    lbl_y1->setFont(gP4app->getBoldFont());
    edt_y1_ = new QLineEdit{"1", this};

    if (haveVirtualCheckBox_)
        chk_plotvirtuals_ = new QCheckBox{"Plot Virtual Singularities", this};

#ifdef TOOLTIPS
    btn_sphere_->setToolTip(
        "Spherical projection of the plane (Poincare or Poincare-Lyapunov)");
    btn_plane_->setToolTip(
        "Planar view of a compact, rectangular region in the plane");
    btn_U1_->setToolTip("View of the U1 chart.  Infinity is shown as a line.");
    btn_U2_->setToolTip("View of the U2 chart.  Infinity is shown as a line.");
    btn_V1_->setToolTip("View of the V1 chart.  Infinity is shown as a line.");
    btn_V2_->setToolTip("View of the V2 chart.  Infinity is shown as a line.");
    edt_projection_->setToolTip(
        "Choose projection point of the Poincare sphere.\n"
        "(Only relevant for spherical projection; point must be < 0)");
    edt_x0_->setToolTip("Minimum coordinate on the horizontal axis");
    edt_y0_->setToolTip("Minimum coordinate on the vertical axis");
    edt_x1_->setToolTip("Maximum coordinate on the horizontal axis");
    edt_y1_->setToolTip("Maximum coordinate on the vertical axis");
    btn_square_->setToolTip("Fills fields MinY, MaxX, MaxY with "
                            "MinX,-MinX,-MinX respectively,\nto make a square "
                            "rectangle around the origin.");
    if (haveVirtualCheckBox_)
        chk_plotvirtuals_->setToolTip(
            "Determines wheter or not to plot singularities of vector fields "
            "outside their domain of definition.\nYou have to press the "
            "Refresh button after changing this option.");
#endif

    // layout

    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    mainLayout_->addWidget(p4title);

    auto kindLayout = new QGridLayout{};
    kindLayout->addWidget(kindlabel, 0, 0);
    kindLayout->addWidget(btn_sphere_, 0, 1);
    kindLayout->addWidget(btn_plane_, 0, 2);
    kindLayout->addWidget(btn_U1_, 1, 1);
    kindLayout->addWidget(btn_U2_, 1, 2);
    kindLayout->addWidget(btn_V1_, 2, 1);
    kindLayout->addWidget(btn_V2_, 2, 2);

    mainLayout_->addLayout(kindLayout);

    auto layout1 = new QHBoxLayout{};
    layout1->addWidget(lbl_projection);
    layout1->addWidget(edt_projection_);
    layout1->addStretch(0);

    auto layout2 = new QHBoxLayout{};
    layout2->addWidget(lbl_x0);
    layout2->addWidget(edt_x0_);
    layout2->addWidget(btn_square_);
    layout2->addStretch(0);

    auto layout3 = new QHBoxLayout{};
    layout3->addWidget(lbl_y0);
    layout3->addWidget(edt_y0_);
    layout3->addStretch(0);

    auto layout4 = new QHBoxLayout{};
    layout4->addWidget(lbl_x1);
    layout4->addWidget(edt_x1_);
    layout4->addStretch(0);

    auto layout5 = new QHBoxLayout{};
    layout5->addWidget(lbl_y1);
    layout5->addWidget(edt_y1_);
    layout5->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);
    if (haveVirtualCheckBox_)
        mainLayout_->addWidget(chk_plotvirtuals_);
    mainLayout_->addStretch(0);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections

    QObject::connect(btn_sphere_, &QRadioButton::toggled, this,
                     &P4ViewDlg::btn_sphere_toggled);
    QObject::connect(btn_plane_, &QRadioButton::toggled, this,
                     &P4ViewDlg::btn_plane_toggled);
    QObject::connect(btn_U1_, &QRadioButton::toggled, this,
                     &P4ViewDlg::btn_U1_toggled);
    QObject::connect(btn_U2_, &QRadioButton::toggled, this,
                     &P4ViewDlg::btn_U2_toggled);
    QObject::connect(btn_V1_, &QRadioButton::toggled, this,
                     &P4ViewDlg::btn_V1_toggled);
    QObject::connect(btn_V2_, &QRadioButton::toggled, this,
                     &P4ViewDlg::btn_V2_toggled);
    QObject::connect(btn_square_, &QPushButton::clicked, this,
                     &P4ViewDlg::btn_square_clicked);
    QObject::connect(edt_projection_, &QLineEdit::textChanged, this,
                     &P4ViewDlg::onFieldChange);
    QObject::connect(edt_x0_, &QLineEdit::textChanged, this,
                     &P4ViewDlg::onFieldChange);
    QObject::connect(edt_x1_, &QLineEdit::textChanged, this,
                     &P4ViewDlg::onFieldChange);
    QObject::connect(edt_y0_, &QLineEdit::textChanged, this,
                     &P4ViewDlg::onFieldChange);
    QObject::connect(edt_y1_, &QLineEdit::textChanged, this,
                     &P4ViewDlg::onFieldChange);
    if (haveVirtualCheckBox_)
        QObject::connect(chk_plotvirtuals_, &QCheckBox::stateChanged, this,
                         &P4ViewDlg::plotvirtuals_stateChanged);

    // finishing

    updateDlgData();

    setP4WindowTitle(this, "View Parameters");
}

void P4ViewDlg::onFieldChange() { changed_ = true; }

void P4ViewDlg::btn_sphere_toggled()
{
    changed_ = true;

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        edt_projection_->setEnabled(false);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        edt_projection_->setEnabled(true);
        btn_square_->setEnabled(false);
    }
}

void P4ViewDlg::btn_plane_toggled()
{
    changed_ = true;

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        edt_projection_->setEnabled(false);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        edt_projection_->setEnabled(true);
        btn_square_->setEnabled(false);
    }
}

void P4ViewDlg::btn_U1_toggled()
{
    changed_ = true;

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        edt_projection_->setEnabled(false);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        edt_projection_->setEnabled(true);
        btn_square_->setEnabled(false);
    }
}

void P4ViewDlg::btn_U2_toggled()
{
    changed_ = true;

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        edt_projection_->setEnabled(false);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        edt_projection_->setEnabled(true);
        btn_square_->setEnabled(false);
    }
}

void P4ViewDlg::btn_V1_toggled()
{
    changed_ = true;

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        edt_projection_->setEnabled(false);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        edt_projection_->setEnabled(true);
        btn_square_->setEnabled(false);
    }
}

void P4ViewDlg::btn_V2_toggled()
{
    changed_ = true;

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        edt_projection_->setEnabled(false);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        edt_projection_->setEnabled(true);
        btn_square_->setEnabled(false);
    }
}

void P4ViewDlg::btn_square_clicked()
{
    double x0;

    if (readFloatField(edt_x0_, x0, X_MIN, MIN_FLOAT, MAX_FLOAT) == false) {
        // no error reading field

        if (x0 > 0)
            x0 = -x0;

        QString buf;
        buf.sprintf("%g", x0);
        edt_x0_->setText(buf);
        buf.sprintf("%g", -x0);
        edt_x1_->setText(buf);
        buf.sprintf("%g", x0);
        edt_y0_->setText(buf);
        buf.sprintf("%g", -x0);
        edt_y1_->setText(buf);
    }
}

bool P4ViewDlg::readFloatField(QLineEdit *edt, double &presult, double defvalue,
                               double minvalue, double maxvalue)
{
    // returns true in case of error

    QString t{edt->text()};
    bool ok;
    presult = t.toDouble(&ok);
    if (!ok || presult < minvalue || presult > maxvalue) {
        markBad(edt);
        presult = defvalue;
        return true;
    }

    QString x;
    x.sprintf("%g", presult);
    if (x != t)
        edt->setText(x);

    return false;
}

void P4ViewDlg::markBad(QLineEdit *edt)
{
    QString t{edt->text()};
    int i;
    while ((i = t.indexOf('?')) >= 0)
        t.remove(i, 1);
    t = t.trimmed();
    t.append(" ???");

    edt->setText(t);
}

bool P4ViewDlg::getDataFromDlg()
{
    if (!changed_) {
        return false;
    }

    changed_ = false;
    if (btn_sphere_->isChecked()) {
        if (gVFResults.typeofview_ != TYPEOFVIEW_SPHERE) {
            changed_ = true;
            gVFResults.typeofview_ = TYPEOFVIEW_SPHERE;
        }
    }
    if (btn_plane_->isChecked()) {
        if (gVFResults.typeofview_ != TYPEOFVIEW_PLANE) {
            changed_ = true;
            gVFResults.typeofview_ = TYPEOFVIEW_PLANE;
        }
    }
    if (btn_U1_->isChecked()) {
        if (gVFResults.typeofview_ != TYPEOFVIEW_U1) {
            changed_ = true;
            gVFResults.typeofview_ = TYPEOFVIEW_U1;
        }
    }
    if (btn_U2_->isChecked()) {
        if (gVFResults.typeofview_ != TYPEOFVIEW_U2) {
            changed_ = true;
            gVFResults.typeofview_ = TYPEOFVIEW_U2;
        }
    }
    if (btn_V1_->isChecked()) {
        if (gVFResults.typeofview_ != TYPEOFVIEW_V1) {
            changed_ = true;
            gVFResults.typeofview_ = TYPEOFVIEW_V1;
        }
    }
    if (btn_V2_->isChecked()) {
        if (gVFResults.typeofview_ != TYPEOFVIEW_V2) {
            changed_ = true;
            gVFResults.typeofview_ = TYPEOFVIEW_V2;
        }
    }

    double oldxmin{gVFResults.xmin_};
    double oldymin{gVFResults.ymin_};
    double oldxmax{gVFResults.xmax_};
    double oldymax{gVFResults.ymax_};

    changed_ |=
        readFloatField(edt_projection_, gVFResults.config_projection_,
                       DEFAULT_PROJECTION, MIN_PROJECTION, MAX_PROJECTION);
    changed_ |=
        readFloatField(edt_x0_, gVFResults.xmin_, X_MIN, MIN_FLOAT, MAX_FLOAT);
    changed_ |=
        readFloatField(edt_y0_, gVFResults.ymin_, Y_MIN, MIN_FLOAT, MAX_FLOAT);
    changed_ |=
        readFloatField(edt_x1_, gVFResults.xmax_, X_MAX, MIN_FLOAT, MAX_FLOAT);
    changed_ |=
        readFloatField(edt_y1_, gVFResults.ymax_, Y_MAX, MIN_FLOAT, MAX_FLOAT);

    if (haveVirtualCheckBox_) {
        if (chk_plotvirtuals_->checkState() == Qt::Unchecked) {
            if (gVFResults.plotVirtualSingularities_) {
                gVFResults.plotVirtualSingularities_ = false;
                changed_ = true;
            }
        } else {
            if (!gVFResults.plotVirtualSingularities_) {
                gVFResults.plotVirtualSingularities_ = true;
                changed_ = true;
            }
        }
    }

    if (oldxmin != gVFResults.xmin_ || oldymin != gVFResults.ymin_ ||
        oldxmax != gVFResults.xmax_ || oldymax != gVFResults.ymax_ ||
        changed_) {
        changed_ = false;
        return true;
    }

    return false;
}

void P4ViewDlg::updateDlgData()
{
    QString buf;

    changed_ = false;

    if (gVFResults.typeofview_ == TYPEOFVIEW_PLANE)
        btn_plane_->toggle();
    if (gVFResults.typeofview_ == TYPEOFVIEW_SPHERE)
        btn_sphere_->toggle();
    if (gVFResults.typeofview_ == TYPEOFVIEW_U1)
        btn_U1_->toggle();
    if (gVFResults.typeofview_ == TYPEOFVIEW_U2)
        btn_U2_->toggle();
    if (gVFResults.typeofview_ == TYPEOFVIEW_V1)
        btn_V1_->toggle();
    if (gVFResults.typeofview_ == TYPEOFVIEW_V2)
        btn_V2_->toggle();

    if (gVFResults.typeofstudy_ == TYPEOFSTUDY_ONE)
        btn_sphere_->setEnabled(false);
    else
        btn_sphere_->setEnabled(true);

    buf.sprintf("%g", gVFResults.config_projection_);
    edt_projection_->setText(buf);
    if (gVFResults.typeofview_ == TYPEOFVIEW_SPHERE && !gVFResults.plweights_)
        edt_projection_->setEnabled(true);
    else
        edt_projection_->setEnabled(false);

    buf.sprintf("%g", gVFResults.xmin_);
    edt_x0_->setText(buf);
    buf.sprintf("%g", gVFResults.xmax_);
    edt_x1_->setText(buf);
    buf.sprintf("%g", gVFResults.ymin_);
    edt_y0_->setText(buf);
    buf.sprintf("%g", gVFResults.ymax_);
    edt_y1_->setText(buf);

    if (btn_sphere_->isChecked() == false) {
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);
        edt_x1_->setEnabled(true);
        edt_y1_->setEnabled(true);
        btn_square_->setEnabled(true);
    } else {
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);
        edt_x1_->setEnabled(false);
        edt_y1_->setEnabled(false);
        btn_square_->setEnabled(false);
    }

    if (haveVirtualCheckBox_)
        chk_plotvirtuals_->setChecked(gVFResults.plotVirtualSingularities_);
}

void P4ViewDlg::plotvirtuals_stateChanged(int state)
{
    if (haveVirtualCheckBox_) {
        if (state == Qt::Unchecked && gVFResults.plotVirtualSingularities_) {
            gVFResults.plotVirtualSingularities_ = false;
            changed_ = true;
            return;
        }
        if (state == Qt::Checked && !gVFResults.plotVirtualSingularities_) {
            gVFResults.plotVirtualSingularities_ = true;
            changed_ = true;
            return;
        }
    }
}
