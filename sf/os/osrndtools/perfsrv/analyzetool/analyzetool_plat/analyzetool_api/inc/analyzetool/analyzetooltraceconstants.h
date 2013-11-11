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
* Description:  Common declarations/definitions for Analyze Tool.
*
*/

#ifndef __ANALYZETOOLTRACECONSTANTS_H__
#define __ANALYZETOOLTRACECONSTANTS_H__

#include <analyzetool/atcommon.h>

// Trace version information.
const TUint KATTraceVersion = 0x3;

// The default logging mode 
const TATLogOption KDefaultLoggingMode = EATLogToTraceFast;

// When needed, update the data file's version number directly inside
// the _LIT8 macro. Note, if you change this string, also remember to update
// the constant "KVersionStringLength" below.

_LIT8( KDataFileVersion, "ATOOL_BINARY_FILE_VERSION 1\n" );
_LIT( KOpenSquareBracket, "[" );
_LIT( KCloseSquareBracket, "]" );
_LIT( KUnderLine, "_" );
_LIT( KLeftBracket, "(" );
_LIT( KRightBracket, ")" );

_LIT8( KSpace, " " );
_LIT8( KUdeb, "UDEB" );
_LIT8( KUrel, "UREL" );
    
// Constants for logging through debug channel

// remember to change length of KTagLength when length of constants changes

_LIT8( KATIdentifier, "<AT> " );
_LIT8( KProcessStart, "PCS " );
_LIT8( KProcessEnd, "PCE " );
_LIT8( KVersionsInfo, "DEVINFO ");
_LIT8( KThreadStart, "TDS " );
_LIT8( KThreadEnd, "TDE " );
_LIT8( KDllLoad, "DLL " );
_LIT8( KDllUnload, "DLU " );
_LIT8( KTestStart, "TSS " );
_LIT8( KTestEnd, "TSE " );
_LIT8( KHandleLeak, "HDL " );
_LIT8( KLoggingCancelled, "LGC " );
_LIT8( KErrorOccured, "ERR " );
_LIT8( KMemoryAllocHeader, "ALH " );
_LIT8( KMemoryAllocFragment, "ALF " );
_LIT8( KMemoryFreedHeader, "FRH " );
_LIT8( KMemoryFreedFragment, "FRF " );
_LIT8( KMemoryReallocHeader, "RAH " );
_LIT8( KMemoryReallocFragment, "RAF " );

//to be implemented on carbide side (was logged from storage server/ carbide extension)
//_LIT( KSubtestStart, "TEST_START " );
//_LIT( KSubtestEnd, "TEST_END " );

// File name format
_LIT( KFormat, "%S%S%02d%S%S");// pad char="0", field width=2

// A string for setting time January 1st, 1970 AD nominal Gregorian
_LIT( KJanuaryFirst1970, "19700000:000000.000000" );


// todo
// Module name when it cannot be defined
// was used in "handle leak" message - always unknown
//_LIT8( KUnknownModule, "Unknown" );

// Constant time variable used to calculate timestamps for pc side.
const TInt64  KMicroSecondsAt1970 = 62168256000000000;

// The length of the string KDataFileVersion
const TInt KVersionStringLength = 27;

// The maximum length of one word (32 bits) represented in the hexadecimal text format
// without "0x" prefix
const TInt KHexa32Length = 8;

// The maximum length of one word (32 bits) represented in the decimal text format
const TInt KDec32Length = 10;

// The maximum length of a TInt64 represented in the hexadecimal text format without
// "0x" prefix
const TInt KHexa64Length = 16;

// The length of one space character in text
const TInt KSpaceLength = 1;

// The length of the AT message identifier with one space character ("<AT> ")
const TInt KATIdentifierLength = 5;

// The length of the tag with one space character (e.g. "FRH ")
const TInt KTagLength = 4;

// The length of atool version string (x.x.x)
const TInt KAtoolVersionLength = 6;

// The length of api version string (x.x.x)
const TInt KApiVersionLength = 5;

// The length of sw version version string
const TInt KSwVersionLength = 64;

// The length of the combination atId+processId+msgTag (<AT> NNN PCS )
const TInt KTraceHeaderLength = KATIdentifierLength + KHexa32Length + KSpaceLength + KTagLength;

// Max length of alloc, free, realloc last item (callstack adress)
const TInt KLastItemLength = KHexa32Length + KSpaceLength;

// The maximum length of the process start (PCS) buffer
// <<AT>> <Process ID> PCS <Process name> <Time stamp> <Udeb> <Version>
const TInt KProcessStartBufLength = KTraceHeaderLength + 
									KMaxProcessName + KSpaceLength + 
									KHexa32Length + KSpaceLength + 
									KHexa64Length + KSpaceLength +
									KHexa32Length;

// The maximum length of the versions info (VER) buffer
// <<AT>> <Process ID> VER <AT version> <API version> <S60 version> <ROM ID>
const TInt KVersionsInfoBufLength = KTraceHeaderLength + 
		                            KAtoolVersionLength + KSpaceLength +
		                            KApiVersionLength + KSpaceLength +
									KSwVersionLength + KSpaceLength + 
									KHexa32Length;

// The maximum length of the process end PCE buffer
// <<AT>> <Process ID> PCE
const TInt KProcessEndBufLength = KTraceHeaderLength;

// The maximum length of the load dll (DLL) buffer
// <<AT>> <Process ID> DLL <DLL name> <Memory start address> <Memory end address>
const TInt KDllLoadBufLength = KTraceHeaderLength +
		                       KMaxLibraryName + KSpaceLength +
		                       KHexa32Length + KSpaceLength + 
		                       KHexa32Length;

// The maximum length of the unload dll (DLU) buffer
// <<AT>> <Process ID> DLU <DLL name> <Memory start address> <Memory end address>
const TInt KDllUnloadBufLength = KTraceHeaderLength + 
		                         KMaxLibraryName + KSpaceLength +
		                         KHexa32Length + KSpaceLength + 
		                         KHexa32Length;
        
// The maximum length of the error (ERR) buffer
// <<AT>> <Process ID> ERR <Thread ID> <Error code> <Error message> 
const TInt KErrOccuredBufLength = KTraceHeaderLength +
		                          KHexa64Length + KSpaceLength +
		                          KHexa32Length + KSpaceLength +
		                          200; //err message length
                                        
// The maximum length of the alloc "ALH /ALF" buffer. 
const TInt KMemAllocBufLength = 255;

// The maximum length of the free "FRH/FRF" buffer.
const TInt KMemFreedBufLength = 255;

// The maximum length of the realloc "REH/REF" buffer.
const TInt KMemReallocBufLength = 255;

// The maximum length of the handle leak "HDL" buffer
// <<AT>> <Process ID> HDL <Handle count>
const TInt KHandleLeakBufLength = KTraceHeaderLength +
                                  KHexa32Length;
                                        
// The maximum length of the test satart "TSS" buffer
// <<AT>> <Process ID> TSS <Sub test name> <Handle count>
const TInt KTestStartBufLength = KTraceHeaderLength +
                                 KATMaxSubtestIdLength + KSpaceLength +
                                 KHexa64Length;

// The maximum length of the test end (TSE) buffer
// <<AT>> <Process ID> TSE <Sub test name> <Handle count>
const TInt KTestEndBufLength = KTraceHeaderLength + 
		                       KATMaxSubtestIdLength + KSpaceLength + 
		                       KHexa64Length;

// The maximum length of the thread start (TDS) buffer.
// <<AT>> <Process ID> TDS <Thread ID>
const TInt KThreadStartBufLength = KTraceHeaderLength +
                                   KHexa64Length; 


// The maximum length of the thread end (TDE) buffer.
// <<AT>> <Process ID> TDE <Thread ID>
const TInt KThreadEndBufLength = KTraceHeaderLength +
                                 KHexa64Length; 

// The maximun length of the file name extension buffer.
const TInt KExtensionLength = 50;

// The maximun length of the process UID3 buffer.
const TInt KProcessUidLength = 20;

#endif // __ANALYZETOOLTRACECONSTANTS_H__
