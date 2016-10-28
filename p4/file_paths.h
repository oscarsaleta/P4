#ifndef EXTERN_METHODS_H
#define EXTERN_METHODS_H


#include <QString>


// Methods implemented in file_paths.cpp

QString GetDefaultP4Path( void );
QString GetDefaultP4TempPath( void );
QString GetDefaultP4SumTablePath( void );
QString GetDefaultMathManipulator( void );
QString GetDefaultP4HelpPath( void );
QString GetDefaultP4ReducePath( void );
QString GetDefaultP4MaplePath( void );
QString GetDefaultP4BinPath( void );
QString GetDefaultReduceInstallation( void );
QString GetDefaultMapleInstallation( void );
void RemoveFile( QString fname );


#endif // EXTERN_METHODS_H
