#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <QSettings>
#include <QString>
#include <QMessageBox>

#include "file_paths.h"

QByteArray Win_GetLongPathName( QByteArray );
QByteArray Win_GetShortPathName( QByteArray );

// -----------------------------------------------------------------------
//                          GETDEFAULTP4PATH
// -----------------------------------------------------------------------
//
// Retrieves the path in which the P4 program is installed.
//
// The path is returned without trailing separator (/usr/local/P4 instead of /usr/local/P4/)
//
// Structure of the intallation:
//
//
//      /bin                contains binary of P4, sep and lyapunov, together with the maple file(s)
//      /sum_tables         contains intermediary results for sep and lyapunov.
//      /reduce             contains P4 reduce files (only under Unix)
//                          In particular, this folder contains the file p4.b
//      /help               contains P4 help files (.HTM)
//
//
// Under UNIX, the path is retrieved from the environment variable P4_DIR
//
// Note: Under UNIX, forward slashes are used, Under Windows, backward slashes are used.

QString GetDefaultP4Path( void )
{
    QString f;
#ifndef Q_OS_WIN
    if( getenv( "P4_DIR" ) != nullptr )
        f = (QString)getenv( "P4_DIR" );
#endif

    if( f.isNull() )
    {
        // try to find by another means
        
        f = QApplication::applicationFilePath();
        if( !f.isNull() )
        {
            if( f.length() > 0 )
            {
                if( f[f.length()-1] == QDir::separator() )
                {
                    // remove trailing slash if it is present
                    f = f.left(f.length()-1);
                }
            }
         }
         QFileInfo fi0(f);
         f = fi0.path();  // drop the executable's name (p4.exe)
         QFileInfo fi(f);
         QString fp, fn;
         fp = fi.path();
         fn = fi.fileName().toLower();
         
         if( fn != "bin" )
         {
             // error: the P4 exectuble is not stored in basepath/bin.
             // what to do???
#ifdef Q_OS_WIN
             f = "C:\\Program Files\\P4";    // return Windows default path
#else
             f = "/usr/local/p4";            // return Linux default path
#endif
         }
         else
             f = QDir::toNativeSeparators(fp);
    }

    if( !f.isNull() )
    {
        if( f.length() > 0 )
        {
            if( f[f.length()-1] == QDir::separator() )
            {
                // remove trailing slash if it is present
                f = f.left(f.length()-1);
            }
        }
     }
     
    return f;       // otherwhise f remains unassigned and is Null.
}

// -----------------------------------------------------------------------
//                          GETDEFAULTP4TEMPPATH
// -----------------------------------------------------------------------

QString GetDefaultP4TempPath( void )
{
#ifdef Q_OS_WIN
    QString f;
    QByteArray ba_f;

    f = QDir::toNativeSeparators( QDir::tempPath() );
    ba_f = Win_GetLongPathName( QFile::encodeName(f) );
    return QFile::decodeName( ba_f );

#else
    return QDir::toNativeSeparators( QDir::tempPath() );
#endif
}

// -----------------------------------------------------------------------
//                          GETP4SUMTABLEPATH
// -----------------------------------------------------------------------

QString GetDefaultP4SumTablePath( void )
{
    QString f;
    QString g;

    f = GetDefaultP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
#ifdef Q_OS_X11
        g += "sum_tables";
#else
        g += "sumtables";
#endif
    }

    return g;
}

// -----------------------------------------------------------------------
//                          GETDEFAULTMATHMANIPULATOR
// -----------------------------------------------------------------------

QString GetDefaultMathManipulator( void )
{
#ifdef Q_OS_WIN
     return "Maple";
#else
     return "Reduce";
#endif
}

// -----------------------------------------------------------------------
//                          GETDEFAULTP4HELPPATH
// -----------------------------------------------------------------------
//
// Retrieves the path in which the P4 help files are installed.
//
// The main P4 file is stored in main_p4.htm.

QString GetDefaultP4HelpPath( void )
{
    QString f;
    QString g;

    f = GetDefaultP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "help";
    }

    return g;
}

// -----------------------------------------------------------------------
//                          GETDEFAULTP4REDUCEPATH
// -----------------------------------------------------------------------

QString GetDefaultP4ReducePath( void )
{
    QString f;
    QString g;

    f = GetDefaultP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "reduce";
    }

    return g;
}

// -----------------------------------------------------------------------
//                          GETDEFAULTP4MAPLEPATH
// -----------------------------------------------------------------------
//
// The main maple file is stored together with the binary files in the /bin
// subdirectory

QString GetDefaultP4MaplePath( void )
{
    QString f;
    QString g;

    f = GetDefaultP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "bin";
    }

    return g;
}

// -----------------------------------------------------------------------
//                          GETDEFAULTP4BINPATH
// -----------------------------------------------------------------------
//
// The bin path is the path where the P4 executable is stored, together
// with the binary files of separatrice and lyapunov.

QString GetDefaultP4BinPath( void )
{
    QString f;
    QString g;

    f = GetDefaultP4Path();
    if( f.isNull() == false )
    {
        g = f;
        g += QDir::separator();
        g += "bin";
    }

    return g;
}

// -----------------------------------------------------------------------
//                          GETDEFAULTREDUCEINSTALLATION
// -----------------------------------------------------------------------
//
// Returns the full path name that leads to the reduce binary.  This
// may be a batch file name as well.
//
// Under Windows, return nullptr
//
// Under Linux, return the batch file name "reduce".

QString GetDefaultReduceInstallation( void )
{
#ifdef Q_OS_WIN
    QString r;
    return r;       // return nullptr (Windows Reduce version not supported)
#else
    return "reduce";
#endif
}

// -----------------------------------------------------------------------
//                          GETDEFAULTMAPLEINSTALLATION
// -----------------------------------------------------------------------
//
// Returns the full path name that leads to the maple-command line binary.
// This may be a batch file name as well.
//
// Under Windows, use registry
//
// Under Linux, return the batch file name "cmaple".

QString GetDefaultMapleInstallation( void )
{
#ifdef Q_OS_WIN
    QSettings * maplesettings;
    QString mapleversion, maplepath, exepath, version;

    maplesettings = new QSettings("HKEY_LOCAL_MACHINE\\Software\\Waterloo Maple",
                           QSettings::NativeFormat);
    mapleversion = maplesettings->value( "/CURRENT_MAPLE" ).toString();
    delete maplesettings;
    maplesettings = nullptr;
    
    if( mapleversion.length() != 0 )
    {
        maplesettings = new QSettings("HKEY_LOCAL_MACHINE\\Software\\Waterloo Maple\\" + mapleversion,
                        QSettings::NativeFormat );
        maplepath = maplesettings->value( "/EXEPATH" ).toString();
        delete maplesettings;
        maplesettings = nullptr;
        if( maplepath.length() != 0 )
        {
            QFileInfo maplepat( maplepath );
            if( maplepat.isDir() )
            {
                // mapleversion is of the form "Maple 9.5".  We search for the first occurence of digits
                version = mapleversion;
                while( version.length() > 0 )
                    if( version[0].isDigit() )
                        break;
                    else
                        version = version.mid(1);
            
                exepath = maplepath + "\\cmaple" + version + ".exe";

                maplepat.setFile( exepath );
                if( maplepat.exists() )
                {
                    if( exepath.contains( ' ' ) )
                        exepath = "\"" + exepath + "\"";
                    return exepath;
                }

                exepath = maplepath + "\\cmaple.exe";
            
                maplepat.setFile( exepath );
                if( maplepat.exists() )
                {
                    if( exepath.contains( ' ' ) )
                        exepath = "\"" + exepath + "\"";
                    return exepath;
                }
            }
        }
    }
    return "cmaple";
#else
    return "cmaple";
#endif
}

// -----------------------------------------------------------------------
//                          REMOVEFILE
// -----------------------------------------------------------------------

void RemoveFile( QString fname )
{
    QFile::remove( fname );
}


// --------------------------------------------------------------------
//               Some Windows Path Functions
// --------------------------------------------------------------------

#ifdef Q_OS_WIN

#include <windows.h>
#include <memory.h>
#include <string.h>

/*
DWORD ShortToLongPathName(
    LPCTSTR lpszShortPath,
    LPTSTR lpszLongPath, 
    DWORD cchBuffer)
{
    // Catch null pointers.
    if (!lpszShortPath || !lpszLongPath)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    // Check whether the input path is valid.
    if (0xffffffff == GetFileAttributes(lpszShortPath))
        return 0;

    // Special characters.
    TCHAR const sep = ('\\');
    TCHAR const colon = (':');

    // Make some short aliases for basic_strings of TCHAR.
    typedef std::basic_string<TCHAR> tstring;
    typedef tstring::traits_type traits;
    typedef tstring::size_type size;
    size const npos = tstring::npos;

    // Copy the short path into the work buffer and convert forward 
    // slashes to backslashes.
    tstring path = lpszShortPath;
    std::replace(path.begin(), path.end(), ('/'), sep);

    // We need a couple of markers for stepping through the path.
    size left = 0;
    size right = 0;

    // Parse the first bit of the path.
    if (path.length() >= 2 && isalpha(path[0]) && colon == path[1]) // Drive letter?
    {
        if (2 == path.length()) // 'bare' drive letter
        {
            right = npos; // skip main block
        }
        else if (sep == path[2]) // drive letter + backslash
        {
            // FindFirstFile doesn't like "X:\"
            if (3 == path.length())
            {
                right = npos; // skip main block
            }
            else
            {
                left = right = 3;
            }
        }
        else return 0; // parsing failure
    }
    else if (path.length() >= 1 && sep == path[0])
    {
        if (1 == path.length()) // 'bare' backslash
        {
            right = npos;  // skip main block
        }
        else 
        {
            if (sep == path[1]) // is it UNC?
            {
                // Find end of machine name
                right = path.find_first_of(sep, 2);
                if (npos == right)
                    return 0;

                // Find end of share name
                right = path.find_first_of(sep, right + 1);
                if (npos == right)
                    return 0;
            }
            ++right;
        }
    }
    // else FindFirstFile will handle relative paths

    // The data block for FindFirstFile.
    WIN32_FIND_DATA fd;

    // Main parse block - step through path.
    while (npos != right)
    {
        left = right; // catch up

        // Find next separator.
        right = path.find_first_of(sep, right);

        // Temporarily replace the separator with a null character so that
        // the path so far can be passed to FindFirstFile.
        if (npos != right)
            path[right] = 0;

        // See what FindFirstFile makes of the path so far.
        HANDLE hf = FindFirstFile(path.c_str(), &fd);
        if (INVALID_HANDLE_VALUE == hf)
            return 0;
        FindClose(hf);

        // Put back the separator.
        if (npos != right)
            path[right] = sep;

        // The file was found - replace the short name with the long.
        size old_len = (npos == right) ? path.length() - left : right - left;
        size new_len = traits::length(fd.cFileName);
        path.replace(left, old_len, fd.cFileName, new_len);

        // More to do?
        if (npos != right)
        {
            // Yes - move past separator .
            right = left + new_len + 1;

            // Did we overshoot the end? (i.e. path ends with a separator).
            if (right >= path.length())
                right = npos;
        }
    }

    // If buffer is too small then return the required size.
    if (cchBuffer <= path.length())
        return path.length() + 1;

    // Copy the buffer and return the number of characters copied.
    traits::copy(lpszLongPath, path.c_str(), path.length() + 1);
    return path.length();
}
*/

// -----------------------------------------------------------------------
//                          WIN_GETSHORTPATHNAME
// -----------------------------------------------------------------------

QByteArray Win_GetShortPathName( QByteArray f )
{
    char * shortfname;
    const char * longfname;
    QByteArray ba_fname;
    DWORD siz, reqsiz;

    longfname = (const char *)f;
    shortfname = (char *)malloc( siz = ba_fname.length()+1 );

    // call to Windows API

    reqsiz = GetShortPathNameA( longfname, shortfname, siz );
    if( reqsiz > siz )
    {
        shortfname = (char *)realloc( shortfname, reqsiz );
        reqsiz = GetShortPathNameA( longfname, shortfname, reqsiz );
    }
    if( reqsiz == 0 )
        free( shortfname );
    else
    {
        f = QByteArray(shortfname);
        free( shortfname );
    }
    return f;
}

// -----------------------------------------------------------------------
//                          WIN_GETLONGPATHNAME
// -----------------------------------------------------------------------

QByteArray Win_GetLongPathName( QByteArray f )
{
/*
    char * longfname;
    const char * shortfname;
    QByteArray ba_fname;
    DWORD siz, reqsiz;

    shortfname = (const char *)f;
    longfname = (char *)malloc( siz = ba_fname.length() );

    // call to Windows API

    reqsiz = ShortToLongPathName( shortfname, longfname, siz );

    if( reqsiz > siz )
    {
        longfname = (char *)realloc( longfname, reqsiz );
        reqsiz = ShortToLongPathName( shortfname, longfname, reqsiz );
    }

    if( reqsiz == 0 )
        free( longfname );
    else
    {
        f = QByteArray(longfname);
        free( longfname );
    }
*/
    return f;
}

#endif
