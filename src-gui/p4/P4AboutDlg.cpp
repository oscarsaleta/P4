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

#include "P4SettingsDlg.hpp"
#include "main.hpp"
#include "p4settings.hpp"

P4AboutDlg::P4AboutDlg(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    // general initialization

    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

    btn_ok_ = std::make_unique<QPushButton>("&Ok");
    btn_settings_ = std::make_unique<QPushButton>("&Main Settings");

#ifdef TOOLTIPS
    btn_ok_->setToolTip("Go back to program");
    btn_settings_->setToolTip("Access general program settings");
#endif

    // define placement of controls

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom);

    auto lay00 = std::make_unique<QGridLayout>();
    auto label1 =
        std::make_unique<QLabel>("P4 (Planar Polynomial Phase Portraits) by");
    auto label2 = std::make_unique<QLabel>("");
    auto label3 = std::make_unique<QLabel>(
        "    J.C. Artés (Universitat Autònoma de Barcelona)");
    auto label4 =
        std::make_unique<QLabel>("    P. De Maesschalck (Hasselt University)");
    auto label5 =
        std::make_unique<QLabel>("    F. Dumortier (Hasselt University)");
    auto label6 =
        std::make_unique<QLabel>("    C. Herssens (Hasselt University)");
    auto label7 = std::make_unique<QLabel>(
        "    J. Llibre (Universitat Autònoma de Barcelona)");
    auto label8 = std::make_unique<QLabel>(
        "    O. Saleta (Universitat Autònoma de Barcelona)");
    auto label9 = std::make_unique<QLabel>(
        "    J. Torregrosa (Universitat Autònoma de Barcelona)");
    auto label10 = std::make_unique<QLabel>("");
    auto label11 = std::make_unique<QLabel>(" Version " + gP4version + "   " +
                                            gP4versionDate + " " + gP4platform);

    lay00->addWidget(label1.get(), 0, 1);
    lay00->addWidget(label1.get(), 1, 1);
    lay00->addWidget(label3.get(), 2, 1);
    lay00->addWidget(label4.get(), 3, 1);
    lay00->addWidget(label5.get(), 4, 1);
    lay00->addWidget(label6.get(), 5, 1);
    lay00->addWidget(label7.get(), 6, 1);
    lay00->addWidget(label8.get(), 7, 1);
    lay00->addWidget(label9.get(), 8, 1);
    lay00->addWidget(label10.get(), 9, 1);
    lay00->addWidget(label11.get(), 10, 1);

    auto l = std::make_unique<QLabel>("(missing image)");
    if (p4image_.load(getP4BinPath() + "/portrait.png"))
        l->setPixmap(p4image_);

    lay00->addWidget(l.get(), 0, 0, 10, 1);

    mainLayout_->addLayout(lay00.get());
    auto buttons = std::make_unique<QHBoxLayout>();
    buttons->addStretch(1);
    buttons->addWidget(btn_ok_.get());
    buttons->addStretch(0);
    buttons->addWidget(btn_settings_.get());
    mainLayout_->addLayout(buttons.get());
    setLayout(mainLayout_.get());

    // connections
    QObject::connect(btn_ok_.get(), &QPushButton::clicked, this,
                     &P4AboutDlg::onOk);
    QObject::connect(btn_settings_.get(), &QPushButton::clicked, this,
                     &P4AboutDlg::onSettings);

    btn_ok_->setFocus();

    setP4WindowTitle(this, "About P4");
}

void P4AboutDlg::onOk() { done(0); }

void P4AboutDlg::onSettings()
{
    int value;
    auto psettings = std::make_unique<P4SettingsDlg>(this, 0);
    value = psettings->exec();
    psettings.reset();
    if (value)
        done(0);
}
