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

#include "win_settings.h"

#include "custom.h"
#include "file_paths.h"
#include "main.h"
#include "p4settings.h"

#include <QFileDialog>
#include <QMessageBox>

/*
    The Settings Window is a modal dialog box!
*/

static QString StripSlash(QString p)
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

static QString StripQuotes(QString p)
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

static QString AddQuotes(QString p)
{
    if (p.isNull())
        return p;

    if (p.contains(' ')) {
        p = "\"" + p + "\"";
    }

    return p;
}

QSettingsDlg::QSettingsDlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    edt_base_ = new QLineEdit(StripQuotes(getP4Path()), this);
    lbl_base_ = new QLabel("&Base Installation Path", this);
    btn_base_ = new QPushButton("Browse...", this);
    lbl_base_->setBuddy(edt_base_);

    edt_sum_ = new QLineEdit(StripQuotes(getP4SumTablePath()), this);
    lbl_sum_ = new QLabel("&Sumtable Path", this);
    btn_sum_ = new QPushButton("Browse...", this);
    lbl_sum_->setBuddy(edt_sum_);

    edt_temp_ = new QLineEdit(StripQuotes(getP4TempPath()), this);
    lbl_temp_ = new QLabel("&Temporary Files Path", this);
    btn_temp_ = new QPushButton("Browse...", this);
    lbl_temp_->setBuddy(edt_temp_);

    edt_maple_ = new QLineEdit(StripQuotes(getMapleExe()), this);
    lbl_maple_ = new QLabel("&Maple Executable", this);
    btn_maple_ = new QPushButton("Browse...", this);
    lbl_maple_->setBuddy(edt_maple_);

    // edt_red = new QLineEdit(StripQuotes(getReduceExe()), this);
    // edt_red->setReadOnly(true);
    // edt_red->setEnabled(false);
    // lbl_red = new QLabel("R&educe Executable", this);
    // lbl_red->setEnabled(false);
    // btn_red = new QPushButton("Browse...", this);
    // btn_red->setEnabled(false);
    // lbl_red->setBuddy(edt_red);

    btn_ok_ = new QPushButton("&Ok", this);
    btn_reset_ = new QPushButton("&Reset", this);
    btn_cancel_ = new QPushButton("&Cancel", this);

    btn_ok_->setDefault(true);

#ifdef TOOLTIPS
    btn_base_->setToolTip("Search for this path on your computer");
    btn_sum_->setToolTip("Search for this path on your computer");
    btn_temp_->setToolTip("Search for this path on your computer");
    btn_maple_->setToolTip("Search for this file on your computer.\nYou need a "
                           "command-line version of Maple.\nThe name would be "
                           "something like cmaple9.5.exe in Windows.");
    // btn_red->setToolTip("Search for this file on your computer.\n");

    edt_base_->setToolTip("The base installation path.  In here, one finds "
                          "bin/, help/ and reduce/ subdirectories.");
    edt_sum_->setToolTip("The path where sumtables are stored when calculating "
                         "Lyapunov constants.");
    edt_temp_->setToolTip("Temporary path name.\nLeave blank when you want "
                          "temporary files to be stored in the current working "
                          "directory.");
    edt_maple_->setToolTip(
        "The name of the Maple executable (command-line version)");
    // edt_red->setToolTip("The name of the reduce executable");

    btn_ok_->setToolTip("Store changes, and go back to program");
    btn_cancel_->setToolTip("Undo any changes");
    btn_reset_->setToolTip("Reset to defaults, try to locate Maple/Reduce "
                           "executable automatically");
#endif

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout_->addSpacing(4);
    mainLayout_->addWidget(new QLabel(
        "These settings are application-wide and will be saved upon exit.\n"
        "An improper value may lead to failure of evaluating vector fields.\n"
#ifdef Q_OS_WIN
        "\n"
        "Note: you are allowed to used spaces in the path names."
#endif
        ,
        this));
    mainLayout_->addSpacing(4);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch(0);
    buttons->addWidget(btn_ok_);
    buttons->addWidget(btn_reset_);
    buttons->addWidget(btn_cancel_);
    buttons->addStretch(0);

    QGridLayout *lay00 = new QGridLayout();

    lay00->addWidget(lbl_base_, 0, 0);
    lay00->addWidget(edt_base_, 0, 1);
    lay00->addWidget(btn_base_, 0, 2);
    lay00->addWidget(lbl_sum_, 1, 0);
    lay00->addWidget(edt_sum_, 1, 1);
    lay00->addWidget(btn_sum_, 1, 2);
    lay00->addWidget(lbl_temp_, 2, 0);
    lay00->addWidget(edt_temp_, 2, 1);
    lay00->addWidget(btn_temp_, 2, 2);
    lay00->addWidget(lbl_maple_, 3, 0);
    lay00->addWidget(edt_maple_, 3, 1);
    lay00->addWidget(btn_maple_, 3, 2);
    // lay00->addWidget(lbl_red, 4, 0);
    // lay00->addWidget(edt_red, 4, 1);
    // lay00->addWidget(btn_red, 4, 2);

    mainLayout_->addLayout(lay00);
    mainLayout_->addSpacing(3);
    // mainLayout_->addSpacing(4);
    mainLayout_->addLayout(buttons);

    setLayout(mainLayout_);

    connect(btn_base_, &QPushButton::clicked, this,
            &QSettingsDlg::onBrowseBase);
    connect(btn_sum_, &QPushButton::clicked, this, &QSettingsDlg::onBrowseSum);
    connect(btn_temp_, &QPushButton::clicked, this,
            &QSettingsDlg::onBrowseTemp);
    connect(btn_maple_, &QPushButton::clicked, this,
            &QSettingsDlg::onBrowseMaple);
    // connect(btn_red, &QPushButton::clicked, this,
    // &QSettingsDlg::OnBrowseReduce);
    connect(btn_ok_, &QPushButton::clicked, this, &QSettingsDlg::onOk);
    connect(btn_reset_, &QPushButton::clicked, this, &QSettingsDlg::onReset);
    connect(btn_cancel_, &QPushButton::clicked, this, &QSettingsDlg::onCancel);

    //#ifdef Q_OS_WIN
    //    edt_red->setEnabled(false);
    //    btn_red->setEnabled(false);
    //#endif

    setP4WindowTitle(this, "Main Program Settings");
}

void QSettingsDlg::onCancel(void) { done(0); }

void QSettingsDlg::onOk(void)
{
    QString s;

    // Strip outer Quotes and white space, in any order, and possible also
    // trailing slash

    s = StripSlash(StripQuotes(StripQuotes(edt_base_->text()).trimmed()));
    setP4Path(s);
    s = StripSlash(StripQuotes(StripQuotes(edt_sum_->text()).trimmed()));
    setP4SumTablePath(s);
    s = StripSlash(StripQuotes(StripQuotes(edt_temp_->text()).trimmed()));
    setP4TempPath(s);

    // for exe's, trailing slash is irrelevant

    s = StripQuotes(StripQuotes(edt_maple_->text()).trimmed());

#ifdef Q_OS_WIN
    // what to do when the user enters a value for a maple executable that does
    // not exist?

    QFileInfo maplepat;
    int vb;

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

    setMapleExe(AddQuotes(s));
    // s = StripQuotes(StripQuotes(edt_red->text()).trimmed());
    // setReduceExe(AddQuotes(s));

    done(1);
}

void QSettingsDlg::onReset(void)
{
    edt_base_->setText(StripQuotes(getDefaultP4Path()));
    edt_sum_->setText(StripQuotes(getDefaultP4SumTablePath()));
    edt_temp_->setText(StripQuotes(getDefaultP4TempPath()));
    edt_maple_->setText(StripQuotes(getDefaultMapleInstallation()));
    // edt_red->setText(StripQuotes(getDefaultReduceInstallation()));
}

/*void QSettingsDlg::OnBrowseReduce(void)
{
    browseForExistingPathOrFile(edt_red, QString("Select reduce executable:"),
                                true);
}*/

void QSettingsDlg::onBrowseMaple(void)
{
    browseForExistingPathOrFile(edt_maple_, "Select maple executable:", true);
}

void QSettingsDlg::onBrowseTemp(void)
{
    browseForExistingPathOrFile(edt_temp_, "Select temporary path:", false);
}

void QSettingsDlg::onBrowseSum(void)
{
    browseForExistingPathOrFile(edt_sum_, "Select sumtable path:", false);
}

void QSettingsDlg::onBrowseBase(void)
{
    browseForExistingPathOrFile(edt_base_,
                                "Select P4 base installation path:", false);
}

void QSettingsDlg::browseForExistingPathOrFile(QLineEdit *edt, QString caption,
                                               bool isfile)
{
    // browse for an existing path if isfile = false, or for an existing file if
    // isfile = true.

    QString oldstr, newstr;

    oldstr = edt->text();
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
