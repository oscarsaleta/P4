#include "file_tab.h"

#include "file_vf.h"
#include "math_p4.h"
#include "main.h"


/*
    This file contains the code to read the information from reduce/maple.
    The relevant structures for saddles etc are set up here.
*/

// -----------------------------------------------------------------------
//                              QVFStudy CONSTRUCTOR
// -----------------------------------------------------------------------

QVFStudy::QVFStudy()
{
    // initialize vector field structures:

    f_vec_field[0] = nullptr;      f_vec_field[1] = nullptr;
    vec_field_U1[0] = nullptr;     vec_field_U1[1] = nullptr;
    vec_field_U2[0] = nullptr;     vec_field_U2[1] = nullptr;
    vec_field_V1[0] = nullptr;     vec_field_V1[1] = nullptr;
    vec_field_V2[0] = nullptr;     vec_field_V2[1] = nullptr;
    vec_field_C[0] = nullptr;      vec_field_C[1] = nullptr;

    // initialize singular points structures:

    first_saddle_point = nullptr;
    first_se_point = nullptr;
    first_node_point = nullptr;
    first_sf_point = nullptr;
    first_wf_point = nullptr;
    first_de_point = nullptr;

    // initialize GCF:

    gcf = nullptr;
    gcf_U1 = nullptr;
    gcf_U2 = nullptr;
    gcf_V1 = nullptr;
    gcf_V2 = nullptr;
    gcf_C = nullptr;
    gcf_points = nullptr;

    // initialize limit cycles & orbits

    first_lim_cycle = nullptr;
    first_orbit = nullptr;
    current_orbit = nullptr;

    // initialize others

    xmin = -1.0;
    xmax = 1.0;
    ymin = -1.0;
    ymax = 1.0;
    p = 1;
    q = 1;
    typeofstudy = TYPEOFSTUDY_ALL;
    singinf = false;
    VFResults.dir_vec_field = 1;

    // initialize parameters

    config_lc_value = DEFAULT_LCORBITS;         // number of orbits in the limit cycle window
    config_lc_numpoints = DEFAULT_LCPOINTS;     // number of points in the limit cycle window
    config_hma = DEFAULT_HMA;                   // maximum step size
    config_hmi = DEFAULT_HMI;                   // minimum step size
    config_step = DEFAULT_STEPSIZE;             // step size
    config_currentstep = DEFAULT_STEPSIZE;      // current step size (during integration)
    config_tolerance = DEFAULT_TOLERANCE;       // tolerance
    config_projection = DEFAULT_PROJECTION;     // projection in the case of Poincare sphere
    config_intpoints = DEFAULT_INTPOINTS;       // number of points to integrate
    config_dashes = DEFAULT_LINESTYLE;          // line style (dashes or points)
    config_kindvf = DEFAULT_INTCONFIG;
}


// -----------------------------------------------------------------------
//                              QVFStudy DESTRUCTOR
// -----------------------------------------------------------------------

QVFStudy::~QVFStudy()
{
    DeleteVF();
}

// -----------------------------------------------------------------------
//                          QVFStudy::DeleteVF
// -----------------------------------------------------------------------

void QVFStudy::DeleteVF()
{
    // Delete Vector Field

    delete_term2( f_vec_field[0] );     delete_term2( f_vec_field[1] );
    delete_term2( vec_field_U1[0] );    delete_term2( vec_field_U1[1] );
    delete_term2( vec_field_U2[0] );    delete_term2( vec_field_U2[1] );
    delete_term2( vec_field_V1[0] );    delete_term2( vec_field_V1[1] );
    delete_term2( vec_field_V2[0] );    delete_term2( vec_field_V2[1] );
    delete_term3( vec_field_C[0] );     delete_term3( vec_field_C[1] );

    f_vec_field[0] = nullptr;      f_vec_field[1] = nullptr;
    vec_field_U1[0] = nullptr;     vec_field_U1[1] = nullptr;
    vec_field_U2[0] = nullptr;     vec_field_U2[1] = nullptr;
    vec_field_V1[0] = nullptr;     vec_field_V1[1] = nullptr;
    vec_field_V2[0] = nullptr;     vec_field_V2[1] = nullptr;
    vec_field_C[0] = nullptr;      vec_field_C[1] = nullptr;

    // Delete singular points

    DeleteSaddle( first_saddle_point );
    DeleteSemiElementary( first_se_point );
    DeleteNode( first_node_point );
    DeleteDegenerate( first_de_point );
    DeleteStrongFocus( first_sf_point );
    DeleteWeakFocus( first_wf_point );

    first_saddle_point = nullptr;
    first_se_point = nullptr;
    first_node_point = nullptr;
    first_sf_point = nullptr;
    first_wf_point = nullptr;
    first_de_point = nullptr;

    // Delete GCF:

    delete_term2( gcf );
    delete_term2( gcf_U1 );
    delete_term2( gcf_U2 );
    delete_term2( gcf_V1 );
    delete_term2( gcf_V2 );
    delete_term3( gcf_C );
    DeleteOrbitPoint( gcf_points );

    gcf = nullptr;
    gcf_U1 = nullptr;
    gcf_U2 = nullptr;
    gcf_V1 = nullptr;
    gcf_V2 = nullptr;
    gcf_C = nullptr;
    gcf_points = nullptr;

    // Delete all orbits

    DeleteOrbit( first_orbit );
    first_orbit = nullptr;
    current_orbit = nullptr;

    // Delete limit cycles

    DeleteLimitCycle( first_lim_cycle );
    first_lim_cycle = nullptr;

    // reset others

    xmin = -1.0;
    xmax = 1.0;
    ymin = -1.0;
    ymax = 1.0;
    p = 1;
    q = 1;
    typeofstudy = TYPEOFSTUDY_ALL;
    singinf = false;
    VFResults.dir_vec_field = 1;

    lasterror = "";
}

// -----------------------------------------------------------------------
//                          QVFStudy::DeleteSaddle
// -----------------------------------------------------------------------

void QVFStudy::DeleteSaddle( struct saddle * p )
{
    struct saddle * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_saddle;
    
        delete_term2( q->vector_field[0] );
        delete_term2( q->vector_field[1] );
        if( q->notadummy )
            DeleteSeparatrices( q->separatrices );  // separatrices may be nullptr
        //free(q);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteSemiElementary
// -----------------------------------------------------------------------

void QVFStudy::DeleteSemiElementary( struct semi_elementary * p )
{
    struct semi_elementary * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_se;
    
        delete_term2( q->vector_field[0] );
        delete_term2( q->vector_field[1] );
        if( q->notadummy )
            DeleteSeparatrices( q->separatrices );  // separatrices may be nullptr
        //free(q);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                          QVFStudy::DeleteNode
// -----------------------------------------------------------------------

void QVFStudy::DeleteNode( struct node * p )
{
    struct node * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_node;
        delete q;
        q = nullptr;
        //free(q);
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteStrongFocus
// -----------------------------------------------------------------------

void QVFStudy::DeleteStrongFocus( struct strong_focus * p )
{
    struct strong_focus * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_sf;
        //free(q);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                          QVFStudy::DeleteWeakFocus
// -----------------------------------------------------------------------

void QVFStudy::DeleteWeakFocus( struct weak_focus * p )
{
    struct weak_focus * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_wf;
        //free(q);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteDegenerate
// -----------------------------------------------------------------------

void QVFStudy::DeleteDegenerate( struct degenerate * p )
{
    struct degenerate * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_de;

        if( q->notadummy )
            DeleteBlowup( q->blow_up );
        //free(q);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteSeparatrices
// -----------------------------------------------------------------------

void QVFStudy::DeleteSeparatrices( struct sep * p )
{
    struct sep * q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_sep;

        DeleteOrbitPoint( q->first_sep_point );
        if( q->notadummy )
            delete_term1( q->separatrice );
        //free( q );
        delete q;
        q = nullptr;
    }
}
   
// -----------------------------------------------------------------------
//                      QVFStudy::DeleteTransformations
// -----------------------------------------------------------------------

void QVFStudy::DeleteTransformations( struct transformations * t )
{
    struct transformations * u;

    while( t != nullptr )
    {
        u = t;
        t = t->next_trans;
        //free(u);
        delete u;
        u = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteBlowup
// -----------------------------------------------------------------------

void QVFStudy::DeleteBlowup( struct blow_up_points * b )
{
    struct blow_up_points * c;

    while( b != nullptr )
    {
        c = b;
        b = b->next_blow_up_point;
        DeleteTransformations( c->trans );
        delete_term2( c->vector_field[0] );
        delete_term2( c->vector_field[1] );
        delete_term1( c->sep );
        DeleteOrbitPoint( c->first_sep_point );
        //free(c);
        delete c;
        c = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteLimitCycle
// -----------------------------------------------------------------------

void QVFStudy::DeleteLimitCycle( struct orbits * p )
{
    DeleteOrbit(p); // limit cycle is implemented as orbit.
}

// -----------------------------------------------------------------------
//                  QVFStudy::DeleteOrbitPoint
// -----------------------------------------------------------------------

void QVFStudy::DeleteOrbitPoint( P4ORBIT p )
{
    P4ORBIT q;
    
    while( p != nullptr )
    {
        q = p;
        p = p->next_point;

        //free(q);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      QVFStudy::DeleteOrbit
// -----------------------------------------------------------------------

void QVFStudy::DeleteOrbit( struct orbits * p )
{
    struct orbits * q;

    while( p !=nullptr )
    {
        q = p;
        p = p->next_orbit;
        
        DeleteOrbitPoint( q->f_orbits );
        //free(q);
        delete q;
        q = nullptr;
   }
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTables
// -----------------------------------------------------------------------
//
// read filename_vec.tab
// read filename_inf.tab
// read filename_fin.tab

bool QVFStudy::ReadTables( QString basename )
{
    FILE * fp;
    int j;
    int flag;

    DeleteVF();     // initialize structures, delete previous vector field if any

    fp = fopen( QFile::encodeName( basename + "_vec.tab" ), "rt" );
    if( fp == nullptr )
    {
        Dump(basename,"Cannot open file *_vec.tab");
        DeleteVF();
        return false;
    }

    if( fscanf( fp, "%d %d %d ", &typeofstudy, &p, &q ) != 3 )
    {
        Dump(basename,"Cannot read typeofstudy in *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( typeofstudy == TYPEOFSTUDY_ONE )
    {
        if( fscanf( fp, "%lf %lf %lf %lf", &xmin, &xmax, &ymin, &ymax ) != 4 )
        {
            Dump(basename,"Cannot read min-max coords in *_vec.tab");
            DeleteVF();
            fclose(fp);
            return false;
        }
        p = q = 1;
        typeofview = TYPEOFVIEW_PLANE;
    }
    else
        typeofview = TYPEOFVIEW_SPHERE;

    plweights = (( p == 1 && q == 1 ) ? false : true);

    double_p = (double)p;
    double_q = (double)q;
    double_p_plus_q = (double)(p+q);
    double_p_minus_1 = (double)(p-1);
    double_q_minus_1 = (double)(q-1);
    double_q_minus_p = (double)(q-p);

    if( !ReadGCF( fp ) )
    {
        Dump(basename,"Cannot read gcf *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( !ReadVectorField( fp, f_vec_field ) )
    {
        Dump(basename,"Cannot read vector field in *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( !ReadVectorField( fp, vec_field_U1 ) )
    {
        Dump(basename,"Cannot read vector field in U1-chart in *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( !ReadVectorField( fp, vec_field_V1 ) )
    {
        Dump(basename,"Cannot read vector field in V1-chart in *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( !ReadVectorField( fp, vec_field_U2 ) )
    {
        Dump(basename,"Cannot read vector field in U2-chart in *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( !ReadVectorField( fp, vec_field_V2 ) )
    {
        Dump(basename,"Cannot read vector field in V2-chart in *_vec.tab");
        DeleteVF();
        fclose(fp);
        return false;
    }

    if( plweights )
    {
        if( !ReadVectorFieldCylinder( fp, vec_field_C ) )
        {
            Dump(basename,"Cannot read vector field in Cylinder-chart in *_vec.tab");
            DeleteVF();
            fclose(fp);
            return false;
        }
        singinf = 0;
    }
    else
    {
        if( fscanf( fp, "%d %d", &flag, &VFResults.dir_vec_field ) != 2 )
        {
            Dump(basename,"Cannot read sing-at-infinity flag and directions flag in *_vec.tab");
            DeleteVF();
            fclose(fp);
            return false;
        }
        singinf = ((flag == 0) ? false : true);
    }

    fclose( fp );

    if( typeofstudy != TYPEOFSTUDY_INF )
    {
        fp = fopen( QFile::encodeName( basename + "_fin.tab" ), "rt" );
        if( fp != nullptr )
        {
            if( !ReadPoints( fp ) )
            {
                Dump(basename,QString("Problem reading singularity info from *_fin.tab:") + lasterror );
                DeleteVF();
                fclose( fp );
                return false;
            }
            fclose( fp );
        }
        else
        {
            Dump(basename,"Cannot open *_fin.tab");
            DeleteVF();
            return false;
        }
    }

    if( typeofstudy != TYPEOFSTUDY_ONE && typeofstudy != TYPEOFSTUDY_FIN )
    {
        fp = fopen( QFile::encodeName( basename + "_inf.tab" ), "rt" );
        if( fp != nullptr )
        {
            if( p==1 && q==1 )
            {
                for( j = 1; j <= 2; j++ )
                {
                    if( !ReadPoints( fp ) )
                    {
                        Dump(basename,QString("Cannot read singular points in *_inf.tab (")+QString::number(j)+"):" + lasterror );
                        DeleteVF();
                        fclose(fp);
                        return false;
                    }
                }
            }
            else
            {
                for( j = 1; j <= 4; j++ )
                {
                    if( !ReadPoints( fp ) )
                    {
                        Dump(basename,QString("Cannot read singular points in *_inf.tab (")+QString::number(j)+"):" + lasterror );
                        DeleteVF();
                        fclose(fp);
                        return false;
                    }
                }
            }
            fclose(fp);
        }
        else
        {
            Dump(basename,"Cannot open *_inf.tab");
            DeleteVF();
            return false;
        }
    }

    Dump(basename, "all's well.");
    return true;
} 

// -----------------------------------------------------------------------
//                      QVFStudy::ReadGCF
// -----------------------------------------------------------------------

bool QVFStudy::ReadGCF( FILE * fp )
{
    int N, degree_gcf;

    if( fscanf( fp, "%d", &degree_gcf ) != 1 )
        return false;

    if( degree_gcf )
    {
        if( fscanf( fp, "%d", &N ) != 1 )
            return false;

        gcf = new term2;//(P4POLYNOM2)malloc( sizeof(struct term2) );
        gcf->next_term2 = nullptr;

        if( !ReadTerm2( fp, gcf, N ) )
            return false;

        if( fscanf( fp, "%d", &N ) != 1 )
            return false;

        gcf_U1= new term2;//(P4POLYNOM2)malloc( sizeof(struct term2) );
        gcf_U1->next_term2 = nullptr;

        if( !ReadTerm2( fp, gcf_U1, N ) )
            return false;

        if( fscanf( fp, "%d", &N ) != 1 )
            return false;

        gcf_U2= new term2;//(P4POLYNOM2)malloc( sizeof(struct term2) );
        gcf_U2->next_term2 = nullptr;

        if( !ReadTerm2( fp, gcf_U2, N ) )
            return false;

        if( fscanf( fp, "%d", &N ) != 1 )
            return false;

        gcf_V1= new term2;//(P4POLYNOM2)malloc( sizeof(struct term2) );
        gcf_V1->next_term2 = nullptr;
        if( !ReadTerm2( fp, gcf_V1, N ) )
            return false;
        
        if( fscanf( fp, "%d", &N ) != 1 )
            return false;
        gcf_V2= new term2;//(P4POLYNOM2)malloc( sizeof(struct term2) );
        gcf_V2->next_term2 = nullptr;
        if( !ReadTerm2( fp, gcf_V2, N ) )
            return false;

        if( p!=1 || q!=1 )
        {
            if( fscanf( fp, "%d", &N ) != 1 )
                return false;

            gcf_C = new term3;// (P4POLYNOM3)malloc( sizeof(struct term3) );
            gcf_C->next_term3 = nullptr;
            if( !ReadTerm3( fp, gcf_C, N ) )
                return false;
        }
    }
    else
    {
        gcf = nullptr;
        gcf_U1 = nullptr;
        gcf_U2 = nullptr;
        gcf_V1 = nullptr;
        gcf_V2 = nullptr;
        gcf_C = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadVectorField
// -----------------------------------------------------------------------

bool QVFStudy::ReadVectorField( FILE * fp, P4POLYNOM2 * vf )
{
    int M,N;

    vf[0] = new term2;//(P4POLYNOM2)malloc( sizeof( struct term2 ) );
    vf[0]->next_term2 = nullptr;
    vf[1] = new term2;//(P4POLYNOM2)malloc( sizeof( struct term2 ) );
    vf[1]->next_term2 = nullptr;

    if( fscanf( fp, "%d", &M ) != 1 )
        return false;
    if( !ReadTerm2( fp, vf[0], M ) )
        return false;
    if( fscanf( fp, "%d", &N ) != 1 )
        return false;
    if( !ReadTerm2( fp, vf[1], N ) )
        return false;

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadVectorFieldCylinder
// -----------------------------------------------------------------------

bool QVFStudy::ReadVectorFieldCylinder( FILE * fp, P4POLYNOM3 * vf )
{
    int N;

    vf[0] = new term3;// (P4POLYNOM3)malloc( sizeof( struct term3 ) ); 
    vf[0]->next_term3 = nullptr;
    vf[1] = new term3;//(P4POLYNOM3)malloc( sizeof( struct term3 ) ); 
    vf[1]->next_term3 = nullptr;

    if( fscanf( fp, "%d", &N ) != 1 )
        return false;
    if( !ReadTerm3( fp, vf[0], N ) )
        return false;
    if( fscanf( fp, "%d", &N ) != 1 )
        return false;
    if( !ReadTerm3( fp, vf[1], N ) )
        return false;

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadPoints
// -----------------------------------------------------------------------

bool QVFStudy::ReadPoints( FILE * fp )
{
    int N, i, typ;

    if( fscanf( fp, "%d ", &N ) != 1 )
    {
        lasterror = "#sing not readable";
        return false;
    }

    for( i = 1; i <= N; i++ )
    {
        if( fscanf( fp, "%d ", &typ ) != 1 )
        {
            lasterror = QString( "sing #") + QString::number(i) + " type not readable";
            return false;
        }
        switch( typ )
        {
        case SADDLE:
            if( !ReadSaddlePoint( fp ) )
            {
                lasterror = QString( "sing #") + QString::number(i) + " = saddle : " + lasterror;
                return false;
            }
            break;
        case SEMI_HYPERBOLIC:
            if( !ReadSemiElementaryPoint( fp ) )
            {
                lasterror = QString( "sing #") + QString::number(i) + " = semi-el : " + lasterror;
                return false;
            }
            break;
        case NODE:
            if( !ReadNodePoint( fp ) )
            {
                lasterror = QString( "sing #") + QString::number(i) + " = node : " + lasterror;
                return false;
            }
            break;
        case STRONG_FOCUS:
            if( !ReadStrongFocusPoint( fp ) )
            {
                lasterror = QString( "sing #") + QString::number(i) + " = strongfocus : " + lasterror;
                return false;
            }
            break;
        case WEAK_FOCUS:
            if( !ReadWeakFocusPoint( fp ) )
            {
                lasterror = QString( "sing #") + QString::number(i) + " = weakfocus : " + lasterror;
                return false;
            }
            break;
        case NON_ELEMENTARY:
            if( !ReadDegeneratePoint( fp ) )
            {
                lasterror = QString( "sing #") + QString::number(i) + " = degen : " + lasterror;
                return false;
            }
            break;
        default:
            lasterror = QString( "sing #") + QString::number(i) + " type not exist (" + QString::number(typ) + ")";
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTerm1
// -----------------------------------------------------------------------

bool QVFStudy::ReadTerm1( FILE * fp, P4POLYNOM1 p, int N )
{
    int i;
    P4POLYNOM1 _p;

    _p = p;

    p->next_term1 = nullptr;
    if( fscanf( fp, "%d %lf", &(p->exp), &(p->coeff) ) != 2 )
        return false;

    for( i = 2; i <= N; i++ )
    {
        p->next_term1 = new term1;//(P4POLYNOM1)malloc( sizeof( struct term1 ) );
        p = p->next_term1;
        p->next_term1 = nullptr;
        if( fscanf( fp, "%d %lf", &(p->exp), &(p->coeff) ) != 2 )
        {
            delete_term1( _p->next_term1 );
            _p->next_term1 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTerm2
// -----------------------------------------------------------------------

bool QVFStudy::ReadTerm2( FILE * fp, P4POLYNOM2 p, int N )
{
    int i;
    P4POLYNOM2 _p;

    _p = p;
    p->next_term2 = nullptr;
    if( fscanf( fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff) ) != 3 )
        return false;

    for( i = 2; i <= N; i++ )
    {
        p->next_term2 = new term2;//(P4POLYNOM2)malloc( sizeof( struct term2 ) );
        p = p->next_term2;
        p->next_term2 = nullptr;
        if( fscanf( fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff) ) != 3 )
        {
            delete_term2( _p->next_term2 );
            _p->next_term2 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTerm3
// -----------------------------------------------------------------------

bool QVFStudy::ReadTerm3( FILE * fp, P4POLYNOM3 p, int N )
{
    int i;
    P4POLYNOM3 _p;
    _p = p;
    p->next_term3 = nullptr;
    if( fscanf( fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si), &(p->coeff) ) != 4 )
        return false;

    for( i = 2; i <= N; i++ )
    {
        p->next_term3 = new term3;// (P4POLYNOM3)malloc( sizeof( struct term3 ) );
        p = p->next_term3;
        p->next_term3 = nullptr;
        if( fscanf( fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si), &(p->coeff) ) != 4 )
        {
            delete_term3( _p->next_term3 );
            _p->next_term3 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadSaddlePoint
// -----------------------------------------------------------------------

bool QVFStudy::ReadSaddlePoint( FILE * fp )
{
    int N;
    struct sep * sep1;
    struct sep * sep2;

    // make room in structure

    struct saddle * last;
    struct saddle * point;

    last = nullptr;
    point = first_saddle_point;
    while( point != nullptr )
    {
        last = point;
        point = point->next_saddle;
    }

    point = new saddle;//(struct saddle *)malloc( sizeof(struct saddle) );
    if( last == nullptr )
        first_saddle_point = point;
    else
        last->next_saddle = point;

    point->next_saddle = nullptr;

    // fill structure

    if( fscanf( fp, "%lf %lf", &(point->x0), &(point->y0) ) != 2 )
    {
        return false;
    }
    if( fscanf( fp, "%lf %lf %lf %lf", &(point->a11), &(point->a12), &(point->a21), &(point->a22) ) != 4 )
    {
        return false;
    }

    ReadVectorField( fp, point->vector_field );
    if( fscanf( fp, "%d ", &(point->chart) ) != 1 )
    {
        return false;
    }
    point->separatrices = new sep;//(struct sep *)malloc( sizeof(struct sep) );
    sep1 = point->separatrices;
    if( fscanf( fp, "%d ", &(sep1->type) ) != 1 )
    {
        return false;
    }
    if( fscanf( fp, "%d ", &N ) != 1 )
    {
        return false;
    }
    sep1->notadummy = true;
    sep1->separatrice = new term1;//(P4POLYNOM1)malloc( sizeof(struct term1) );
    ReadTerm1( fp, sep1->separatrice, N );
    sep1->direction = 1;
    sep1->d = 0;
    sep1->first_sep_point = nullptr;
    sep1->last_sep_point = nullptr;
    sep1->next_sep = nullptr;

    if( point->chart == CHART_R2 || singinf )
    {
        // point is finite hence we have 4 separatrices or we have a line of singularities
        // at infinity and hence we have also 4 separatrices after removing the line

        sep2 = new sep;//(struct sep *)malloc( sizeof(struct sep) );
        sep1->next_sep = sep2;
        
        sep2->type = sep1->type;
        sep2->direction = -1;
        sep2->d = 0;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
        sep2->first_sep_point = nullptr;
        sep2->last_sep_point = nullptr;

        sep1 = sep2->next_sep = new sep;//(struct sep *)malloc( sizeof(struct sep) );

        if( fscanf( fp, "%d", &(sep1->type) ) != 1 )
        {
            return false;
        }
        if( fscanf( fp, "%d ", &N ) != 1 )
        {
            return false;
        }

        sep1->notadummy = true;
        sep1->separatrice = new term1;//(P4POLYNOM1)malloc( sizeof(struct term1) );
        ReadTerm1( fp, sep1->separatrice, N );
        sep1->direction = 1;
        sep1->d = 1;
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;

        sep2 = new sep;//(struct sep *)malloc( sizeof(struct sep) );
        sep1->next_sep = sep2;

        sep2->type = sep1->type;
        sep2->direction = -1;
        sep2->d = 1;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
        sep2->first_sep_point = nullptr;
        sep2->last_sep_point = nullptr;
        sep2->next_sep = nullptr;
    }

    if( fscanf( fp, "%lf ", &(point->epsilon) ) != 1 )
    {
        return false;
    }
    point->notadummy = true;
 
    // line at infinity a line of singularities in poincare disc
 
    if( singinf && point->chart != CHART_R2 )
    {
        last = point;
        point = new saddle;//(struct saddle *)malloc( sizeof(struct saddle) );
        last->next_saddle = point;
        point->next_saddle = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = ((last->chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point->notadummy = false;
        point->separatrices = last->separatrices;
        point->epsilon = last->epsilon;
        point->a11 = last->a11;
        point->a12 = last->a12;
        point->a21 = last->a21;
        point->a22 = last->a22;
        point->vector_field[0] = nullptr;
        point->vector_field[1] = nullptr;
    } 

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadSemiElementaryPoint
// -----------------------------------------------------------------------

bool QVFStudy::ReadSemiElementaryPoint( FILE * fp )
{
    // make room in structure

    struct semi_elementary * last;
    struct semi_elementary * point;
    int N;
    int s;
    struct sep * sep1;
    double y[2];
    bool ok;

    last = nullptr;
    point = first_se_point;
    while( point != nullptr )
    {
        last = point;
        point = point->next_se;
    }

    point = new semi_elementary;//(struct semi_elementary *)malloc( sizeof(struct semi_elementary) );
    if( last == nullptr )
        first_se_point = point;
    else
        last->next_se = point;

    point->next_se = nullptr;

    if( fscanf( fp, "%lf %lf ", &(point->x0), &(point->y0) ) != 2 )
        return false;
    if( fscanf( fp, "%lf %lf %lf %lf ", &(point->a11), &(point->a12), &(point->a21), &(point->a22) ) != 4 )
        return false;
    ReadVectorField( fp, point->vector_field );
    if( fscanf( fp, "%d %d %d", &(point->type), &s, &(point->chart) ) != 3 )
        return false;

    switch( point->type )
    {
    case 1: // saddle-node 
        if( s && point->chart != CHART_R2 && !singinf )
        {
            // hyperbolic sep = line at infinity, not reported
            // center sep in the wrong direction
            // so no separatrices
            point->separatrices = nullptr;
        }
        else
        {
            // in the finite region (always s=1), the first sep is the center sep, then the hyperbolic sep
            // at infinity, we  have s=0, so the center sep is the line at infinity, not reported so only sep is
            // then the hyperbolic sep.

            point->separatrices = new sep;//(struct sep *)malloc( sizeof(struct sep) );
            sep1 = point->separatrices;
            
            if( s )
                sep1->type = OT_CENT_STABLE;
            else
                sep1->type = OT_UNSTABLE;

            sep1->d = 0;
            if(((p==1) && (q==1) && ((point->chart == CHART_V1) || (point->chart==CHART_V2))) ||
                (point->chart==CHART_R2 || singinf) )
            {
                sep1->direction = -1;
            }
            else
            {
                sep1->direction = 1;
            }

            if( fscanf( fp, "%d ", &N ) != 1 )
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;// (struct term1 *)malloc( sizeof(struct term1) );
            ReadTerm1( fp, sep1->separatrice, N );
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = nullptr;
            if( point->chart == CHART_R2 || singinf)
            {
                // read second (hyperbolic) separatrix

                sep1->next_sep = new sep;// (struct sep *)malloc( sizeof(struct sep) );
                sep1 = sep1->next_sep;
                sep1->type = OT_UNSTABLE;
                sep1->d = 1;
                sep1->direction = 1;
                if( fscanf( fp, "%d ", &N ) != 1 )
                    return false;
                sep1->notadummy = true;
                sep1->separatrice = new term1;//(struct term1 *)malloc( sizeof(struct term1) );
                ReadTerm1( fp, sep1->separatrice, N );
                sep1->first_sep_point = nullptr;
                sep1->last_sep_point = nullptr;

                // it is two-sided, so make a copy in other direction

                sep1->next_sep = new sep;//(struct sep *)malloc( sizeof(struct sep) );
                sep1->next_sep->type = OT_UNSTABLE;
                sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1;
                sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
                sep1->next_sep->first_sep_point = nullptr;
                sep1->next_sep->last_sep_point = nullptr;
                sep1->next_sep->next_sep = nullptr;
            }
        }
        break;


    case 2: // saddle-node 
        point->separatrices = new sep;// (struct sep *) malloc(sizeof(struct sep));
        sep1 = point->separatrices;
        if( s )
            sep1->type = STYPE_CENUNSTABLE;
        else
            sep1->type = STYPE_STABLE;

        sep1->d = 0; 
        if( (p==1) && (q==1) && ((point->chart==CHART_V1) || (point->chart==CHART_V2)) )
            sep1->direction = -1;
        else
            sep1->direction = 1;

        if( fscanf( fp, "%d ", &N ) != 1 )
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
        ReadTerm1( fp, sep1->separatrice, N );
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if( point->chart == CHART_R2 || singinf )
        {
            sep1->next_sep = new sep;//(struct sep *) malloc( sizeof(struct sep) );
            sep1 = sep1->next_sep;
            sep1->type = STYPE_STABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if( fscanf( fp, "%d ", &N ) !=1 )
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;//(struct term1 *)malloc( sizeof(struct term1) );
            ReadTerm1( fp, sep1->separatrice, N );
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;//(struct sep *)malloc(sizeof(struct sep));
            sep1->next_sep->type = STYPE_STABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 3: // saddle-node 
        point->separatrices = new sep;//(struct sep *) malloc(sizeof(struct sep));
        sep1 = point->separatrices;
        if( s )
            sep1->type = STYPE_CENSTABLE;
        else
            sep1->type = STYPE_UNSTABLE;
        sep1->d = 0;
        if( (p==1) && (q==1) && ((point->chart==CHART_V1) || (point->chart==CHART_V2)) )
            sep1->direction = -1;
        else
            sep1->direction = 1;

        if( fscanf( fp, "%d ", &N ) != 1 )
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
        ReadTerm1( fp, sep1->separatrice, N );
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if( point->chart == CHART_R2 || singinf )
        {
            sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1 = sep1->next_sep;
            sep1->type = STYPE_UNSTABLE; sep1->d = 1;
            sep1->direction = 1;
            if( fscanf( fp, "%d ", &N ) !=1 )
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
            ReadTerm1( fp, sep1->separatrice, N );
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1->next_sep->type = STYPE_UNSTABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;   

    case 4: // saddle-node 
        if( s && (point->chart != CHART_R2) && !singinf )
        {
            point->separatrices = nullptr;
        }
        else
        {
            point->separatrices = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1 = point->separatrices;
            if( s )
                sep1->type = STYPE_CENUNSTABLE;
            else
                sep1->type = STYPE_STABLE;

            sep1->d = 0; 
            if(((p == 1) && (q == 1) && ((point->chart == CHART_V1) || (point->chart == CHART_V2))) ||
                (point->chart == CHART_R2 || singinf ))
                sep1->direction = -1;
            else
                sep1->direction = 1;

            if( fscanf( fp, "%d ", &N ) != 1 )
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
            ReadTerm1( fp, sep1->separatrice, N );
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = nullptr;
            if( point->chart == CHART_R2 || singinf )
            {
                sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
                sep1 = sep1->next_sep;
                sep1->type = STYPE_STABLE;
                sep1->d = 1;
                sep1->direction = 1;
                if( fscanf( fp, "%d ", &N ) !=1 )
                    return false;
                sep1->notadummy = true;
                sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
                ReadTerm1( fp, sep1->separatrice, N );
                sep1->first_sep_point = nullptr;
                sep1->last_sep_point = nullptr;
                sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
                sep1->next_sep->type = STYPE_STABLE;
                sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1;
                sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
                sep1->next_sep->first_sep_point = nullptr;
                sep1->next_sep->last_sep_point = nullptr;
                sep1->next_sep->next_sep = nullptr;
            }
        } 
        break;   

    case 6: // saddle 
        point->separatrices = new sep;//(struct sep *) malloc(sizeof(struct sep));
        sep1 = point->separatrices;
        if( s )
            sep1->type = STYPE_CENUNSTABLE;
        else
            sep1->type = STYPE_STABLE;
        sep1->d = 0; 
        if( (p == 1) && (q == 1) && ((point->chart== CHART_V1) || (point->chart== CHART_V2)) )
            sep1->direction = -1;
        else
            sep1->direction = 1;
        if( fscanf( fp, "%d ", &N ) !=1 )
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
        ReadTerm1( fp, sep1->separatrice, N );
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if( point->chart== CHART_R2 || singinf )
        {
            sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1->next_sep->type = STYPE_CENUNSTABLE;
            sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1 = sep1->next_sep->next_sep;
            sep1->type = STYPE_STABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if( fscanf( fp, "%d ", &N ) != 1 )
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
            ReadTerm1( fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1->next_sep->type = STYPE_STABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 7: // saddle 
        point->separatrices = new sep;//(struct sep *) malloc(sizeof(struct sep));
        sep1 = point->separatrices;
        if( s )
            sep1->type = STYPE_CENSTABLE;
        else
            sep1->type = STYPE_UNSTABLE;
        sep1->d = 0; 
        if( (p==1) && (q==1) && ((point->chart==CHART_V1) || (point->chart==CHART_V2)) )
            sep1->direction = -1;
        else
            sep1->direction = 1;
        if( fscanf( fp, "%d ", &N ) != 1 )
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
        ReadTerm1( fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if( point->chart== CHART_R2 || singinf )
        {
            sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1->next_sep->type = STYPE_CENSTABLE;
            sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1 = sep1->next_sep->next_sep;
            sep1->type = STYPE_UNSTABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if( fscanf( fp, "%d ",&N ) != 1 )
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;//(struct term1 *) malloc(sizeof(struct term1));
            ReadTerm1( fp, sep1->separatrice, N );
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;//(struct sep *) malloc(sizeof(struct sep));
            sep1->next_sep->type = STYPE_UNSTABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    default:
        point->separatrices = nullptr; 

        // change type of node if we have a gcf ? 
        y[0] = point->x0;
        y[1] = point->y0;
        ok = false;
        switch( point->chart )
        {
        case CHART_R2:
            if( eval_term2( gcf, y ) < 0 )
                ok = true;
            break;
        case CHART_U1:
            if( eval_term2( gcf_U1, y ) < 0 )
                ok = true;
            break;
        case CHART_V1:
            if((p==1) && (q==1))
                y[0] = -y[0];
            if( eval_term2( gcf_V1, y ) < 0 )
                ok = true;
            break;        
        case CHART_U2:
            if( eval_term2( gcf_U2, y ) < 0 )
                ok = true;
            break;
        case CHART_V2:
            if((p==1) && (q==1))
                y[0] = -y[0];
            if( eval_term2( gcf_V2, y ) < 0 )
                ok = true;
            break;  
        }
        if( ok )
        {
            if( point->type == 5 )
                point->type = 8;
            else
                point->type = 5;
        }
    }

    if( fscanf( fp, "%lf ", &(point->epsilon) ) != 1 )
        return false;
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc 

    if( singinf && (point->chart != CHART_R2) )
    {
        point->next_se = new semi_elementary;//(struct semi_elementary *) malloc(sizeof(struct semi_elementary));
        point->next_se->x0 = point->x0;
        point->next_se->y0 = 0.0;
        if( VFResults.dir_vec_field == 1 )
            point->next_se->type = point->type;
        else
            switch( point->type )
            {
            case 1: point->next_se->type = 3; break;
            case 2: point->next_se->type = 4; break;
            case 3: point->next_se->type = 1; break;
            case 4: point->next_se->type = 2; break;
            case 6: point->next_se->type = 7; break;
            case 7: point->next_se->type = 6; break;
            default:point->next_se->type = point->type;
            }

        point->next_se->chart = (point->chart== CHART_U1) ? CHART_V1: CHART_V2;
        point->next_se->notadummy = false;
        point->next_se->separatrices = point->separatrices; 
        point->next_se->epsilon = point->epsilon;
        point->next_se->a11 = point->a11;
        point->next_se->a12 = point->a12;
        point->next_se->a21 = point->a21;
        point->next_se->a22 = point->a22;
        point->next_se->vector_field[0] = nullptr;
        point->next_se->vector_field[1] = nullptr;
        point->next_se->next_se = nullptr;
    } 
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadStrongFocusPoint
// -----------------------------------------------------------------------

bool QVFStudy::ReadStrongFocusPoint( FILE * fp )
{
    double y[2];

    // make room in structure

    struct strong_focus * last;
    struct strong_focus * point;

    last = nullptr;
    point = first_sf_point;
    while( point != nullptr )
    {
        last = point;
        point = point->next_sf;
    }

    point = new strong_focus;//(struct strong_focus *)malloc( sizeof(struct strong_focus) );
    if( last == nullptr )
        first_sf_point = point;
    else
        last->next_sf = point;

    point->next_sf = nullptr;

    // fill structure

    if( fscanf( fp, "%d %lf %lf ", &(point->stable), &(point->x0), &(point->y0) ) != 3 )
        return false;
    if( fscanf( fp, "%d ", &(point->chart) ) != 1 )
        return false;

    // change type of node if we have a gcf ? 

    y[0] = point->x0;
    y[1] = point->y0;

    switch( point->chart )
    {
    case CHART_R2:
        if( eval_term2( gcf, y ) < 0 )
            point->stable *= -1;
        break;

    case CHART_U1:
        if( eval_term2( gcf_U1, y ) < 0 )
            point->stable *= -1;
        break;

    case CHART_V1:
        if( p==1 && q==1 )
            y[0] =- y[0];

        if( eval_term2( gcf_V1, y ) < 0 )
            point->stable *= -1;
        break;

    case CHART_U2:
        if( eval_term2( gcf_U2, y ) < 0 )
            point->stable *= -1;
        break;

    case CHART_V2:
        if( p==1 && q==1 )
            y[0] = -y[0];

        if( eval_term2( gcf_V2, y ) < 0 )
            point->stable *= -1;
        break;
    }

    // line at infinity a line of singularities in poincare disc
    
    if( singinf && point->chart != CHART_R2 )
    {
        last = point;
        point = new strong_focus;//(struct strong_focus *)malloc( sizeof(struct strong_focus) );
        last->next_sf = point;
        point->next_sf = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (( point->chart == CHART_U1 ) ? CHART_V1 : CHART_V2 );
        point->stable = last->stable * ((VFResults.dir_vec_field == -1) ? -1 : 1);
    }  

    return true;
}

bool QVFStudy::ReadWeakFocusPoint( FILE * fp )
{
    // make room in structure

    struct weak_focus * last;
    struct weak_focus * point;
    double y[2];

    last = nullptr;
    point = first_wf_point;
    while( point != nullptr )
    {
        last = point;
        point = point->next_wf;
    }

    point = new weak_focus;//(struct weak_focus *)malloc( sizeof(struct weak_focus) );
    if( last == nullptr )
        first_wf_point = point;
    else
        last->next_wf = point;

    point->next_wf = nullptr;

    if( fscanf( fp, "%lf %lf ", &(point->x0), &(point->y0) ) !=2 )
        return false;
    if( fscanf( fp, "%d %d ", &(point->type), &(point->chart) ) != 2 )
        return false;

    ///* change type of node if we have a gcf ? 
 
    if( ( point->type == STYPE_STABLE ) || ( point->type == STYPE_UNSTABLE) )
    {
        y[0] = point->x0;
        y[1] = point->y0;

        switch( point->chart )
        {
        case CHART_R2:
            if( eval_term2( gcf, y ) < 0 )
                point->type *= -1;
            break;

        case CHART_U1:
            if( eval_term2( gcf_U1, y ) < 0 )
                point->type *= -1;
            break;

        case CHART_V1:
            if( (p==1) && (q==1) )
                y[0] = -y[0];
            if( eval_term2( gcf_V1, y ) < 0 )
                point->type *= -1;
            break;

        case CHART_U2:
            if( eval_term2( gcf_U2, y ) < 0 )
                point->type *= -1;
            break;

        case CHART_V2:
            if( (p==1) && (q==1) )
                y[0] = -y[0];
            if( eval_term2( gcf_V2, y ) < 0 )
                point->type*=-1;
            break;
        } 
    }

    if( singinf && (point->chart != CHART_R2) )
    {
        point->next_wf = new weak_focus;//(struct weak_focus *) malloc(sizeof(struct weak_focus));
        point->next_wf->x0 = point->x0;
        point->next_wf->y0 = 0.0;

        if( VFResults.dir_vec_field == 1 )
            point->next_wf->type = point->type;
        else
            switch( point->type )
            {
                case STYPE_STABLE:      point->next_wf->type = STYPE_UNSTABLE;  break;
                case STYPE_UNSTABLE:    point->next_wf->type = STYPE_STABLE;    break;
                default:                point->next_wf->type = point->type;     break;
            }

        point->next_wf->chart = (point->chart==CHART_U1) ? CHART_V1: CHART_V2;
        point = point->next_wf;
        point->next_wf = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                  QVFStudy::ReadDegeneratePoint
// -----------------------------------------------------------------------

bool QVFStudy::ReadDegeneratePoint( FILE * fp )
{
    int n;

    // make room in structure

    struct degenerate * last;
    struct degenerate * point;

    last = nullptr;
    point = first_de_point;
    while( point != nullptr )
    {
        last = point;
        point = point->next_de;
    }

    point = new degenerate;//(struct degenerate *)malloc( sizeof(struct degenerate) );
    if( last == nullptr )
        first_de_point = point;
    else
        last->next_de = point;

    point->next_de = nullptr;

    // load structure

    if( fscanf( fp, "%lf %lf %lf %d ", &(point->x0), &(point->y0), &(point->epsilon), &n ) != 4 )
        return false;
    point->blow_up=nullptr;
    if( n )
    {
        point->blow_up = new blow_up_points;//(struct blow_up_points *)malloc( sizeof(struct blow_up_points) );
        ReadBlowupPoints( fp, point->blow_up, n );
        point->blow_up->blow_up_vec_field = true;
    }

    if( fscanf( fp, "%d ", &(point->chart) ) != 1 )
        return false;
    
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc 

    if( singinf && point->chart != CHART_R2 )
    {
        last = point;
        point = new degenerate;//(struct degenerate *)malloc( sizeof(struct degenerate) );
        last->next_de = point;
        point->next_de = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (last->chart== CHART_U1) ? CHART_V1 : CHART_V2;
        point->epsilon = last->epsilon;
        point->notadummy = false;
        point->blow_up = last->blow_up;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadNodePoint
// -----------------------------------------------------------------------

bool QVFStudy::ReadNodePoint( FILE * fp )
{
    double y[2];

    // make room in structure

    struct node * last;
    struct node * point;

    last = nullptr;
    point = first_node_point;
    while( point != nullptr )
    {
        last = point;
        point = point->next_node;
    }

    point = new node;//(struct node *)malloc( sizeof(struct node) );
    if( last == nullptr )
        first_node_point = point;
    else
        last->next_node = point;

    point->next_node = nullptr;

    // load point structure

    if( fscanf( fp, "%lf %lf %d ", &(point->x0), &(point->y0), &(point->stable) ) != 3 )
        return false;
    if( fscanf( fp, "%d ", &(point->chart) ) != 1)
        return false;

    // change type of node if we have a gcf ?

    y[0] = point->x0;
    y[1]= point->y0;
    
    switch( point->chart )
    {
    case CHART_R2:
        if( eval_term2(gcf,y) < 0 )
            point->stable *= -1;
        break;

    case CHART_U1:
        if( eval_term2(gcf_U1,y) < 0 )
            point->stable *= -1;
        break;

    case CHART_V1:
        if( p==1 && q==1 )
            y[0] = -y[0];

        if( eval_term2( gcf_V1, y ) < 0 )
            point->stable *= -1;
        break;

    case CHART_U2:
        if( eval_term2( gcf_U2, y ) < 0 )
            point->stable *= -1;
        break;

    case CHART_V2:
        if( p==1 && q==1)
            y[0] = -y[0];
        if( eval_term2( gcf_V2, y ) < 0 )
            point->stable *= -1;
        break;
    }
 
    // if line at infinity a line of singularities in poincare disc:
    // duplicate singularity by using a symmetry

    if( singinf != 0 && point->chart != CHART_R2 )
    {
        last = point;
        point = new node;//(struct node *)malloc( sizeof(struct node) );
        last->next_node = point;
        point->next_node = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (last->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->stable = last->stable * ( VFResults.dir_vec_field == -1 ) ? -1 : 1;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadTransformations
// -----------------------------------------------------------------------

bool QVFStudy::ReadTransformations( FILE * fp, struct transformations * trans, int n )
{

    int i;

    if ( fscanf( fp, "%lf %lf %d %d %d %d %d %d %d",
                &(trans->x0), &(trans->y0), &(trans->c1), &(trans->c2),
                &(trans->d1),& (trans->d2), &(trans->d3), &(trans->d4), &(trans->d) ) != 9 )
        return false;
    
    trans->next_trans = nullptr;

    for( i = 2; i <= n; i++ )
    {
        trans->next_trans = new transformations;//(struct transformations *)malloc( sizeof(struct transformations) );
        trans = trans->next_trans;
    
        if( fscanf( fp, "%lf %lf %d %d %d %d %d %d %d",
                    &(trans->x0), &(trans->y0), &(trans->c1), &(trans->c2),
                    &(trans->d1),& (trans->d2), &(trans->d3), &(trans->d4), &(trans->d) ) != 9 )
            return false;

        trans->next_trans=nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadBlowupPoints
// -----------------------------------------------------------------------

bool QVFStudy::ReadBlowupPoints( FILE * fp, struct blow_up_points * b, int n )
{
    int i, N, typ;

    for( i = 1; i <= n; i++ )
    {
        if( fscanf( fp, "%d ", &(b->n) ) != 1 )
            return false;
        
        b->trans = new transformations;//(struct transformations *)malloc( sizeof(struct transformations) );

        ReadTransformations( fp, b->trans, b->n );
        if( fscanf( fp, "%lf %lf ", &(b->x0), &(b->y0) ) != 2 )
            return false;
        if( fscanf( fp, "%lf %lf %lf %lf ",&(b->a11), &(b->a12), &(b->a21),&(b->a22) ) != 4 )
            return false;
        ReadVectorField( fp, b->vector_field );
        b->sep = new term1;//(struct term1 *)malloc( sizeof(struct term1) );
        if( fscanf( fp, "%d ", &N ) != 1)
            return false;
        ReadTerm1( fp, b->sep, N );
   
        if( fscanf( fp, "%d ", &typ ) != 1 )
            return false;
        switch(typ )
        {
            case 1:  b->type = STYPE_UNSTABLE;          break;
            case 2:  b->type = STYPE_STABLE;            break;
            case 3:  b->type = STYPE_UNSTABLE;          break;
            case 4:  b->type = STYPE_STABLE;            break;
            case 5:  b->type = STYPE_CENUNSTABLE;       break;
            case 6:  b->type = STYPE_CENSTABLE;         break;
            case 7:  b->type = STYPE_CENUNSTABLE;       break; 
            case 8:  b->type = STYPE_CENSTABLE;         break;
            case 9:  b->type = STYPE_UNSTABLE;          break;
            case 10: b->type = STYPE_UNSTABLE;          break;
            case 11: b->type = STYPE_STABLE;            break;
            case 12: b->type = STYPE_STABLE;            break;
            case 13: b->type = STYPE_UNSTABLE;          break;
            case 14: b->type = STYPE_UNSTABLE;          break;
            case 15: b->type = STYPE_STABLE;            break;
            case 16: b->type = STYPE_STABLE;            break;

        }
        b->first_sep_point = nullptr;
        b->last_sep_point = nullptr;
        b->next_blow_up_point = nullptr;
        
        if( i < n )
        {
            b->next_blow_up_point = new blow_up_points;//(struct blow_up_points *)malloc( sizeof(struct blow_up_points) );
            b = b->next_blow_up_point;
        }
    }
    return true;
}

#define DUMP(x) m->append( s.sprintf x );
#define DUMPSTR(x) m->append( x );

void QVFStudy::DumpSeparatrices( QTextEdit * m, struct sep * separ, int margin )
{
    QString s;
    char smargin[80];
    const char * stype;
    strcpy(smargin,"                              ");
    smargin[margin]=0;  // make number of spaces

    if( separ == nullptr )
    {
        DUMP(( "%s/", smargin ))
            return;
    }

    while( separ != nullptr )
    {
        switch( separ->type )
        {
        case STYPE_STABLE:      stype = "Stable         ";  break;
        case STYPE_UNSTABLE:    stype = "unstable       ";  break;
        case STYPE_CENSTABLE:   stype = "center-stable  ";  break;
        case STYPE_CENUNSTABLE: stype = "center-unstable";  break;
        default:                stype = "???????????????";  break;
        }

        DUMP(( "%sType=%s  Dir=%-2d  original=%d", smargin, stype, separ->direction, separ->notadummy ))
        if( separ->d == 0 )
        {
            DUMP(( "%sTaylor: (x,y)=(t,%s)", smargin, DumpPoly1( separ->separatrice, "t" ) ))
        }
        else
        {
            DUMP(( "%sTaylor: (x,y)=(%s,t)", smargin, DumpPoly1( separ->separatrice, "t" ) ))
        }

        separ = separ->next_sep;
    }

}

void QVFStudy::DumpSingularities( QTextEdit * m, struct genericsingularity * p, const char * type, bool longversion )
{
    const char * chart;
    QString s;
    QByteArray ss;

    struct saddle * sa;
    struct degenerate * de;
    struct strong_focus * sf;
    struct weak_focus * wf;
    struct node * no;
    struct semi_elementary * se;

    while( p != nullptr )
    {
        switch( p->chart )
        {
        case CHART_R2:  chart = "R2";   break;
        case CHART_U1:  chart = "U1";   break;
        case CHART_U2:  chart = "U2";   break;
        case CHART_V1:  chart = "V1";   break;
        case CHART_V2:  chart = "V2";   break;
        default:
            chart = "?";    break;
        }
        DUMP(( "%s (x0,y0)=(%g,%g)  Chart %s", type, (float)(p->x0), (float)(p->y0), chart ))
        if( longversion )
        {
            switch( type[0]*type[1] )
            {
            case 'S'*'A':
                // saddle
                sa = (struct saddle *)p;
                DUMP(( "   Epsilon = %g  original=%d", (float)(sa->epsilon), sa->notadummy ))
                DUMP(( "   Transformation Matrix = [ [%g,%g], [%g,%g] ]", (float)(sa->a11),
                                        (float)(sa->a12), (float)(sa->a21), (float)(sa->a22) ))
                DUMP(( "   Vector Field:" ))
                DUMP(( "      x' = %s", DumpPoly2( sa->vector_field[0], "x", "y" ) ))
                DUMP(( "      y' = %s", DumpPoly2( sa->vector_field[1], "x", "y" ) ))
                DUMP(( "   Separatrices:" ))
                DumpSeparatrices( m, sa->separatrices, 6 );
                break;

            case 'D'*'E':
                // degenerate
                de = (struct degenerate *)p;
                DUMP(( "   Epsilon = %g  original=%d", (float)(de->epsilon), de->notadummy ))
                break;

            case 'S'*'T':
                // strong focus
                sf = (struct strong_focus *)p;
                if( sf->stable == -1 ) s = "(stable)";
                else if( sf->stable == +1 ) s = "(unstable)";
                else s = "( ??? )";

                ss = s.toLatin1();
                DUMP(( "    Stability = %d %s", sf->stable, (const char *)ss ))
                break;
            case 'W'*'E':
                // weak focus
                wf = (struct weak_focus *)p;
                DUMP(( "    Type = %d", wf->type ))
                break;
            case 'N'*'O':
                // node
                no = (struct node *)p;
                if( no->stable == -1 ) s = "(stable)";
                else if( no->stable == +1 ) s = "(unstable)";
                else s = "( ??? )";
                ss = s.toLatin1();
                DUMP(( "    Stability = %d %s", no->stable, (const char *)ss ))
                break;
            case 'S'*'E':
                // semi-elementary
                se = (struct semi_elementary *)p;
                DUMP(( "   Type    = %d", se->type ))
                DUMP(( "   Epsilon = %g  original=%d", (float)(se->epsilon), se->notadummy ))
                DUMP(( "   Transformation Matrix = [ [%g,%g], [%g,%g] ]", (float)(se->a11),
                                        (float)(se->a12), (float)(se->a21), (float)(se->a22) ))
                DUMP(( "   Vector Field = " ))
                DUMP(( "      x' = %s", DumpPoly2( se->vector_field[0], "x", "y" ) ))
                DUMP(( "      y' = %s", DumpPoly2( se->vector_field[1], "x", "y" ) ))
                DUMP(( "   Separatrices:" ))
                DumpSeparatrices( m, se->separatrices, 6 );
                break;
            }
            DUMP((" "))
        }
        p = p->next;
    }
}

void QVFStudy::Dump( QString basename, QString info )
{
    QTextEdit * m;
    QString s;
    QByteArray ss;

   return;
    m = new QTextEdit();
    
    DUMP(( "General Info" ))
    DUMP(( "------------" ))
    DUMP(( "  Type of study = %d", typeofstudy ))
    DUMP(( "  (p,q) = (%d,%d)", p, q ))
    DUMP(( "  plweights = %d", plweights ))
    DUMP(( "  doubles p,q,p+q,p-1,q-1,q-p = (%g,%g,%g,%g,%g,%g)", (float)double_p,
            (float)double_q, (float)double_p_plus_q, (float)double_p_minus_1,
            (float)double_q_minus_1, (float)double_q_minus_p ))
    DUMP(( "  Range x: [%g,%g] Range y: [%g,%g]", (float)xmin, (float)xmax, (float)ymin, (float)ymax ))
    DUMP(( "  Line at infinity singular? %d", singinf ))
    DUMP(( "  Direction of vector field: %d", VFResults.dir_vec_field ))
    DUMP(( " " ))
    DUMP(( "Vector Fields" ))
    DUMP(( "-------------" ))
    DUMP(( "  Finite chart:" ))
    DUMP(( "     x' = %s", DumpPoly2( f_vec_field[0], "x", "y" ) ))
    DUMP(( "     y' = %s", DumpPoly2( f_vec_field[1], "x", "y" ) ))
    DUMP(( "  U1 chart:" ))
    DUMP(( "     x' = %s", DumpPoly2( vec_field_U1[0], "x", "y" ) ))
    DUMP(( "     y' = %s", DumpPoly2( vec_field_U1[1], "x", "y" ) ))
    DUMP(( "  U2 chart:" ))
    DUMP(( "     x' = %s", DumpPoly2( vec_field_U2[0], "x", "y" ) ))
    DUMP(( "     y' = %s", DumpPoly2( vec_field_U2[1], "x", "y" ) ))
    DUMP(( "  V1 chart:" ))
    DUMP(( "     x' = %s", DumpPoly2( vec_field_V1[0], "x", "y" ) ))
    DUMP(( "     y' = %s", DumpPoly2( vec_field_V1[1], "x", "y" ) ))
    DUMP(( "  V2 chart:" ))
    DUMP(( "     x' = %s", DumpPoly2( vec_field_V2[0], "x", "y" ) ))
    DUMP(( "     y' = %s", DumpPoly2( vec_field_V2[1], "x", "y" ) ))
    DUMP(( "  Cylinder chart: (Co=cos(theta),Si=sin(theta))" ))
    DUMP(( "     r'     = %s", DumpPoly3( vec_field_C[0], "r", "Co", "Si" ) ))
    DUMP(( "     theta' = %s", DumpPoly3( vec_field_C[1], "r", "Co", "Si" ) ))
    DUMP(( " " ))
    DUMP(( "Greatest Common Factor" ))
    DUMP(( "----------------------" ))
    DUMP(( "  Finite chart:   %s", DumpPoly2( gcf, "x", "y" ) ))
    DUMP(( "  U1 chart:       %s", DumpPoly2( gcf_U1, "x", "y" ) ))
    DUMP(( "  U2 chart:       %s", DumpPoly2( gcf_U2, "x", "y" ) ))
    DUMP(( "  V1 chart:       %s", DumpPoly2( gcf_V1, "x", "y" ) ))
    DUMP(( "  V2 chart:       %s", DumpPoly2( gcf_V2, "x", "y" ) ))
    DUMP(( "  Cylinder chart: %s", DumpPoly3( gcf_C, "r", "Co", "Si" ) ))
    DUMP(( " " ))
    DUMP(( "Singular points - summary" ))
    DUMP(( "-------------------------" ))
    DUMP(( " " ))
    DumpSingularities( m, (struct genericsingularity *)first_saddle_point,  "SADDLE         ", false );
    DumpSingularities( m, (struct genericsingularity *)first_se_point,      "SEMI-ELEMENTARY", false );
    DumpSingularities( m, (struct genericsingularity *)first_node_point,    "NODE           ", false );
    DumpSingularities( m, (struct genericsingularity *)first_wf_point,      "WEAK FOCUS     ", false );
    DumpSingularities( m, (struct genericsingularity *)first_sf_point,      "STRONG FOCUS   ", false );
    DumpSingularities( m, (struct genericsingularity *)first_de_point,      "DEGENERATE     ", false );
    DUMP(( " " ))
    DUMP(( "Singular points - full description" ))
    DUMP(( "----------------------------------" ))
    DUMP(( " " ))
    DumpSingularities( m, (struct genericsingularity *)first_saddle_point,  "SADDLE         ", true );
    DumpSingularities( m, (struct genericsingularity *)first_se_point,      "SEMI-ELEMENTARY", true );
    DumpSingularities( m, (struct genericsingularity *)first_node_point,    "NODE           ", true );
    DumpSingularities( m, (struct genericsingularity *)first_wf_point,      "WEAK FOCUS     ", true );
    DumpSingularities( m, (struct genericsingularity *)first_sf_point,      "STRONG FOCUS   ", true );
    DumpSingularities( m, (struct genericsingularity *)first_de_point,      "DEGENERATE     ", true );
    DUMP(( " " ))
    DUMP(( "Default integration parameters" ))
    DUMP(( "------------------------------" ))
    DUMP(( " " ))
    DUMP(( "  LC Value           %d", config_lc_value ))
    DUMP(( "  Max Step Size      %g", (float)config_hma ))
    DUMP(( "  Min Step Size      %g", (float)config_hmi ))
    DUMP(( "  Step Size          %g", (float)config_step ))
    DUMP(( "  Current Step Size  %g", (float)config_currentstep ))
    DUMP(( "  Tolerance          %g", (float)config_tolerance ))
    DUMP(( "  Projection         %g", (float)config_projection ))
    DUMP(( "  Integration Points %d", config_intpoints ))
    DUMP(( "  LC NumPoints       %d", config_lc_numpoints ))
    DUMP(( "  Dashes             %d", config_dashes ))
    
    if( config_kindvf == INTCONFIG_ORIGINAL ) s = "(Original)";
        else if( config_kindvf == INTCONFIG_REDUCED ) s = "(Reduced)";
            else s = "???";

    ss = s.toLatin1();
    DUMP(( "  Kind of VF         %d %s", config_kindvf, (const char *)ss ))

    ss= basename.toLatin1() + "_fin.tab";
    QFile ffin( ss );
    if( ffin.open( QIODevice::ReadOnly ) )
    {
        DUMP(( " " ))
        DUMP(( "Contents of filename_fin.tab" ))
        DUMP(( "----------------------------" ))
        DUMP(( " " ))
        QTextStream tfin( &ffin );
        while( !tfin.atEnd() )
        {
            s = tfin.readLine();
            m->append( s );
        }
        ffin.close();
    }
    else
    {
        DUMP(( " " ))
        DUMP(( "Contents of filename_fin.tab" ))
        DUMP(( "----------------------------" ))
        DUMP(( " " ))
        DUMP(( "FILE DOES NOT EXIST." ))
    }
    QFile finf( basename + "_inf.tab" );
    if( finf.open( QIODevice::ReadOnly ) )
    {
        DUMP(( " " ))
        DUMP(( "Contents of filename_inf.tab" ))
        DUMP(( "----------------------------" ))
        DUMP(( " " ))

        QTextStream tinf( &finf );
        while( !tinf.atEnd() )
        {
            s = tinf.readLine();
            m->append( s );
        }
        finf.close();
    }
    else
    {
        DUMP(( " " ))
        DUMP(( "Contents of filename_inf.tab" ))
        DUMP(( "----------------------------" ))
        DUMP(( " " ))
        DUMP(( "FILE DOES NOT EXIST." ))
    }
    DUMP(( " " ))
    ss = info.toLatin1();
    DUMP(( ss ))

    m->setFont( *(p4app->CourierFont) );
    m->setReadOnly(true);
    m->resize(640,480);
    SetP4WindowTitle( m, "DUMP SCREEN" );
    m->show();
}

