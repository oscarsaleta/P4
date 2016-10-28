#ifndef P4SETTINGS_H
#define P4SETTINGS_H


#include <QString>


QString GetP4Path( void );
QString GetP4TempPath( void );
QString GetP4SumTablePath( void );
QString GetMapleExe( void );
QString GetReduceExe( void );
void SetMathManipulator( QString s );
void SetP4Path( QString s );
void SetP4TempPath( QString s );
void SetP4SumTablePath( QString s );
void SetMapleExe( QString s );
void SetReduceExe( QString s );
extern QString GetP4MaplePath( void );
QString GetMathManipulator( void );
int GetMathPackage(void);
void SetMathPackage( int pck );
QString GetP4HelpPath( void );
QString GetP4BinPath( void );
QString GetP4ReducePath( void );
bool ReadP4Settings( void );
void SaveP4Settings( void );



#endif // P4SETTINGS_H
