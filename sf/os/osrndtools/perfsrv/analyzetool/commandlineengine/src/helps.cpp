/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains help texts for CLE.
*
*/
#include "../inc/ATCommonDefines.h"

//Function declarations
void print_help( void );
void print_syntax_examples( void );

/**
* Print help info.
*/
void print_help( void )
{
	cout<< "AnalyzeTool v";
	cout<< ATOOL_VERSION;
	cout<< " - ";
	cout<< ATOOL_DATE;
	cout<< "\n";
	cout<< "Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).\nAll rights reserved.\n\n";
	cout<< "Usage:\n";
	cout<< "atool -tr [optional parameters] abld [abld parameters]\n";
	cout<< "      -lf [optional parameters] abld [abld parameters]\n";
	cout<< "      -a data-file [output-file] [-s symbol-file] [-l#]\n";
	cout<< "      -p data-file output-file\n";
	cout<< "      -c\n";
	cout<< "      -v\n";
	cout<< "      -help\n";
	cout<< "\n";
	cout<< "Mandatory parameter to choose:\n";
	cout<< " -tr             Build project for analysis using output to trace.\n";
	cout<< " -lf             Build project for analysis using logging to file.\n";
	cout<< " abld...         Use SBS v.1 build system with defined build command,\n";
	cout<< "                 which includes supported platform and build type.\n";
	cout<< " sbs...          Use SBS v.2 (Raptor) build system with defined sbs build\n";
	cout<< "                 command, which includes supported platform and build type.\n";
	cout<< " -a              Analyze report file.\n";
	cout<< " -c              Clear AnalyzeTool changes, temporary files and releasable(s).\n";
	cout<< " -p              Parse trace file to data file.\n";
	cout<< " -help           Show full help including syntax examples.\n";
	cout<< " -v              Print version info.\n";
	cout<< "\n";

	cout<< "Optional parameters:\n";
	cout<< " -acs size       Specifies the call stack size gathered when memory\n";
	cout<< "                 is allocated. Size can be between 0-256.\n";
	cout<< " -fcs size       Specifies the call stack size gathered when memory is freed.\n";
	cout<< "                 Size can be between 0-256.\n";
	cout<< " -f data file    Specifies the filename for logging to file.\n";
	cout<< "                 (50 chars max). Cannot contain path.\n";
	cout<< " -fp file path   Specifies the path of log file.\n";
	cout<< "                 (in format <driveLetter>:\\<dir1>\\<dir2>\\...\\<dirN>\\)\n";
	cout<< " output-file     Specifies the results data file name.\n";
	cout<< " -l              Logging level of analyze report(0,1,2,3), default 3.\n";
	cout<< " -s symbol-file  Specifies rom/rofs symbol file(s) to be used in analyze.\n";
	cout<< "                 Multiple -s options can be given.\n";
	cout<< " -nobuild        Instruments the project.\n";
	//cout<< "  -show_debug       Print debug messages to console.\n";
	//cout<< "  -show_debug_all	  Print debug messages to console (all).\n";
	//cout<< "  -show_dbgview     Print debug messages to windows debug api.\n";
	//cout<< "  -show_dbgview_all Print debug messages to windows debug api (all).\n";

	/*
	* Old style parameters, just for notes.
	printf("Usage:\n");
	printf("atool -m [mmp-file] [mode] [data-file] [-variant variant-name] [build] [-sbs2] [-e]\n");
	printf("      -i [mmp-file] [mode] [data-file] [-variant variant-name] [build] [-sbs2] [-e]\n");
	printf("      -a [mmp-file | target-name] data-file [-s symbol-file]\n");
	printf("         [output-file] [-l#]\n");
	printf("      -p data-file [output-file]\n");
	printf("      -c\n");
	//printf("      -u\n");
	printf("      -v\n");
	printf("      -help\n");
	printf("\n");
	printf("Mandatory option to choose:\n");
	printf("  -m           Build project for analysis.\n");
	printf("  -i           Instrument project.\n");
	printf("  -a           Analyze report file.\n");
	//printf("  -u           Create listing & map files.\n");
	printf("  -c           Clear AnalyzeTool changes and temporary files.\n");
	printf("  -p           Parse trace file.\n");
	printf("  -help        Show full help including syntax examples.\n");
	printf("\n");

	printf("Optional options:\n");
	printf("  mmp-file     Specifies one of the component from bld.inf which is used.\n");
	printf("  target-name  Specifies the target name of binary to which create\n");
	printf("               analyze report.\n");
	printf("  mode         Logging mode (trace or S60), default trace.\n");
	printf("  data-file    Specifies the user defined data file name (50 chars max).\n");
	printf("  build        Build target (udeb or urel), default udeb.\n");
	printf("  output-file  Specifies the results data file name.\n");
	printf("  -l           Logging level of analyze report(0,1,2,3), default 3.\n");
	printf("  -variant     Specifies Symbian binary variant to be used.\n");
	printf("  variant-name Specifies the variant name to be used.\n");
	printf("  -s           Specifies rom symbol file be used in analyze.\n");
	printf("  symbol-file  Full path to rom symbol file used in analyze.\n");
	printf("  -sbs2        Use Symbian Build System v.2.\n");
	printf("  -winscw      Build project for emulator (winscw platform).\n");
	printf("  -v           Print version info.\n");
	*/
}

void print_syntax_examples( void )
{
//Helper line showing width of 80 chars.
//80cout<< "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
	cout<< endl;
	cout<< "Building syntax examples:" << endl;
	cout<< "Cmd   Description" << endl;
	
	cout<< "atool -tr abld build armv5 udeb" << endl;
	cout<< "      Hooks every component from bld.inf. Using output to trace," << endl;
	cout<< "      udeb build type and armv5 platform." << endl;

	cout<< "atool -lf -acs 5 sbs -c winscw_udeb" << endl;
	cout<< "      Hooks every component from bld.inf. Using Symbian Build System v.2" << endl;
	cout<< "      Using loging to data file, call stack size of 5" << endl;
	cout<< "      in allocations, udeb build type and winscw platform." << endl;
	
	cout<< "atool -tr abld build armv5 udeb foobar" << endl;
	cout<< "      Hooks only foobar.mmp component from bld.inf. Using " << endl;
	cout<< "      output to trace, udeb build type and armv5 platform." << endl;
	
	cout<< "atool -lf -acs 0 -fcs 0 abld build armv5.default udeb" << endl;
	cout<< "      Hooks every component from bld.inf. Using default binary variant," << endl;
	cout<< "      output to trace, 0 sized callstacks in allocation(s)/free(s)," << endl;
	cout<< "      udeb build type and armv5 platform." << endl;
	
	cout<< "atool -tr sbs -b bld.inf -c armv5_udeb" << endl;
	cout<< "      Hooks every component from bld.inf. Using Symbian Build System v.2" << endl;
	cout<< "      , output to trace, udeb build type and armv5 platform." << endl;
	
	cout<< "atool -tr sbs -s system.xml --config=winscw_udeb" << endl;
	cout<< "      Hooks every layer defined in system.xml system model. Using Symbian Build" << endl;
	cout<< "      System v.2, output to trace, udeb build type and winscw platform." << endl;

	cout<< endl;
	cout<< "Instrumenting syntax examples:" << endl;
	cout<< "Cmd   Description" << endl;
	cout<< "atool -tr -nobuild abld build winscw udeb" << endl;
	cout<< "      Instruments every component from bld.inf. Using output to trace,"<<endl;
	cout<< "      udeb build target and winscw platform." << endl;
    cout<< "   After project is instrumented, it needs to compiled using same platform and" << endl;
	cout<< "   build type as given to instrumenting. Use -c parameter to remove changes." << endl;

	cout<< endl;
	cout<< "Analyzing syntax examples:" << endl;
	cout<< "Cmd   Description" << endl;
	cout<< "atool -a foo.dat" << endl;
	cout<< "      Create analyze report from foo.dat device data file." << endl;
	cout<< "atool -a foo.trace" << endl;
	cout<< "      Create analyze report from foo.trace raw data file." << endl;
	cout<< "atool -a foo.dat -s my_rom.symbol -s my_rofs1.symbol" << endl;
	cout<< "      Create analyze report from foo.dat using also rom and rofs symbol files." << endl;
	
	cout<< endl;
	cout<< "Parsing syntax examples:" << endl;
	cout<< "Cmd   Description" << endl;
	cout<< "atool -p foo.trace foo.dat" << endl;
	cout<< "      Parse foo.trace raw data file to foo.dat device data file." << endl;
	}

//EOF
