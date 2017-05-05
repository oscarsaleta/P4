/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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

#include "win_curve.h"

#include "custom.h"
#include "file_vf.h"
#include "math_curve.h"

#include <QMessageBox>

QCurveDlg::QCurveDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere = sp;
    plotwnd = plt;

    edt_curve = new QLineEdit("", this);
    QLabel *lbl0 = new QLabel("Curve: ", this);

    btn_dots = new QRadioButton("Dots", this);
    btn_dashes = new QRadioButton("Dashes", this);

    QLabel *lbl1 = new QLabel("Appearance: ", this);

    edt_points = new QLineEdit("", this);
    QLabel *lbl2 = new QLabel("#Points: ", this);

    edt_precis = new QLineEdit("", this);
    QLabel *lbl3 = new QLabel("Precision: ", this);

    edt_memory = new QLineEdit("", this);
    QLabel *lbl4 = new QLabel("Max. Memory: ", this);

    btn_evaluate = new QPushButton("&Evaluate", this);
    btn_plot = new QPushButton("&Plot", this);
    btn_plot->setEnabled(false);

#ifdef TOOLTIPS
    btn_dots->setToolTip(
        "Plot individual points of the curve of singularities");
    btn_dashes->setToolTip("Connect points of the curve of singularities with "
                           "small line segments");
    edt_points->setToolTip("Number of points");
    edt_precis->setToolTip("Required precision");
    edt_memory->setToolTip(
        "Maximum amount of memory (in kilobytes) spent on plotting the curve");
    btn_evaluate->setToolTip("Evaluate singular points of plynomial curve");
    btn_plot->setToolTip("Plot curve (using symbolic manipulator)");
#endif

    // layout

    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(lbl1);
    layout1->addWidget(btn_dots);
    layout1->addWidget(btn_dashes);

    QGridLayout *lay00 = new QGridLayout();
    lay00->addWidget(lbl0, 0, 0);
    lay00->addWidget(edt_curve, 0, 1);
    lay00->addWidget(lbl2, 1, 0);
    lay00->addWidget(edt_points, 1, 1);
    lay00->addWidget(lbl3, 2, 0);
    lay00->addWidget(edt_precis, 2, 1);
    lay00->addWidget(lbl4, 3, 0);
    lay00->addWidget(edt_memory, 3, 1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch(0);
    layout2->addWidget(btn_evaluate);
    layout2->addWidget(btn_plot);
    layout2->addStretch(0);

    mainLayout->addLayout(layout1);
    mainLayout->addLayout(lay00);
    mainLayout->addLayout(layout2);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    // connections

    QObject::connect(btn_evaluate, SIGNAL(clicked()), this,
                     SLOT(onbtn_evaluate()));
    QObject::connect(btn_plot, SIGNAL(clicked()), this, SLOT(onbtn_plot()));
    QObject::connect(btn_dots, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dots_toggled(bool)));
    QObject::connect(btn_dashes, SIGNAL(toggled(bool)), this,
                     SLOT(btn_dashes_toggled(bool)));

    // finishing

    setP4WindowTitle(this, "Curve plot");
}

void QCurveDlg::Reset(void)
{
    QString buf;

    edt_curve->setText("");

    buf.sprintf("%d", DEFAULT_CURVEPOINTS);
    edt_points->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEPRECIS);
    edt_precis->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEMEMORY);
    edt_memory->setText(buf);

    if (VFResults.config_dashes)
        ExclusiveToggle(true, btn_dashes, btn_dots);
    else
        ExclusiveToggle(true, btn_dots, btn_dashes);
}

void QCurveDlg::btn_dots_toggled(bool on)
{
    ExclusiveToggle(on, btn_dots, btn_dashes);
}

void QCurveDlg::btn_dashes_toggled(bool on)
{
    ExclusiveToggle(on, btn_dashes, btn_dots);
}

void QCurveDlg::ExclusiveToggle(bool on, QRadioButton *first,
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

void QCurveDlg::onbtn_evaluate(void)
{
    if (edt_curve->text().isNull() || edt_curve->text().isEmpty()) {
        QMessageBox::information(this, "P4",
                                 "The curve field has to be filled\n"
                                 "with the equation of a curve.\n");
        return;
    }
    ThisVF->curve = edt_curve->text().trimmed();

    // FIRST: create filename_veccurve.tab for transforming the curve QString to
    // a list of P4POLYNOM2
    ThisVF->evaluateCurveTable();
    btn_plot->setEnabled(true);
}

void QCurveDlg::onbtn_plot(void)
{
    bool dashes, result;
    int points, precis, memory;

    bool ok;
    QString buf;

    dashes = btn_dashes->isChecked();

    ok = true;

    buf = edt_points->text();
    points = buf.toInt();

    if (points < MIN_CURVEPOINTS || points > MAX_CURVEPOINTS) {
        buf += " ???";
        edt_points->setText(buf);
        ok = false;
    }

    buf = edt_precis->text();
    precis = buf.toInt();
    if (precis < MIN_CURVEPRECIS || precis > MAX_CURVEPRECIS) {
        buf += " ???";
        edt_precis->setText(buf);
        ok = false;
    }

    buf = edt_memory->text();
    memory = buf.toInt();
    if (memory < MIN_CURVEMEMORY || memory > MAX_CURVEMEMORY) {
        buf += " ???";
        edt_memory->setText(buf);
        ok = false;
    }

    if (!ok) {
        QMessageBox::information(
            this, "P4", "One of the fields has a value that is out of bounds.\n"
                        "Please correct before continuing.\n");
        return;
    }

    // SECOND: read the resulting file and store the list
    if (!VFResults.readCurve(ThisVF->getbarefilename())) {
        QMessageBox::critical(this, "P4", "Cannot read curve.\n"
                                          "Please check the input field!\n");
        return;
    }

    // THIRD: evaluate curve with given parameters {dashes, points, precis,
    // memory}.

    evaluating_points = points;
    evaluating_memory = memory;
    evaluating_precision = precis;

    btn_plot->setEnabled(false);

    ThisVF->curveDlg = this;
    result = evalCurveStart(mainSphere, dashes, points, precis);
    if (!result) {
        btn_plot->setEnabled(true);
        QMessageBox::critical(this, "P4", "An error occured while plotting the "
                                          "curve.\nThe singular locus may not "
                                          "be visible, or may be partially "
                                          "visible.");
        return;
    }
}

void QCurveDlg::finishCurveEvaluation(void)
{
    bool result;

    if (btn_plot->isEnabled() == true)
        return; // not busy??

    result = evalCurveContinue(evaluating_points, evaluating_precision);

    if (result) {
        btn_plot->setEnabled(false);
        result = evalCurveFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4", "An error occured while plotting "
                                              "the curve.\nThe singular locus "
                                              "may not be visible, or may "
                                              "be partially visible.");
        }
    }
}
