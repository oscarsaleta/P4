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

#ifndef P4SETTINGS_H
#define P4SETTINGS_H

#include <QString>

void setP4Path(QString s);
QString getP4Path(void);

void setP4TempPath(QString s);
QString getP4TempPath(void);

void setP4SumTablePath(QString s);
QString getP4SumTablePath(void);

void setMapleExe(QString s);
QString getMapleExe(void);

void setReduceExe(QString s);
QString getReduceExe(void);

QString getP4HelpPath(void);
QString getP4BinPath(void);
QString getP4ReducePath(void);
QString getP4MaplePath(void);

void setMathManipulator(QString s);
QString getMathManipulator(void);

void setMathPackage(int pck);
int getMathPackage(void);

bool readP4Settings(void);
void saveP4Settings(void);

#endif // P4SETTINGS_H
