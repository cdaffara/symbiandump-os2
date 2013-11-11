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
* Description:
*
*/

#ifndef MEMSPYCOMMANDS_H
#define MEMSPYCOMMANDS_H

// Literal constants
_LIT( KMemSpyCLINewLine, "\r\n" );
_LIT( KMemSpyCLIName, "MemSpy CommandLineInterpreter" );
_LIT( KMemSpyCLIInputPrompt, " > %S" );
_LIT( KMemSpyCLIWildcardCharacter, "*" );

// Numerical constants
const TInt KMemSpyMaxDisplayLengthThreadName = 48;
const TInt KMemSpyMaxDisplayLengthSizeText = 14;
const TInt KMemSpyMaxInputBufferLength = 200;

// Key constants (S60)
const TInt KMemSpyUiS60KeyCodeButtonOk = 2000;
const TInt KMemSpyUiS60KeyCodeButtonCancel = 2001;
const TInt KMemSpyUiS60KeyCodeRockerEnter = 63557;

// Status messages
_LIT( KOutputChangeMessage, "Change output mode opetarion in progress" );
_LIT( KHeapDumpMessage, "Heap dump opetarion in progress" );
_LIT( KSWMTMessage, "System Wide Memory Tracking opetarion in progress" );

// Help text
_LIT( KHelpMessage, "=== MemSpy CommandLineInterpreter Help ===\r\n" );
_LIT( KHelpOutputCommand, "Change output mode to trace: 'memspy output trace'\r\n" );
_LIT( KHelpOutputToFileCommand, "Change output mode to trace: 'memspy output file'\r\n" );
_LIT( KHelpHeapDumpCommand, "Heap dump: 'memspy heapdup <all | kernel | user heap filter >'\r\n" );
_LIT( KHelpSwmtCommand, "SWMT: 'memspy swmt <starttimer <value in seconds> | stop timer | dumpnow > <categories>'\r\n" );
_LIT( KHelpKillServerCommand, "Kill server: 'memspy killserver'\r\n" );
_LIT( KHelpCommand, "Press 'c' to continue" );

//new commands

//HELP
_LIT( KMemSpyCmdHelp1, "-?" );
_LIT( KMemSpyCmdHelp2, "-h" );
_LIT( KMemSpyCmdHelp3, "-help" );
_LIT( KMemSpyCmdHelp4, "--help" );

//OUTPUT
_LIT( KMemSpyCmdOutput, "output" );
_LIT( KMemSpyCmdOutputParameterTrace, "trace" );
_LIT( KMemSpyCmdOutputParameterFile, "file" );
//<directory> //parameter to be parsed

//HEAP DUMP
_LIT( KMemSpyCmdHeapDump, "heapdump" );
_LIT( KMemSpyCmdHeapDumpParameterAll, "all" ); //default
_LIT( KMemSpyCmdHeapDumpParameterKernel, "kernel" ); //kernel heap dump

//SWMT
_LIT( KMemSpyCmdSwmt, "swmt" );
_LIT( KMemSpyCmdSwmtParameterStarttimer, "starttimer" ); //<value in seconds> optionaly
_LIT( KMemSpyCmdSwmtParameterStoptimer, "stoptimer" );
_LIT( KMemSpyCmdSwmtParameterDumpnow, "dumpnow" );

//KILL SERVER
_LIT( KMemSpyCmdKillServer, "killserver"); //kills the server in case of it is running

//SWMT CATEGORIES (TYPES)
_LIT( KMemSpyCmdSWMTTypeHeap,                    "heap" );
_LIT( KMemSpyCmdSWMTTypeChunk,                   "chnk" );
_LIT( KMemSpyCmdSWMTTypeCode,                    "code" );
_LIT( KMemSpyCmdSWMTTypeStack,                   "stak" );
_LIT( KMemSpyCmdSWMTTypeGlobalData,              "glob" );
_LIT( KMemSpyCmdSWMTTypeRamDrive,                "ramd" );
_LIT( KMemSpyCmdSWMTTypeOpenFile,                "file" );
_LIT( KMemSpyCmdSWMTTypeDiskSpace,               "disk" );
_LIT( KMemSpyCmdSWMTTypeHandleGeneric,           "hgen" );
_LIT( KMemSpyCmdSWMTTypeFbserv,                  "fabs" );
_LIT( KMemSpyCmdSWMTTypeFileServerCache,         "f32c" );
_LIT( KMemSpyCmdSWMTTypeSystemMemory,            "sysm" );
_LIT( KMemSpyCmdSWMTTypeWindowGroup,             "wndg" );
_LIT( KMemSpyCmdSWMTTypeAll,					 "all"	); //default value, dumps all categories expect heap dumps	
#endif
