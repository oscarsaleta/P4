#ifndef P4APPLICATION_H
#define P4APPLICATION_H


#include <QApplication>
#include <QFont>
#include <QProcess>


#ifdef Q_OS_WIN
#define USERPLATFORM        "WINDOWS"
#else
#define USERPLATFORM        "LINUX"
#endif


class QP4Application : public QApplication
{
Q_OBJECT

public:
    QP4Application( int & argc, char ** argv );
    
    QFont * StandardFont;
    QFont * BoldFont;
    QFont * CourierFont;
    QFont * BoldCourierFont;
    QFont * TitleFont;
    QFont * LegendFont;

public slots:
    void Signal_Evaluating( void );
    void Signal_Evaluated( int );
    void Signal_GcfEvaluated( int );
    void Signal_Changed( void );
    void Signal_Loaded( void );
    void Signal_Saved( void );
    void cathProcessError( QProcess::ProcessError );
};

extern QP4Application * p4app;


#endif /* P4APPLICATION_H */
