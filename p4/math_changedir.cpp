/* if we use the original vector field and we plot a separatrice then it
   is possible that we have to change the direction */

#include <qobject.h>
#include <math.h>
#include "table.h"
#include "custom.h"
#include "math_p4.h"

// -----------------------------------------------------------------------
//                          CHANGE_DIR_POINCARE
// -----------------------------------------------------------------------

int change_dir_poincare( double p[3] )
{
    double y[2];
    double theta;
 
    if( p[2] > ZCOORD )
    {
        // finite point

        psphere_to_R2( p[0], p[1], p[2], y );
        if( eval_term2( VFResults.gcf, y) >= 0 )
            return 0;
        else
            return 1;
    }

    theta = atan2( fabs(p[1]), fabs(p[0]) );

    if( (theta <PI_DIV4) && (theta>-PI_DIV4) )
    {
        if( p[0] > 0 )
        {
            psphere_to_U1( p[0], p[1], p[2], y );
            if( eval_term2( VFResults.gcf_U1, y ) >= 0 )
                return 0;
            else
                return 1;
        }
        else
        {
            psphere_to_V1( p[0], p[1], p[2], y );
            if( eval_term2( VFResults.gcf_V1, y ) >= 0 )
                return 0;
            else
                return 1;
        }
    }
    else
    {
        if( p[1] > 0 )
        {
            psphere_to_U2( p[0], p[1], p[2], y );
            if( eval_term2( VFResults.gcf_U2, y ) >= 0 )
                return 0;
            else
                return 1;
        }
        else
        {
            psphere_to_V2( p[0], p[1], p[2], y );
            if( eval_term2( VFResults.gcf_V2, y ) >= 0 )
                return 0;
            else
                return 1;
        }
    }
}

// -----------------------------------------------------------------------
//                          CHANGE_DIR_LYAPUNOV
// -----------------------------------------------------------------------

int change_dir_lyapunov( double p[3] )
{
    double y[2];

    if( p[0]==0 )
    {
        y[0] = p[1];
        y[1] = p[2];
        if( eval_term2( VFResults.gcf, y ) >= 0 )
            return 0;
        else
            return 1;
    }
    else
    {
        y[0] = p[1];
        y[1] = p[2];
        if( eval_term3( VFResults.gcf_C, y ) >= 0 )
            return 0;
        else
            return 1;
    }
}
