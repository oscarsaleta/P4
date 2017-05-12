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

#include "win_gcf.h"

#include "custom.h"
#include "file_vf.h"
#include "math_gcf.h"

#include <QMessageBox>

QGcfDlg::QGcfDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere_ = sp;
    plotwnd_ = plt;

    btn_dots_ = new QRadioButton("Dots", this);
    btn_dashes_ = new QRadioButton("Dashes", this);

    QLabel *lbl1 = new QLabel("Appearance: ", this);

    edt_points_ = new QLineEdit("", this);
    QLabel *lbl2 = new QLabel("#Points: ", this);

    edt_precis_ = new QLineEdit("", this);
    QLabel *lbl3 = new QLabel("Precision: ", this);

    edt_memory_ = new QLineEdit("", this);
    QLabel *lbl4 = new QLabel("Max. Memory: ", this);

    btn_evaluate_ = new QPushButton("&Evaluate", this);

#ifdef TOOLTIPS
    btn_dots_->setToolTip(
        "Plot individual points of the curve of singularities");
    btn_dashes_->setToolTip("Connect points of the curve of singularities with "
                           "small line segments");
    edt_points_->setToolTip("Number of points");
    edt_precis_->setToolTip("Required precision");
    edt_memory_->setToolTip(
        "Maximum amount of memory (in kilobytes) spent on plotting GCF");
    btn_evaluate_->setToolTip("Start evaluation (using symbolic manipulator)");
#endif

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(lbl1);
    layout1->addWidget(btn_dots_);
    layout1->addWidget(btn_dashes_);

    QGridLayout *lay00 = new QGridLayout();
    lay00->addWidget(lbl2, 0, 0);
    lay00->addWidget(edt_points_, 0, 1);
    lay00->addWidget(lbl3, 1, 0);
    lay00->addWidget(edt_precis_, 1, 1);
    lay00->addWidget(lbl4, 2, 0);
    lay00->addWidget(edt_memory_, 2, 1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch(0);
    layout2->addWidget(btn_evaluate_);
    layout2->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(lay00);
    mainLayout_->addLayout(layout2);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections

    QObject::connect(btn_evaluate_, SIGNAL(clicked()), this,
                     SLOT(onbtn_evaluate()));
    QObject::connect(btn_dots_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dots_toggled(bool)));
    QObject::connect(btn_dashes_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dashes_toggled(bool)));

    // finishing

    setP4WindowTitle(this, "GCF Plot");
}

void QGcfDlg::reset(void)
{
    QString buf;

    buf.sprintf("%d", DEFAULT_GCFPOINTS);
    edt_points_->setText(buf);

    buf.sprintf("%d", DEFAULT_GCFPRECIS);
    edt_precis_->setText(buf);

    buf.sprintf("%d", DEFAULT_GCFMEMORY);
    edt_memory_->setText(buf);

    if (VFResults.config_dashes_)
        exclusiveToggle(true, btn_dashes_, btn_dots_);
    else
        exclusiveToggle(true, btn_dots_, btn_dashes_);
}

void QGcfDlg::btn_dots_toggled(bool on)
{
    exclusiveToggle(on, btn_dots_, btn_dashes_);
}

void QGcfDlg::btn_dashes_toggled(bool on)
{
    exclusiveToggle(on, btn_dashes_, btn_dots_);
}

void QGcfDlg::exclusiveToggle(bool on, QRadioButton *first,
                              QRadioButton *second)
{
    if (on) {
        if (first->isChecked() == false)
            first->toggle();

        if (second->isChecked() == true)
            second->toggle();
    } else {
        if (second->isChecked() == false)
            first->toggle();
    }
}

void QGcfDlg::onbtn_evaluate(void)
{
    bool dashes, result;
    int points, precis, memory;

    bool ok;
    QString buf;

    dashes = btn_dashes_->isChecked();

    ok = true;

    buf = edt_points_->text();
    points = buf.toInt();

    if (points < MIN_GCFPOINTS || points > MAX_GCFPOINTS) {
        buf += " ???";
        edt_points_->setText(buf);
        ok = false;
    }

    buf = edt_precis_->text();
    precis = buf.toInt();
    if (precis < MIN_GCFPRECIS || precis > MAX_GCFPRECIS) {
        buf += " ???";
        edt_precis_->setText(buf);
        ok = false;
    }

    buf = edt_memory_->text();
    memory = buf.toInt();
    if (memory < MIN_GCFMEMORY || memory > MAX_GCFMEMORY) {
        buf += " ???";
        edt_memory_->setText(buf);
        ok = false;
    }

    if (!ok) {
        QMessageBox::information(
            this, "P4", "One of the fields has a value that is out of bounds.\n"
                        "Please correct before continuing.\n");

        return;
    }

    // Evaluate GCF with given parameters {dashes, points, precis, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btn_evaluate_->setEnabled(false);

    ThisVF->gcfDlg_ = this;
    result = evalGcfStart(mainSphere_, dashes, points, precis);
    if (!result) {
        btn_evaluate_->setEnabled(true);
        QMessageBox::critical(this, "P4", "An error occured while plotting the "
                                          "GCF.\nThe singular locus may not be "
                                          "visible, or may "
                                          "be partially visible.");
    }
}

void QGcfDlg::finishGcfEvaluation(void)
{
    bool result;

    if (btn_evaluate_->isEnabled() == true)
        return; // not busy??

    result = evalGcfContinue(evaluating_points_, evaluating_precision_);

    if (result) {
        btn_evaluate_->setEnabled(true);
        result = evalGcfFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4", "An error occured while plotting "
                                              "the GCF.\nThe singular locus "
                                              "may not be visible, or may "
                                              "be partially visible.");
        }
    }
}
