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

#include "win_vf.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "win_find.h"

QVectorFieldDlg::QVectorFieldDlg(QFindDlg *finddlg)
#ifdef DOCK_VFWINDOW
    : QWidget(finddlg)
#else
    : QWidget()
#endif
{
    parent = finddlg;
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

#ifdef DOCK_VFWINDOW
    QLabel *p4title = new QLabel("Specify the vector field:", this);
    p4title->setFont(*(p4app->TitleFont));
#endif

    edt_xprime = new QLineEdit(ThisVF->xdot, this);
    QLabel *xlabel = new QLabel("&x' = ", this);
    xlabel->setFont(*(p4app->BoldFont));
    xlabel->setBuddy(edt_xprime);

    edt_yprime = new QLineEdit(ThisVF->ydot, this);
    QLabel *ylabel = new QLabel("&y' = ", this);
    ylabel->setFont(*(p4app->BoldFont));
    ylabel->setBuddy(edt_yprime);

    edt_gcf = new QLineEdit(ThisVF->gcf, this);
    QLabel *glabel = new QLabel("&Gcf: ", this);
    glabel->setFont(*(p4app->BoldFont));
    glabel->setBuddy(edt_gcf);

    QLabel *plabel = new QLabel("Number of Parameters: ", this);
    plabel->setFont(*(p4app->BoldFont));

    spin_numparams = new QSpinBox(this);
    spin_numparams->setMinimum(0);
    spin_numparams->setMaximum(MAXNUMPARAMS);

#ifdef TOOLTIPS
    edt_xprime->setToolTip("Enter your differential equations here.\n"
                           "Use syntax conform to the symbolic manipulator.");
    edt_yprime->setToolTip("Enter your differential equations here.\n"
                           "Use syntax conform to the symbolic manipulator.");
    edt_gcf->setToolTip("Enter greatest common factor here.\n"
                        "Enter \"1\" if you are sure there is no GCF\n"
                        "Enter \"0\" if you want P4 to search for a GCF");
    spin_numparams->setToolTip("If your vector field contains parameters,\n"
                               "specify the number of parameters here.");
#endif
    // layout

    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

#ifdef DOCK_VFWINDOW
    mainLayout->addWidget(p4title);
#endif

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(xlabel);
    layout1->addWidget(edt_xprime);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(ylabel);
    layout2->addWidget(edt_yprime);

    QHBoxLayout *layout22 = new QHBoxLayout();
    layout22->addWidget(glabel);
    layout22->addWidget(edt_gcf);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(plabel);
    layout3->addWidget(spin_numparams);
    layout3->addStretch(0);

    paramLayout = new QHBoxLayout();

    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout22);
    mainLayout->addLayout(layout3);
    mainLayout->addLayout(paramLayout);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    sb_params = nullptr;
    params = nullptr;

    if (ThisVF->numparams != 0) {
        if (ThisVF->numparams > MAXNUMPARAMSSHOWN) {
            sb_params = new QScrollBar(Qt::Vertical, this);
            sb_params->setRange(0, ThisVF->numparams - MAXNUMPARAMSSHOWN);
            sb_params->setSingleStep(1);
            sb_params->setPageStep(MAXNUMPARAMSSHOWN);
        } else
            sb_params = nullptr;

        params = new QVFParams(this, sb_params);
        paramLayout->addWidget(params);
        if (ThisVF->numparams > MAXNUMPARAMSSHOWN) {
            paramLayout->addWidget(sb_params);
            QObject::connect(sb_params, SIGNAL(valueChanged(int)), params,
                             SLOT(paramsSliderChanged(int)));
            QObject::connect(sb_params, SIGNAL(sliderMoved(int)), params,
                             SLOT(paramsSliderChanged(int)));
        }
        params->show();
        if (ThisVF->numparams > MAXNUMPARAMSSHOWN)
            sb_params->show();

        spin_numparams->setValue(ThisVF->numparams);
    }

    // connections

    QObject::connect(spin_numparams, SIGNAL(valueChanged(int)), this,
                     SLOT(numParamsChanged(int)));

#ifndef DOCK_VFWINDOW
    SetP4WindowTitle(this, "Vector Field");
#endif
}

void QVectorFieldDlg::numParamsChanged(int val)
{
    if (val >= 0 && val <= MAXNUMPARAMS && val != ThisVF->numparams) {
        delete params;
        params = nullptr;

        if (sb_params != nullptr) {
            delete sb_params;
            sb_params = nullptr;
        }

        if (val != 0) {
            ThisVF->numparams = val;
            if (ThisVF->changed == false) {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
            if (val > MAXNUMPARAMSSHOWN) {
                sb_params = new QScrollBar(Qt::Vertical, this);
                sb_params->setRange(0, ThisVF->numparams - MAXNUMPARAMSSHOWN);
                sb_params->setSingleStep(1);
                sb_params->setPageStep(MAXNUMPARAMSSHOWN);
            } else
                sb_params = nullptr;

            params = new QVFParams(this, sb_params);
            paramLayout->addWidget(params);
            if (val > MAXNUMPARAMSSHOWN) {
                paramLayout->addWidget(sb_params);
                QObject::connect(sb_params, SIGNAL(valueChanged(int)), params,
                                 SLOT(paramsSliderChanged(int)));
                QObject::connect(sb_params, SIGNAL(sliderMoved(int)), params,
                                 SLOT(paramsSliderChanged(int)));
            }
            params->show();
            if (val > MAXNUMPARAMSSHOWN)
                sb_params->show();
        } else {
            if (ThisVF->changed == false) {
                ThisVF->changed = true;
                p4app->Signal_Changed();
            }
            ThisVF->numparams = val;
        }
    }
}

QVectorFieldDlg::~QVectorFieldDlg()
{
    if (params != nullptr) {
        delete params;
        params = nullptr;
    }
    GetDataFromDlg();
}

void QVectorFieldDlg::GetDataFromDlg(void)
{
    QString xdot;
    QString ydot;
    QString gcf;

    xdot = edt_xprime->text();
    ydot = edt_yprime->text();
    gcf = edt_gcf->text();

    xdot = xdot.trimmed();
    ydot = ydot.trimmed();
    gcf = gcf.trimmed();

    if (xdot.compare(ThisVF->xdot) || ydot.compare(ThisVF->ydot) ||
        gcf.compare(ThisVF->gcf)) {
        ThisVF->xdot = xdot;
        ThisVF->ydot = ydot;
        ThisVF->gcf = gcf;
        if (ThisVF->changed == false) {
            ThisVF->changed = true;
            p4app->Signal_Changed();
        }
    }
    if (params != nullptr) {
        params->GetDataFromDlg();
    }
}

void QVectorFieldDlg::UpdateDlgData(void)
{
    edt_xprime->setText(ThisVF->xdot);
    edt_yprime->setText(ThisVF->ydot);
    edt_gcf->setText(ThisVF->gcf);
    spin_numparams->setValue(ThisVF->numparams);

    if (params != nullptr) {
        if (!params->UpdateDlgData()) {
            delete params;
            params = nullptr;
        }
    }

    if (ThisVF->numparams != 0 && params == nullptr) {
        if (ThisVF->numparams > MAXNUMPARAMSSHOWN) {
            sb_params = new QScrollBar(Qt::Vertical, this);
            sb_params->setRange(0, ThisVF->numparams - MAXNUMPARAMSSHOWN);
            sb_params->setSingleStep(1);
            sb_params->setPageStep(MAXNUMPARAMSSHOWN);
        } else
            sb_params = nullptr;

        params = new QVFParams(this, sb_params);
        paramLayout->addWidget(params);
        if (ThisVF->numparams > MAXNUMPARAMSSHOWN) {
            paramLayout->addWidget(sb_params);
            QObject::connect(sb_params, SIGNAL(valueChanged(int)), params,
                             SLOT(paramsSliderChanged(int)));
            QObject::connect(sb_params, SIGNAL(sliderMoved(int)), params,
                             SLOT(paramsSliderChanged(int)));
        }
        params->show();
        if (ThisVF->numparams > MAXNUMPARAMSSHOWN)
            sb_params->show();
    }
}

QVFParams::QVFParams(QVectorFieldDlg *parent, QScrollBar *sb) : QWidget(parent)
{
    int i;

    QLabel *label0 = new QLabel("Enter values for all parameters");
    label0->setFont(*(p4app->TitleFont));

    datainvalid = false;
    sb_params = sb;
    currentnumparams = ThisVF->numparams;
    currentshownparams = (currentnumparams < MAXNUMPARAMSSHOWN)
                             ? currentnumparams
                             : MAXNUMPARAMSSHOWN;
    currentpageindex = 0;

    for (i = 0; i < currentshownparams; i++) {
        // parlabel and parvalue might (or not) be filled from the input file
        Param_Names[i] = new QLineEdit(ThisVF->parlabel[i], this);
        Param_Labels[i] = new QLabel(" = ", this);
        Param_Values[i] = new QLineEdit(ThisVF->parvalue[i], this);
#ifdef TOOLTIPS
        Param_Names[i]->setToolTip("Enter the name of the parameter");
        Param_Values[i]->setToolTip(
            "Enter the value of this parameter during this evaluation session");
#endif
    }

    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->addWidget(label0);
    for (i = 0; i < currentshownparams; i++) {
        Param_Lines[i] = new QHBoxLayout();
        Param_Lines[i]->addWidget(Param_Names[i]);
        Param_Lines[i]->addWidget(Param_Labels[i]);
        Param_Lines[i]->addWidget(Param_Values[i]);
        mainLayout->addLayout(Param_Lines[i]);
    }

    for (i = 0; i < currentnumparams; i++) {
        S_Labels[i] = ThisVF->parlabel[i];
        S_Values[i] = ThisVF->parvalue[i];
    }
    setLayout(mainLayout);
}

QVFParams::~QVFParams()
{
    if (!datainvalid)
        GetDataFromDlg();
}

bool QVFParams::focusNextPrevChild(bool next)
{
    QWidget *p;
    p = focusWidget();
    if (p == Param_Names[0] && currentpageindex != 0 && !next) {
        sb_params->setValue(currentpageindex - 1);
        return focusNextPrevChild(true);
    }
    if (p == Param_Values[currentshownparams - 1] &&
        currentpageindex < currentnumparams - MAXNUMPARAMSSHOWN && next) {
        sb_params->setValue(currentpageindex + 1);
        return focusNextPrevChild(false);
    }
    return QWidget::focusNextPrevChild(next);
}

bool QVFParams::GetDataFromDlg(void)
{
    int i;
    bool changed;
    QString temp;

    if (ThisVF == nullptr)
        return false;

    if (ThisVF->numparams != currentnumparams)
        return false;

    currentnumparams = ThisVF->numparams;

    changed = false;

    for (i = 0; i < currentshownparams; i++) {
        S_Labels[i + currentpageindex] = Param_Names[i]->text();
        S_Values[i + currentpageindex] = Param_Values[i]->text();
    }
    for (i = 0; i < currentnumparams; i++) {
        temp = S_Labels[i];
        temp = temp.trimmed();
        if (temp.compare(ThisVF->parlabel[i])) {
            changed = true;
            ThisVF->parlabel[i] = temp;
        }
        temp = S_Values[i];
        temp = temp.trimmed();
        if (temp.compare(ThisVF->parvalue[i])) {
            changed = true;
            ThisVF->parvalue[i] = temp;
        }
    }

    if (changed) {
        if (ThisVF->changed == false) {
            ThisVF->changed = true;
            p4app->Signal_Changed();
        }
    }
    return true;
}

// only called when vf is loaded, and only when numparams did not change.

bool QVFParams::UpdateDlgData(void)
{
    int i;

    if (ThisVF->numparams != currentnumparams) {
        datainvalid = true;
        return false;

        // when the number of parameters changed, signal it to the parent
        // window.  This parent window will close this window and re-open it.
        // In order to prevent data-overwriting when the window is closed,
        // set a flag to say that the current data in this object is no longer
        // valid.
    }

    for (i = 0; i < currentshownparams; i++) {
        Param_Names[i]->setText(ThisVF->parlabel[i + currentpageindex]);
        Param_Values[i]->setText(ThisVF->parvalue[i + currentpageindex]);
    }

    return true;
}

void QVFParams::paramsSliderChanged(int value)
{
    int i;
    int newindex;

    newindex = value;
    if (newindex < 0)
        newindex = 0;
    if (newindex > currentnumparams - MAXNUMPARAMSSHOWN)
        newindex = currentnumparams - MAXNUMPARAMSSHOWN;

    if (newindex != currentpageindex) {
        for (i = 0; i < currentshownparams; i++) {
            S_Labels[i + currentpageindex] = Param_Names[i]->text();
            S_Values[i + currentpageindex] = Param_Values[i]->text();
        }

        currentpageindex = newindex;

        for (i = 0; i < currentshownparams; i++) {
            Param_Names[i]->setText(S_Labels[i + currentpageindex]);
            Param_Values[i]->setText(S_Values[i + currentpageindex]);
        }
    }
}
