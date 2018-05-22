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

#include "P4PrintDlg.hpp"

#include <cmath>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

#include "custom.hpp"
#include "main.hpp"

/*
    The Print Window is a modal dialog box!

    It lets the user select between different print methods and returns one of
    the P4PRINT_ constants.
*/

bool P4PrintDlg::sM_lastBlackWhite{false};
double P4PrintDlg::sM_lastLineWidth{DEFAULT_LINEWIDTH};
double P4PrintDlg::sM_lastSymbolSize{DEFAULT_SYMBOLSIZE};
int P4PrintDlg::sM_lastResolution{DEFAULT_RESOLUTION};

P4PrintDlg::P4PrintDlg(QWidget *parent, Qt::WindowFlags f) : QDialog{parent, f}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    if (gP4smallIcon != nullptr)
        setWindowIcon(*gP4smallIcon);

#ifdef USE_SYSTEM_PRINTER
    btn_default_ = new QPushButton{"&System printer", this};
#endif
    btn_epsimage_ = new QPushButton{"&EPS Image", this};
    btn_xfigimage_ = new QPushButton{"&XFIG Image", this};
    btn_jpeg_ = new QPushButton{"&JPEG Image", this};
    btn_cancel_ = new QPushButton{"&Cancel", this};

    auto label_1 = new QLabel{"Output resolution (in DPI): ", this};
    auto label_2 = new QLabel{"Line width (in mm):", this};
    auto label_3 = new QLabel{"Symbol size (in mm):", this};

    edt_resolution_ = new QLineEdit{"", this};
    edt_linewidth_ = new QLineEdit{"", this};
    edt_symbolsize_ = new QLineEdit{"", this};

    btn_blackwhite_ = new QCheckBox{"&Black&&White printing", this};
    btn_blackwhite_->setChecked(sM_lastBlackWhite);

    auto lbl_bgcolor = new QLabel{"Print background colour:", this};
    btn_whitebg_ = new QRadioButton{"&White", this};
    btn_blackbg_ = new QRadioButton{"B&lack", this};

    auto bgcolors = new QButtonGroup{this};
    bgcolors->addButton(btn_whitebg_);
    bgcolors->addButton(btn_blackbg_);

    if (bgColours::PRINT_WHITE_BG)
        btn_whitebg_->toggle();
    else
        btn_blackbg_->toggle();

#ifdef TOOLTIPS
#ifdef USE_SYSTEM_PRINTER
    btn_default_->setToolTip("Print through default printing mechanism.\nIn "
                             "Linux, this is based on postscript printing.");
#endif
    btn_epsimage_->setToolTip(
        "Produce a \"Encapsulated Postscript\" image file");
    btn_xfigimage_->setToolTip(
        "Produce a .FIG image file that can be read using XFIG");
    btn_jpeg_->setToolTip("Produce a .JPG image file");
    btn_cancel_->setToolTip("Cancel printing");
    btn_blackwhite_->setToolTip(
        "If checked, produce a black&white image, rather than a colour image");
    lbl_bgcolor->setToolTip(
        "Select the background colour to use for printing, regardless of plot "
        "background colour configuration in main settings");
    btn_whitebg_->setToolTip("Use white background for printing");
    btn_blackbg_->setToolTip("Use black background for printing");
    edt_resolution_->setToolTip(
        "Choose a resolution. Images of 15cm wide are produced. The number of "
        "horizontal pixels is hence 15*resolution/2.54.\n This number affects "
        "the quality, but not the phsyical dimensions of "
        "picture.");
    edt_linewidth_->setToolTip(
        "The width of lines (orbits, separatrices, ...) in millimeters");
    edt_symbolsize_->setToolTip("The size of the singularity annotation");
#endif

    mainLayout_ = new QBoxLayout{QBoxLayout::TopToBottom, this};

    mainLayout_->addWidget(
        new QLabel{"Please select printing method...", this});

    auto buttons = new QHBoxLayout{};
#ifdef USE_SYSTEM_PRINTER
    buttons->addWidget(btn_default_);
#endif
    buttons->addWidget(btn_epsimage_);
    buttons->addWidget(btn_xfigimage_);
    buttons->addWidget(btn_jpeg_);
    buttons->addWidget(btn_cancel_);
    mainLayout_->addLayout(buttons);

#ifdef USE_SYSTEM_PRINTER
    QObject::connect(btn_default_, &QPushButton::clicked, this,
                     &P4PrintDlg::onDefaultPrinter);
#endif
    QObject::connect(btn_epsimage_, &QPushButton::clicked, this,
                     &P4PrintDlg::onEpsImagePrinter);
    QObject::connect(btn_xfigimage_, &QPushButton::clicked, this,
                     &P4PrintDlg::onXfigImagePrinter);
    QObject::connect(btn_jpeg_, &QPushButton::clicked, this,
                     &P4PrintDlg::onJpegImagePrinter);
    QObject::connect(btn_cancel_, &QPushButton::clicked, this,
                     &P4PrintDlg::onCancel);
    QObject::connect(btn_whitebg_, &QRadioButton::toggled, this,
                     [=]() { bgColours::PRINT_WHITE_BG = true; });
    QObject::connect(btn_blackbg_, &QRadioButton::toggled, this,
                     [=]() { bgColours::PRINT_WHITE_BG = false; });

    mainLayout_->addSpacing(2);
    mainLayout_->addWidget(btn_blackwhite_);
    mainLayout_->addSpacing(2);

    auto printColourLayout = new QHBoxLayout{};
    printColourLayout->addWidget(lbl_bgcolor);
    printColourLayout->addStretch(0);
    printColourLayout->addWidget(btn_whitebg_);
    printColourLayout->addWidget(btn_blackbg_);

    mainLayout_->addLayout(printColourLayout);
    mainLayout_->addSpacing(2);

    auto l1 = new QHBoxLayout{};
    l1->addWidget(label_1);
    l1->addWidget(edt_resolution_);
    l1->addStretch(0);

    auto l2 = new QHBoxLayout{};
    l2->addWidget(label_2);
    l2->addWidget(edt_linewidth_);
    l2->addStretch(0);

    auto l3 = new QHBoxLayout{};
    l3->addWidget(label_3);
    l3->addWidget(edt_symbolsize_);
    l3->addStretch(0);

    mainLayout_->addLayout(l1);
    mainLayout_->addLayout(l2);
    mainLayout_->addLayout(l3);
    setLayout(mainLayout_);

    QString s;
    s.sprintf("%g", sM_lastLineWidth);
    edt_linewidth_->setText(s);
    s.sprintf("%g", sM_lastSymbolSize);
    edt_symbolsize_->setText(s);
    s.sprintf("%d", sM_lastResolution);
    edt_resolution_->setText(s);

    setP4WindowTitle(this, "Print Phase Portrait");

    return;
}

bool P4PrintDlg::readDialog()
{
    double res;
    bool result;

    sM_lastBlackWhite = btn_blackwhite_->isChecked();

    result = readFloatField(edt_resolution_, res, DEFAULT_RESOLUTION, 72, 4800);
    sM_lastResolution = std::floor(res);
    result |= readFloatField(edt_linewidth_, sM_lastLineWidth,
                             DEFAULT_LINEWIDTH, MIN_LINEWIDTH, MAX_LINEWIDTH);
    result |=
        readFloatField(edt_symbolsize_, sM_lastSymbolSize, DEFAULT_SYMBOLSIZE,
                       MIN_SYMBOLSIZE, MAX_SYMBOLSIZE);

    return !result;
}

void P4PrintDlg::onDefaultPrinter()
{
    if (!readDialog())
        return;

    if (sM_lastBlackWhite)
        done(-P4PRINT_DEFAULT);
    else
        done(P4PRINT_DEFAULT);
}

void P4PrintDlg::onEpsImagePrinter()
{
    if (!readDialog())
        return;
    if (sM_lastBlackWhite)
        done(-P4PRINT_EPSIMAGE);
    else
        done(P4PRINT_EPSIMAGE);
}

void P4PrintDlg::onXfigImagePrinter()
{
    if (!readDialog())
        return;

    /*
        if( sM_lastResolution != 1200 )
        {
            int i;
            i = QMessageBox::warning( this, "Warning",
                                   "XFig printing can only  be done with a
       resolution of 1200 DPI.",
                                   "Continue at 1200 DPI", "Cancel", 0, 0, 1
       ); if( i != 0 )
            {
                done( P4PRINT_NONE );
                return;
            }

            sM_lastResolution = 1200;
        }
    */
    if (sM_lastBlackWhite)
        done(-P4PRINT_XFIGIMAGE);
    else
        done(P4PRINT_XFIGIMAGE);
}

void P4PrintDlg::onJpegImagePrinter()
{
    if (!readDialog())
        return;
    if (sM_lastBlackWhite)
        done(-P4PRINT_JPEGIMAGE);
    else
        done(P4PRINT_JPEGIMAGE);
}

void P4PrintDlg::onCancel() { done(P4PRINT_NONE); }

bool P4PrintDlg::readFloatField(QLineEdit *edt, double &presult,
                                double defvalue, double minvalue,
                                double maxvalue)
{
    // returns true in case of error

    QString x, t;
    bool ok;

    t = edt->text();
    presult = t.toDouble(&ok);
    if (!ok || presult < minvalue || presult > maxvalue) {
        markBad(edt);
        presult = defvalue;
        return true;
    }

    x.sprintf("%g", presult);
    if (x != t)
        edt->setText(x);

    return false;
}

void P4PrintDlg::markBad(QLineEdit *edt)
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

int P4PrintDlg::getChosenResolution() { return sM_lastResolution; }
double P4PrintDlg::getChosenLineWidth() { return sM_lastLineWidth; }
double P4PrintDlg::getChosenSymbolSize() { return sM_lastSymbolSize; }
