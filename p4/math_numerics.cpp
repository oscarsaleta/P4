// -----------------------------------------------------------------------
//
//						NUMERIC ROUTINES
//
// - bisection
// - regula falsi
// - newton method to find a root
// - Runge-Kutta integration 7/8
//
// -----------------------------------------------------------------------

#include "math_numerics.h"

#include <cmath>
#include <cfloat>


#define p4_finite		finite

static double PRECISION1 = 1e-16;
static double PRECISION2 = 1e-8;

// -----------------------------------------------------------------------
//								BISECTION
// -----------------------------------------------------------------------
//
// dx is a pointer to an array of two elements

static void bisection( double (* f)(double), double * x, double e )
{
	double fx0, fmid, xmid;

	if( (*f)( x[0] ) > 0 )
	{
		xmid = x[0];
		x[0] = x[1];
		x[1] = xmid;
	}
	
	for( ;; )
	{
		xmid = (x[0] + x[1])/2;
	
		fx0 = (*f)( x[0] );
		fmid = (*f)( xmid );

		if( fx0 * fmid < 0)
			x[1] = xmid;
		else
			x[0] = xmid;

		if( fabs( x[1] - x[0] ) < e )
			break;
	}
}   
   
// -----------------------------------------------------------------------
//							REGULA FALSI
// -----------------------------------------------------------------------
//
// x is an array of two elements

static double regula_falsi( double (*f)(double), double * x, double e )
{
	double x2;
	double y;

	x2 = 2*x[1] - x[0];
	
	for(;;)
	{ 
		y = x[1] - (*f)(x[1]) * ((x[1] - x[0])/((*f)(x[1]) - (*f)(x[0])));
	
		if( fabs( y - x2 ) < e && f(y) < PRECISION2 )
			break;

		if( (*f)(x[1]) * (*f)(y) <= 0 )
			x[0] = y;
		else
			x[1] = y;
	
		x2 = y;
	}

	return y;
}

// -----------------------------------------------------------------------
//								NEWTON
// -----------------------------------------------------------------------

double newton( double (*f)(double), double (*df)(double), double x, double e )
{
	double dx;

	if( fabs( (*f)(x) ) < PRECISION1 && fabs( (*f)(x) / (*df)(x) ) < e )
		return x;

	for(;;)
	{
		dx = (*f)(x) / (*df)(x);
		x -= dx;
		if( fabs( (*f)(x) ) < PRECISION1 || fabs(dx) < e )
			break;
	}

	return x;
}

// -----------------------------------------------------------------------
//								FIND_ROOT
// -----------------------------------------------------------------------
//
// value is a pointer to an array of two elements

double find_root( double (*f)(double), double (*df)(double), double * value )
{
	double y;

	bisection( f, value, 0.01 );
	y = regula_falsi( f, value, 0.01);
	y = newton(f, df, y, 1e-8 );

	return y;
}

// -----------------------------------------------------------------------
//								RK78
// -----------------------------------------------------------------------

void rk78( void (*deriv)( double *, double * ), double y[2], double * hh,
			double hmi, double hma, double e1)
{
	double beta[79],c[11],d,dd,e3,h,r[13][2],b[2],f[2];
	int k;
	int direction;

	h = *hh;
	if( h < 0 )
		direction = -1;
	else
		direction=1;

	beta[0] = 0.;
	beta[1] = .07407407407407407;
	beta[2] = .027777777777777776;
	beta[3] = .083333333333333329;
	beta[4] = .041666666666666664;
	beta[5] = 0.;
	beta[6] = .125;
	beta[7] = .41666666666666669;
	beta[8] = 0.;
	beta[9] = -1.5625;
	beta[10] = -beta[9];
	beta[11] = .05;
	beta[12] = 0.;
	beta[13] = 0.;
	beta[14] = .25;
	beta[15] = .2;
	beta[16] = -.23148148148148148;
	beta[17] = 0.;
	beta[18] = 0.;
	beta[19] = 1.1574074074074074;
	beta[20] = -2.4074074074074074;
	beta[21] = beta[19] * 2.;
	beta[22] = .10333333333333333;
	beta[23] = 0.;
	beta[24] = 0.;
	beta[25] = 0.;
	beta[26] = .27111111111111114;
	beta[27] = -.22222222222222221;
	beta[28] = .014444444444444444;
	beta[29] = 2.;
	beta[30] = 0.;
	beta[31] = 0.;
	beta[32] = -8.8333333333333339;
	beta[33] = 15.644444444444444;
	beta[34] = -11.888888888888889;
	beta[35] = .74444444444444446;
	beta[36] = 3.;
	beta[37] = -.84259259259259256;
	beta[38] = 0.;
	beta[39] = 0.;
	beta[40] = .21296296296296297;
	beta[41] = -7.2296296296296294;
	beta[42] = 5.7592592592592595;
	beta[43] = -.31666666666666665;
	beta[44] = 2.8333333333333335;
	beta[45] = -.083333333333333329;
	beta[46] = .58121951219512191;
	beta[47] = 0.;
	beta[48] = 0.;
	beta[49] = -2.0792682926829267;
	beta[50] = 4.3863414634146345;
	beta[51] = -3.6707317073170733;
	beta[52] = .52024390243902441;
	beta[53] = .54878048780487809;
	beta[54] = .27439024390243905;
	beta[55] = .43902439024390244;
	beta[56] = .014634146341463415;
	beta[57] = 0.;
	beta[58] = 0.;
	beta[59] = 0.;
	beta[60] = 0.;
	beta[61] = -.14634146341463414;
	beta[62] = -.014634146341463415;
	beta[63] = -.073170731707317069;
	beta[64] = -beta[63];
	beta[65] = -beta[61];
	beta[66] = 0.;
	beta[67] = -.43341463414634146;
	beta[68] = 0.;
	beta[69] = 0.;
	beta[70] = beta[49];
	beta[71] = beta[50];
	beta[72] = -3.524390243902439;
	beta[73] = .53487804878048784;
	beta[74] = .62195121951219512;
	beta[75] = .20121951219512196;
	beta[76] = .29268292682926828;
	beta[77] = 0.;
	beta[78] = 1.;

	c[0] = .04880952380952381;
	c[1] = 0.;
	c[2] = 0.;
	c[3] = 0.;
	c[4] = 0.;
	c[5] = .32380952380952382;
	c[6] = .25714285714285712;
	c[7] = c[6];
	c[8] = .03214285714285714;
	c[9] = c[8];
	c[10] = c[0];

	for(;;)
	{
		for(k=0;k<2;++k)
			b[k]=y[k];
		deriv(b,r[0]); 

		for(k=0;k<2;++k)
			b[k]=y[k]+beta[1]*r[0][k]*h;
		deriv(b,r[1]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[2]*r[0][k]+beta[3]*r[1][k])*h;
		deriv(b,r[2]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[4]*r[0][k]+beta[6]*r[2][k])*h;
		deriv(b,r[3]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[7]*r[0][k]+beta[9]*(r[2][k]-r[3][k]))*h;
		deriv(b,r[4]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[11]*r[0][k]+beta[14]*r[3][k]+beta[15]*r[4][k])*h;
		deriv(b,r[5]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[16]*r[0][k]+beta[19]*r[3][k]+beta[20]*r[4][k]+beta[21]*r[5][k])*h;
		deriv(b,r[6]);
		
		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[22]*r[0][k]+beta[26]*r[4][k]+beta[27]*r[5][k]+beta[28]*r[6][k])*h;
		deriv(b,r[7]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[29]*r[0][k]+beta[32]*r[3][k]+beta[33]*r[4][k]+beta[34]*r[5][k]+beta[35]*r[6][k]+beta[36]*r[7][k])*h;
		deriv(b,r[8]);
		
		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[37]*r[0][k]+beta[40]*r[3][k]+beta[41]*r[4][k]+beta[42]*r[5][k]+beta[43]*r[6][k]+beta[44]*r[7][k]+beta[45]*r[8][k])*h;
		deriv(b,r[9]);

		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[46]*r[0][k]+beta[49]*r[3][k]+beta[50]*r[4][k]+beta[51]*r[5][k]+beta[52]*r[6][k]+beta[53]*r[7][k]+beta[54]*r[8][k]+beta[55]*r[9][k])*h;
		deriv(b,r[10]);
		
		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[56]*r[0][k]+beta[61]*(r[5][k]-r[9][k])+beta[62]*r[6][k]+beta[63]*(r[7][k]-r[8][k]))*h;
		deriv(b,r[11]);
		
		for(k=0;k<2;++k)
			b[k]=y[k]+(beta[67]*r[0][k]+beta[70]*r[3][k]+beta[71]*r[4][k]+beta[72]*r[5][k]+beta[73]*r[6][k]+beta[74]*r[7][k]+beta[75]*r[8][k]+beta[76]*r[9][k]+r[11][k])*h;
		deriv(b,r[12]);
 
		d=0;
		dd=0;

		for(k=0;k<2;++k)
		{
			b[k]=c[5]*r[5][k]+c[6]*(r[6][k]+r[7][k])+c[8]*(r[8][k]+r[9][k]);
			f[k]=y[k]+h*(b[k]+c[0]*(r[11][k]+r[12][k]));
			b[k]=y[k]+h*(b[k]+c[0]*(r[0][k]+r[10][k]));
			d=d+fabs(f[k]-b[k]); 
			dd=dd+fabs(f[k]);
		}
		d=d/2;
		e3=e1*(1.0+dd*1.E-2); 
        if( ((fabs(h)<=hmi)||(d<e3)) && !(std::isnan(f[0])) && !(std::isnan(f[1])) && p4_finite(f[0]) && p4_finite(f[1]) )
			break;
		
		h = h*0.9*sqrt(sqrt(sqrt(e3/d)));

        if((fabs(h)<hmi) || std::isnan(h) || !p4_finite(h) )  /* h=hmi*h/fabs(h); */
			h=hmi*direction;
	}
	if( d < e3 / 512 )
		d = e3 / 512;

	h = h * 0.9 * sqrt(sqrt(sqrt(e3/d)));
	for(k = 0; k < 2; ++k )
		y[k]=f[k];

	if( fabs(h) > hma )
		h = hma*direction;

    if((fabs(h)<hmi) || std::isnan(h) || !p4_finite(h) )
		h = hmi*direction;
	*hh = h;
}
