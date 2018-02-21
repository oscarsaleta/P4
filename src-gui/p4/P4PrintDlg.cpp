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

#include "P4PrintDlg.h"

#include "custom.h"
#include "main.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

/*
    The Print Window is a modal dialog box!

    It lets the user select between different print methods and returns one of
    the P4PRINT_ constants.
*/

bool P4PrintDlg::sm_LastBlackWhite{false};
double P4PrintDlg::sm_LastLineWidth{DEFAULT_LINEWIDTH};
double P4PrintDlg::sm_LastSymbolSize{DEFAULT_SYMBOLSIZE};
int P4PrintDlg::sm_LastResolution{DEFAULT_RESOLUTION};

P4PrintDlg::P4PrintDlg(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    if (gP4smallIcon)
        setWindowIcon(*gP4smallIcon);

#ifdef USE_SYSTEM_PRINTER
    btn_default_ = std::make_unique<QPushButton>("&System printer", this);
#endif
    btn_epsimage_ = std::make_unique<QPushButton>("&EPS Image", this);
    btn_xfigimage_ = std::make_unique<QPushButton>("&XFIG Image", this);
    btn_jpeg_ = std::make_unique<QPushButton>("&JPEG Image", this);
    btn_cancel_ = std::make_unique<QPushButton>("&Cancel", this);

    std::unique_ptr<QLabel> label_1{
        std::make_unique<QLabel>("Output resolution (in DPI): ", this)};
    std::unique_ptr<QLabel> label_2{
        std::make_unique<QLabel>("Line width (in mm):", this)};
    std::unique_ptr<QLabel> label_3{
        std::make_unique<QLabel>("Symbol size (in mm):", this)};

    edt_resolution_ = std::make_unique<QLineEdit>("", this);
    edt_linewidth_ = std::make_unique<QLineEdit>("", this);
    edt_symbolsize_ = std::make_unique<QLineEdit>("", this);

    btn_blackwhite_ =
        std::make_unique<QCheckBox>("&Black&&White printing", this);
    btn_blackwhite_->setChecked(sm_LastBlackWhite);

    lbl_bgcolor_ = std::make_unique<QLabel>("Print background colour:", this);
    btn_whitebg_ = std::make_unique<QRadioButton>("&White", this);
    btn_blackbg_ = std::make_unique<QRadioButton>("B&lack", this);

    std::unique_ptr<QButtonGroup> bgcolors{
        std::make_unique<QButtonGroup>(this)};
    bgcolors->addButton(btn_whitebg_.get());
    bgcolors->addButton(btn_blackbg_.get());

    if (bgColours::PRINT_WHITE_BG)
        btn_whitebg_->toggle();
    else
        btn_blackbg_->toggle();

#ifdef TOOLTIPS
#ifdef USE_SYSTEM_PRINTER
    btn_default_->setToolTip(
        "Print through default printing mechanism.\nIn "
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
    lbl_bgcolor_->setToolTip(
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

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(
        new QLabel("Please select printing method...", this));

    std::unique_ptr<QHBoxLayout> buttons{std::make_unique<QHBoxLayout>()};
#ifdef USE_SYSTEM_PRINTER
    buttons->addWidget(btn_default_.get());
#endif
    buttons->addWidget(btn_epsimage_.get());
    buttons->addWidget(btn_xfigimage_.get());
    buttons->addWidget(btn_jpeg_.get());
    buttons->addWidget(btn_cancel_.get());
    mainLayout_->addLayout(buttons.get());

#ifdef USE_SYSTEM_PRINTER
    QObject::connect(btn_default_.get(), &QPushButton::clicked, this,
                     &P4PrintDlg::onDefaultPrinter);
#endif
    QObject::connect(btn_epsimage_.get(), &QPushButton::clicked, this,
                     &P4PrintDlg::onEpsImagePrinter);
    QObject::connect(btn_xfigimage_.get(), &QPushButton::clicked, this,
                     &P4PrintDlg::onXfigImagePrinter);
    QObject::connect(btn_jpeg_.get(), &QPushButton::clicked, this,
                     &P4PrintDlg::onJpegImagePrinter);
    QObject::connect(btn_cancel_.get(), &QPushButton::clicked, this,
                     &P4PrintDlg::onCancel);
    QObject::connect(btn_whitebg_.get(), &QRadioButton::toggled, this,
                     [=]() { bgColours::PRINT_WHITE_BG = true; });
    QObject::connect(btn_blackbg_.get(), &QRadioButton::toggled, this,
                     [=]() { bgColours::PRINT_WHITE_BG = false; });

    mainLayout_->addSpacing(2);
    mainLayout_->addWidget(btn_blackwhite_.get());
    mainLayout_->addSpacing(2);

    std::unique_ptr<QHBoxLayout> printColourLayout{
        std::make_unique<QHBoxLayout>()};
    printColourLayout->addWidget(lbl_bgcolor_.get());
    printColourLayout->addStretch(0);
    printColourLayout->addWidget(btn_whitebg_.get());
    printColourLayout->addWidget(btn_blackbg_.get());

    mainLayout_->addLayout(printColourLayout.get());
    mainLayout_->addSpacing(2);

    std::unique_ptr<QHBoxLayout> l1{std::make_unique<QHBoxLayout>()};
    l1->addWidget(label_1.get());
    l1->addWidget(edt_resolution_.get());
    l1->addStretch(0);

    std::unique_ptr<QHBoxLayout> l2{std::make_unique<QHBoxLayout>()};
    l2->addWidget(label_2.get());
    l2->addWidget(edt_linewidth_.get());
    l2->addStretch(0);

    std::unique_ptr<QHBoxLayout> l3{std::make_unique<QHBoxLayout>()};
    l3->addWidget(label_3.get());
    l3->addWidget(edt_symbolsize_.get());
    l3->addStretch(0);

    mainLayout_->addLayout(l1.get());
    mainLayout_->addLayout(l2.get());
    mainLayout_->addLayout(l3.get());
    setLayout(mainLayout_);

    QString s;
    s.sprintf("%g", (float)sm_LastLineWidth);
    edt_linewidth_->setText(s);
    s.sprintf("%g", (float)sm_LastSymbolSize);
    edt_symbolsize_->setText(s);
    s.sprintf("%d", sm_LastResolution);
    edt_resolution_->setText(s);

    setP4WindowTitle(this, "Print Phase Portrait");

    return;
}

bool P4PrintDlg::readDialog()
{
    double res;
    bool result;

    sm_LastBlackWhite = btn_blackwhite_->isChecked();

    result = readFloatField(edt_resolution_.get(), res, DEFAULT_RESOLUTION, 72,
                            4800);
    sm_LastResolution = floor(res);
    result |= readFloatField(edt_linewidth_.get(), sm_LastLineWidth,
                             DEFAULT_LINEWIDTH, MIN_LINEWIDTH, MAX_LINEWIDTH);
    result |=
        readFloatField(edt_symbolsize_.get(), sm_LastSymbolSize,
                       DEFAULT_SYMBOLSIZE, MIN_SYMBOLSIZE, MAX_SYMBOLSIZE);

    return !result;
}

void P4PrintDlg::onDefaultPrinter()
{
    if (!readDialog())
        return;

    if (sm_LastBlackWhite)
        done(-P4PRINT_DEFAULT);
    else
        done(P4PRINT_DEFAULT);
}

void P4PrintDlg::onEpsImagePrinter()
{
    if (!readDialog())
        return;
    if (sm_LastBlackWhite)
        done(-P4PRINT_EPSIMAGE);
    else
        done(P4PRINT_EPSIMAGE);
}

void P4PrintDlg::onXfigImagePrinter()
{
    if (!readDialog())
        return;

    /*
        if( sm_LastResolution != 1200 )
        {
            int i;
            i = QMessageBox::warning( this, "Warning",
                                   "XFig printing can only  be done with a
       resolution of 1200 DPI.",
                                   "Continue at 1200 DPI", "Cancel", 0, 0, 1 );
            if( i != 0 )
            {
                done( P4PRINT_NONE );
                return;
            }

            sm_LastResolution = 1200;
        }
    */
    if (sm_LastBlackWhite)
        done(-P4PRINT_XFIGIMAGE);
    else
        done(P4PRINT_XFIGIMAGE);
}

void P4PrintDlg::onJpegImagePrinter()
{
    if (!readDialog())
        return;
    if (sm_LastBlackWhite)
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

int P4PrintDlg::getChosenResolution() { return sm_LastResolution; }
double P4PrintDlg::getChosenLineWidth() { return sm_LastLineWidth; }
double P4PrintDlg::getChosenSymbolSize() { return sm_LastSymbolSize; }
