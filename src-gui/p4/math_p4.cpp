/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
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

#include "math_p4.h"

#include "math_charts.h"
#include "win_intparams.h"
#include "win_main.h"
#include "win_plot.h"
#include "win_sphere.h"

#include <cmath>


bool less_poincare( double * p1, double * p2 )
{
    if((p1[0]*p2[2])<(p2[0]*p1[2]))return(1);
    if(((p1[0]*p2[2])==(p2[0]*p1[2])) && ((p1[1]*p2[2])<(p2[1]*p1[2])))return( true );
    return( false );
}

double eval_lc_poincare( double * pp,double a, double b, double c )
{
    return (a*pp[0]+b*pp[1]+c*pp[2]);
}

double eval_lc_lyapunov( double * pp,double a, double b, double c )
{
    if( pp[0] )
        return    a * pow(pp[1],VFResults.double_q)*cos(pp[2])
                + b * pow(pp[1],VFResults.double_p)*sin(pp[2])
                + c * pow(pp[1],VFResults.double_p_plus_q);
    else
        return ( a * pp[1] + b * pp[2] + c );
}
 
bool less_lyapunov(double * p1, double * p2 )
{
    double u[2],v[2];

    plsphere_annulus(p1[0],p1[1],p1[2],u);
    plsphere_annulus(p2[0],p2[1],p2[2],v);
    if(u[0]<v[0])return(1);
    if((u[0]==v[0]) && (u[1]<v[1]))return(1);
    return(0);
}  

void set_current_step( double curstep )
{
    VFResults.config_currentstep = curstep;
    
    if( p4startdlg != nullptr )
        if( p4startdlg->Plot_Window != nullptr )
            if( p4startdlg->Plot_Window->IntParams_Window != nullptr )
                p4startdlg->Plot_Window->IntParams_Window->SetCurrentStep( curstep );

}
