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

#include "win_view.h"

#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "main.h"
#include "math_p4.h"
#include "p4application.h"

#include <QButtonGroup>

QViewDlg::~QViewDlg() { getDataFromDlg(); }

QViewDlg::QViewDlg(QWidget *parent)
    : QWidget(parent = nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    QLabel *p4title = new QLabel("View Parameters", this);
    p4title->setFont(*(g_p4app->titleFont_));

    QLabel *kindlabel = new QLabel("Type of view: ", this);
    kindlabel->setFont(*(g_p4app->boldFont_));
    QButtonGroup *btngrp = new QButtonGroup(this);
    btn_sphere_ = new QRadioButton("Spherical", this);
    btn_plane_ = new QRadioButton("Planar", this);
    btn_U1_ = new QRadioButton("U1", this);
    btn_U2_ = new QRadioButton("U2", this);
    btn_V1_ = new QRadioButton("V1", this);
    btn_V2_ = new QRadioButton("V2", this);
    btngrp->addButton(btn_sphere_);
    btngrp->addButton(btn_plane_);
    btngrp->addButton(btn_U1_);
    btngrp->addButton(btn_U2_);
    btngrp->addButton(btn_V1_);
    btngrp->addButton(btn_V2_);

    QLabel *lbl_projection_ = new QLabel("Projection:", this);
    lbl_projection_->setFont(*(g_p4app->boldFont_));
    edt_projection_ = new QLineEdit("-1", this);

    QLabel *lbl_x0_ = new QLabel("Min. x:", this);
    lbl_x0_->setFont(*(g_p4app->boldFont_));
    edt_x0_ = new QLineEdit("-1", this);

    btn_square_ = new QPushButton("&Square", this);

    QLabel *lbl_y0_ = new QLabel("Min. y:", this);
    lbl_y0_->setFont(*(g_p4app->boldFont_));
    edt_y0_ = new QLineEdit("-1", this);

    QLabel *lbl_x1_ = new QLabel("Max. x", this);
    lbl_x1_->setFont(*(g_p4app->boldFont_));
    edt_x1_ = new QLineEdit("1", this);

    QLabel *lbl_y1_ = new QLabel("Max. y", this);
    lbl_y1_->setFont(*(g_p4app->boldFont_));
    edt_y1_ = new QLineEdit("1", this);

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
#endif

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(p4title);

    QGridLayout *kindLayout = new QGridLayout();
    kindLayout->addWidget(kindlabel, 0, 0);
    kindLayout->addWidget(btn_sphere_, 0, 1);
    kindLayout->addWidget(btn_plane_, 0, 2);
    kindLayout->addWidget(btn_U1_, 1, 1);
    kindLayout->addWidget(btn_U2_, 1, 2);
    kindLayout->addWidget(btn_V1_, 2, 1);
    kindLayout->addWidget(btn_V2_, 2, 2);

    mainLayout_->addLayout(kindLayout);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(lbl_projection_);
    layout1->addWidget(edt_projection_);
    layout1->addStretch(0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(lbl_x0_);
    layout2->addWidget(edt_x0_);
    layout2->addWidget(btn_square_);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl_y0_);
    layout3->addWidget(edt_y0_);
    layout3->addStretch(0);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(lbl_x1_);
    layout4->addWidget(edt_x1_);
    layout4->addStretch(0);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(lbl_y1_);
    layout5->addWidget(edt_y1_);
    layout5->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);
    mainLayout_->addStretch(0);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections

    connect(btn_sphere_, &QRadioButton::toggled, this,
            &QViewDlg::btn_sphere_toggled);
    connect(btn_plane_, &QRadioButton::toggled, this,
            &QViewDlg::btn_plane_toggled);
    connect(btn_U1_, &QRadioButton::toggled, this, &QViewDlg::btn_U1_toggled);
    connect(btn_U2_, &QRadioButton::toggled, this, &QViewDlg::btn_U2_toggled);
    connect(btn_V1_, &QRadioButton::toggled, this, &QViewDlg::btn_V1_toggled);
    connect(btn_V2_, &QRadioButton::toggled, this, &QViewDlg::btn_V2_toggled);
    connect(btn_square_, &QPushButton::clicked, this,
            &QViewDlg::btn_square_clicked);
    connect(edt_projection_, &QLineEdit::textChanged, this,
            &QViewDlg::onFieldChange);
    connect(edt_x0_, &QLineEdit::textChanged, this, &QViewDlg::onFieldChange);
    connect(edt_x1_, &QLineEdit::textChanged, this, &QViewDlg::onFieldChange);
    connect(edt_y0_, &QLineEdit::textChanged, this, &QViewDlg::onFieldChange);
    connect(edt_y1_, &QLineEdit::textChanged, this, &QViewDlg::onFieldChange);

    // finishing

    updateDlgData();

    setP4WindowTitle(this, "View Parameters");
}

void QViewDlg::onFieldChange(const QString &dummy)
{
    UNUSED(dummy);
    changed_ = true;
}

void QViewDlg::btn_sphere_toggled()
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

void QViewDlg::btn_plane_toggled()
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

void QViewDlg::btn_U1_toggled()
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

void QViewDlg::btn_U2_toggled()
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

void QViewDlg::btn_V1_toggled()
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

void QViewDlg::btn_V2_toggled()
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

void QViewDlg::btn_square_clicked(void)
{
    double x0;

    if (readFloatField(edt_x0_, &x0, X_MIN, MIN_FLOAT, MAX_FLOAT) == false) {
        // no error reading field

        if (x0 > 0)
            x0 = -x0;

        QString buf;
        buf.sprintf("%g", (float)(x0));
        edt_x0_->setText(buf);
        buf.sprintf("%g", (float)(-x0));
        edt_x1_->setText(buf);
        buf.sprintf("%g", (float)(x0));
        edt_y0_->setText(buf);
        buf.sprintf("%g", (float)(-x0));
        edt_y1_->setText(buf);
    }
}

bool QViewDlg::readFloatField(QLineEdit *edt, double *presult, double defvalue,
                              double minvalue, double maxvalue)
{
    // returns true in case of error

    QString x, t;
    bool ok;

    t = edt->text();
    *presult = t.toDouble(&ok);
    if (!ok || *presult < minvalue || *presult > maxvalue) {
        markBad(edt);
        *presult = defvalue;
        return true;
    }

    x.sprintf("%g", (float)(*presult));
    if (x != t)
        edt->setText(x);

    return false;
}

void QViewDlg::markBad(QLineEdit *edt)
{
    QString t;
    int i;

    t = edt->text();
    while ((i = t.indexOf('?')) >= 0)
        t.remove(i, 1);
    t = t.trimmed();
    t.append(" ???");

    edt->setText(t);
}

bool QViewDlg::getDataFromDlg(void)
{
    if (!changed_) {
        return false;
    }

    changed_ = false;
    if (btn_sphere_->isChecked()) {
        if (g_VFResults.typeofview_ != TYPEOFVIEW_SPHERE) {
            changed_ = true;
            g_VFResults.typeofview_ = TYPEOFVIEW_SPHERE;
        }
    }
    if (btn_plane_->isChecked()) {
        if (g_VFResults.typeofview_ != TYPEOFVIEW_PLANE) {
            changed_ = true;
            g_VFResults.typeofview_ = TYPEOFVIEW_PLANE;
        }
    }
    if (btn_U1_->isChecked()) {
        if (g_VFResults.typeofview_ != TYPEOFVIEW_U1) {
            changed_ = true;
            g_VFResults.typeofview_ = TYPEOFVIEW_U1;
        }
    }
    if (btn_U2_->isChecked()) {
        if (g_VFResults.typeofview_ != TYPEOFVIEW_U2) {
            changed_ = true;
            g_VFResults.typeofview_ = TYPEOFVIEW_U2;
        }
    }
    if (btn_V1_->isChecked()) {
        if (g_VFResults.typeofview_ != TYPEOFVIEW_V1) {
            changed_ = true;
            g_VFResults.typeofview_ = TYPEOFVIEW_V1;
        }
    }
    if (btn_V2_->isChecked()) {
        if (g_VFResults.typeofview_ != TYPEOFVIEW_V2) {
            changed_ = true;
            g_VFResults.typeofview_ = TYPEOFVIEW_V2;
        }
    }

    double oldxmin = g_VFResults.xmin_;
    double oldymin = g_VFResults.ymin_;
    double oldxmax = g_VFResults.xmax_;
    double oldymax = g_VFResults.ymax_;

    changed_ |=
        readFloatField(edt_projection_, &(g_VFResults.config_projection_),
                       DEFAULT_PROJECTION, MIN_PROJECTION, MAX_PROJECTION);
    changed_ |= readFloatField(edt_x0_, &(g_VFResults.xmin_), X_MIN, MIN_FLOAT,
                               MAX_FLOAT);
    changed_ |= readFloatField(edt_y0_, &(g_VFResults.ymin_), Y_MIN, MIN_FLOAT,
                               MAX_FLOAT);
    changed_ |= readFloatField(edt_x1_, &(g_VFResults.xmax_), X_MAX, MIN_FLOAT,
                               MAX_FLOAT);
    changed_ |= readFloatField(edt_y1_, &(g_VFResults.ymax_), Y_MAX, MIN_FLOAT,
                               MAX_FLOAT);

    if (oldxmin != g_VFResults.xmin_ || oldymin != g_VFResults.ymin_ ||
        oldxmax != g_VFResults.xmax_ || oldymax != g_VFResults.ymax_ ||
        changed_) {
        changed_ = false;
        return true;
    }

    return false;
}

void QViewDlg::updateDlgData(void)
{
    QString buf;

    changed_ = false;

    if (g_VFResults.typeofview_ == TYPEOFVIEW_PLANE)
        btn_plane_->toggle();
    if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE)
        btn_sphere_->toggle();
    if (g_VFResults.typeofview_ == TYPEOFVIEW_U1)
        btn_U1_->toggle();
    if (g_VFResults.typeofview_ == TYPEOFVIEW_U2)
        btn_U2_->toggle();
    if (g_VFResults.typeofview_ == TYPEOFVIEW_V1)
        btn_V1_->toggle();
    if (g_VFResults.typeofview_ == TYPEOFVIEW_V2)
        btn_V2_->toggle();

    if (g_VFResults.typeofstudy_ == TYPEOFSTUDY_ONE)
        btn_sphere_->setEnabled(false);
    else
        btn_sphere_->setEnabled(true);

    buf.sprintf("%g", (float)(g_VFResults.config_projection_));
    edt_projection_->setText(buf);
    if (g_VFResults.typeofview_ == TYPEOFVIEW_SPHERE && !g_VFResults.plweights_)
        edt_projection_->setEnabled(true);
    else
        edt_projection_->setEnabled(false);

    buf.sprintf("%g", (float)(g_VFResults.xmin_));
    edt_x0_->setText(buf);
    buf.sprintf("%g", (float)(g_VFResults.xmax_));
    edt_x1_->setText(buf);
    buf.sprintf("%g", (float)(g_VFResults.ymin_));
    edt_y0_->setText(buf);
    buf.sprintf("%g", (float)(g_VFResults.ymax_));
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
}
