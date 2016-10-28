#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <math.h>
#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "win_p4.h"
#include "math_p4.h"
#include "plot_tools.h"

extern int PrintColorTable[NUMXFIGCOLORS];

extern void (* print_saddle)( double, double );
extern void (* print_stablenode)( double, double );
extern void (* print_unstablenode)( double, double );
extern void (* print_stableweakfocus)( double, double );
extern void (* print_unstableweakfocus)( double, double );
extern void (* print_weakfocus)( double, double );
extern void (* print_stablestrongfocus)( double, double );
extern void (* print_unstablestrongfocus)( double, double );
extern void (* print_sesaddle)( double, double );
extern void (* print_sesaddlenode)( double, double );
extern void (* print_sestablenode)( double, double );
extern void (* print_seunstablenode)( double, double );
extern void (* print_degen)( double, double );
extern void (* print_center)( double, double );
extern void (* print_elips)( double, double, double, double, int, bool, struct P4POLYLINES * );
extern void (* print_line)( double, double, double, double, int );
extern void (* print_point)( double, double, int );
extern void (* print_comment)( QString );

extern void spherePlotLine( QWinSphere * sp, double * p1, double * p2, int color );
extern void spherePlotPoint( QWinSphere * sp, double * p, int color );
extern void spherePrintLine( QWinSphere * sp, double * p1, double * p2, int color );
extern void spherePrintPoint( QWinSphere * sp, double * p, int color );

static bool PSBlackWhitePrint = true;

static QFile * PSFile = nullptr;
static QTextStream PSFileStream;

//static void PrintPSEllips( double x0, double y0, double a, double b );

void FinishPostscriptPrinting( void );
void PrintPostscript( void );

// ---------------------------------------------------------------------------------------

void ps_print_comment( QString s )
{
    if( PSFile != nullptr )
    {
        PSFileStream << "%% " << s << "\n";
    }
}

void ps_print_saddle( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f box\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CSADDLE], (float)x, (float)y );
        PSFileStream << s;
    }
}

void ps_print_stablenode( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f box\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CNODE_S], (float)x, (float)y );
        PSFileStream << s;
    }
}

void ps_print_unstablenode( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f box\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CNODE_U], (float)x, (float)y );
        PSFileStream << s;
    }
}

void ps_print_stableweakfocus( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f diamond\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CWEAK_FOCUS_S], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_unstableweakfocus( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f diamond\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CWEAK_FOCUS_U], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_weakfocus( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f diamond\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CWEAK_FOCUS], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_center( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f diamond\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CCENTER], (float)x, (float)y );
        PSFileStream << s;
    }
}

void ps_print_stablestrongfocus( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f diamond\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CSTRONG_FOCUS_S], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_unstablestrongfocus( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f diamond\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CSTRONG_FOCUS_U], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_sesaddle( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f triangle\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CSADDLE], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_sesaddlenode( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f triangle\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CSADDLE_NODE], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_sestablenode( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f triangle\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CNODE_S], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_seunstablenode( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f triangle\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CNODE_U], (float)x, (float)y );
        PSFileStream << s;
    }
}
void ps_print_degen( double x, double y )
{
    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( 
                "LW 2.6 mul setlinewidth\n"
                "col%d %f %f cross\n"
                "LW setlinewidth\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[CDEGEN], (float)x, (float)y );
        PSFileStream << s;
    }
}

void ps_print_elips( double x0, double y0, double a, double b, int color, bool dotted, struct P4POLYLINES * )
{
    double t,h;
    QString s;

    // we do not use the precompiled form of the ellipse printing.  Here, it is not necessary,
    // since the PS command for drawing ellipses works just fine.

    color = PrintColorTable[color];

    if( PSFile != nullptr )
    {
        if( dotted )
             PSFileStream << "gsave\n";
 
        s.sprintf( "col%d\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : color );
        PSFileStream << s;
        PSFileStream << "newpath\n";
        
        s.sprintf( "%f %f moveto\n", (float)(x0+a), (float)y0 );
        PSFileStream << s;
        h = PI/100;
        for(t = h; t < TWOPI; t += h )
        {
             s.sprintf( "%f %f lineto \n", (float)(x0+a*cos(t)), (float)(y0+b*sin(t)) );
             PSFileStream << s;
        }

        PSFileStream << "closepath\n";
        PSFileStream << "LW setlinewidth\n";
        if( dotted )
        {
            PSFileStream << "[DS ] 0 setdash\n";
        }
        PSFileStream << "stroke\n" ;
        if( dotted )
        {
            PSFileStream << "grestore\n";
        }
    }
}

void ps_print_line( double x0, double y0, double x1, double y1, int color )
{
    if( x0==x1 && y0==y1 ) 
        return;

    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "%f %f moveto\n%f %f lineto col%d stroke\n", (float)x0, (float)y0, (float)x1, (float)y1, 
                PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[color] );
        PSFileStream << s;
    }
}

static double lastpsx0 = 0;
static double lastpsy0 = 0;
static int lastpscolor = 0;

void ps_print_point( double x0, double y0, int color )
{
    if( lastpsx0==x0 && lastpsy0==y0 && lastpscolor==color )
        return; // just a small protection against big PS files: do not print series of the same points.

    lastpsx0=x0;
    lastpsy0=y0;
    lastpscolor=color;

    if( PSFile != nullptr )
    {
        QString s;
        s.sprintf( "col%d %f %f dot\n", PSBlackWhitePrint ? PrintColorTable[CFOREGROUND] : PrintColorTable[color], (float)x0, (float)y0 );
        PSFileStream << s;
    }
}

// ---------------------------------------------------------------------------------------

void PreparePostscriptPrinting( int x0, int y0, int w, int h, bool iszoom, bool isblackwhite,
                                int resolution, int linewidth, int symbolwidth )
{
    QString s;
    double bbx0, bby0, bbw, bbh, bbx1,  bby1;
    double scalefactor;
    scalefactor = 72;
    scalefactor /= resolution;

    bbx0 = x0;  bbx0 /= resolution; bbx0 *= 72; bbx0 += 0.5;
    bby0 = y0;  bby0 /= resolution; bby0 *= 72; bby0 += 0.5;
    bbw = w;    bbw /= resolution;  bbw *= 72;  bbw += 0.5;
    bbh = h;    bbh /= resolution;  bbh *= 72;  bbh += 0.5;
    bbx1 = bbx0 + bbw;
    bby1 = bby0 + bbh;

    PSBlackWhitePrint = isblackwhite;

    s = ThisVF->getbarefilename() + ".eps";

    PSFile = new QFile( s );
    if( PSFile->open( QIODevice::WriteOnly ) )
        PSFileStream.setDevice( PSFile );
    else
    {
        delete PSFile;
        PSFile = nullptr;
    }

    lastpscolor = -1;

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = ps_print_saddle;
    print_stablenode = ps_print_stablenode;
    print_unstablenode = ps_print_unstablenode;
    print_stableweakfocus = ps_print_stableweakfocus;
    print_unstableweakfocus = ps_print_unstableweakfocus;
    print_weakfocus = ps_print_weakfocus;
    print_stablestrongfocus = ps_print_stablestrongfocus;
    print_unstablestrongfocus = ps_print_unstablestrongfocus;
    print_sesaddle = ps_print_sesaddle;
    print_sesaddlenode = ps_print_sesaddlenode;
    print_sestablenode = ps_print_sestablenode;
    print_seunstablenode = ps_print_seunstablenode;
    print_degen = ps_print_degen;
    print_center = ps_print_center;
    print_elips = ps_print_elips;
    print_point = ps_print_point;
    print_line = ps_print_line;
    print_comment = ps_print_comment;

    QString title;
    QString datestring;

    title = "Phase portrait of \"" + ThisVF->getbarefilename() + "\"";
    if( iszoom )
        title += " (zoom window)";

    if( PSFile != nullptr )
    {
        // to calculate bounding box, we need to convert to point measurements (1 point = 1/72 inch)

        s.sprintf( 
            "%%!PS-Adobe-3.0 EPSF-3.0\n"
            "%%%%BoundingBox: %d %d %d %d\n",
            (int)bbx0, (int)bby0, (int)bbx1, (int)bby1 );

        PSFileStream << s;
        PSFileStream << "%%%%Title: " << title << "\n";
        PSFileStream << "%%%%Creator: P4\n";
        PSFileStream << "%%%%CreationDate: " << datestring << "\n";
        PSFileStream << "%%%%EndComments\n\n";

        s.sprintf(
                "%% User specified resolution = %d DPI\n"
                "%% User specified Linewidth and symbolwidth, in pixels:\n\n"
                "/LW %d def\n"
                "/SW %d def\n\n"
                "%% Derived dash pattern:\n"
                "/DS %d def\n\n", resolution, linewidth, symbolwidth, linewidth*6 );
        PSFileStream << s;

        s.sprintf( 
                "%% Transformation from default 72 DPI to chosen resolution, and shift towards center of page:\n"
                "/scaletransformation{\n"
                "   %f %f scale\n"
                "   %d %d translate}def\n\n",  (float)scalefactor, (float)scalefactor, x0, y0 );

        PSFileStream << s;

        s.sprintf( 
            "/frame{\n"
            "   0 setlinewidth\n"
            "   newpath 0 0 moveto 0 %d lineto %d %d lineto %d 0 lineto closepath\n"
            "   clip stroke}def\n\n", h, w, h, w );
        PSFileStream << s;

        PSFileStream <<
            "%% Color table:\n"
            "/col-1 {} def\n"
            "/col0 {0.000 0.000 0.000 setrgbcolor} bind def\n"
            "/col1 {0.000 0.000 1.000 setrgbcolor} bind def\n"
            "/col2 {0.000 1.000 0.000 setrgbcolor} bind def\n"
            "/col3 {0.000 1.000 1.000 setrgbcolor} bind def\n"
            "/col4 {1.000 0.000 0.000 setrgbcolor} bind def\n"
            "/col5 {1.000 0.000 1.000 setrgbcolor} bind def\n"
            "/col6 {1.000 1.000 0.000 setrgbcolor} bind def\n"
            "/col7 {1.000 1.000 1.000 setrgbcolor} bind def\n"
            "/col8 {0.000 0.000 0.560 setrgbcolor} bind def\n"
            "/col9 {0.000 0.000 0.690 setrgbcolor} bind def\n"
            "/col10 {0.000 0.000 0.820 setrgbcolor} bind def\n"
            "/col11 {0.530 0.810 1.000 setrgbcolor} bind def\n"
            "/col12 {0.000 0.560 0.000 setrgbcolor} bind def\n"
            "/col13 {0.000 0.690 0.000 setrgbcolor} bind def\n"
            "/col14 {0.000 0.820 0.000 setrgbcolor} bind def\n"
            "/col15 {0.000 0.560 0.560 setrgbcolor} bind def\n"
            "/col16 {0.000 0.690 0.690 setrgbcolor} bind def\n"
            "/col17 {0.000 0.820 0.820 setrgbcolor} bind def\n"
            "/col18 {0.560 0.000 0.000 setrgbcolor} bind def\n"
            "/col19 {0.690 0.000 0.000 setrgbcolor} bind def\n"
            "/col20 {0.820 0.000 0.000 setrgbcolor} bind def\n"
            "/col21 {0.560 0.000 0.560 setrgbcolor} bind def\n"
            "/col22 {0.690 0.000 0.690 setrgbcolor} bind def\n"
            "/col23 {0.820 0.000 0.820 setrgbcolor} bind def\n"
            "/col24 {0.500 0.190 0.000 setrgbcolor} bind def\n"
            "/col25 {0.630 0.250 0.000 setrgbcolor} bind def\n"
            "/col26 {0.750 0.380 0.000 setrgbcolor} bind def\n"
            "/col27 {1.000 0.500 0.500 setrgbcolor} bind def\n"
            "/col28 {1.000 0.630 0.630 setrgbcolor} bind def\n"
            "/col29 {1.000 0.750 0.750 setrgbcolor} bind def\n"
            "/col30 {1.000 0.880 0.880 setrgbcolor} bind def\n"
            "/col31 {1.000 0.840 0.000 setrgbcolor} bind def\n";

        PSFileStream <<
            "/box{ moveto\n"
            "SW neg 2 div SW 2 div rmoveto\n"
            "SW  0 rlineto\n"
            "0  SW neg rlineto\n"
            "SW neg 0 rlineto\n"
            "closepath\n"
            "fill\n"
            "}bind def\n\n";

        PSFileStream <<
            "/dot{\n"
            "LW 2 div 0 360 arc fill\n"
            "}bind def\n\n";

        PSFileStream << 
            "/diamond{\n"
            "moveto\n"
            "0  SW 2 div 1.3 mul rmoveto\n"
            "SW 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
            "SW neg 2 div 1.3 mul SW neg 2 div 1.3 mul rlineto\n"
            "SW neg 2 div 1.3 mul SW 2 div 1.3 mul rlineto\n"
            "closepath\n"
            "fill\n"
            "}bind def\n\n";

        PSFileStream <<
            "/triangle{\n"
            "moveto\n"
            "0  SW 2 div 1.2 mul rmoveto\n"
            "SW 2 div 1.2 mul SW neg 1.2 mul rlineto\n"
            "SW 1.2 mul neg 0  rlineto\n"
            "fill\n"
            "}bind def\n\n";

        PSFileStream <<
            "/cross{\n"
            "moveto\n"
            "SW 2 div SW 2 div rmoveto\n"
            "SW neg SW neg rlineto\n"
            "0  SW rmoveto\n"
            "SW SW neg rlineto\n"
            "stroke\n"
            "}bind def\n\n";

        s.sprintf(
            "%% Main Picture (local coordinates (x0,y0,x1,y1)=(0,0,%d,%d):\n"
            "gsave\n"
            "scaletransformation\n"
            "[] 0 setdash\n", w, h );
        PSFileStream << s;

        if( iszoom || VFResults.typeofview == TYPEOFVIEW_PLANE )
        {
            PSFileStream << "frame\n";
        }

        PSFileStream <<
            "\n"
            "%% Plot the orbits\n\n"
            "LW setlinewidth\n";
    }
}

void FinishPostscriptPrinting( void )
{
    if( PSFile != nullptr )
    {
        PSFileStream <<
            "grestore\n"
            "showpage\n"
            "%%EOF\n";

        PSFileStream.flush();
        PSFileStream.setDevice(nullptr);
        PSFile->close();
        delete PSFile;
        PSFile = nullptr;
    }

    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
}
