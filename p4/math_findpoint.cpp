#include <qobject.h>
#include <math.h>
#include "custom.h"
#include "table.h"
#include "math_p4.h"

#define p4_isnan    isnan
#define p4_finite   finite

extern QString CurrentSingularityInfo[4];       // see win_separatrices.cpp
extern double CurrentSeparatriceEpsilon;

// -----------------------------------------------------------------------
//
// This file implements the selection of a critical point, when the
// user clicks with the mouse inside the plot window.  The nearesst singularity
// having separatrices is searched, the corresponding separatrice is selected
// and the separatrice window is set up so that it handes this separatrice.
//
// -----------------------------------------------------------------------

extern void (* change_epsilon)( QWinSphere *, double );
extern void (*start_plot_sep)( QWinSphere * );
extern void (*cont_plot_sep)( QWinSphere * );
extern void (*plot_next_sep)( QWinSphere * );
extern void (*select_next_sep)( QWinSphere * );

extern void start_plot_saddle_sep( QWinSphere * );
extern void cont_plot_saddle_sep( QWinSphere * );
extern void plot_next_saddle_sep( QWinSphere * );
extern void select_next_saddle_sep( QWinSphere * );
extern void change_epsilon_saddle( QWinSphere *, double );

extern void start_plot_se_sep( QWinSphere * );
extern void cont_plot_se_sep( QWinSphere * );
extern void plot_next_se_sep( QWinSphere * );
extern void select_next_se_sep( QWinSphere * );
extern void change_epsilon_se( QWinSphere *, double );

extern void start_plot_de_sep( QWinSphere * );
extern void cont_plot_de_sep( QWinSphere * );
extern void plot_next_de_sep( QWinSphere * );
extern void select_next_de_sep( QWinSphere * );
extern void change_epsilon_de( QWinSphere *, double );

static double find_distance_saddle(struct saddle *point, double x, double y,
                                   double distance,int *type)
{
    double d,pcoord[3],ucoord[2];

    if(point)
    {
        do
        {
            switch(point->chart)
            {
            case CHART_R2: MATHFUNC(R2_to_sphere)(point->x0,point->y0,pcoord); break;
            case CHART_U1: MATHFUNC(U1_to_sphere)(point->x0,point->y0,pcoord); break;
            case CHART_V1: MATHFUNC(V1_to_sphere)(point->x0,point->y0,pcoord);  break;
            case CHART_U2: MATHFUNC(U2_to_sphere)(point->x0,point->y0,pcoord);  break;
            case CHART_V2: MATHFUNC(V2_to_sphere)(point->x0,point->y0,pcoord);  break;
            }
            MATHFUNC(sphere_to_viewcoord)(pcoord[0],pcoord[1],pcoord[2],ucoord);

            d=(x-ucoord[0])*(x-ucoord[0])+(y-ucoord[1])*(y-ucoord[1]);
            if((d<distance && !p4_isnan(d) && p4_finite(d) ) || (distance==-1))
            {
                distance=d;
                VFResults.selected_saddle_point=point;
                VFResults.selected_ucoord[0] = ucoord[0];
                VFResults.selected_ucoord[1] = ucoord[1];
                *type=SADDLE;
            }
        }
        while((point=point->next_saddle)!=NULL);
    }

    return(distance);
}



static double find_distance_se(struct semi_elementary *point, double x,
                               double y,double distance,int *type)
{
    double d,pcoord[3],ucoord[2];

    if( point )
    {
        do
        {
            if(point->separatrices)
            {
                switch(point->chart)
                {
                case CHART_R2: MATHFUNC(R2_to_sphere)(point->x0,point->y0,pcoord);  break;
                case CHART_U1: MATHFUNC(U1_to_sphere)(point->x0,point->y0,pcoord);  break;
                case CHART_V1: MATHFUNC(V1_to_sphere)(point->x0,point->y0,pcoord);  break;
                case CHART_U2: MATHFUNC(U2_to_sphere)(point->x0,point->y0,pcoord);  break;
                case CHART_V2: MATHFUNC(V2_to_sphere)(point->x0,point->y0,pcoord);  break;
                }
                MATHFUNC(sphere_to_viewcoord)(pcoord[0],pcoord[1],pcoord[2],ucoord);
                d=(x-ucoord[0])*(x-ucoord[0])+(y-ucoord[1])*(y-ucoord[1]);
                if((d<distance && !p4_isnan(d) && p4_finite(d) ) || (distance==-1))
                {
                    distance=d;
                    VFResults.selected_se_point=point;
                    VFResults.selected_ucoord[0] = ucoord[0];
                    VFResults.selected_ucoord[1] = ucoord[1];
                    *type=SEMI_HYPERBOLIC;
                }
            }
        }
        while( (point=point->next_se) != NULL );
    }
    return(distance);
}

static double find_distance_de(struct degenerate *point, double x, double y,
                               double distance,int *type)
{
    double d, pcoord[3], ucoord[2];

    if( point )
    do
    {
        if(point->blow_up)
        {
            switch(point->chart)
            {
            case CHART_R2: MATHFUNC(R2_to_sphere)(point->x0,point->y0,pcoord);
            break;
            case CHART_U1: MATHFUNC(U1_to_sphere)(point->x0,point->y0,pcoord);
            break;
            case CHART_V1: MATHFUNC(V1_to_sphere)(point->x0,point->y0,pcoord);
            break;
            case CHART_U2: MATHFUNC(U2_to_sphere)(point->x0,point->y0,pcoord);
            break;
            case CHART_V2: MATHFUNC(V2_to_sphere)(point->x0,point->y0,pcoord);
            break;
            }
            MATHFUNC(sphere_to_viewcoord)(pcoord[0],pcoord[1],pcoord[2],ucoord);          
            d=(x-ucoord[0])*(x-ucoord[0])+(y-ucoord[1])*(y-ucoord[1]);
            if((d<distance && !p4_isnan(d) && p4_finite(d) ) || (distance==-1))
            if((d<distance) || (distance==-1))
            {
                distance=d;
                VFResults.selected_ucoord[0] = ucoord[0];
                VFResults.selected_ucoord[1] = ucoord[1];
                VFResults.selected_de_point = point;
                *type=NON_ELEMENTARY;
            }
        }
    }
    while( (point=point->next_de) != NULL );

    return(distance);
}
 
  
int find_critical_point( QWinSphere * spherewnd, double x, double y )
{
    int type;
    int sepcount;
    double distance, epsilon, pcoord[3];
    QString s,sx,sy,sz;
    struct sep * sepc;
    struct blow_up_points * bc;

    CurrentSingularityInfo[0] = "";
    CurrentSingularityInfo[1] = "";
    CurrentSingularityInfo[2] = "";
    CurrentSingularityInfo[3] = "";

    distance = -1;
    distance = find_distance_saddle( VFResults.first_saddle_point, x, y, distance, &type );
    distance = find_distance_se( VFResults.first_se_point, x, y, distance, &type );
    distance = find_distance_de( VFResults.first_de_point, x, y, distance, &type );

    if( distance == -1 )
        return false;

    sepcount = 0;

   sx="";
   sy="";
   sz="";
   epsilon=0;

    switch( type )
    {
    case SADDLE:
        sepc = VFResults.selected_sep = VFResults.selected_saddle_point->separatrices;
        draw_selected_sep( spherewnd, VFResults.selected_sep->first_sep_point, CW_SEP );

        while( sepc != NULL )
        {
            sepcount++;
            sepc = sepc->next_sep;
        }

        start_plot_sep = start_plot_saddle_sep;
        cont_plot_sep = cont_plot_saddle_sep;
        plot_next_sep = plot_next_saddle_sep;
        select_next_sep = select_next_saddle_sep;
        s = "SADDLE";
        epsilon = VFResults.selected_saddle_point->epsilon;
        change_epsilon = change_epsilon_saddle;

        if(VFResults.selected_saddle_point->chart==CHART_R2)
        {
            sx.sprintf( "x = %f", (float)(VFResults.selected_saddle_point->x0) );
            sy.sprintf( "y = %f", (float)(VFResults.selected_saddle_point->y0) );
            sz="";
        } 
        else
        {
            switch(VFResults.selected_saddle_point->chart)
            {
            case CHART_U1: MATHFUNC(U1_to_sphere)(VFResults.selected_saddle_point->x0,0.0,pcoord); break;
            case CHART_V1: MATHFUNC(V1_to_sphere)(VFResults.selected_saddle_point->x0,0.0,pcoord); break;
            case CHART_U2: MATHFUNC(U2_to_sphere)(VFResults.selected_saddle_point->x0,0.0,pcoord); break;
            case CHART_V2: MATHFUNC(V2_to_sphere)(VFResults.selected_saddle_point->x0,0.0,pcoord); break;
            }

            if( VFResults.plweights == false )
            {
                sx.sprintf( "X= %f", (float)(pcoord[0]) );
                sy.sprintf( "Y= %f", (float)(pcoord[1]) );
                sz.sprintf( "Z= 0" );
            }
            else
            {
                sx.sprintf( "r= %f", (float)(pcoord[1]) );
                sy.sprintf( "theta= %f", (float)(pcoord[2]) );
                sz = "";
            }
        }
        break;

    case SEMI_HYPERBOLIC:
        sepc = VFResults.selected_sep = VFResults.selected_se_point->separatrices;
        draw_selected_sep( spherewnd, VFResults.selected_sep->first_sep_point, CW_SEP );
        
        while( sepc != NULL )
        {
            sepcount++;
            sepc = sepc->next_sep;
        }

        start_plot_sep = start_plot_se_sep;
        cont_plot_sep = cont_plot_se_sep;
        plot_next_sep = plot_next_se_sep;
        select_next_sep = select_next_se_sep;
        s = "SEMI-HYPERBOLIC";
        epsilon = VFResults.selected_se_point->epsilon;
        change_epsilon = change_epsilon_se;
        if( VFResults.selected_se_point->chart == CHART_R2 )
        {
            sx.sprintf( "x=%f", (float)(VFResults.selected_se_point->x0) );
            sy.sprintf( "y=%f", (float)(VFResults.selected_se_point->y0) );
            sz = "";
        } 
        else
        {
            switch(VFResults.selected_se_point->chart)
            {
            case CHART_U1: MATHFUNC(U1_to_sphere)(VFResults.selected_se_point->x0,0.0,pcoord);  break;
            case CHART_V1: MATHFUNC(V1_to_sphere)(VFResults.selected_se_point->x0,0.0,pcoord);  break;
            case CHART_U2: MATHFUNC(U2_to_sphere)(VFResults.selected_se_point->x0,0.0,pcoord);  break;
            case CHART_V2: MATHFUNC(V2_to_sphere)(VFResults.selected_se_point->x0,0.0,pcoord);  break;
            }
            if( VFResults.plweights == false )
            {
                sx.sprintf( "X= %f", (float)(pcoord[0]) );
                sy.sprintf( "Y= %f", (float)(pcoord[1]) );
                sz.sprintf( "Z= 0" );
            }
            else
            {
                sx.sprintf( "r= %f", (float)(pcoord[1]) );
                sy.sprintf( "theta= %f", (float)(pcoord[2]) );
                sz = "";
            }
        }      
        break; 

    case NON_ELEMENTARY:
        VFResults.selected_de_sep = VFResults.selected_de_point->blow_up;
        draw_selected_sep(spherewnd, VFResults.selected_de_sep->first_sep_point,CW_SEP);

        for( bc = VFResults.selected_de_sep; bc != NULL; bc = bc->next_blow_up_point )
            sepcount++;

        start_plot_sep=start_plot_de_sep;
        cont_plot_sep=cont_plot_de_sep;
        plot_next_sep=plot_next_de_sep;
        select_next_sep=select_next_de_sep;
        s = "NON-ELEMENTARY";
        epsilon=VFResults.selected_de_point->epsilon;
        change_epsilon=change_epsilon_de;
        if(VFResults.selected_de_point->chart==CHART_R2)
        {
            sx.sprintf("x=%f",(float)(VFResults.selected_de_point->x0));
            sy.sprintf("y=%f",(float)(VFResults.selected_de_point->y0));
            sz = "";
        } 
        else
        {
            switch(VFResults.selected_de_point->chart)
            {
            case CHART_U1: MATHFUNC(U1_to_sphere)(VFResults.selected_de_point->x0,0.0,pcoord);  break;
            case CHART_V1: MATHFUNC(V1_to_sphere)(VFResults.selected_de_point->x0,0.0,pcoord);  break;
            case CHART_U2: MATHFUNC(U2_to_sphere)(VFResults.selected_de_point->x0,0.0,pcoord);  break;
            case CHART_V2: MATHFUNC(V2_to_sphere)(VFResults.selected_de_point->x0,0.0,pcoord);  break;
            }
            if( VFResults.plweights == false )
            {
                sx.sprintf("X= %f",(float)(pcoord[0]));
                sy.sprintf("Y= %f",(float)(pcoord[1]));
                sz.sprintf("Z= 0");
            }
            else
            {
                sx.sprintf("r= %f",(float)(pcoord[1]));
                sy.sprintf("theta= %f",(float)(pcoord[2]));
                sz = "";
            }
        }
        break;
    }

    CurrentSingularityInfo[0] = s;
    CurrentSingularityInfo[1] = sx;
    CurrentSingularityInfo[2] = sy;
    CurrentSingularityInfo[3] = sz;
    CurrentSeparatriceEpsilon = epsilon;

    return true;
}
