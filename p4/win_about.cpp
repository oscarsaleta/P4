#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qboxlayout.h>
#include "custom.h"
#include "table.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_about.h"
#include "win_settings.h"
#include "p4application.h"

extern void SetP4WindowTitle( QWidget *, QString );
extern QString GetP4BinPath();

QP4AboutDlg::QP4AboutDlg( QWidget * parent, Qt::WindowFlags f )
    : QDialog( parent, f )
{
    // general initialization

//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    // define controls

    if( p4smallicon != nullptr )
        setWindowIcon( *p4smallicon );

    btn_ok = new QPushButton( "&Ok" );
    btn_settings = new QPushButton( "&Main Settings" );

#ifdef TOOLTIPS
    btn_ok->setToolTip( "Go back to program" );
    btn_settings->setToolTip( "Access general program settings" );
#endif

    // define placement of controls

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom );

    QGridLayout * lay00 = new QGridLayout();
    lay00->addWidget( new QLabel( "P4 (Planar Polynomial Phase Portraits) by" ), 0, 1 );
    lay00->addWidget( new QLabel( "" ), 1, 1 );
    lay00->addWidget(new QLabel( QString::fromUtf8("    O. Saleta (Universitat Autònoma de Barcelona)")), 2, 1);
    lay00->addWidget( new QLabel( QString::fromUtf8("    J.C. Artés (Universitat Autònoma de Barcelona)") ), 3, 1 );
    lay00->addWidget( new QLabel( "    C. Herssens (Hasselt University)" ), 4, 1 );
    lay00->addWidget( new QLabel( "    P. De Maesschalck (Hasselt University)" ), 5, 1 );
    lay00->addWidget( new QLabel( "    F. Dumortier (Hasselt University)" ), 6, 1 );
    lay00->addWidget( new QLabel( QString::fromUtf8("    J. Llibre (Universitat Autònoma de Barcelona)") ), 7, 1 );
    lay00->addWidget( new QLabel( "" ), 8, 1 );


    QString versionstr;
    versionstr = " Version " + P4Version + "   " + P4VersionDate + " " + P4Platform;

    lay00->addWidget( new QLabel( versionstr ), 9, 1 );

    QLabel * l;
    l = new QLabel( "(missing image)" );
    if( p4image.load( GetP4BinPath() + "/portrait.png" ) )
        l->setPixmap( p4image );

    lay00->addWidget( l, 0, 0, 9, 1 );

    mainLayout->addLayout( lay00 );
    QHBoxLayout * buttons = new QHBoxLayout();
    buttons->addStretch(1);
    buttons->addWidget( btn_ok );
    buttons->addStretch(0);
    buttons->addWidget( btn_settings );
    mainLayout->addLayout( buttons );
    setLayout( mainLayout );

    // connections

    QObject::connect( btn_ok, SIGNAL(clicked()), this, SLOT(OnOk()) );
    QObject::connect( btn_settings, SIGNAL(clicked()), this, SLOT(OnSettings()) );

    btn_ok->setFocus();

    SetP4WindowTitle( this, "About P4" );
}

void QP4AboutDlg::OnOk( void )
{
    done(0);
}

void QP4AboutDlg::OnSettings( void )
{
    int value;
    QSettingsDlg * psettings;
    psettings = new QSettingsDlg( this, 0 );
    value = psettings->exec();
    delete psettings;
    psettings = nullptr;
    if( value )
        done(0);
}
