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

#include "P4SettingsDlg.h"

#include "custom.h"
#include "file_paths.h"
#include "main.h"
#include "p4settings.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

/*
    The Settings Window is a modal dialog box!
*/

static QString stripSlash(QString p)
{
    if (p.isNull())
        return p;
    if (p.length() > 0) {
        int i;
        i = p.length() - 1;
        if (p[i] == '/' || p[i] == '\\')
            p = p.mid(0, i);
    }
    return p;
}

static QString stripQuotes(QString p)
{
    if (p.isNull())
        return p;
    if (p[0] == '\"')
        p = p.mid(1);

    if (p.length() != 0) {
        int i;
        i = p.length() - 1;
        if (p[i] == '\"') {
            p = p.mid(0, i);
        }
    }

    return p;
}

static QString addQuotes(QString p)
{
    if (p.isNull())
        return p;

    if (p.contains(' ')) {
        p = "\"" + p + "\"";
    }

    return p;
}

P4SettingsDlg::P4SettingsDlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    edt_base_ = std::make_unique<QLineEdit>(stripQuotes(getP4Path()), this);
    lbl_base_ = std::make_unique<QLabel>("&Base Installation Path", this);
    btn_base_ = std::make_unique<QPushButton>("Browse...", this);
    lbl_base_->setBuddy(edt_base_);

    edt_sum_ =
        std::make_unique<QLineEdit>(stripQuotes(getP4SumTablePath()), this);
    lbl_sum_ = std::make_unique<QLabel>("&Sumtable Path", this);
    btn_sum_ = std::make_unique<QPushButton>("Browse...", this);
    lbl_sum_->setBuddy(edt_sum_);

    edt_temp_ = std::make_unique<QLineEdit>(stripQuotes(getP4TempPath()), this);
    lbl_temp_ = std::make_unique<QLabel>("&Temporary Files Path", this);
    btn_temp_ = std::make_unique<QPushButton>("Browse...", this);
    lbl_temp_->setBuddy(edt_temp_);

    edt_maple_ = std::make_unique<QLineEdit>(stripQuotes(getMapleExe()), this);
    lbl_maple_ = std::make_unique<QLabel>("&Maple Executable", this);
    btn_maple_ = std::make_unique<QPushButton>("Browse...", this);
    lbl_maple_->setBuddy(edt_maple_);

    lbl_bgcolor_ = std::make_unique<QLabel>("Plot background color", this);
    btn_bgblack_ = std::make_unique<QRadioButton>("Black", this);
    btn_bgwhite_ = std::make_unique<QRadioButton>("White", this);

    std::unique_ptr<QButtonGroup> bgcolors{
        std::make_unique<QButtonGroup>(this)};
    bgcolors->addButton(btn_bgblack_.get());
    bgcolors->addButton(btn_bgwhite_.get());

    btn_ok_ = std::make_unique<QPushButton>("&Ok", this);
    btn_reset_ = std::make_unique<QPushButton>("&Reset", this);
    btn_cancel_ = std::make_unique<QPushButton>("&Cancel", this);

    btn_ok_->setDefault(true);

#ifdef TOOLTIPS
    btn_base_->setToolTip("Search for this path on your computer");
    btn_sum_->setToolTip("Search for this path on your computer");
    btn_temp_->setToolTip("Search for this path on your computer");
    btn_maple_->setToolTip(
        "Search for this file on your computer.\nYou need a "
        "command-line version of Maple.\nThe name would be "
        "something like cmaple9.5.exe in Windows.");
    // btn_red->setToolTip("Search for this file on your computer.\n");

    edt_base_->setToolTip(
        "The base installation path.  In here, one finds "
        "bin/, help/ and reduce/ subdirectories.");
    edt_sum_->setToolTip(
        "The path where sumtables are stored when calculating "
        "Lyapunov constants.");
    edt_temp_->setToolTip(
        "Temporary path name.\nLeave blank when you want "
        "temporary files to be stored in the current working "
        "directory.");
    edt_maple_->setToolTip(
        "The name of the Maple executable (command-line version)");
    // edt_red->setToolTip("The name of the reduce executable");

    btn_ok_->setToolTip("Store changes, and go back to program");
    btn_cancel_->setToolTip("Undo any changes");
    btn_reset_->setToolTip(
        "Reset to defaults, try to locate Maple/Reduce "
        "executable automatically");
#endif

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    mainLayout_->addSpacing(4);
    mainLayout_->addWidget(new QLabel(
        "These settings are application-wide and will be saved upon exit.\n"
        "An improper value may lead to failure of evaluating vector fields.\n"
#ifdef Q_OS_WIN
        "\n"
        "Note: you are allowed to use spaces in the path names."
#endif
        ,
        this));
    mainLayout_->addSpacing(4);

    std::unique_ptr<QHBoxLayout> buttons{std::make_unique<QHBoxLayout>{}};
    buttons->addStretch(0);
    buttons->addWidget(btn_ok_.get());
    buttons->addWidget(btn_reset_.get());
    buttons->addWidget(btn_cancel_.get());
    buttons->addStretch(0);

    std::unique_ptr<QGridLayout> lay00{std::make_unique<QGridLayout>{}};

    lay00->addWidget(lbl_base_.get(), 0, 0);
    lay00->addWidget(edt_base_.get(), 0, 1);
    lay00->addWidget(btn_base_.get(), 0, 2);
    lay00->addWidget(lbl_sum_.get(), 1, 0);
    lay00->addWidget(edt_sum_.get(), 1, 1);
    lay00->addWidget(btn_sum_.get(), 1, 2);
    lay00->addWidget(lbl_temp_.get(), 2, 0);
    lay00->addWidget(edt_temp_.get(), 2, 1);
    lay00->addWidget(btn_temp_.get(), 2, 2);
    lay00->addWidget(lbl_maple_.get(), 3, 0);
    lay00->addWidget(edt_maple_.get(), 3, 1);
    lay00->addWidget(btn_maple_.get(), 3, 2);

    std::unique_ptr<QHBoxLayout> bgbuttons{std::make_unique<QHBoxLayout>{}};
    bgbuttons->addWidget(lbl_bgcolor_.get());
    bgbuttons->addStretch(0);
    bgbuttons->addWidget(btn_bgblack_.get());
    bgbuttons->addWidget(btn_bgwhite_.get());

    if (bgColours::CBACKGROUND == BLACK)
        btn_bgblack_->toggle();
    else
        btn_bgwhite_->toggle();

    mainLayout_->addLayout(lay00);
    mainLayout_->addSpacing(3);
    mainLayout_->addLayout(bgbuttons);
    mainLayout_->addSpacing(3);
    // mainLayout_->addSpacing(4);
    mainLayout_->addLayout(buttons);

    setLayout(mainLayout_);

    QObject::connect(btn_base_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onBrowseBase);
    QObject::connect(btn_sum_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onBrowseSum);
    QObject::connect(btn_temp_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onBrowseTemp);
    QObject::connect(btn_maple_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onBrowseMaple);
    QObject::connect(btn_ok_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onOk);
    QObject::connect(btn_reset_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onReset);
    QObject::connect(btn_cancel_, &QPushButton::clicked, this,
                     &P4SettingsDlg::onCancel);

    QObject::connect(btn_bgblack_, &QRadioButton::toggled, this, [=]() {
        bgColours::CFOREGROUND = WHITE;
        bgColours::CBACKGROUND = BLACK;
        bgColours::CORBIT = YELLOW;
    });
    QObject::connect(btn_bgwhite_, &QRadioButton::toggled, this, [=]() {
        bgColours::CFOREGROUND = BLACK;
        bgColours::CBACKGROUND = WHITE;
        bgColours::CORBIT = GREEN1;
    });

    setP4WindowTitle(this, "Main Program Settings");
}

void P4SettingsDlg::onCancel() { done(0); }

void P4SettingsDlg::onOk()
{
    QString s;

    // Strip outer Quotes and white space, in any order, and possible also
    // trailing slash

    s = stripSlash(stripQuotes(stripQuotes(edt_base_->text()).trimmed()));
    setP4Path(s);
    s = stripSlash(stripQuotes(stripQuotes(edt_sum_->text()).trimmed()));
    setP4SumTablePath(s);
    s = stripSlash(stripQuotes(stripQuotes(edt_temp_->text()).trimmed()));
    setP4TempPath(s);

    // for exe's, trailing slash is irrelevant

    s = stripQuotes(stripQuotes(edt_maple_->text()).trimmed());

#ifdef Q_OS_WIN
    // what to do when the user enters a value for a maple executable that does
    // not exist?

    QFileInfo maplepat;
    bool vb;

    maplepat.setFile(s);
    if (!maplepat.exists()) {
        vb = false;

        QMessageBox msgBox;
        msgBox.setText(
            "The maple executable was not found on disc.\n\n"
            "P4 will not be able to evaluate/plot vector fields.\n\n");
        msgBox.addButton("&Go back to settings window",
                         QMessageBox::AcceptRole);
        msgBox.addButton("&Don't care", QMessageBox::RejectRole);
        switch (msgBox.exec()) {
        case QMessageBox::AcceptRole:
            // yes was clicked
            vb = false;
            break;
        case QMessageBox::RejectRole:
            // no was clicked
            vb = true;
            break;
        default:
            // should never be reached
            break;
        }
        if (!vb)
            return;
    }
#endif

    setMapleExe(addQuotes(s));

    done(1);
}

void P4SettingsDlg::onReset()
{
    edt_base_->setText(stripQuotes(getDefaultP4Path()));
    edt_sum_->setText(stripQuotes(getDefaultP4SumTablePath()));
    edt_temp_->setText(stripQuotes(getDefaultP4TempPath()));
    edt_maple_->setText(stripQuotes(getDefaultMapleInstallation()));
}

void P4SettingsDlg::onBrowseMaple()
{
    browseForExistingPathOrFile(edt_maple_, "Select maple executable:", true);
}

void P4SettingsDlg::onBrowseTemp()
{
    browseForExistingPathOrFile(edt_temp_, "Select temporary path:", false);
}

void P4SettingsDlg::onBrowseSum()
{
    browseForExistingPathOrFile(edt_sum_, "Select sumtable path:", false);
}

void P4SettingsDlg::onBrowseBase()
{
    browseForExistingPathOrFile(edt_base_,
                                "Select P4 base installation path:", false);
}

void P4SettingsDlg::browseForExistingPathOrFile(QLineEdit *edt, QString caption,
                                                bool isfile)
{
    // browse for an existing path if isfile = false, or for an existing file if
    // isfile = true.

    QString oldstr{edt->text()};
    QString newstr;
    
    if (!isfile)
        newstr = QFileDialog::getExistingDirectory(this, caption, oldstr,
                                                   QFileDialog::ShowDirsOnly);
    else
        newstr = QFileDialog::getOpenFileName(this, caption, oldstr);

    if (!newstr.isNull()) {
        newstr = QDir::toNativeSeparators(newstr);
        edt->setText(newstr);
    }
}
