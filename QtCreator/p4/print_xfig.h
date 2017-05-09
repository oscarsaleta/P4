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

#ifndef PRINT_XFIG_H
#define PRINT_XFIG_H

#define XFIG_LINE_MAXPOINTS 2000 // group line pieces together in polylines
                                 //  of at most this number of points.

void prepareXFigPrinting(int w, int h, bool iszoom, bool isblackwhite,
                         int resolution, int linewidth, int symbolwidth);
void finishXFigPrinting(void);

#endif // PRINT_XFIG_H
