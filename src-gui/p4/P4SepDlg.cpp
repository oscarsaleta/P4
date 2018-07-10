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

#include "P4SepDlg.hpp"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "P4ParentStudy.hpp"
#include "P4PlotWnd.hpp"
#include "P4Sphere.hpp"
#include "custom.hpp"
#include "main.hpp"
#include "math_separatrice.hpp"

QString gCurrentSingularityInfo[4]{"", "", "", ""};
double gCurrentSeparatriceEpsilon{0};

P4SepDlg::P4SepDlg(P4PlotWnd *plt, P4Sphere *sp)
    : QWidget(plt, Qt::Tool | Qt::WindowStaysOnTopHint), plotWnd_{plt},
      mainSphere_{sp}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    btn_start_ = new QPushButton{"&Start integrating sep", this};
    btn_cont_ = new QPushButton{"&Cont integrating sep", this};
    btn_intnext_ = new QPushButton{"&Integrate next sep", this};
    btn_selectnext_ = new QPushButton{"Select &Next sep", this};

    edt_epsilon_ = new QLineEdit{"", this};
    auto lbl1 = new QLabel{"&Epsilon = ", this};
    lbl1->setBuddy(edt_epsilon_);

    lbl_info_0_ = new QLabel{"", this};
    lbl_info_1_ = new QLabel{"", this};
    lbl_info_2_ = new QLabel{"", this};
    lbl_info_3_ = new QLabel{"", this};

    // layout

    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    mainLayout_->addWidget(lbl_info_0_);
    mainLayout_->addWidget(lbl_info_1_);
    mainLayout_->addWidget(lbl_info_2_);
    mainLayout_->addWidget(lbl_info_3_);

    auto layout3 = new QHBoxLayout{};
    layout3->addWidget(lbl1);
    layout3->addWidget(edt_epsilon_);

    auto layout4 = new QHBoxLayout{};
    layout4->addWidget(btn_start_);
    layout4->addWidget(btn_cont_);

    auto layout5 = new QHBoxLayout{};
    layout5->addWidget(btn_intnext_);
    layout5->addWidget(btn_selectnext_);

    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

#ifdef TOOLTIPS
    edt_epsilon_->setToolTip(
        "Epsilon value for this separatrix\nConfirm any change by pressing "
        "ENTER");
    btn_start_->setToolTip("Start integrating this separatrix");
    btn_cont_->setToolTip("Continue integrating this separatrix");
    btn_intnext_->setToolTip("Select next separatrix, and start integrating");
    btn_selectnext_->setToolTip(
        "Select next separatrix of this singular point");
#endif

    // connections
    QObject::connect(btn_selectnext_, &QPushButton::clicked, this,
                     &P4SepDlg::onbtn_selectnext);
    QObject::connect(btn_intnext_, &QPushButton::clicked, this,
                     &P4SepDlg::onbtn_intnext);
    QObject::connect(btn_start_, &QPushButton::clicked, this,
                     &P4SepDlg::onbtn_start);
    QObject::connect(btn_cont_, &QPushButton::clicked, this,
                     &P4SepDlg::onbtn_cont);
    QObject::connect(edt_epsilon_, &QLineEdit::returnPressed, this,
                     &P4SepDlg::onepsilon_enter);

    // finishing
    reset();

    setP4WindowTitle(this, "Separatrices");
}

void P4SepDlg::setInitialPoint()
{
    QString buf;

    plotWnd_->getDlgData();

    selected_ = true;
    started_ = false;
    btn_start_->setEnabled(true);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    lbl_info_0_->setText(gCurrentSingularityInfo[0]);
    lbl_info_1_->setText(gCurrentSingularityInfo[1]);
    lbl_info_2_->setText(gCurrentSingularityInfo[2]);
    lbl_info_3_->setText(gCurrentSingularityInfo[3]);

    buf.sprintf("%g", gCurrentSeparatriceEpsilon);

    edt_epsilon_->setText(buf);
    show();
    //  raise();
}

void P4SepDlg::onbtn_selectnext()
{
    if (!selected_)
        return;

    plotWnd_->getDlgData();

    started_ = false;

    btn_start_->setEnabled(true);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*select_next_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void P4SepDlg::onbtn_intnext()
{
    if (!selected_)
        return;

    plotWnd_->getDlgData();

    started_ = true;

    btn_start_->setEnabled(false);
    btn_cont_->setEnabled(true);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*plot_next_sep)(mainSphere_, gVFResults.selected_sep_vfindex_);
    mainSphere_->finishDrawing();
}

void P4SepDlg::onbtn_start()
{
    if (!selected_ || started_)
        return;

    plotWnd_->getDlgData();

    started_ = true;
    btn_start_->setEnabled(false);
    btn_cont_->setEnabled(true);
    btn_selectnext_->setEnabled(true);
    btn_intnext_->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*start_plot_sep)(mainSphere_, gVFResults.selected_sep_vfindex_);
    mainSphere_->finishDrawing();
}

void P4SepDlg::onbtn_cont()
{
    if (!selected_ || !started_)
        return;

    plotWnd_->getDlgData();

    mainSphere_->prepareDrawing();
    (*cont_plot_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void P4SepDlg::reset()
{
    lbl_info_0_->setText("no point selected_.");
    lbl_info_1_->setText("");
    lbl_info_2_->setText("Use Shift+Left Button");
    lbl_info_3_->setText("in the plot window to select.");

    started_ = false;
    selected_ = false;

    btn_start_->setEnabled(false);
    btn_cont_->setEnabled(false);
    btn_selectnext_->setEnabled(false);
    btn_intnext_->setEnabled(false);
}

void P4SepDlg::sepEvent(int i)
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

void P4SepDlg::markBad(QLineEdit *edt)
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

void P4SepDlg::onepsilon_enter()
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

    gCurrentSeparatriceEpsilon = eps;
    s.sprintf("%g", static_cast<double>(eps));
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
