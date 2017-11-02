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

#include "win_about.h"

#include "main.h"
#include "p4settings.h"
#include "win_settings.h"

#include <QLabel>

QP4AboutDlg::QP4AboutDlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    // general initialization

    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    btn_ok_ = new QPushButton("&Ok");
    btn_settings_ = new QPushButton("&Main Settings");

#ifdef TOOLTIPS
    btn_ok_->setToolTip("Go back to program");
    btn_settings_->setToolTip("Access general program settings");
#endif

    // define placement of controls

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom);

    QGridLayout *lay00 = new QGridLayout();
    lay00->addWidget(new QLabel("P4 (Planar Polynomial Phase Portraits) by"), 0,
                     1);
    lay00->addWidget(new QLabel(""), 1, 1);
    lay00->addWidget(new QLabel(QString::fromUtf8(
                         "    J.C. Artés (Universitat Autònoma de Barcelona)")),
                     2, 1);
    lay00->addWidget(new QLabel("    P. De Maesschalck (Hasselt University)"),
                     3, 1);
    lay00->addWidget(new QLabel("    F. Dumortier (Hasselt University)"), 4, 1);
    lay00->addWidget(new QLabel("    C. Herssens (Hasselt University)"), 5, 1);
    lay00->addWidget(new QLabel(QString::fromUtf8(
                         "    J. Llibre (Universitat Autònoma de Barcelona)")),
                     6, 1);
    lay00->addWidget(new QLabel(QString::fromUtf8(
                         "    O. Saleta (Universitat Autònoma de Barcelona)")),
                     7, 1);
    lay00->addWidget(
        new QLabel(QString::fromUtf8(
            "    J. Torregrosa (Universitat Autònoma de Barcelona)")),
        8, 1);
    lay00->addWidget(new QLabel(""), 9, 1);

    QString versionstr;
    versionstr = " Version " + g_p4Version + "   " + g_p4VersionDate + " " +
                 g_p4Platform;

    lay00->addWidget(new QLabel(versionstr), 10, 1);

    QLabel *l;
    l = new QLabel("(missing image)");
    if (p4image_.load(getP4BinPath() + "/portrait.png"))
        l->setPixmap(p4image_);

    lay00->addWidget(l, 0, 0, 10, 1);

    mainLayout_->addLayout(lay00);
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch(1);
    buttons->addWidget(btn_ok_);
    buttons->addStretch(0);
    buttons->addWidget(btn_settings_);
    mainLayout_->addLayout(buttons);
    setLayout(mainLayout_);

    // connections

    connect(btn_ok_, &QPushButton::clicked, this, &QP4AboutDlg::onOk);
    connect(btn_settings_, &QPushButton::clicked, this,
                     &QP4AboutDlg::onSettings);

    btn_ok_->setFocus();

    setP4WindowTitle(this, "About P4");
}

void QP4AboutDlg::onOk(void) { done(0); }

void QP4AboutDlg::onSettings(void)
{
    int value;
    QSettingsDlg *psettings;
    psettings = new QSettingsDlg(this, 0);
    value = psettings->exec();
    delete psettings;
    psettings = nullptr;
    if (value)
        done(0);
}
