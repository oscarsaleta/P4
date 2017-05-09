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

/* if we use the original vector field and we plot a separatrice then it
   is possible that we have to change the direction */

#include "math_changedir.h"

#include "custom.h"
#include "math_charts.h"
#include "math_p4.h"
#include "math_polynom.h"

#include <cmath>

// -----------------------------------------------------------------------
//                          CHANGE_DIR_POINCARE
// -----------------------------------------------------------------------

int change_dir_poincare(double p[3])
{
    double y[2];
    double theta;

    if (p[2] > ZCOORD) {
        // finite point

        psphere_to_R2(p[0], p[1], p[2], y);
        if (eval_term2(VFResults.gcf, y) >= 0)
            return 0;
        else
            return 1;
    }

    theta = atan2(fabs(p[1]), fabs(p[0]));

    if ((theta < PI_DIV4) && (theta > -PI_DIV4)) {
        if (p[0] > 0) {
            psphere_to_U1(p[0], p[1], p[2], y);
            if (eval_term2(VFResults.gcf_U1, y) >= 0)
                return 0;
            else
                return 1;
        } else {
            psphere_to_V1(p[0], p[1], p[2], y);
            if (eval_term2(VFResults.gcf_V1, y) >= 0)
                return 0;
            else
                return 1;
        }
    } else {
        if (p[1] > 0) {
            psphere_to_U2(p[0], p[1], p[2], y);
            if (eval_term2(VFResults.gcf_U2, y) >= 0)
                return 0;
            else
                return 1;
        } else {
            psphere_to_V2(p[0], p[1], p[2], y);
            if (eval_term2(VFResults.gcf_V2, y) >= 0)
                return 0;
            else
                return 1;
        }
    }
}

// -----------------------------------------------------------------------
//                          CHANGE_DIR_LYAPUNOV
// -----------------------------------------------------------------------

int change_dir_lyapunov(double p[3])
{
    double y[2];

    if (p[0] == 0) {
        y[0] = p[1];
        y[1] = p[2];
        if (eval_term2(VFResults.gcf, y) >= 0)
            return 0;
        else
            return 1;
    } else {
        y[0] = p[1];
        y[1] = p[2];
        if (eval_term3(VFResults.gcf_C, y) >= 0)
            return 0;
        else
            return 1;
    }
}
