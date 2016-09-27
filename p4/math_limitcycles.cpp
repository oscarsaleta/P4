#include <qobject.h>
#include <math.h>
#include "custom.h"
#include "table.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_sphere.h"

#define __p                 VFResults.double_p              // p
#define __q                 VFResults.double_q              // q

static bool eval_orbit(double qp[3],double a,double b,double c,double pp[3],int dir );

extern void DrawOrbit( QWinSphere *, double *, struct orbits_points *, int );
void DrawLimitCycle(QWinSphere * spherewnd, double x,double y,double a,double b,double c);

extern bool stop_search_limit( void );                  // see WIN_LIMITCYCLES
extern void write_to_limit_window( double x, double y );

static bool less( double x0, double y0, double x1, double y1 )
{
    if(x0<x1)
        return(1);
    if((x0==x1) && (y0<y1))
        return(1);

    return(0);
}


void SearchLimitCycle( QWinSphere * spherewnd, double x0, double y0, double x1, double y1, double grid )
{
    double z,p1[3],pf1[3],pb1[3],rf1[2],rb1[2];
    double p2[3],pf2[3],pb2[3],rf2[2],rb2[2],p3[3];
    double a,b,c,h1,h2,x,y;
    int okf1,okb1,okf2,okb2,found;
    int t=0;

    if(x1<x0)
    {
        z=x0; x0=x1; x1=z;
        z=y0; y0=y1; y1=z;
    }
    else
        if((x0==x1) && (y1<y0))
        {
            z=y0;
            y0=y1;
            y1=z;
        }
    
    z=atan2(y1-y0,x1-x0);
    h1=grid*cos(z);
    h2=grid*sin(z);
    if((z<=PI_DIV4) && (z>=-PI_DIV4))
    {
        a = (y1-y0)/(x1-x0);
        b = -1.0;
        c = y0 - a*x0;
    }
    else
    {
        a=-1.0;
        b = (x1-x0)/(y1-y0);
        c=x0-b*y0;
    }

    // transverse section is located on the line a*X+b*Y+c*Z=0
    
    okf1 = 0;
    okb1 = 0; 

    MATHFUNC(R2_to_sphere)(x1,y1,p2);

    found = 1;
    while( found )
    {
        found = 0;      // assume not found
        for(;;) 
        {
            if( ++t == VFResults.config_lc_value )
            {
                t = 0;
                if( stop_search_limit() )
                {
                    okf1 = false;
                    okb1 = false;
                    break;
                }
            }
        
            MATHFUNC(R2_to_sphere)(x0,y0,p1);
            if( okf1 )
                okf1 = 0;
            else
            {
                okf1 = eval_orbit( p1, a, b, c, pf1, 1 );
                if( okf1 && MATHFUNC(less2)(p1,pf1) && MATHFUNC(less2)(pf1,p2) )
                    break;
            }
            if( okb1 )
                okb1 = 0;
            else 
            {
                okb1 = eval_orbit( p1, a, b, c, pb1, -1 );
                if( okb1 && MATHFUNC(less2)(p1,pb1) && MATHFUNC(less2)(pb1,p2) )
                    break; 
            } 
            if( okf1 && okb1 && MATHFUNC(less2)(pf1,p1) && MATHFUNC(less2)(p2,pb1) )
            {
                okf1 = false;
                okb1 = false;
                break;
            }
            if( okf1 && okb1 && MATHFUNC(less2)(p2,pf1) && MATHFUNC(less2)(pb1,p1) )
            {
                okf1 = false;
                okb1 = false;
                break;
            }
            
            x0 += h1;
            y0 += h2;

            if( less(x1,y1,x0,y0) )
            {
                okf1 = false;
                okb1 = false;
                break;
            } 
            write_to_limit_window( x0, y0 );
        }        
        if( okf1 || okb1 )
        {
            for(;;)
            {
                x=x0; y=y0; 
//              write_to_limit_window(x0,y0);
                if( ++t == VFResults.config_lc_value )
                {
                    t = 0;
                    if( stop_search_limit() )
                    {
                        okf1 = 0;
                        okb1 = 0;
                        break;
                    }
                }

                if( okf1 )
                {
                    MATHFUNC(sphere_to_R2)(pf1[0],pf1[1],pf1[2],rf1);
                    for(;;)
                    {
                        x=x+h1; y=y+h2;
                        if(less(rf1[0],rf1[1],x,y))break;
                    }
                    if(less(x1,y1,x,y))
                        break;
                    MATHFUNC(R2_to_sphere)(x,y,p3); 
                    okf2 = eval_orbit(p3,a,b,c,pf2,1 ); 
                    if( okf2 )
                    {
                        if(MATHFUNC(less2)(pf2,p3) && MATHFUNC(less2)(p1,pf2))
                        {
                            MATHFUNC(sphere_to_R2)(pf2[0],pf2[1],pf2[2],rf2);
                            spherewnd->prepareDrawing();
                            DrawLimitCycle(spherewnd,(rf1[0]+rf2[0])/2,(rf1[1]+rf2[1])/2,a,b,c);
                            spherewnd->finishDrawing();
                            found=1; 
                            p1[0]=p3[0]; p1[1]=p3[1]; p1[2]=p3[2];
                            x0=x; y0=y;
                            break;
                        }
                        if(MATHFUNC(less2)(pf2,p2))
                        {
                            p1[0]=p3[0]; p1[1]=p3[1]; p1[2]=p3[2];
                            pf1[0]=pf2[0]; pf1[1]=pf2[1]; pf1[2]=pf2[2];
                            x0=x; y0=y;
                        }
                    }
                    else
                    {
                        found = 1;
                        x0 = x;
                        y0 = y;
                        break;
                    }
                }
                else 
                {
                    MATHFUNC(sphere_to_R2)(pb1[0],pb1[1],pb1[2],rb1);
                    for(;;)
                    {
                        x=x+h1; y=y+h2;
                        if(less(rb1[0],rb1[1],x,y))
                            break;
                    }
                    if(less(x1,y1,x,y))
                        break;       
                    MATHFUNC(R2_to_sphere)(x,y,p3); 
                    okb2=eval_orbit(p3,a,b,c,pb2,-1); 
                    if(okb2)
                    {
                        if(MATHFUNC(less2)(pb2,p3) && MATHFUNC(less2)(p1,pb2))
                        {
                            MATHFUNC(sphere_to_R2)(pb2[0],pb2[1],pb2[2],rb2);
                            spherewnd->prepareDrawing();
                            DrawLimitCycle(spherewnd,(rb1[0]+rb2[0])/2,(rb1[1]+rb2[1])/2,a,b,c);
                            spherewnd->finishDrawing();
                            found=1; 
                            p1[0]=p3[0]; p1[1]=p3[1]; p1[2]=p3[2];
                            x0=x; y0=y;
                            break;
                        } 
                        if(MATHFUNC(less2)(pb2,p2))
                        {
                            p1[0]=p3[0]; p1[1]=p3[1]; p1[2]=p3[2];
                            pb1[0]=pb2[0]; pb1[1]=pb2[1]; pb1[2]=pb2[2]; 
                            x0=x; y0=y;
                        }
                    }
                    else
                    {
                        found=1; x0=x; y0=y;
                        break;
                    }
                }
            }
        }
    }
}

static bool eval_orbit( double qp[3], double a, double b, double c, double pp[3], int dir )
{
    double p1[3], p2[3];
    double hhi, h_max, h_min, hhi2, i;
    int dashes, d;
    bool ok;

    ok = false;

    hhi = (double)dir * VFResults.config_step;
    h_max = VFResults.config_hma;
    h_min = VFResults.config_hmi;
    copy_x_into_y( qp, p1 ); 
    MATHFUNC(integrate_sphere_orbit)( p1[0], p1[1], p1[2], p2, &hhi, &dashes, &d, h_min, h_max);
    for( i = 0; i <= VFResults.config_lc_numpoints; i++ )
    {
        copy_x_into_y(p2,p1);
        MATHFUNC(integrate_sphere_orbit)( p1[0], p1[1], p1[2], p2, &hhi, &dashes, &d, h_min, h_max); 
        if((MATHFUNC(eval_lc)(p1,a,b,c)*MATHFUNC(eval_lc)(p2,a,b,c)) <= 0 ) 
        {
            // we have crossed the line on which the transverse section is located
            // (a*X+b*Y+c*Z has changed sign)

            ok = true;
            break;
        }
    }   

    if( ok )
    {
        ok = 0;
        copy_x_into_y(p2,p1);
        MATHFUNC(integrate_sphere_orbit)( p1[0], p1[1], p1[2], p2, &hhi, &dashes, &d, h_min, h_max );
        for( i = 0; i <= VFResults.config_lc_numpoints; i++ )
        {
            copy_x_into_y(p2,p1); 
            hhi2=hhi;
            MATHFUNC(integrate_sphere_orbit)( p1[0], p1[1], p1[2], p2, &hhi, &dashes, &d, h_min, h_max);                
            if((MATHFUNC(eval_lc)(p1,a,b,c)*MATHFUNC(eval_lc)(p2,a,b,c)) <= 0 )
            {
                ok=1;
                break;
            } 
        }    
        if( ok )
        {
            if( fabs(hhi2) < fabs(hhi) )
                hhi = hhi2;

                /* find the intersection point p3 with the poincare section   
                    with a precision of 1e-8         */     

            if((fabs(MATHFUNC(eval_lc)(p2,a,b,c))<=1e-8) || fabs(hhi2)<=h_min)
            {
                copy_x_into_y(p2,pp); 
            }
            else 
            {
                for(;;)
                {
                    hhi2=hhi/2.0;
                    MATHFUNC(integrate_sphere_orbit)(p1[0],p1[1],p1[2],pp,&hhi2,&dashes,&d,h_min,fabs(hhi2)); 
                    if((fabs(MATHFUNC(eval_lc)(pp,a,b,c))<=1e-8) || (fabs(hhi2)<=h_min))
                        break;
                    if((MATHFUNC(eval_lc)(p1,a,b,c)*MATHFUNC(eval_lc)(pp,a,b,c))>0)
                    {
                        copy_x_into_y(pp,p1);
                        hhi=(double)dir*(fabs(hhi)-fabs(hhi2));
                    }
                    else hhi=hhi2;
                }                
            }
        }
    }
    return ok;
}

void DrawLimitCycle(QWinSphere * spherewnd, double x,double y,double a,double b,double c)
{
    double p1[3],p2[3];
    double hhi,h_max,h_min;
    int dashes,d;

    if( VFResults.current_lim_cycle == NULL )
    {
        VFResults.first_lim_cycle=new orbits;//(struct orbits *) malloc(sizeof(struct orbits));
        VFResults.current_lim_cycle=VFResults.first_lim_cycle;
    }
    else
    {
        VFResults.current_lim_cycle->next_orbit=new orbits;//(struct orbits *) malloc(sizeof(struct orbits));
        VFResults.current_lim_cycle=VFResults.current_lim_cycle->next_orbit;
    }

    MATHFUNC(R2_to_sphere)(x,y,p1);
    copy_x_into_y(p1,VFResults.current_lim_cycle->pcoord);
    VFResults.current_lim_cycle->color=CLIMIT;
    VFResults.current_lim_cycle->f_orbits=NULL;
    VFResults.current_lim_cycle->next_orbit=NULL;
    hhi=VFResults.config_step;
    h_max=VFResults.config_hma;
    h_min=VFResults.config_hmi;
    (*plot_p)(spherewnd,p1,CLIMIT);
    MATHFUNC(integrate_sphere_orbit)(p1[0],p1[1],p1[2],p2,&hhi,&dashes,&d,h_min,h_max);
    VFResults.current_lim_cycle->f_orbits = new orbits_points;//(struct orbits_points *)malloc(sizeof(struct orbits_points));
    VFResults.current_lim_cycle->current_f_orbits = VFResults.current_lim_cycle->f_orbits;
    copy_x_into_y(p2,VFResults.current_lim_cycle->current_f_orbits->pcoord);
    VFResults.current_lim_cycle->current_f_orbits->color = CLIMIT;
    VFResults.current_lim_cycle->current_f_orbits->dashes = VFResults.config_dashes;
    VFResults.current_lim_cycle->current_f_orbits->next_point = NULL;
    if( VFResults.config_dashes ) 
        (*plot_l)(spherewnd,p1,p2,CLIMIT);
    else
        (*plot_p)(spherewnd,p2,CLIMIT); 
    for(;;)
    { 
        copy_x_into_y(p2,p1);
        MATHFUNC(integrate_sphere_orbit)(p1[0],p1[1],p1[2],p2,&hhi,&dashes,&d,h_min,h_max);
        VFResults.current_lim_cycle->current_f_orbits->next_point= new orbits_points;//(struct orbits_points *)malloc(sizeof(struct orbits_points));
        VFResults.current_lim_cycle->current_f_orbits=VFResults.current_lim_cycle->current_f_orbits->next_point;
        copy_x_into_y(p2,VFResults.current_lim_cycle->current_f_orbits->pcoord);
        VFResults.current_lim_cycle->current_f_orbits->color=CLIMIT;
        VFResults.current_lim_cycle->current_f_orbits->dashes=VFResults.config_dashes;
        VFResults.current_lim_cycle->current_f_orbits->next_point=NULL;
        if(VFResults.config_dashes) 
            (*plot_l)(spherewnd,p1,p2,CLIMIT);
        else
            (*plot_p)(spherewnd,p2,CLIMIT); 
        
        if((MATHFUNC(eval_lc)(p1,a,b,c)*MATHFUNC(eval_lc)(p2,a,b,c))<=0)
            break;
    }
    copy_x_into_y(p2,p1);
    MATHFUNC(integrate_sphere_orbit)(p1[0],p1[1],p1[2],p2,&hhi,&dashes,&d,h_min,h_max); 
    for(;;)
    { 
        copy_x_into_y(p2,p1);
        MATHFUNC(integrate_sphere_orbit)(p1[0],p1[1],p1[2],p2,&hhi,&dashes,&d,h_min,h_max);
        VFResults.current_lim_cycle->current_f_orbits->next_point=new orbits_points;//(struct orbits_points *)malloc(sizeof(struct orbits_points));
        VFResults.current_lim_cycle->current_f_orbits=VFResults.current_lim_cycle->current_f_orbits->next_point;
        copy_x_into_y(p2,VFResults.current_lim_cycle->current_f_orbits->pcoord);
        VFResults.current_lim_cycle->current_f_orbits->color=CLIMIT;
        VFResults.current_lim_cycle->current_f_orbits->dashes=VFResults.config_dashes;
        VFResults.current_lim_cycle->current_f_orbits->next_point=NULL;
        if((MATHFUNC(eval_lc)(p1,a,b,c)*MATHFUNC(eval_lc)(p2,a,b,c))<=0)
            break; 
        if(VFResults.config_dashes)
            (*plot_l)(spherewnd,p1,p2,CLIMIT);
        else
            (*plot_p)(spherewnd,p2,CLIMIT);
    } 
    MATHFUNC(R2_to_sphere)(x,y,p2);
    copy_x_into_y(p2,VFResults.current_lim_cycle->current_f_orbits->pcoord);
    if(VFResults.config_dashes)
        (*plot_l)(spherewnd,p1,p2,CLIMIT);
    else
        (*plot_p)(spherewnd,p2,CLIMIT);
}

void DrawLimitCycles( QWinSphere * spherewnd )
{
    struct orbits * orbit;

    orbit = VFResults.first_lim_cycle;

    if( orbit!=NULL )
    {
        do
        {
            DrawOrbit( spherewnd, orbit->pcoord, orbit->f_orbits, orbit->color );
        }
        while((orbit=orbit->next_orbit)!=NULL); 
    }
}

// -----------------------------------------------------------------------
//                      DELETELASTLIMITCYCLE
// -----------------------------------------------------------------------

void DeleteLastLimitCycle( QWinSphere * spherewnd )
{
    struct orbits *orbit1,*orbit2;

    if( VFResults.current_lim_cycle == NULL )
        return;

    orbit2 = VFResults.current_lim_cycle;
    DrawOrbit( spherewnd, orbit2->pcoord, orbit2->f_orbits, spherewnd->spherebgcolor );

    if( VFResults.first_lim_cycle == VFResults.current_lim_cycle )
    {
        VFResults.first_lim_cycle = NULL;
        VFResults.current_lim_cycle = NULL;
    }
    else
    {
        orbit1 = VFResults.first_lim_cycle;
    
        do
        {
            VFResults.current_lim_cycle = orbit1;
            orbit1 = orbit1->next_orbit;
        }
        while( orbit1 != orbit2 );
        
        VFResults.current_lim_cycle->next_orbit = NULL;
    }
    VFResults.DeleteOrbitPoint( orbit2->f_orbits ); 
    free( orbit2 );
}
