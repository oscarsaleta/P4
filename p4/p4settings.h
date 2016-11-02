#ifndef P4SETTINGS_H
#define P4SETTINGS_H


#include <QString>


void setP4Path( QString s );
QString getP4Path( void );

void setP4TempPath( QString s );
QString getP4TempPath( void );

void setP4SumTablePath( QString s );
QString getP4SumTablePath( void );

void setMapleExe( QString s );
QString getMapleExe( void );

void setReduceExe( QString s );
QString getReduceExe( void );

QString getP4HelpPath( void );
QString getP4BinPath( void );
QString getP4ReducePath( void );
QString getP4MaplePath( void );

void setMathManipulator( QString s );
QString getMathManipulator( void );

void setMathPackage( int pck );
int getMathPackage(void);

bool readP4Settings( void );
void saveP4Settings( void );



#endif // P4SETTINGS_H
