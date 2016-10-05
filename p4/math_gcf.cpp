#include <qfile.h>
#include <math.h>
#include "table.h"
#include "custom.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_sphere.h"

#define EVAL_GCF_NONE               0
#define EVAL_GCF_R2                 1
#define EVAL_GCF_U1                 2
#define EVAL_GCF_U2                 3
#define EVAL_GCF_V1                 4
#define EVAL_GCF_V2                 5
#define EVAL_GCF_FINISHPOINCARE     6
#define EVAL_GCF_LYP_R2             7
#define EVAL_GCF_CYL1               8
#define EVAL_GCF_CYL2               9
#define EVAL_GCF_CYL3               10
#define EVAL_GCF_CYL4               11
#define EVAL_GCF_FINISHLYAPUNOV     12

static int GcfTask = EVAL_GCF_NONE;
static QWinSphere * GcfSphere = nullptr;
static int GcfDashes = 0;
static bool GcfError = false;

struct orbits_points * last_gcf_point = nullptr;

static void insert_gcf_point( double x0, double y0, double z0, int dashes );
static bool RunTask( int, int, int );
static bool ReadTaskResults( int ); // , int, int, int );

static bool read_gcf( void (*chart)(double,double,double *) );

bool EvalGcfStart( QWinSphere * sp, int dashes, int points, int precis )
{
    if( VFResults.gcf_points != nullptr )
    {
        sp->prepareDrawing();
        draw_gcf( sp, VFResults.gcf_points, CBACKGROUND, GcfDashes );
        sp->finishDrawing();
        VFResults.DeleteOrbitPoint( VFResults.gcf_points );
        VFResults.gcf_points = nullptr;
    }

    if( VFResults.plweights )
        GcfTask = EVAL_GCF_LYP_R2;
    else
        GcfTask = EVAL_GCF_R2;

    GcfError = false;
    GcfSphere = sp;
    GcfDashes = dashes;
    return RunTask( GcfTask, points, precis );
}

bool EvalGcfContinue( int points, int prec ) // returns true when finished.  Then run EvalGCfFinish to see if error occurred or not
{
    if( GcfTask == EVAL_GCF_NONE )
        return true;

    if( !ReadTaskResults( GcfTask ) ) // , points, prec, memory ) )
    {
        GcfError = true;
        return true;
    }
    GcfTask++;
    if( GcfTask == EVAL_GCF_FINISHPOINCARE || GcfTask == EVAL_GCF_FINISHLYAPUNOV )
    {
        return true;
    }
    
    if( !RunTask( GcfTask, points, prec ) )
    {
        GcfError = true;
        return true;
    }

    return false; // still busy
}

bool EvalGcfFinish( void )      // return false in case an error occured
{
    if( GcfTask != EVAL_GCF_NONE )
    {
        GcfSphere->prepareDrawing();
        draw_gcf( GcfSphere, VFResults.gcf_points, CSING, 1 );
        GcfSphere->finishDrawing();

        GcfTask = EVAL_GCF_NONE;

        if( GcfError )
        {
            GcfError = false;
            return false;
        }
    }
    return true;
}

bool RunTask( int task, int points, int prec )
{
    bool value;

    switch( task )
    {
    case EVAL_GCF_R2:       value = ThisVF->PrepareGcf( VFResults.gcf, -1, 1, prec, points  ); break;
    case EVAL_GCF_U1:       value = ThisVF->PrepareGcf( VFResults.gcf_U1, 0, 1, prec, points  ); break;
    case EVAL_GCF_V1:       value = ThisVF->PrepareGcf( VFResults.gcf_U1, -1, 0, prec, points  ); break;
    case EVAL_GCF_U2:       value = ThisVF->PrepareGcf( VFResults.gcf_U2, 0, 1, prec, points  ); break;
    case EVAL_GCF_V2:       value = ThisVF->PrepareGcf( VFResults.gcf_U2, -1, 0, prec, points ); break;
    case EVAL_GCF_LYP_R2:   value = ThisVF->PrepareGcf_LyapunovR2(prec, points); break;
    case EVAL_GCF_CYL1:     value = ThisVF->PrepareGcf_LyapunovCyl(-PI_DIV4,PI_DIV4, prec, points); break;
    case EVAL_GCF_CYL2:     value = ThisVF->PrepareGcf_LyapunovCyl( PI_DIV4, PI-PI_DIV4, prec, points); break;
    case EVAL_GCF_CYL3:     value = ThisVF->PrepareGcf_LyapunovCyl( PI-PI_DIV4, PI+PI_DIV4, prec, points); break;
    case EVAL_GCF_CYL4:     value = ThisVF->PrepareGcf_LyapunovCyl(-PI+PI_DIV4,-PI_DIV4, prec, points); break;
    default:        value = false;
        break;
    }

    if( value )
        return ThisVF->EvaluateGcf();
    else
        return false;
}

void rplane_plsphere0( double x, double y, double * pcoord )
{
    R2_to_plsphere(x*cos(y),x*sin(y),pcoord);
}

bool ReadTaskResults( int task ) // , int points, int prec, int memory )
{
    bool value;

    switch( task )
    {
    case EVAL_GCF_R2:       value = read_gcf( R2_to_psphere ); break;
    case EVAL_GCF_U1:       value = read_gcf( U1_to_psphere ); break;
    case EVAL_GCF_V1:       value = read_gcf( VV1_to_psphere ); break;
    case EVAL_GCF_U2:       value = read_gcf( U2_to_psphere ); break;
    case EVAL_GCF_V2:       value = read_gcf( VV2_to_psphere ); break;
    case EVAL_GCF_LYP_R2:   value = read_gcf( rplane_plsphere0 ); break;
                            // here: the old herc files contained (the equivalent of)
                            // value = read_gcf( rplane_psphere );
    case EVAL_GCF_CYL1:     value = read_gcf( cylinder_to_plsphere ); break;
    case EVAL_GCF_CYL2:     value = read_gcf( cylinder_to_plsphere ); break;
    case EVAL_GCF_CYL3:     value = read_gcf( cylinder_to_plsphere ); break;
    case EVAL_GCF_CYL4:     value = read_gcf( cylinder_to_plsphere ); break;
    default:
        value = false;
    }
    
    return value;
}

void draw_gcf( QWinSphere * spherewnd, struct orbits_points * sep, int color, int dashes )
{
    double pcoord[3];

    while( sep != nullptr )
    {
        if(sep->dashes && dashes)
            (*plot_l)(spherewnd,pcoord,sep->pcoord,color);
        else
            (*plot_p)(spherewnd,sep->pcoord,color);
        copy_x_into_y(sep->pcoord,pcoord);

        sep=sep->next_point;
    }
} 

static void insert_gcf_point( double x0, double y0, double z0, int dashes )
{
    if( VFResults.gcf_points != nullptr  )
    {
        last_gcf_point->next_point = new orbits_points;//(struct orbits_points *) malloc(sizeof(struct orbits_points));
        last_gcf_point = last_gcf_point->next_point; 
    }
    else
    {
        last_gcf_point = new orbits_points;//(struct orbits_points *) malloc(sizeof(struct orbits_points));
        VFResults.gcf_points = last_gcf_point;
    }

    last_gcf_point->pcoord[0] = x0;
    last_gcf_point->pcoord[1] = y0;
    last_gcf_point->pcoord[2] = z0;
    
    last_gcf_point->dashes = dashes;
    last_gcf_point->color = CSING;
    last_gcf_point->next_point = nullptr;
}

static bool read_gcf( void (*chart)(double,double,double *) )
{
    int t;
    int k;
    FILE * fp;
    double x, y;
    double pcoord[3];
    int d,c;

    fp = fopen( QFile::encodeName( ThisVF->getfilename_gcf() ), "r" );
    if( fp == nullptr )
        return false;

    if( ThisVF->symbolicpackage == PACKAGE_REDUCE )
    {
        // search the x-label, and check for error.
        
    FILE * fp2;
    fp2 = fopen( QFile::encodeName( ThisVF->getfilename_gcfresults() ), "r" );
    if( fp ==nullptr )
    {
        fclose(fp);
        return false;
    }
        char str[256];
        t = 3;
        while( !feof( fp2 ) )
        {
            fscanf( fp2, "%s", str );
            if( !strcmp( str, "Heap" ) )
            {
                // Reduce ran out of memory
                t=3;
                break;
            }
            if( !strcmp( str, "xlabel" ) )
            {
                fscanf( fp2, "%s", str );
                if( !strcmp( str, "\"x\"") )
                    t = 1;      // order is ok: it is a x versus y plot
                else
                    t = 0;      // order is reversed: it is a y versus x plot
                break;
            }
        }
        fclose(fp2);
        if( t == 3 )
            return false;

        fscanf(fp,"%lf %lf",&x,&y);      // seems we have to skip 8 values
        fscanf(fp,"%lf %lf",&x,&y);
        fscanf(fp,"%lf %lf",&x,&y);
        fscanf(fp,"%lf %lf",&x,&y);
        fscanf(fp,"%lf %lf",&x,&y); 
        if( t != 0 ) chart( x, y, pcoord ); else chart( y, x, pcoord );
        insert_gcf_point(pcoord[0],pcoord[1],pcoord[2],0);
        getc(fp);
        getc(fp);
        c=getc(fp);
        while( !feof(fp) )
        {
            d = 0;
            if( c != '\n' )
            {
                d = 1;
                ungetc(c, fp);
            }
            fscanf(fp,"%lf %lf",&x,&y); 
            if( t != 0 ) chart( x, y, pcoord ); else chart( y, x, pcoord );
            insert_gcf_point(pcoord[0],pcoord[1],pcoord[2],d);
            getc(fp);
            getc(fp);
            c = getc(fp);
        }
    }
    else
    {
        k=0;
        while( 1 )
        {
            d=0;
            while( fscanf( fp, "%lf %lf", &x, &y ) == 2 )
            {
                k++;
                chart(x,y,pcoord);
                insert_gcf_point( pcoord[0], pcoord[1], pcoord[2], d);
                //d=1;
                d=GcfDashes;
            }
            for( c = getc(fp); isspace(c); )
                c =getc(fp);
            if( c != ',' )
                break;
        }
    }
    
    fclose(fp);
    return true;
}
