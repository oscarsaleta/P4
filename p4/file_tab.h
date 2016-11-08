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

/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TABLE_H
#define TABLE_H


#include <QObject>
#include <QString>
#include <QTextEdit>


// -----------------------------------------------------------------------
//						General polynomial expressions
// -----------------------------------------------------------------------

// Linked list of univariate terms a*x^i

struct term1
{
	int exp;
	double coeff;
	struct term1 * next_term1;
};

typedef struct term1 * P4POLYNOM1;

// Linked list of terms a*x^i*y^j

struct term2
{
	int exp_x;
	int exp_y;
	double coeff;
	struct term2 * next_term2;
};

// Linked list of terms a*r^i*cos(theta)^j*sin(theta)^k

typedef struct term2 * P4POLYNOM2;

struct term3
{
	int exp_r;
	int exp_Co;
	int exp_Si;
	double coeff;
	struct term3 * next_term3;
};

typedef struct term3 * P4POLYNOM3;

// -----------------------------------------------------------------------
//								Orbits
// -----------------------------------------------------------------------

struct orbits_points
{
	int color;			// color of seperatrice

	double pcoord[3];	// point on the poincare sphere -> p=(X,Y,Z)
						// or 
						// on the poincare-lyapunov sphere 
						// -> p=(0,x,y) or p=(1,r,theta)
	int dashes;
	int dir;			// if we have a line of sing at infinity and have to change
	int type;			// the direction if we integrate the orbit of separatrice 
						// and sometimes the type

	struct orbits_points * next_point;
};

typedef struct orbits_points * P4ORBIT;

struct orbits
{
	double pcoord[3];				// startpoint
	int color;
	P4ORBIT f_orbits;				// orbit
	P4ORBIT current_f_orbits;		// orbit
	struct orbits * next_orbit;
};

// -----------------------------------------------------------------------
//						Blow up structure
// -----------------------------------------------------------------------

struct transformations
{
	double x0, y0;						// translation point
	int c1,c2, d1,d2,d3,d4;				// F(x,y)=(c1*x^d1*y^d2,c2*x^d3*y^d4)
	int d;								// X/x^d
	struct transformations * next_trans;
};

struct blow_up_points
{
	int n;								// number of transformations
	struct transformations * trans;
	double x0,y0;						// last point that is not degenerate
	double a11,a12,a21,a22;				// transformation matrix
	struct term2 *vector_field[2];		// vector field
	struct term1 * sep;					// sep (t,g(t))
	int type;							// type of seperatrice (STYPE_STABLE, ...)
	int blow_up_vec_field;				// if true then use the blow up vector field if 
										// the modulus of the last point of the separatrices is less
										// than 1
	double point[2];					// end point sep in blow up chart

	struct orbits_points * first_sep_point;
	struct orbits_points * last_sep_point;
	struct blow_up_points * next_blow_up_point;
};

struct sep
{
	struct orbits_points * first_sep_point;
	struct orbits_points * last_sep_point;
	int type;							// STYPE_STABLE, UNSTABLE, CENSTABLE or CENUNSTABLE
	int direction;
	int d;
    bool notadummy;						// false if separatrice is a copy of a structure (obtained
										// through a symmetry)
	struct term1 * separatrice;			// if d=0 -> (t,f(t)), d=1 ->(f(t),t)
	struct sep * next_sep;
};

// -----------------------------------------------------------------------
//							Singularities
// -----------------------------------------------------------------------

struct genericsingularity		// part of the structure that is the same for all types
{
	double x0;
	double y0;
	struct genericsingularity * next;
	int chart;
};

struct saddle
{
	double x0;
	double y0;
	struct saddle * next_saddle;
	int chart;

	double epsilon;
    int notadummy;

	struct sep * separatrices;
	struct term2 * vector_field[2];		// vector field {xdot,ydot}
	double a11,a12,a21,a22;				// transformation matrix
};

struct semi_elementary
{
	double x0;
	double y0;
	struct semi_elementary * next_se;
	int chart;

	double epsilon;
    int notadummy;
	
	struct sep * separatrices;			// center sep (t,f(t)), sep (g(t),t)
	struct term2 * vector_field[2];		// vector field
	double a11,a12,a21,a22;				// transformation matrix

	int type;							// type of semi-elementary point
};

struct degenerate
{
	double x0;
	double y0;
	struct degenerate * next_de;
	int chart;
	
	double epsilon;
    int notadummy;

	struct blow_up_points * blow_up;
};

struct node
{
	double x0;
	double y0;
	struct node * next_node;
	int chart;

	int stable;
};

struct strong_focus
{
	double x0;
	double y0;
	struct strong_focus * next_sf;
	int chart;

	int stable;
};

struct weak_focus
{
	double x0;
	double y0;
	struct weak_focus * next_wf;
	int chart;

	int type;
};


// -----------------------------------------------------------------------
//							Some definitions
// -----------------------------------------------------------------------

#define CHART_R2			0
#define CHART_U1			1
#define CHART_U2			2
#define CHART_V1			3
#define CHART_V2			4

#define SADDLE				1
#define NODE				2
#define WEAK_FOCUS			3
#define STRONG_FOCUS		4
#define SEMI_HYPERBOLIC		5  
#define NON_ELEMENTARY		6 

#define STYPE_UNSTABLE		(1)		// separatrice type
#define STYPE_STABLE		(-1)
#define STYPE_CENUNSTABLE	2
#define STYPE_CENSTABLE		(-2)
#define STYPE_ORBIT			3

#define FOCUSTYPE_UNKNOWN   0
#define FOCUSTYPE_UNSTABLE	(1)
#define FOCUSTYPE_STABLE	(-1)
#define FOCUSTYPE_CENTER	4

#define SETYPE_SADDLENODE_UNSTABSEP     1
#define SETYPE_SADDLENODE_UNSTABSEP2    2

#define OT_STABLE			STYPE_STABLE
#define OT_UNSTABLE			STYPE_UNSTABLE
#define OT_CENT_STABLE		STYPE_CENSTABLE
#define OT_CENT_UNSTABLE	STYPE_CENUNSTABLE
#define OT_ORBIT			STYPE_ORBIT

enum TYPEOFVIEWS {
    TYPEOFVIEW_PLANE	= 0,
    TYPEOFVIEW_SPHERE	= 1,
    TYPEOFVIEW_U1		= 2,
    TYPEOFVIEW_U2		= 3,
    TYPEOFVIEW_V1		= 4,
    TYPEOFVIEW_V2		= 5 };

// -----------------------------------------------------------------------
//							Results class
// -----------------------------------------------------------------------
class QVFStudy : public QObject
{
public:
	// general information

	int typeofstudy;
    TYPEOFVIEWS typeofview;	// TYPEOFVIEW_PLANE or TYPEOFVIEW_SPHERE
	int p;
	int q;
	bool plweights;						// true if p<>1 or q<>1; false if p=q=1

	double double_p;					// shortcuts: = (double)p
	double double_q;					// = (double)q
	double double_p_plus_q;				// = (double)(p+q)
	double double_p_minus_1;			// = (double)(p-1)
	double double_q_minus_1;			// = (double)(q-1)
	double double_q_minus_p;			// = (double)(q-p)


	double xmin, xmax, ymin, ymax;		// in case of local study
	bool singinf;
	int dir_vec_field;

    QString lasterror;

	// vector field in various charts

	P4POLYNOM2					f_vec_field[2];
	P4POLYNOM2					vec_field_U1[2];
	P4POLYNOM2					vec_field_U2[2];
	P4POLYNOM2					vec_field_V1[2];
	P4POLYNOM2					vec_field_V2[2];
	P4POLYNOM3					vec_field_C[2];

	// singular points and their properties:

    saddle *				first_saddle_point;
    semi_elementary *       first_se_point;
    node *                  first_node_point;
    strong_focus *          first_sf_point;
    weak_focus *			first_wf_point;
    degenerate *			first_de_point;

	// Greatest common factor if present:

	P4POLYNOM2					gcf;
	P4POLYNOM2					gcf_U1;
	P4POLYNOM2					gcf_U2;
	P4POLYNOM2					gcf_V1;
	P4POLYNOM2					gcf_V2;
	P4POLYNOM3					gcf_C;

    orbits_points * gcf_points;

	// limit cycles

    orbits *				first_lim_cycle;
    orbits *				first_orbit;

	// ------ Configuration

	int							config_lc_value;
	double						config_hma;
	double						config_hmi;
	double						config_step;
	double						config_currentstep;
	double						config_tolerance;
	double						config_projection;
	int							config_intpoints;
	int							config_lc_numpoints;
	bool						config_dashes;
	bool						config_kindvf;				// true for original VF, false for reduced

	// run-time when plotting

    orbits *				current_orbit;
    orbits *				current_lim_cycle;

    double					selected_ucoord[2];
    saddle *				selected_saddle_point;
    semi_elementary *   	selected_se_point;
    degenerate *			selected_de_point;
    sep *                   selected_sep;
    blow_up_points *		selected_de_sep;

	// coordinate transformation routines, set up when starting the plot

	void (*viewcoord_to_sphere)( double, double, double * );
	bool (*sphere_to_viewcoordpair)( double *, double *, double *, double *, double *, double * );

	void (*finite_to_viewcoord)( double, double, double * );
	void (*sphere_to_viewcoord)( double, double, double, double * );
	bool (*is_valid_viewcoord)( double, double, double * );
	void (*integrate_sphere_sep)( double, double, double, double *,
								double *, int *, int *, int *, int *, double, double );
	void (*U1_to_sphere)( double, double, double * );
	void (*U2_to_sphere)( double, double, double * );
	void (*V1_to_sphere)( double, double, double * );
	void (*V2_to_sphere)( double, double, double * );

	void (*sphere_to_U1)( double, double, double, double * );
	void (*sphere_to_U2)( double, double, double, double * );
	void (*sphere_to_V1)( double, double, double, double * );
	void (*sphere_to_V2)( double, double, double, double * );

	void (*sphere_to_R2)( double, double, double, double * );
	void (*R2_to_sphere)( double, double, double * );
	void (*integrate_sphere_orbit)( double, double, double, double *,
								double *, int *, int *, double, double );
	double (*eval_lc)( double *, double, double, double );
	bool (*less2)( double *, double * );
	int (*change_dir)( double * );

public:
	QVFStudy();					// constructor
	~QVFStudy();				// destructor

public:

	// initialization and destruction of structures

    void deleteVF( void );

    void deleteSaddle( saddle * );
    void deleteSemiElementary( semi_elementary * );
    void deleteNode( node * );
    void deleteStrongFocus( strong_focus * );
    void deleteWeakFocus( weak_focus * );
    void deleteDegenerate( degenerate * );
    void deleteSeparatrices( sep * );
    void deleteTransformations( transformations * );
    void deleteBlowup( blow_up_points * b );

    void deleteLimitCycle( orbits * );
    void deleteOrbitPoint( P4ORBIT p );
    void deleteOrbit( orbits * );

	// reading of the Maple/Reduce results

    bool readTables( QString basename );
    bool readGCF( FILE * fp );
    bool readVectorField( FILE * fp, P4POLYNOM2 * vf );
    bool readVectorFieldCylinder( FILE * fp, P4POLYNOM3 * vf );
    bool readPoints( FILE * fp );
    bool readTerm1( FILE * fp, P4POLYNOM1 p, int N );
    bool readTerm2( FILE * fp, P4POLYNOM2 p, int N );
    bool readTerm3( FILE * fp, P4POLYNOM3 p, int N );

    bool readSaddlePoint( FILE * fp );
    bool readSemiElementaryPoint( FILE * fp );
    bool readStrongFocusPoint( FILE * fp );
    bool readWeakFocusPoint( FILE * fp );
    bool readDegeneratePoint( FILE * fp );
    bool readNodePoint( FILE * fp );
    bool readBlowupPoints( FILE * fp, blow_up_points * b, int n );
    bool readTransformations( FILE * fp, transformations * trans, int n );

    void setupCoordinateTransformations( void );	// see math_p4.cpp

    void dump( QString basename, QString info="" );

private:
    void dumpSeparatrices( QTextEdit * m, sep * separ, int margin );
    void dumpSingularities( QTextEdit * m, genericsingularity * p, const char * type, bool longversion );

};

#define __p                 VFResults.double_p              // p
#define __q                 VFResults.double_q              // q
#define __q_minus_p         VFResults.double_q_minus_p      // q-p
#define __p_minus_1         VFResults.double_p_minus_1      // p-1
#define __q_minus_1         VFResults.double_q_minus_1      // q-1
#define MATHFUNC(function) (*(VFResults.function))

extern QVFStudy VFResults;	// (VFResults.p,VFResults.q) are lyapunov weights

#define LINESTYLE_DASHES	1
#define LINESTYLE_POINTS	0


#endif /*TABLE_H*/
