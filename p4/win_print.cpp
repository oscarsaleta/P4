#include "win_print.h"

#include "custom.h"
#include "main.h"

#include <QLabel>


/*
    The Print Window is a modal dialog box!

    It lets the user select between different print methods and returns one of
    the P4PRINT_ constants.
*/

bool QPrintDlg::LastBlackWhite = false;
double QPrintDlg::LastLineWidth = DEFAULT_LINEWIDTH;
double QPrintDlg::LastSymbolSize = DEFAULT_SYMBOLSIZE;
int QPrintDlg::LastResolution = DEFAULT_RESOLUTION;

QPrintDlg::QPrintDlg( QWidget * parent, Qt::WindowFlags f )
    : QDialog(  parent, f )
{
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    if( p4smallicon != nullptr )
        setWindowIcon( *p4smallicon );

#ifdef USE_SYSTEM_PRINTER
    btn_default     = new QPushButton( "&System printer", this );
#endif
    btn_epsimage    = new QPushButton( "&EPS Image", this );
    btn_xfigimage   = new QPushButton( "&XFIG Image", this );
    btn_jpeg        = new QPushButton( "&JPEG Image", this );
    btn_cancel      = new QPushButton( "&Cancel", this );

    QLabel * label_1 = new QLabel( "Output resolution (in DPI): ", this );
    QLabel * label_2 = new QLabel( "Line width (in mm):", this );
    QLabel * label_3 = new QLabel( "Symbol size (in mm):", this );

    edt_resolution = new QLineEdit( "", this );
    edt_linewidth = new QLineEdit( "", this );
    edt_symbolsize = new QLineEdit( "", this );

    btn_blackwhite = new QCheckBox( "&Black&&White printing", this );
    btn_blackwhite->setChecked( LastBlackWhite );

#ifdef TOOLTIPS
#ifdef USE_SYSTEM_PRINTER
    btn_default->setToolTip( "Print through default printing mechanism.\nIn Linux, this is based on postscript printing." );
#endif
    btn_epsimage->setToolTip( "Produce a \"Encapsulated Postscript\" image file" );
    btn_xfigimage->setToolTip( "Produce a .FIG image file that can be read using XFIG" );
    btn_jpeg->setToolTip( "Produce a .JPG image file" );
    btn_cancel->setToolTip( "Cancel printing" );
    btn_blackwhite->setToolTip( "If checked, produce a black&white image, rather than a color image" );
    edt_resolution->setToolTip( "Choose a resolution. Images of 15cm wide are produced."
                                    "The number of horizontal pixels is hence 15*resolution/2.54.\n"
                                    "This number affects the quality, but not the phsyical dimensions of picture." );
    edt_linewidth->setToolTip( "The width of lines (orbits, separatrices, ...) in millimeters" );
    edt_symbolsize->setToolTip( "The size of the singularity annotation" );
#endif

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    mainLayout->addWidget( new QLabel( "Please select printing method...", this ) );

    QHBoxLayout * buttons = new QHBoxLayout();
#ifdef USE_SYSTEM_PRINTER
    buttons->addWidget( btn_default );
#endif
    buttons->addWidget( btn_epsimage );
    buttons->addWidget( btn_xfigimage );
    buttons->addWidget( btn_jpeg );
    buttons->addWidget( btn_cancel );
    mainLayout->addLayout( buttons );

#ifdef USE_SYSTEM_PRINTER
    QObject::connect( btn_default, SIGNAL(clicked()), this, SLOT(OnDefaultPrinter()) );
#endif
    QObject::connect( btn_epsimage, SIGNAL(clicked()), this, SLOT(OnEpsImagePrinter()) );
    QObject::connect( btn_xfigimage, SIGNAL(clicked()), this, SLOT(OnXfigImagePrinter()) );
    QObject::connect( btn_jpeg, SIGNAL(clicked()), this, SLOT(OnJpegImagePrinter()) );
    QObject::connect( btn_cancel, SIGNAL(clicked()), this, SLOT(OnCancel()) );

    mainLayout->addSpacing( 2 );
    mainLayout->addWidget( btn_blackwhite );
    mainLayout->addSpacing( 2 );

    QHBoxLayout * l1 = new QHBoxLayout();
    l1->addWidget( label_1 );
    l1->addWidget( edt_resolution );
    l1->addStretch(0);

    QHBoxLayout * l2 = new QHBoxLayout();
    l2->addWidget( label_2 );
    l2->addWidget( edt_linewidth );
    l2->addStretch(0);

    QHBoxLayout * l3 = new QHBoxLayout();
    l3->addWidget( label_3 );
    l3->addWidget( edt_symbolsize );
    l3->addStretch(0);

    mainLayout->addLayout( l1 );
    mainLayout->addLayout( l2 );
    mainLayout->addLayout( l3 );
    setLayout(mainLayout);

    QString s;
    s.sprintf("%g", (float)LastLineWidth);
    edt_linewidth->setText( s );
    s.sprintf("%g", (float)LastSymbolSize);
    edt_symbolsize->setText( s );
    s.sprintf("%d", LastResolution);
    edt_resolution->setText( s );

    setP4WindowTitle( this, "Print Phase Portrait" );

    return;
}

bool QPrintDlg::ReadDialog( void )
{
    double res;
    bool result;

    LastBlackWhite = btn_blackwhite->isChecked();
    result = ReadFloatField( edt_resolution, &res, DEFAULT_RESOLUTION, 72, 4800 );
    LastResolution = (int)res;
    result |= ReadFloatField( edt_linewidth, &LastLineWidth, DEFAULT_LINEWIDTH, MIN_LINEWIDTH, MAX_LINEWIDTH );
    result |= ReadFloatField( edt_symbolsize, &LastSymbolSize, DEFAULT_SYMBOLSIZE, MIN_SYMBOLSIZE, MAX_SYMBOLSIZE );
    
    return !result;
}

void QPrintDlg::OnDefaultPrinter( void )
{
    if( !ReadDialog() )
        return;

    if( LastBlackWhite )
        done( -P4PRINT_DEFAULT );
    else
        done( P4PRINT_DEFAULT );
}

void QPrintDlg::OnEpsImagePrinter( void )
{
    if( !ReadDialog() )
        return;
    if( LastBlackWhite )
        done( -P4PRINT_EPSIMAGE );
    else
        done( P4PRINT_EPSIMAGE );
}

void QPrintDlg::OnXfigImagePrinter( void )
{
    if( !ReadDialog() )
        return;

/*
    if( LastResolution != 1200 )
    {
        int i;
        i = QMessageBox::warning( this, "Warning",
                               "XFig printing can only  be done with a resolution of 1200 DPI.",
                               "Continue at 1200 DPI", "Cancel", 0, 0, 1 );
        if( i != 0 )
        {
            done( P4PRINT_NONE );
            return;
        }

        LastResolution = 1200;
    }
*/
    if( LastBlackWhite )
        done( -P4PRINT_XFIGIMAGE );
    else
        done( P4PRINT_XFIGIMAGE );
}

void QPrintDlg::OnJpegImagePrinter( void )
{
    if( !ReadDialog() )
        return;
    if( LastBlackWhite )
        done( -P4PRINT_JPEGIMAGE );
    else
        done( P4PRINT_JPEGIMAGE );
}

void QPrintDlg::OnCancel( void )
{
    done( P4PRINT_NONE );
}

bool QPrintDlg::ReadFloatField( QLineEdit * edt, double * presult,
                                   double defvalue, double minvalue, double maxvalue )
{
    // returns true in case of error

    QString x, t;
    bool ok;

    t = edt->text();
    *presult = t.toDouble( &ok );
    if( !ok || *presult < minvalue || *presult > maxvalue )
    {
        MarkBad( edt );
        *presult = defvalue;
        return true;
    }

    x.sprintf( "%g", (float)*presult );
    if( x != t )
        edt->setText(x);
    
    return false;
}

void QPrintDlg::MarkBad( QLineEdit * edt )
{
    QString t;
    int i;

    t = edt->text();
    while( (i = t.indexOf( '?' )) >= 0 )
        t.remove(i,1);
    t = t.trimmed();
    t.append( " ???" );

    edt->setText(t);
}

int QPrintDlg::GetChosenResolution( void ) { return LastResolution; }
double QPrintDlg::GetChosenLineWidth( void ) { return LastLineWidth; }
double QPrintDlg::GetChosenSymbolSize( void ) { return LastSymbolSize; }
