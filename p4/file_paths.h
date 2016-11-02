#ifndef EXTERN_METHODS_H
#define EXTERN_METHODS_H


#include <QString>


// Methods implemented in file_paths.cpp

QString getDefaultP4Path( void );
QString getDefaultP4TempPath( void );
QString getDefaultP4SumTablePath( void );
QString getDefaultMathManipulator( void );
QString getDefaultP4HelpPath( void );
QString getDefaultP4ReducePath( void );
QString getDefaultP4MaplePath( void );
QString getDefaultP4BinPath( void );
QString getDefaultReduceInstallation( void );
QString getDefaultMapleInstallation( void );
void removeFile( QString fname );


#endif // EXTERN_METHODS_H
