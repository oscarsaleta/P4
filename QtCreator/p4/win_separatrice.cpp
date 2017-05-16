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

#include "win_separatrice.h"

#include "custom.h"
#include "math_separatrice.h"

QString g_CurrentSingularityInfo[4] = {"", "", "", ""};
double g_CurrentSeparatriceEpsilon = 0;

QSepDlg::QSepDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere_ = sp;
    plotwnd_ = plt;

    btn_start_ = new QPushButton("&Start integrating sep", this);
    btn_cont_ = new QPushButton("&Cont integrating sep", this);
    btn_intnext_ = new QPushButton("&Integrate next sep", this);
    btn_selectnext_ = new QPushButton("Select &Next sep", this);

    edt_epsilon_ = new QLineEdit("", this);
    QLabel *lbl1 = new QLabel("&Epsilon = ", this);
    lbl1->setBuddy(edt_epsilon_);

    lbl_info_[0] = new QLabel("", this);
    lbl_info_[1] = new QLabel("", this);
    lbl_info_[2] = new QLabel("", this);
    lbl_info_[3] = new QLabel("", this);

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(lbl_info_[0]);
    mainLayout_->addWidget(lbl_info_[1]);
    mainLayout_->addWidget(lbl_info_[2]);
    mainLayout_->addWidget(lbl_info_[3]);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl1);
    layout3->addWidget(edt_epsilon_);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(btn_start_);
    layout4->addWidget(btn_cont_);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(btn_intnext_);
    layout5->addWidget(btn_selectnext_);

    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

#ifdef TOOLTIPS
    edt_epsilon_->setToolTip("Epsilon value for this separatrix\n"
                             "Confirm any change by pressing ENTER");
    btn_start_->setToolTip("Start integrating this separatrix");
    btn_cont_->setToolTip("Continue integrating this separatrix");
    btn_intnext_->setToolTip("Select next separatrix, and start integrating");
    btn_selectnext_->setToolTip(
        "Select next separatrix of this singular point");
#endif

    // connections

    QObject::connect(btn_selectnext_, SIGNAL(clicked()), this,
                     SLOT(onbtn_selectnext()));
    QObject::connect(btn_intnext_, SIGNAL(clicked()), this,
                     SLOT(onbtn_intnext()));
    QObject::connect(btn_start_, SIGNAL(clicked()), this, SLOT(onbtn_start()));
    QObject::connect(btn_cont_, SIGNAL(clicked()), this, SLOT(onbtn_cont()));
    QObject::connect(edt_epsilon_, SIGNAL(returnPressed()), this,
                     SLOT(onepsilon_enter()));

    // finishing

    reset();

    setP4WindowTitle(this, "Separatrices");
}

void QSepDlg::setInitialPoint(void)
{
    QString buf;

    plotwnd_->getDlgData();

    selected_ = true;
    started_ = false;
    btn_start_->setEnabled(true);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    lbl_info_[0]->setText(g_CurrentSingularityInfo[0]);
    lbl_info_[1]->setText(g_CurrentSingularityInfo[1]);
    lbl_info_[2]->setText(g_CurrentSingularityInfo[2]);
    lbl_info_[3]->setText(g_CurrentSingularityInfo[3]);

    buf.sprintf("%g", (float)g_CurrentSeparatriceEpsilon);

    edt_epsilon_->setText(buf);
    show();
    //  raise();
}

void QSepDlg::onbtn_selectnext(void)
{
    if (!selected_)
        return;

    plotwnd_->getDlgData();

    started_ = false;

    btn_start_->setEnabled(true);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*select_next_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::onbtn_intnext(void)
{
    if (!selected_)
        return;

    plotwnd_->getDlgData();

    started_ = true;

    btn_start_->setEnabled(false);
    btn_cont_->setEnabled(true);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*plot_next_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::onbtn_start(void)
{
    if (!selected_ || started_)
        return;

    plotwnd_->getDlgData();

    started_ = true;
    btn_start_->setEnabled(false);
    btn_cont_->setEnabled(true);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*start_plot_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::onbtn_cont(void)
{
    if (!selected_ || !started_)
        return;

    plotwnd_->getDlgData();

    mainSphere_->prepareDrawing();
    (*cont_plot_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::reset(void)
{
    lbl_info_[0]->setText("no point selected_.");
    lbl_info_[1]->setText("");
    lbl_info_[2]->setText("Use Shift+Left Button");
    lbl_info_[3]->setText("in the plot window to select.");

    started_ = false;
    selected_ = false;

    btn_start_->setEnabled(false);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(false);
    btn_intnext_->setEnabled(false);
}

void QSepDlg::sepEvent(int i)
{
    switch (i) {
    case -1:
        setInitialPoint();
        break;
    case 0:
        onbtn_cont();
        break;
    case 1:
        onbtn_start();
        break;
    case 2:
        onbtn_intnext();
        break;
    case 3:
        onbtn_selectnext();
        break;
    }
}

void QSepDlg::markBad(QLineEdit *edt)
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

void QSepDlg::onepsilon_enter(void)
{
    // called when user presses ENTER after changing the epsilon value

    QString s;
    double eps;
    bool ok;

    s = edt_epsilon_->text();
    eps = s.toDouble(&ok);
    if (!ok || eps <= 0) {
        markBad(edt_epsilon_);
        return;
    }

    g_CurrentSeparatriceEpsilon = eps;
    s.sprintf("%g", (float)eps);
    edt_epsilon_->setText(s);

    // pass on to math routines

    if (!selected_)
        return;

    started_ = false;

    btn_start_->setEnabled(true);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*change_epsilon)(mainSphere_, eps);
    mainSphere_->finishDrawing();
}
