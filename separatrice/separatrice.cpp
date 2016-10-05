#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../version.h"

// #define LOGACTIVITY		// define when you want to create a log file of all tests (sep.log)

// constants

#define	NUMBER		100
#define	MIN_ERROR	1.e-8
#define	MAX_ERROR	1.e-6
#define	PI			3.1415926535897932384626433832
#define	TWOPI		(2.*PI)

// structures

struct term1
{
	int exp;
	double coeff;
	struct term1 * next_term1;
};

struct term2
{
	int    exp_x,exp_y;
	double coeff;
	struct term2 *next_term2;
};

// function prototypes

double eval_term1( struct term1 *, double );
double diff_eval_term1( struct term1 *, double );
double eval_vec_field( int, double, double );
double find_step( struct term1 *, double );
void find_slopes( double *, double *, double *, double *, double );
void read_term2( FILE *, struct term2 *, int );
void read_term1( FILE *, struct term1 *, int );
double read_sep( const char * );
int relative_error_test( double *, double *, double *, double * );

#ifdef LOGACTIVITY
char * printterm1( char *, struct term1 *, bool, char * );
char * printterm2( char *, struct term2 *, bool, char *, char * );
#endif

// variables

int stable = 0;
int direction = 0;
struct term1 * separatrice = nullptr;
struct term2 * vec_field[2] = { nullptr, nullptr };     // vec_field={P(x,y),Q(x,y)}

// --------------------------------------------------------------------------
//                      MAIN
// --------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	char buf[256];
	bool isfirst;
	double vec_slope[NUMBER];
	double sep_slope[NUMBER];
	double diff_slope[NUMBER];
	double dist[NUMBER];
	double epsilon;
	double ok;
#ifdef LOGACTIVITY
	FILE * logfile;
	struct term1 * t1;
	struct term2 * t2;
#endif

	if( argc <= 1 )
	{
        	printf( "Separatrice Version: %s Date: %s\n%s", VERSION, VERSIONDATE,
                "THIS FILE IS PART OF THE P4 DISTRIBUTION.\n"
                "It is called automatically from reduce/maple.  It tests\n"
                "whether a given formal power series expansion is a valid\n"
                "approximation for a stable/unstable/center manifold.\n\n"
                "SYNTAX: separatrice inputfile\n"
                "\tThe input file contains the separatrix and the vector field\n"
                "\tin a normalized form.  The result of the test is returned\n"
                "\tto the caller by means of the exit value (0 or 1).\n"
                "\t0       = the expansion is a bad approximation\n"
                "\t1       = the expansion is a good approximation\n"
                "\t(other) = an error occurred.\n" );
        	exit(-1);
	}

	epsilon = read_sep( argv[1] );
	if( epsilon < 0 )
		return (-1); 

	/*
		make the test in a circle with radius of epsilon+epsilon/2
		if test ok then we start integrating from a distance of epsilon
		if test failed then we calculate the taylor expansion again
		with a higher degree       
	*/

	find_slopes( vec_slope, sep_slope, diff_slope, dist, epsilon+epsilon/2.0 );

	ok = relative_error_test( vec_slope, sep_slope, diff_slope, dist );

#ifdef LOGACTIVITY
	printf( "Separatrice test:\n" );
	printf( "  Vector field:\n" );
	printf( "\tx'= " );
	isfirst=true;
	t2 = vec_field[0];
	while( t2 != nullptr )
	{
		printterm2( buf, t2, isfirst, "x", "y" );
		printf( "%s", buf );
		t2 = t2->next_term2;
		isfirst=false;
	}
	printf( "\n" );
	printf( "\ty'= " );
	isfirst=true;
	t2 = vec_field[1];
	while( t2 != nullptr )
	{
		printterm2( buf, t2, isfirst, "x", "y" );
		printf( "%s", buf );
		t2 = t2->next_term2;
		isfirst=false;
	}
	printf( "\n" );
	printf( "  Separatrix:\n" );
	printf( "\ty = " );
	isfirst=true;
	t1=separatrice;
	while( t1 != nullptr )
	{
		printterm1( buf, t1, isfirst, "x" );
		printf( "%s", buf );
		t1 = t1->next_term1;
		isfirst=false;
	}
	printf( "\n" );
	printf( "Result: %d\n", (int)ok );

	logfile = fopen( "sep.log", "a" );

	fprintf( logfile, "Separatrice test:\n" );
	fprintf( logfile, "  Vector field:\n" );
	fprintf( logfile, "\tx'= " );
	t2 = vec_field[0];
	while( t2 != nullptr )
	{
		printterm2( buf, t2, isfirst, "x", "y" );
		fprintf( logfile, "%s", buf );
		t2 = t2->next_term2;
		isfirst=false;
	}
	fprintf( logfile, "\n" );
	fprintf( logfile, "\ty'= " );
	isfirst=true;
	t2 = vec_field[1];
	while( t2 != nullptr )
	{
		printterm2( buf, t2, isfirst, "x", "y" );
		fprintf( logfile, "%s", buf );
		t2 = t2->next_term2;
		isfirst=false;
	}
	fprintf( logfile, "\n" );
	fprintf( logfile, "  Separatrix:\n" );
	fprintf( logfile, "\ty = " );
	isfirst=true;
	t1=separatrice;
	while( t1 != nullptr )
	{
		printterm1( buf, t1, isfirst, "x" );
		fprintf( logfile, "%s", buf );
		t1 = t1->next_term1;
		isfirst=false;
	}
	fprintf( logfile,"\n" );
	fprintf( logfile,"Result: %d\n", (int)ok );

	fclose(logfile);
#endif
	return (int)ok;
}

// --------------------------------------------------------------------------
//                      RELATIVE_ERROR_TEST
// --------------------------------------------------------------------------

int relative_error_test( double * vec_slope, double * sep_slope, double * diff_slope, double * dist )
{
	double a, b;
	int i, ok;

	a = MAX_ERROR;
	b = MIN_ERROR;
	ok = 1;

	for( i = 0; i < NUMBER; ++i )
	{
		/*
		printf("i=%d vec_slope=%15.13e sep_slope=%15.13e diff_slope=%15.13e a=%10.9e\n",
					i,vec_slope[i],sep_slope[i],diff_slope[i],a);
		*/
		if( diff_slope[i] > a )
		{
			printf( "test failed for i=%d \n", i );
			ok=0;
			break;
		}
	}

	return(ok);
}

// --------------------------------------------------------------------------
//                      EVAL_TERM1
// --------------------------------------------------------------------------

double eval_term1( struct term1 * current_term1, double t )
{
	double s=0;

	if(current_term1 != nullptr)
	{
		do
		{
			if( current_term1->exp )
				s += current_term1->coeff*pow( t, (double)current_term1->exp );
			else
				s+=current_term1->coeff;
		}
		while( (current_term1=current_term1->next_term1) != nullptr );
	}
	return s;
}

// --------------------------------------------------------------------------
//                      DIFF_EVAL_TERM1
// --------------------------------------------------------------------------

double diff_eval_term1( struct term1 * current_term1, double t )
{
	double s=0;
	int exp;
	
	if( current_term1 != nullptr )
	{
		do
		{
			exp = current_term1->exp;
			if( exp == 1 )
				s += current_term1->coeff;
			else
				if( exp )
					s += current_term1->coeff*exp*pow(t,(double)exp-1);
		}
		while( (current_term1=current_term1->next_term1) != nullptr );
	}
	return s;
}

// --------------------------------------------------------------------------
//                      EVAL_VEC_FIELD
// --------------------------------------------------------------------------

double eval_vec_field( int i, double a, double b )
{ 
	struct term2 * current_term2;
	double s;

	s = 0;
	current_term2 = vec_field[i];

	if( current_term2 != nullptr )
	{
		do
		{
			if( current_term2->exp_x && current_term2->exp_y )
				s += current_term2->coeff * pow( a, (double)current_term2->exp_x )
										  * pow( b, (double)current_term2->exp_y );
			else
				if( current_term2->exp_x )
					s += current_term2->coeff * pow( a, (double)current_term2->exp_x );
				else
					if( current_term2->exp_y )
						s += current_term2->coeff * pow( b, (double)current_term2->exp_y );
					else
						s += current_term2->coeff;
		}
		while( (current_term2=current_term2->next_term2) != nullptr );
	}

	return s;
}

// --------------------------------------------------------------------------
//                      FIND_STEP
// --------------------------------------------------------------------------

// find a t with a<=t^2+sep(t)^2<=b, a=(epsilon-epsilon/100)^2
//  and b=(epsilon+epsilon/100)^2

double find_step( struct term1 * sep, double epsilon )
{
	double t, t1, t2, r0, a, b;

	t = epsilon*direction;
	a = pow(epsilon-epsilon/100,2.0);
	b = pow(epsilon+epsilon/100,2.0);
	r0 = pow(t,2.0) + pow(eval_term1(sep,t),2.0);

	if( (r0>a) && (r0<b) )
		return t;

	if( r0 <= a )
	{
		do
		{
			t *= 2.0;
			r0 = pow(t,2.0) + pow( eval_term1(sep,t), 2.0 );
		}
		while( r0 <= a );

		if( r0 < b )
			t2 = t;
		else
		{
			t1 = t / 2;
			t2 = (t + t1) / 2;

			for(;;)
			{
				r0 = pow( t2, 2.0 ) + pow( eval_term1(sep,t2), 2.0 );
				if( (r0>a) && (r0<b) )
					break;
				
				if( r0 <= a )
					t1 = t2;
				else
					t = t2;

				t2 = (t+t1)/2;
			}
		}
	}
	else
	{
		do
		{
			t /= 2.0;
			r0 = pow( t, 2.0 ) + pow( eval_term1(sep,t), 2.0 );
		}
		while( r0 >= b );

		if( r0 > a )
			t2 = t;
		else
		{
			t1 = 2.0 * t;
			t2 = (t+t1)/2;
			for(;;)
			{
				r0 = pow( t2, 2.0 ) + pow( eval_term1(sep,t2), 2.0 );
				if( (r0>a) && (r0<b) )
					break;

				if( r0 <= a )
					t = t2;
				else
					t1 = t2;

				t2 = (t+t1)/2;
			}
		}
	} 
	// printf("%f %f %f \n",sqrt(r0),sqrt(a),sqrt(b));

	return t2;
}

// --------------------------------------------------------------------------
//                      FIND_SLOPES
// --------------------------------------------------------------------------

void find_slopes( double * vec_slope, double * sep_slope, double * diff_slope,
					double * dist, double epsilon)
{
	double y, h, t, c, p0,p1,p2;
	int i;

	c=0;
	/*printf("epsilon=%f\n",epsilon);*/

	if( (stable && direction==1) || (!stable && direction==-1) )
		c = PI;

	h = find_step( separatrice, epsilon ) / NUMBER;
	t = h;
	
	for( i = 0; i < NUMBER; ++i )
	{
		y = eval_term1( separatrice, t );
		/*   printf("i=%d x=%f y=%f \n",i,t,y); */
		dist[i] = sqrt(t*t+y*y);

		p0 = eval_vec_field(0,t,y);
		p1 = eval_vec_field(1,t,y);
		p2 = diff_eval_term1(separatrice,t);
		vec_slope[i] = atan2( p1, p0 );
		sep_slope[i] = atan2( p2, 1.0 ) + c;
		
		if( sep_slope[i] > PI )
			sep_slope[i] -= TWOPI;

		if( vec_slope[i] == 0 )
		{
			vec_slope[i] = TWOPI; 
			sep_slope[i] += TWOPI;
		}
		diff_slope[i] = fabs( vec_slope[i] - sep_slope[i] );
		if( diff_slope[i] > PI )
			diff_slope[i] = TWOPI - diff_slope[i];
	
		t += h;
	} 
}

// --------------------------------------------------------------------------
//                      READ_TERM2
// --------------------------------------------------------------------------

void read_term2( FILE * fp, struct term2 * current_term2, int l )
{
    int i;
    fscanf(fp,"%i %i %lf \n\n",&current_term2->exp_x,&current_term2->exp_y,&current_term2->coeff);
    current_term2->next_term2 = nullptr;
    for(i=2;i<=l;i++) {
        current_term2->next_term2 = new term2;//(struct term2 *) malloc (sizeof(struct term2));
        current_term2 = current_term2->next_term2;
        fscanf(fp,"%i %i %lf \n\n",&current_term2->exp_x,&current_term2->exp_y,&current_term2->coeff);
        current_term2->next_term2 = nullptr; 
    }
}

// --------------------------------------------------------------------------
//                      READ_TERM1
// --------------------------------------------------------------------------

void read_term1( FILE * fp, struct term1 * current_term1, int l )
{
    int i;
    fscanf(fp,"%i %lf \n\n",&current_term1->exp,&current_term1->coeff);
    current_term1->next_term1 = nullptr;
    for(i=2;i<=l;i++) {
        current_term1->next_term1 = new term1;//(struct term1 *) malloc (sizeof(struct term1));
        current_term1=current_term1->next_term1;
        fscanf(fp,"%i %lf \n\n",&current_term1->exp,&current_term1->coeff);
        current_term1->next_term1 = nullptr; 
    }
}

// --------------------------------------------------------------------------
//                      READ_SEP
// --------------------------------------------------------------------------

double read_sep( const char * file )
{ 
    FILE *fp;
    int l;
    double epsilon;

    fp=fopen(file,"r");
    fscanf(fp,"%lf \n\n",&epsilon);
    fscanf(fp,"%d \n\n",&direction);
    fscanf(fp,"%d\n\n",&stable);
    fscanf(fp,"%i \n\n",&l);
    vec_field[0] = new term2;//(struct term2 *) malloc (sizeof(struct term2));
    read_term2(fp,vec_field[0],l);
    fscanf(fp,"%i \n\n",&l);
    vec_field[1] = new term2;//(struct term2 *) malloc (sizeof(struct term2));
    read_term2(fp,vec_field[1],l);
    fscanf(fp,"%i \n\n",&l);
    separatrice = new term1;//(struct term1 *) malloc (sizeof(struct term1));
    read_term1(fp,separatrice,l);
    fclose(fp);
    return(epsilon);
}

#ifdef LOGACTIVITY

// --------------------------------------------------------------------------
//                      PRINTTERM1
// --------------------------------------------------------------------------

char * printterm1( char * buf, struct term1 * f, bool isfirst, char * x )
{
	if( f->coeff == 0 )
	{
		if( isfirst )
			strcpy( buf, "0" );
		else
			*buf = 0;
		return buf;
	}

	if( (f->coeff == 1 || f->coeff == -1) && f->exp != 0 )
	{
		if( f->coeff < 0 )
			sprintf( buf, "-" );
		else
			if( isfirst )
				*buf = 0;
			else
				sprintf( buf, "+" );

		if( f->exp != 1 )
			sprintf( buf+strlen(buf), "%s^%d", x, f->exp );
		else
			sprintf( buf+strlen(buf), "%s", x );
		return buf;
	}

	if( isfirst )
		sprintf( buf, "%g", (float)(f->coeff) );
	else
		sprintf( buf, "%+g", (float)(f->coeff) );

	if( f->exp != 0 )
	{
		if( f->exp != 1 )
			sprintf( buf+strlen(buf), "*%s^%d", x, f->exp );
		else
			sprintf( buf+strlen(buf), "*%s", x );
	}

	return buf;
}

// --------------------------------------------------------------------------
//                     	PRINTTERM2
// --------------------------------------------------------------------------

char * printterm2( char * buf, struct term2 * f, bool isfirst, char * x, char * y )
{
	if( f->coeff == 0 )
	{
		if( isfirst )
			strcpy( buf, "0" );
		else
			*buf = 0;
		return buf;
	}

	if( (f->coeff == 1 || f->coeff == -1) && (f->exp_x != 0 || f->exp_y != 0) )
	{
		if( f->coeff < 0 )
			sprintf( buf, "-" );
		else
			if( isfirst )
				*buf = 0;
			else
				sprintf( buf, "+" );

		if( f->exp_x != 0 )
		{
			if( f->exp_x != 1 )
				sprintf( buf+strlen(buf), "%s^%d", x, f->exp_x );
			else
				sprintf( buf+strlen(buf), "%s", x );

			if( f->exp_y != 0 )
			{
				if( f->exp_y != 1 )
					sprintf( buf+strlen(buf), "*%s^%d", y, f->exp_y );
				else
					sprintf( buf+strlen(buf), "*%s", y );
			}
		}
		else if( f->exp_y != 0 )
		{
			if( f->exp_y != 1 )
				sprintf( buf+strlen(buf), "%s^%d", y, f->exp_y );
			else
				sprintf( buf+strlen(buf), "%s", y );
		}

		return buf;
	}

	if( isfirst )
		sprintf( buf, "%g", (float)(f->coeff) );
	else
		sprintf( buf, "%+g", (float)(f->coeff) );

	if( f->exp_x != 0 )
	{
		if( f->exp_x != 1 )
			sprintf( buf+strlen(buf), "*%s^%d", x, f->exp_x );
		else
			sprintf( buf+strlen(buf), "*%s", x );
	}
	if( f->exp_y != 0 )
	{
		if( f->exp_y != 1 )
			sprintf( buf+strlen(buf), "*%s^%d", y, f->exp_y );
		else
			sprintf( buf+strlen(buf), "*%s", y );
	}

	return buf;
}

#endif

