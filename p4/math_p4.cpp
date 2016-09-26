#include <qobject.h>
#include <math.h>
#include "table.h"
#include "custom.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_main.h"
#include "win_plot.h"
#include "win_intparams.h"

extern bool isvalid_psphereviewcoord( double u, double v, double * pcoord );
extern bool isvalid_U1viewcoord( double u, double v, double * pcoord );
extern bool isvalid_U2viewcoord( double u, double v, double * pcoord );
extern bool isvalid_V1viewcoord( double u, double v, double * pcoord );
extern bool isvalid_V2viewcoord( double u, double v, double * pcoord );
extern bool isvalid_R2viewcoord( double u, double v, double * pcoord );
extern bool isvalid_plsphereviewcoord( double u, double v, double * pcoord );

extern void psphere_to_xyrevU1( double X, double Y, double Z, double * rcoord);
extern void psphere_to_xyrevV1( double X, double Y, double Z, double * rcoord);
extern void xyrevU1_to_psphere( double z1, double z2, double * pcoord);
extern void xyrevV1_to_psphere( double z1, double z2, double * pcoord);

extern void plsphere_to_xyrevU1( double X, double Y, double Z, double * rcoord);
extern void plsphere_to_xyrevV1( double X, double Y, double Z, double * rcoord);
extern void xyrevU1_to_plsphere( double z1, double z2, double * pcoord);
extern void xyrevV1_to_plsphere( double z1, double z2, double * pcoord);

extern bool psphere_to_viewcoordpair_discontinuousx( double * p, double * q, double * u1, double * u2, double * u3, double * u4 );
extern bool psphere_to_viewcoordpair_discontinuousy( double * p, double * q, double * u1, double * u2, double * u3, double * u4 );
extern bool plsphere_to_viewcoordpair_discontinuousx( double * p, double * q, double * u1, double * u2, double * u3, double * u4 );
extern bool plsphere_to_viewcoordpair_discontinuousy( double * p, double * q, double * u1, double * u2, double * u3, double * u4 );
extern void default_finite_to_viewcoord( double x, double y, double * ucoord);
extern bool default_sphere_to_viewcoordpair( double * p, double * q, double * u1, double * u2, double * u3, double * u4 );

void (*plot_l)( QWinSphere *, double *, double *, int ) = NULL;
void (*plot_p)( QWinSphere *, double *, int ) = NULL;

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


void QVFStudy::SetupCoordinateTransformations( void )
{
    if( !plweights )
    {
        U1_to_sphere = U1_to_psphere;
        U2_to_sphere = U2_to_psphere;
        V1_to_sphere = VV1_to_psphere;
        V2_to_sphere = VV2_to_psphere;
        sphere_to_R2 = psphere_to_R2;
        R2_to_sphere = R2_to_psphere;
        sphere_to_U1 = psphere_to_U1;
        sphere_to_U2 = psphere_to_U2;
        sphere_to_V1 = psphere_to_VV1;
        sphere_to_V2 = psphere_to_VV2;

        integrate_sphere_sep = integrate_poincare_sep;
        integrate_sphere_orbit = integrate_poincare_orbit;
        eval_lc = eval_lc_poincare;
        less2 = less_poincare;
        change_dir = change_dir_poincare;

        switch( typeofview )
        {
        case TYPEOFVIEW_SPHERE:
            viewcoord_to_sphere = ucircle_psphere;
            sphere_to_viewcoord = psphere_ucircle;
            finite_to_viewcoord = finite_ucircle;
            is_valid_viewcoord  = isvalid_psphereviewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;

        case TYPEOFVIEW_PLANE:
            viewcoord_to_sphere = R2_to_psphere;
            sphere_to_viewcoord = psphere_to_R2;
            finite_to_viewcoord = identitytrf_R2;
            is_valid_viewcoord  = isvalid_R2viewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;

        case TYPEOFVIEW_U1:
            viewcoord_to_sphere = xyrevU1_to_psphere;
            sphere_to_viewcoord = psphere_to_xyrevU1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_U1viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_U2:
            viewcoord_to_sphere = U2_to_psphere;
            sphere_to_viewcoord = psphere_to_U2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_U2viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousy;
            break;

        case TYPEOFVIEW_V1:
            viewcoord_to_sphere = xyrevV1_to_psphere;
            sphere_to_viewcoord = psphere_to_xyrevV1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_V1viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_V2:
            viewcoord_to_sphere = V2_to_psphere;
            sphere_to_viewcoord = psphere_to_V2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_V2viewcoord;
            sphere_to_viewcoordpair = psphere_to_viewcoordpair_discontinuousy;
            break;
        }
    }
    else
    {
        U1_to_sphere = U1_to_plsphere;
        U2_to_sphere = U2_to_plsphere;
        V1_to_sphere = V1_to_plsphere;
        V2_to_sphere = V2_to_plsphere;
        sphere_to_R2 = plsphere_to_R2;
        R2_to_sphere = R2_to_plsphere;
        sphere_to_U1 = plsphere_to_U1;
        sphere_to_U2 = plsphere_to_U2;
        sphere_to_V1 = plsphere_to_V1;
        sphere_to_V2 = plsphere_to_V2;

        integrate_sphere_sep = integrate_lyapunov_sep;
        integrate_sphere_orbit = integrate_lyapunov_orbit;
        eval_lc = eval_lc_lyapunov;
        less2 = less_lyapunov;
        change_dir = change_dir_lyapunov;

        switch( typeofview )
        {
        case TYPEOFVIEW_SPHERE:
            viewcoord_to_sphere = annulus_plsphere;
            sphere_to_viewcoord = plsphere_annulus;
            finite_to_viewcoord = finite_annulus;
            is_valid_viewcoord  = isvalid_plsphereviewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;
        case TYPEOFVIEW_PLANE:
            viewcoord_to_sphere = R2_to_plsphere;
            sphere_to_viewcoord = plsphere_to_R2;
            finite_to_viewcoord = identitytrf_R2;
            is_valid_viewcoord  = isvalid_R2viewcoord;
            sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
            break;
        case TYPEOFVIEW_U1:
            viewcoord_to_sphere = xyrevU1_to_plsphere;
            sphere_to_viewcoord = plsphere_to_xyrevU1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_U1viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_U2:
            viewcoord_to_sphere = U2_to_plsphere;
            sphere_to_viewcoord = plsphere_to_U2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_U2viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousy;
            break;

        case TYPEOFVIEW_V1:
            viewcoord_to_sphere = xyrevV1_to_plsphere;
            sphere_to_viewcoord = plsphere_to_xyrevV1;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_V1viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_V2:
            viewcoord_to_sphere = V2_to_plsphere;
            sphere_to_viewcoord = plsphere_to_V2;
            finite_to_viewcoord = default_finite_to_viewcoord;
            is_valid_viewcoord  = isvalid_V2viewcoord;
            sphere_to_viewcoordpair = plsphere_to_viewcoordpair_discontinuousy;
            break;
        }
    }
}

extern QStartDlg * p4startdlg;

void set_current_step( double curstep )
{
    VFResults.config_currentstep = curstep;
    
    if( p4startdlg != NULL )
        if( p4startdlg->Plot_Window != NULL )
            if( p4startdlg->Plot_Window->IntParams_Window != NULL )
                p4startdlg->Plot_Window->IntParams_Window->SetCurrentStep( curstep );

}
