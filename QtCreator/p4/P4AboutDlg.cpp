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

#include "P4AboutDlg.hpp"

#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "P4SettingsDlg.hpp"
#include "main.hpp"
#include "p4settings.hpp"

P4AboutDlg::P4AboutDlg(Qt::WindowFlags f, QWidget *parent) : QDialog(parent, f)
{
    // general initialization

    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

    btn_ok_ = new QPushButton{"&Ok", this};
    btn_settings_ = new QPushButton{"&Main Settings", this};

#ifdef TOOLTIPS
    btn_ok_->setToolTip("Go back to program");
    btn_settings_->setToolTip("Access general program settings");
#endif

    // define placement of controls

    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    auto lay00 = new QGridLayout{};
    auto label1 = new QLabel{"P4 (Planar Polynomial Phase Portraits) by", this};
    auto label2 = new QLabel{"", this};
    auto label3 =
        new QLabel{"    J.C. Artés (Universitat Autònoma de Barcelona)", this};
    auto label4 =
        new QLabel{"    P. De Maesschalck (Hasselt University)", this};
    auto label5 = new QLabel{"    F. Dumortier (Hasselt University)", this};
    auto label6 = new QLabel{"    C. Herssens (Hasselt University)", this};
    auto label7 =
        new QLabel{"    J. Llibre (Universitat Autònoma de Barcelona)", this};
    auto label8 =
        new QLabel{"    O. Saleta (Universitat Autònoma de Barcelona)", this};
    auto label9 = new QLabel{
        "    J. Torregrosa (Universitat Autònoma de Barcelona)", this};
    auto label10 = new QLabel{"", this};
    auto label11 = new QLabel{" Version " + gP4version + "   " +
                                  gP4versionDate + " " + gP4platform,
                              this};

    lay00->addWidget(label1, 0, 1);
    lay00->addWidget(label2, 1, 1);
    lay00->addWidget(label3, 2, 1);
    lay00->addWidget(label4, 3, 1);
    lay00->addWidget(label5, 4, 1);
    lay00->addWidget(label6, 5, 1);
    lay00->addWidget(label7, 6, 1);
    lay00->addWidget(label8, 7, 1);
    lay00->addWidget(label9, 8, 1);
    lay00->addWidget(label10, 9, 1);
    lay00->addWidget(label11, 10, 1);

    auto l = new QLabel{"(missing image)", this};
    if (p4image_.load(getP4BinPath() + "/portrait.png"))
        l->setPixmap(p4image_);

    lay00->addWidget(l, 0, 0, 10, 1);

    mainLayout_->addLayout(lay00);
    auto buttons = new QHBoxLayout{};
    buttons->addStretch(1);
    buttons->addWidget(btn_ok_);
    buttons->addStretch(0);
    buttons->addWidget(btn_settings_);
    mainLayout_->addLayout(buttons);
    setLayout(mainLayout_);

    // connections
    QObject::connect(btn_ok_, &QPushButton::clicked, this, &P4AboutDlg::onOk);
    QObject::connect(btn_settings_, &QPushButton::clicked, this,
                     &P4AboutDlg::onSettings);

    btn_ok_->setFocus();

    setP4WindowTitle(this, "About P4");
}

void P4AboutDlg::onOk() { done(0); }

void P4AboutDlg::onSettings()
{
    int value;
    auto psettings = new P4SettingsDlg{this, static_cast<Qt::WindowFlags>(0)};
    value = psettings->exec();
    delete psettings;
    psettings = nullptr;
    if (value)
        done(0);
}
