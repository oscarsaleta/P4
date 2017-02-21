/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include "../../src-gui/version.h"

// -----------------------------------------------------------------------
//								GLOBAL VARIABLES
// -----------------------------------------------------------------------

// Variables related to the command-line:

bool InputFileGiven;
bool MapleFileGiven;
bool Opt_MapleCommandGiven;
bool Opt_NoMaple;
bool Opt_NoLogo;
bool Opt_LaTeXEnvironmentGiven;
bool Opt_Quiet;
bool Opt_Stdin;
bool Opt_Stdout;
bool Opt_MapleHeading;

char * InputFileName;
char * MapleFileName;
const char * MapleCommand;
const char * LaTeXEnvironment;

int ExitValue;

// -----------------------------------------------------------------------
//								FUNCTION PROTOTYPES
// -----------------------------------------------------------------------

int main( int, char * [] );
void PrintHelp( void );
int ProcessOption( char *, char * );
void RunMaple( char * );
void GenerateOutputFileName( void );
void TranslateFile( const char *, const char * );
void ReplaceExtension( char *, const char *, const char * );
bool HasExtension( const char * );

// -----------------------------------------------------------------------
//								MAIN
// -----------------------------------------------------------------------

int main( int argc, char * argv[] )
{
	int j, k;
	char * arg;

	Opt_MapleCommandGiven = false;
	InputFileGiven = false;
	MapleFileGiven = false;
	Opt_NoMaple = false;
	Opt_Quiet = false;
	Opt_Stdin = false;
	Opt_Stdout = false;
	Opt_MapleHeading = false;
	Opt_LaTeXEnvironmentGiven = false;
	
	LaTeXEnvironment = "lstlisting";
	MapleCommand = "cmaple9";

	ExitValue = 0;

	// default options:

	Opt_NoLogo = false;

	// process command line:

	if( argc == 1 )
	{
		PrintHelp();
		exit(0);
	}

	for( j = 1; j < argc; j++ )
	{
		arg = argv[j];
		if( *arg == '-' )
		{
			arg++;

			// process option

			if( j+1 >= argc )
				k = ProcessOption(arg, NULL);
			else
				k = ProcessOption(arg, argv[j+1]);
			if( k == -1 )
				exit(ExitValue);

			j += k; // skip extra arguments if this was necessary
			continue;
		}

		// not an option

		if( !InputFileGiven )
		{
			if( !Opt_Stdin )
			{
				InputFileName = arg;
				InputFileGiven = true;
			}
			else
			{
				if( !MapleFileGiven )
				{
					MapleFileName = arg;
					MapleFileGiven = true;
				}
			}
		}
		else
		{
			if( !MapleFileGiven )
			{
				MapleFileName = arg;
				MapleFileGiven = true;
			}
		}
	}

	if( Opt_Stdin && InputFileGiven )
	{
		printf( "Syntax Error: name of input file is given in stdin mode.\n" );
		printf( "Use -h to display help.\n" );
		ExitValue = -1;
		return ExitValue;
	}
	if( Opt_Stdout && MapleFileGiven )
	{
		printf( "Syntax Error: name of maple file is given in stdout mode.\n" );
		printf( "Use -h to display help.\n" );
		ExitValue = -1;
		return ExitValue;
	}

	if( !InputFileGiven && !Opt_Stdin )
	{
		printf( "Syntax Error: name of input file missing.\n" );
		printf( "Use -h to display help.\n" );
		ExitValue = -1;
		return ExitValue;
	}

	if( Opt_Stdout )
	{
		Opt_NoMaple = true;
	}

	if( !MapleFileGiven && !Opt_Stdout )
	{
		GenerateOutputFileName();
	}

	if( !Opt_NoLogo && !Opt_Quiet )
	{
		printf( "MPLSTRIP " VERSION "\n" );
	}

	TranslateFile( InputFileName, MapleFileName );

	if( !Opt_NoMaple && ExitValue == 0 && !Opt_Stdout )
	{
		RunMaple( MapleFileName );
	}

	return ExitValue;
}

// -----------------------------------------------------------------------
//								PRINTHELP
// -----------------------------------------------------------------------
//
// Displays information when program is called with -h, -help or -? option.
//

void PrintHelp( void )
{
	printf(
		"\n"
		"MPLSTRIP is a Maple source code commenter.\n"
		"It translates a LaTeX input file (.tex) to a Maple file (.mpl),\n"
		"extracting all \"lstlisting\" parts from the LaTeX file.\n"
		"\n"
		"Syntax:\n"
		"    MPLSTRIP [-options] inputfile[.tex]  [outputfile[.mpl]]\n\n"
		"Options:\n"
		"    -h          Displays this help.\n"
		"    -q          Quiet mode. (Suppresses display of information).\n"
		"    -stdout     Convert to standard output instead of file.\n"
		"    -stdin      Convert from standard input instead from file.\n"
		"    -nologo     Suppresses printing of logo.\n"
		"    -x          Suppresses call to Maple after conversion.\n"
		"    -head       Inserts an automatically generated heading in the maple file.\n"
		"    -e=name     Extracts all LaTeX environments of given name.\n"
		"    -M=command  Specifies the command that invokes Maple.\n"
		"\n"
	);

}

// -----------------------------------------------------------------------
//								PROCESSOPTION
// -----------------------------------------------------------------------
//
// Interpret an option specified on the command line: -h, -help, -?, -nologo, ...
//
// If an error is encountered, the routine returns -1, making the program stop.
//
// If an option needs to interpret the next command-line argument, it finds
// this argument in the parameter extraarg, and it should return 0.
//
// Otherwhise, it should return 0.

int ProcessOption( char * opt, char * extraarg )
{
	if( !strcmp( opt, "nologo" ) )
	{
		Opt_NoLogo = true;
		return 0;
	}

	if( !strcmp( opt, "x" ) )
	{
		Opt_NoMaple = true;
		return 0;
	}

	if( !strcmp( opt, "M" ) || !strncmp( opt, "M=", 2 ) )
	{
		if( opt[1] == '=' )
		{
			opt+=2;
			while( isspace(*opt) && *opt != 0 ) opt++; // remove white space
			if( *opt != 0 )
			{
				MapleCommand = strdup(opt);
				Opt_MapleCommandGiven = true;
				return 0;
			}
		}
		if( extraarg == NULL )
		{
			printf( "Wrong use of option -M\n" );
			ExitValue = -1;
			return -1;			
		}
		MapleCommand = strdup(extraarg);
		Opt_MapleCommandGiven = true;
		return 1;
	}

	if( !strcmp( opt, "e" ) || !strncmp( opt, "e=", 2 ) )
	{
		if( opt[1] == '=' )
		{
			opt+=2;
			while( isspace(*opt) && *opt != 0 ) opt++; // remove white space
			if( *opt != 0 )
			{
				LaTeXEnvironment = strdup(opt);
				Opt_LaTeXEnvironmentGiven = true;
				return 0;
			}
		}
		if( extraarg == NULL )
		{
			printf( "Wrong use of option -E\n" );
			ExitValue = -1;
			return -1;			
		}
		LaTeXEnvironment = strdup(extraarg);
		Opt_LaTeXEnvironmentGiven = true;
		return 1;
	}

	if( !strcmp( opt, "help" ) || !strcmp( opt, "h" ) || !strcmp( opt, "?" ) )
	{
		PrintHelp();
		ExitValue = 0;
		return -1;
	}

	if( !strcmp( opt, "stdin" ) )
	{
		Opt_Stdin = true;
		return 0;
	}

	if( !strcmp( opt, "stdout" ) )
	{
		Opt_Stdout = true;
		return 0;
	}

	if( !strcmp( opt, "head" ) || !strcmp( opt, "heading" ) )
	{
		Opt_MapleHeading = true;
		return 0;
	}

	if( !strcmp( opt, "q" ) || !strcmp( opt, "quiet" ) )
	{
		Opt_Quiet = true;
		return 0;
	}

	printf( "Unknown option %s\n", opt );
	ExitValue = -1;
	return -1;
}

// -----------------------------------------------------------------------
//								RUNMAPLE
// -----------------------------------------------------------------------
//
// Runs Maple with the result file.

void RunMaple( char * fname )
{
	char * cmd;

	cmd = (char *)malloc( strlen(MapleCommand) + 3 + strlen(fname) + 10 );
	
	if( cmd == NULL )
	{
		perror(NULL);
		ExitValue = 1;
		return;
	}

	if( strchr( MapleCommand, ' ' ) )
		strcat( strcat( strcat( strcat( strcpy( cmd, "\"" ), MapleCommand ), "\"" ), " " ), fname );
	else
		strcat( strcat( strcpy( cmd, MapleCommand ), " " ), fname );

	system( cmd );
}

// -----------------------------------------------------------------------
//							HASEXTENSION
// -----------------------------------------------------------------------
//
// Returns true if the filename specification contains an extension,
// or false if not.
//
// Examples:	c:\program files\test.txt	--> true
//				c:\file.					--> true
//				a:\bin.win\alpha			--> false

bool HasExtension( const char * fname )
{
	bool ext;
	int k;

	ext = false;
	for( k = 0; fname[k] != 0; k++ )
	{
		if( fname[k] == '.' )
			ext = true;
		if( fname[k] == '/' || fname[k] == '\\' || fname[k] == ':' )
			ext = false;
	}

	return ext;
}

// -----------------------------------------------------------------------
//							REPLACEEXTENSION
// -----------------------------------------------------------------------
//
// Replaces the extension of a filename with an given extension.  If the
// name of the file had no extension beforehand, then the extension must
// be given.
//
// The ext parameter should start with a dot, or should be NULL to erase
// the file name extension.

void ReplaceExtension( char * buffer, const char * src, const char * ext )
{
	int lastdot;
	int k;

	lastdot = -1;

	for( k = 0; src[k] != 0; k++ )
	{
		buffer[k] = src[k];
		if( src[k] == '.' )
			lastdot = k;

		if( src[k] == '/' || src[k] == '\\' || src[k] == ':' )
			lastdot = -1;
	}

	if( lastdot == -1 )
		lastdot = k;

	if( ext == NULL )
		buffer[lastdot] = 0;
	else
		strcpy( buffer+lastdot, ext );
}

// -----------------------------------------------------------------------
//							GENERATEOUTPUTFILENAME
// -----------------------------------------------------------------------

void GenerateOutputFileName( void )
{
	MapleFileName = (char *)malloc( strlen( InputFileName ) + 10 );
	if( MapleFileName == NULL )
	{
		perror(NULL);
		exit(-1);
	}

	ReplaceExtension( MapleFileName, InputFileName, ".mpl"  );

	if( !strcmp( MapleFileName, InputFileName ) )
	{
		printf( "Error: the input file must NOT be an .mpl file!\n" );
		ExitValue = -1;
		exit( ExitValue );
	}
}

// -----------------------------------------------------------------------
//								TRANSLATEFILE
// -----------------------------------------------------------------------

void TranslateFile( const char * src, const char * dst )
{
	char line[1024];
	char begincmd[256];
	char endcmd[256];
	int linenr;
	struct stat fileinfo;
	int writtenlines;
	char * tmp;
	time_t curtime;
	FILE * fpi;
	FILE * fpo;
	bool copying;

	if( !Opt_Stdin )
	{
		if( !HasExtension(src) )
		{
			tmp = (char *)malloc( strlen(src)+10 );
			if( tmp == NULL )
			{
				perror(NULL);
				ExitValue=-1;
				exit(ExitValue);
			}
			ReplaceExtension( tmp, src, ".tex" );
			src=tmp;
		}
	}
	else
		src = "stdin";
	
	if( !Opt_Stdout )
	{
		if( !HasExtension(dst) )
		{
			tmp = (char *)malloc( strlen(dst)+10 );
			if( tmp == NULL )
			{
				perror(NULL);
				ExitValue=-1;
				exit(ExitValue);
			}
			ReplaceExtension( tmp, dst, ".mpl" );
			dst=tmp;
		}
	}
	else
		dst = "stdout";

	if( !Opt_Quiet)
		printf( "Extracting from %s to %s ...\n" , src, dst );

	if( Opt_Stdin )
		fpi = stdin;
	else
		fpi = fopen( src, "rt" );

	if( fpi == NULL )
	{
		perror( "Cannot open input file" );
		ExitValue = -1;
		return;
	}

	if( Opt_Stdout )
		fpo = stdout;
	else
		fpo = fopen( dst, "wt" );

	if( fpo == NULL )
	{
		perror( "Cannot open maple file for writing" );
		fclose( fpi );
		ExitValue = -1;
		return;
	}

	sprintf( begincmd, "\\begin{%s}", LaTeXEnvironment );
	sprintf( endcmd, "\\end{%s}", LaTeXEnvironment );

	linenr = 0;
	writtenlines = 0;
	copying = false;

	if( Opt_MapleHeading )
	{
		if( Opt_Stdin )
		{
			time( &curtime );
			
			fprintf( fpo,
					"#\n"
					"# This file is automatically generated by MPLSTRIP.\n"
					"# Source file: (unknown)\n"
					"# Time stamp on conversion: %s"
					"#\n",
					ctime( &curtime ) );
		}
		else
		{
			fstat( fileno(fpi), &fileinfo );

			fprintf( fpo,
					"#\n"
					"# This file was automatically generated by MPLSTRIP.\n"
					"# Source file: %s\n"
					"# Time stamp on source file: %s"
					"#\n",
					src, ctime( &(fileinfo.st_mtime) ) );
		}
	}

	while( fgets( line, sizeof(line), fpi ) != NULL )
	{
		linenr ++;
		if( strstr( line, begincmd ) != NULL )
		{
			if( copying )
			{
				if( !Opt_Quiet )
					printf( "Warning (line %d): nested environments %s.\n", linenr, LaTeXEnvironment );
			}
			else
				copying = true;
			continue;
		}
		if( strstr( line, endcmd ) != NULL )
		{
			if( !copying )
			{
				if( !Opt_Quiet )
					printf( "Warning (line %d): unmatched %s environment.\n", linenr, endcmd );
			}
			else
				copying = false;
			continue;
		}
		if( copying )
		{
			fputs( line, fpo );
			writtenlines++;
		}
	}

	if( !Opt_Quiet )
		printf( "A number of %d lines have been extracted from a total of %d lines.\n", writtenlines, linenr );


	if( !Opt_Stdout )
		fclose( fpo );

	if( !Opt_Stdin )
		fclose( fpi );
}
