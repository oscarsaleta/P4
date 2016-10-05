#include <qobject.h>
#include <math.h>
#include "custom.h"
#include "table.h"
#include "math_p4.h"

static struct term2 *vec_field[2];

extern void make_transformations( struct transformations *, double, double, double * );
extern int change_type( int );

void eval_blow_vec_field( double * y, double * f)
{
    f[0]=eval_term2(vec_field[0],y);
    f[1]=eval_term2(vec_field[1],y);
}

struct orbits_points * integrate_blow_up( QWinSphere * spherewnd, //double x0, double y0,
                    double * pcoord2,
                    struct blow_up_points * de_sep, double step,int dir, int type,
                    struct orbits_points ** orbit, int chart )
{
    int i;
    double hhi,point[2];
    double pcoord[3];
    double y[2];
    int color,dashes,ok=true;
    struct orbits_points *first_orbit=nullptr,*last_orbit=nullptr;

    vec_field[0]=de_sep->vector_field[0];
    vec_field[1]=de_sep->vector_field[1];
    if( VFResults.plweights == false && (chart==CHART_V1 || chart==CHART_V2))
        dir=VFResults.dir_vec_field*dir; 
 
    hhi=(double)dir*step;
    y[0]=de_sep->point[0]; y[1]=de_sep->point[1];
    for(i=1;i<=VFResults.config_intpoints;++i)
    {
        rk78(eval_blow_vec_field,y,&hhi,VFResults.config_hmi,VFResults.config_hma,VFResults.config_tolerance); 
        make_transformations(de_sep->trans,
                            de_sep->x0+de_sep->a11*y[0]+de_sep->a12*y[1],
                            de_sep->y0+de_sep->a21*y[0]+de_sep->a22*y[1],point);
        dashes=true;
        switch(chart)
        {
        case CHART_R2:
            MATHFUNC(R2_to_sphere)(point[0],point[1],pcoord);
            color=FindSepColor(VFResults.gcf,type,point);
            break;

        case CHART_U1:
            if(point[1]>=0 || !VFResults.singinf)
            {
                MATHFUNC(U1_to_sphere)(point[0],point[1],pcoord);
                if(!ok)
                {
                    dashes=false; ok=true;
                    if(VFResults.dir_vec_field==1)dir*=-1;
                }
                type=de_sep->type;
                color=FindSepColor(VFResults.gcf_U1,type,point);   
            }
            else
            {
                VV1_to_psphere(point[0],point[1],pcoord);
                if(ok)
                {
                    dashes=false; ok=false;
                    if(VFResults.dir_vec_field==1)dir*=-1;
                }
                if(VFResults.dir_vec_field==1)
                    type=change_type(de_sep->type);
                else
                    type=de_sep->type;

                psphere_to_V1(pcoord[0],pcoord[1],pcoord[2],point);
                color=FindSepColor(VFResults.gcf_V1,type,point);
            }
            break;

        case CHART_V1:
            MATHFUNC(V1_to_sphere)(point[0],point[1],pcoord);
            if(VFResults.plweights == false)
                psphere_to_V1(pcoord[0],pcoord[1],pcoord[2],point);
            color=FindSepColor(VFResults.gcf_V1,type,point);
            break;

        case CHART_U2:
            if(point[1]>=0 || !VFResults.singinf)
            {
                MATHFUNC(U2_to_sphere)(point[0],point[1],pcoord);
                if(!ok)
                {
                    dashes=false; ok=true;
                    if(VFResults.dir_vec_field==1)dir*=-1;
                }
                type=de_sep->type;
                color=FindSepColor(VFResults.gcf_U2,type,point);
            }
            else
            {
                VV2_to_psphere(point[0],point[1],pcoord);
                if(ok){dashes=false; ok=false;
                if(VFResults.dir_vec_field==1)dir*=-1;}
                if(VFResults.dir_vec_field==1)
                    type=change_type(de_sep->type);
                else
                    type=de_sep->type;
                psphere_to_V2(pcoord[0],pcoord[1],pcoord[2],point);
                color=FindSepColor(VFResults.gcf_V2,type,point);
            }
            break;

        case CHART_V2:
            MATHFUNC(V2_to_sphere)(point[0],point[1],pcoord);
            if(VFResults.plweights == false)
                psphere_to_V2(pcoord[0],pcoord[1],pcoord[2],point);
            color=FindSepColor(VFResults.gcf_V2,type,point);
            break;
        
        default:
             color = 0;
             break;
        }
        if(last_orbit==nullptr)
        {
            first_orbit = new orbits_points;//(struct orbits_points *) malloc(sizeof(struct orbits_points));
            last_orbit=first_orbit;
        }
        else
        {
            last_orbit->next_point = new orbits_points;//(struct orbits_points *) malloc(sizeof(struct orbits_points));
            last_orbit=last_orbit->next_point;
        }
        last_orbit->pcoord[0]=pcoord[0]; last_orbit->pcoord[1]=pcoord[1];
        last_orbit->pcoord[2]=pcoord[2]; last_orbit->color=color;
        last_orbit->dashes=dashes * VFResults.config_dashes;
        last_orbit->dir=((VFResults.plweights == false) && (chart==CHART_V1 || chart==CHART_V2)) ? VFResults.dir_vec_field*dir : dir;
        last_orbit->type=type;
        last_orbit->next_point=nullptr;
        if(last_orbit->dashes)
            (*plot_l)( spherewnd, pcoord,pcoord2,color);
        else
            (*plot_p)( spherewnd, pcoord,color);
        
        if(y[0]*y[0]+y[1]*y[1]>=1.0)
        {
            de_sep->blow_up_vec_field=false;
            break;
        }
        copy_x_into_y(pcoord,pcoord2);
    }
    de_sep->point[0]=y[0];
    de_sep->point[1]=y[1]; 
    *orbit=last_orbit;

    return(first_orbit);  
}
