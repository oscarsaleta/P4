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

#pragma once

#include <QWidget>

#include "custom.h"

class P4VectorFieldDlg;

class QScrollBar;
class QBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QLabel;
class QString;

class P4VFParams : public QWidget
{
    Q_OBJECT

   public:
    P4VFParams(P4VectorFieldDlg *parent, std::shared_ptr<QScrollBar> sb);

    bool updateDlgData();

   private:
    int currentNumParams_;
    int currentShownParams_;
    int currentPageIndex_;

    std::unique_ptr<QScrollBar> sb_params_;
    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<QBoxLayout> superLayout_;

    std::vector<std::unique_ptr<QHBoxLayout>> paramLayouts_;
    std::vector<std::unique_ptr<QLineEdit>> paramNames_;
    std::vector<std::unique_ptr<QLabel>> paramEqual_;
    std::vector<std::unique_ptr<QLineEdit>> paramValues_;

    // FIXME what are these?
    //std::vector<QString> strLabels_;
    //std::vector<std::vector<QString>> strValues_;

    bool getLineEditCommonParValue(QLineEdit *, int index);
    void setLineEditCommonParValue(QLineEdit *, int index);

   protected:
    bool focusNextPrevChild(bool next);

   public slots:
    void paramsSliderChanged(int);
    void paramsEditingFinished();
};
