// -----------------------------------------------------------------------
//
//                      ROUTINES TO HANDLE POLYNOMIALS
//
// -----------------------------------------------------------------------


#include "math_polynom.h"

#include <cmath>

#include "file_tab.h"


// -----------------------------------------------------------------------
//                              EVAL_TERM1
// -----------------------------------------------------------------------
//
// Calculates p(t) for a polynomial p and a value t.

double eval_term1( P4POLYNOM1 p, double t )
{
    double s;

    s = 0;
    while( p != nullptr )
    {
        if( p->exp != 0 )
            s += (p->coeff) * pow(t, (double)(p->exp) );
        else
            s += p->coeff;

        p = p->next_term1;
    }

    return s;
}

// -----------------------------------------------------------------------
//                              EVAL_TERM2
// -----------------------------------------------------------------------
//
// Calculates f(x,y) for a polynomial f and values x and y.
//
// value refers to an array containing x and y: value[0]=x, value[1]=y

double eval_term2( P4POLYNOM2 f, double * value )
{
    double s;

    s = 0;
    while( f != nullptr )
    {
        if( f->exp_x != 0 && f->exp_y != 0 )
            s += f->coeff * pow( value[0], (double)(f->exp_x) )
                          * pow( value[1], (double)(f->exp_y) );
        else if( f->exp_x != 0 )
            s += f->coeff * pow( value[0], (double)(f->exp_x) );
        else if( f->exp_y != 0 )
            s += f->coeff * pow( value[1], (double)(f->exp_y) );
        else
            s += f->coeff;

        f = f->next_term2;
    }

    return s;
}

// -----------------------------------------------------------------------
//                              EVAL_TERM3
// -----------------------------------------------------------------------
//
// Calculates F( r, cos(theta), sin(theta) ) for a polynomial f and values
// of r and theta.
//
// value refers to an array containing r and theta: value[0]=r, value[1]=theta

double eval_term3( P4POLYNOM3 F, double * value )
{ 
    double s;
    double t;
    double Co, Si;

    s = 0;
    Co = cos( value[1] );
    Si = sin( value[1] );

    while( F != nullptr )
    {
        if( F->exp_r != 0 )
            t = pow( value[0], (double)(F->exp_r) );
        else
            t = 1.0;

        if( F->exp_Co )
            t *= pow( Co, (double)(F->exp_Co) );

        if( F->exp_Si )
            t *= pow( Si, (double)(F->exp_Si) );

        s += t * F->coeff;
        F = F->next_term3;
    }

    return s;
}

// -----------------------------------------------------------------------
//                              DELETE_TERM1
// -----------------------------------------------------------------------

void delete_term1( P4POLYNOM1 p )
{
    P4POLYNOM1 q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_term1;
        delete q;//free( q );
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                              DELETE_TERM2
// -----------------------------------------------------------------------

void delete_term2( P4POLYNOM2 p )
{
    P4POLYNOM2 q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_term2;
        delete q;//free( q );
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                              DELETE_TERM3
// -----------------------------------------------------------------------

void delete_term3( P4POLYNOM3 p )
{
    P4POLYNOM3 q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_term3;
        delete q;//free( q );
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                              DUMPPOLY1
// -----------------------------------------------------------------------
//
// Make a string representation of a polynomial in one variable.
// The variable name to be used is given as a parameter

const char * dumpPoly1( P4POLYNOM1 f, const char * x )
{
    static QByteArray _s;

    double c;
    int cx;
    QString s;
    QString t;
    s = "";

    if( f == nullptr )
        return "0 (null)";

    while( f != nullptr )
    {
        c = f->coeff;
        cx = f->exp;

        if( (c != 1 && c != -1) || cx == 0 )
        {
            if( s.length() == 0 )
            {
                t.sprintf( "%g", (float)c );
            }
            else
            {
                if( c >= 0 )
                    t.sprintf( " + %g", (float)c );
                else
                    t.sprintf( " - %g", -(float)c );
            }
            s += t;
        }
        else
        {
            if( c == -1 )
            {
                if( s.length() == 0 )
                    s = "-";
                else
                    s += " - ";
            }
            else if( c == 1 )
            {
                if( s.length() != 0 )
                    s += " + ";
            }
        }
        if( cx != 0 )
        {
            if( cx == 1 )
                t = x;
            else
                t.sprintf( "%s^%d", x, cx );
            s+=t;
        }

        f = f->next_term1;
    }
    
    _s = s.toLatin1();
    return (const char *)_s;
}


// -----------------------------------------------------------------------
//                              DUMPPOLY2
// -----------------------------------------------------------------------
//
// Make a string representation of a polynomial in two variables.
// The variable names to be used are given as a parameter

const char * dumpPoly2( P4POLYNOM2 f, const char * x, const char * y )
{
    static QByteArray _s;

    double c;
    int cx, cy;
    QString s;
    QString t;
    s = "";

    if( f == nullptr )
        return "0 (null)";

    while( f != nullptr )
    {
        c = f->coeff;
        cx = f->exp_x;
        cy = f->exp_y;

        if( (c != 1 && c != -1) || (cx == 0 && cy == 0) )
        {
            if( s.length() == 0 )
            {
                t.sprintf( "%g", (float)c );
            }
            else
            {
                if( c >= 0 )
                    t.sprintf( " + %g", (float)c );
                else
                    t.sprintf( " - %g", -(float)c );
            }
            s += t;
        }
        else
        {
            if( c == -1 )
            {
                if( s.length() == 0 )
                    s = "-";
                else
                    s += " - ";
            }
            else if( c == 1 )
            {
                if( s.length() != 0 )
                    s += " + ";
            }
        }
        if( cx != 0 )
        {
            if( cx == 1 )
                t = x;
            else
                t.sprintf( "%s^%d", x, cx );
            s+=t;
        }
        if( cy != 0 )
        {
            if( cy == 1 )
                t = y;
            else
                t.sprintf( "%s^%d", y, cy );
            s += t;
        }

        f = f->next_term2;
    }
    _s = s.toLatin1();
    return (const char *)_s;
}


// -----------------------------------------------------------------------
//                              DUMPPOLY3
// -----------------------------------------------------------------------
//
// Make a string representation of a polynomial in two variables.
// The variable names to be used are given as a parameter

const char * dumpPoly3( P4POLYNOM3 f, const char * x, const char * y, const char * z )
{
    static QByteArray _s;

    double c;
    int cx, cy, cz;
    QString s;
    QString t;
    s = "";

    if( f == nullptr )
        return "0 (null)";

    while( f != nullptr )
    {
        c = f->coeff;
        cx = f->exp_r;
        cy = f->exp_Co;
        cz = f->exp_Si;

        if( (c != 1 && c != -1) || (cx == 0 && cy == 0 && cz == 0 ) )
        {
            if( s.length() == 0 )
            {
                t.sprintf( "%g", (float)c );
            }
            else
            {
                if( c >= 0 )
                    t.sprintf( " + %g", (float)c );
                else
                    t.sprintf( " - %g", -(float)c );
            }
            s += t;
        }
        else
        {
            if( c == -1 )
            {
                if( s.length() == 0 )
                    s = "-";
                else
                    s += " - ";
            }
            else if( c == 1 )
            {
                if( s.length() != 0 )
                    s += " + ";
            }
        }
        if( cx != 0 )
        {
            if( cx == 1 )
                t = x;
            else
                t.sprintf( "%s^%d", x, cx );
            s+=t;
        }
        if( cy != 0 )
        {
            if( cy == 1 )
                t = y;
            else
                t.sprintf( "%s^%d", y, cy );
            s += t;
        }
        if( cz != 0 )
        {
            if( cz == 1 )
                t = z;
            else
                t.sprintf( "%s^%d", z, cz );
            s += t;
        }

        f = f->next_term3;
    }
    
    _s = s.toLatin1();
    return (const char *)_s;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
//
// Following are used in preparation of GCF :

char * printterm2( char * buf, struct term2 * f, bool isfirst, const char * x, const char * y )
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

char * printterm3( char * buf, struct term3 * f, bool isfirst, const char * r, const char * Co, const char * Si )
{
    if( f->coeff == 0 )
    {
        if( isfirst )
            strcpy( buf, "0" );
        else
            *buf = 0;
        return buf;
    }

    if( (f->coeff == 1 || f->coeff == -1) && (f->exp_r != 0 || f->exp_Co != 0 || f->exp_Si != 0 ) )
    {
        if( f->coeff < 0 )
            sprintf( buf, "-" );
        else
            if( isfirst )
                *buf = 0;
            else
                sprintf( buf, "+" );

        if( f->exp_r != 0 )
        {
            if( f->exp_r != 1 )
                sprintf( buf+strlen(buf), "%s^%d", r, f->exp_r );
            else
                sprintf( buf+strlen(buf), "%s", r );

            if( f->exp_Co != 0 )
            {
                if( f->exp_Co != 1 )
                    sprintf( buf+strlen(buf), "*%s^%d", Co, f->exp_Co );
                else
                    sprintf( buf+strlen(buf), "*%s", Co );
            }
            if( f->exp_Si != 0 )
            {
                if( f->exp_Si != 1 )
                    sprintf( buf+strlen(buf), "*%s^%d", Si, f->exp_Si );
                else
                    sprintf( buf+strlen(buf), "*%s", Si );
            }
        }
        else
            if( f->exp_Co != 0 )
            {
                if( f->exp_Co != 1 )
                    sprintf( buf+strlen(buf), "%s^%d", Co, f->exp_Co );
                else
                    sprintf( buf+strlen(buf), "%s", Co );

                if( f->exp_Si != 0 )
                {
                    if( f->exp_Si != 1 )
                        sprintf( buf+strlen(buf), "*%s^%d", Si, f->exp_Si );
                    else
                        sprintf( buf+strlen(buf), "*%s", Si );
                }
            }
            else if( f->exp_Si != 0 )
            {
                if( f->exp_Si != 1 )
                    sprintf( buf+strlen(buf), "%s^%d", Si, f->exp_Si );
                else
                    sprintf( buf+strlen(buf), "%s", Si );
            }

        return buf;
    }

    if( isfirst )
        sprintf( buf, "%g", f->coeff );
    else
        sprintf( buf, "%+g", f->coeff );

    if( f->exp_r != 0 )
    {
        if( f->exp_r != 1 )
            sprintf( buf+strlen(buf), "*%s^%d", r, f->exp_r );
        else
            sprintf( buf+strlen(buf), "*%s", r );
    }
    if( f->exp_Co != 0 )
    {
        if( f->exp_Co != 1 )
            sprintf( buf+strlen(buf), "*%s^%d", Co, f->exp_Co );
        else
            sprintf( buf+strlen(buf), "*%s", Co );
    }
    if( f->exp_Si != 0 )
    {
        if( f->exp_Si != 1 )
            sprintf( buf+strlen(buf), "*%s^%d", Si, f->exp_Si );
        else
            sprintf( buf+strlen(buf), "*%s", Si );
    }

    return buf;
}

