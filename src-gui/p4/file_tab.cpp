/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  it under the terms of the GNU Lesser General Public License as published
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

#include "file_tab.hpp"

#include <stdio.h>
#include <vector>

// -----------------------------------------------------------------------
//          readTerm1
// -----------------------------------------------------------------------
bool readTerm1(FILE *fp, std::vector<p4polynom::term1> &p, int N)
{
    int exp;
    double coeff;
    p.clear();
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d %lf", &exp, &coeff) != 2) {
            return false;
        }
        p.emplace_back(exp, coeff);
    }
    return true;
}

// -----------------------------------------------------------------------
//          readTerm2
// -----------------------------------------------------------------------
bool readTerm2(FILE *fp, std::vector<p4polynom::term2> &p, int N)
{
    int xx, xy;
    double coeff;
    p.clear();
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d %d %lf", &xx, &xy, &coeff) != 3) {
            return false;
        }
        p.emplace_back(xx, xy, coeff);
    }
    return true;
}

// -----------------------------------------------------------------------
//          readTerm3
// -----------------------------------------------------------------------
bool readTerm3(FILE *fp, std::vector<p4polynom::term3> &p, int N)
{
    int xr, xc, xs;
    double coeff;
    p.clear();
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d %d %d %lf", &xr, &xc, &xs, &coeff) != 4) {
            return false;
        }
        p.emplace_back(xr, xc, xs, coeff);
    }
    return true;
}
