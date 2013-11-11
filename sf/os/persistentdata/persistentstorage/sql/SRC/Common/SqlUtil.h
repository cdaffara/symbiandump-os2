// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
// NTT DOCOMO, INC - Fix for Bug 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#ifndef __SQLUTIL_H__
#define __SQLUTIL_H__

#include <e32std.h>			//TInt

/**
Defines the maximum length of text or binary data that can be transferred
from the SQL server to the SQL client in a single IPC call.

This constant affects the behaviour of a number of functions, specifically:
- RSqlStatement::ColumnBinaryL()
- RSqlStatement::ColumnTextL()
.
It also affects the behaviour of member functions of the classes:
- RSqlParamWriteStream
- RSqlColumnReadStream

@see RSqlStatement::ColumnBinaryL()
@see RSqlStatement::ColumnTextL()
@see RSqlParamWriteStream
@see RSqlColumnReadStream

@internalComponent
*/
#ifdef _DEBUG
const TInt KSqlMaxDesLen = 8;
#else
const TInt KSqlMaxDesLen = 256;
#endif

/**
Used internally for making sure config manipulation buffers
are not exceeded
@internalComponent
*/
const TInt KSqlSrvMaxConfigStrLen = 256;

/**
If some server function retuns a non-negative error, which value is bigger than KSqlClientBufOverflowCode,
then it means the server wanted to send a data buffer to the client but the client's 
buffer is not big enough. The client must increase the buffer size and try to retrieve the data only.
@internalComponent
*/
const TInt KSqlClientBufOverflowCode = 0x1000;

/**
@internalComponent
*/
enum TSqlBufFlatType 
	{
	ESqlColumnNamesBuf, 
	ESqlParamNamesBuf, 
	ESqlColumnValuesBuf,
	ESqlDeclColumnTypesBuf
	};

/**
This structure is used for IPC data transfers
@internalComponent
*/
NONSHARABLE_STRUCT(TSqlIpcData)
	{
	inline TSqlIpcData() :
		iPrm1(0),
		iPrm2(0)
		{
		}
	TUint32 iPrm1;
	TUint32 iPrm2;
	};

/**
IPC function codes used for communication between SqlDb.dll and SqlSrv.exe.

The max function code allowed is 0xFF - see KSqlSrvFunctionMask declaration in the same file.

The 32-bit RMessage2 function code format, which is used by the SQL client and server, is:

@code
Bits: 31................................0
Bytes:       0        1        2        3
      XXXXXXTT HHHHHHHH HHHHHHHH FFFFFFFF

Where:
- "X" - unused bits;
- "T" - handle type: 0 - statement handle, 1 - stream handle, 2 - no handle;
- "H" - statement or stream handle, 16 bits;
- "F" - function code, 8 bits (see KSqlSrvFunctionMask);
@endcode

@see RMessage2
@see KSqlSrvFunctionMask
@see KSqlSrvHandleMask
@see KSqlSrvHandleTypeMask
@see KSqlSrvHandleShiftBits
@see CSqlSrvSession
@see CSqlSrvSession::ServiceL()

@internalComponent
*/
enum TSqlSrvFunction
	{
	//Test functions
	ESqlSrvTestBase				= 					0x00,
	ESqlSrvResourceMark			= ESqlSrvTestBase + 0x01,
	ESqlSrvResourceCheck		= ESqlSrvTestBase + 0x02,
	ESqlSrvResourceCount		= ESqlSrvTestBase + 0x03,
	ESqlSrvSetDbHeapFailure		= ESqlSrvTestBase + 0x04,
	ESqlSrvSetHeapFailure		= ESqlSrvTestBase + 0x05,
	//Profiling functions
	ESqlSrvProfilerStart		= ESqlSrvTestBase + 0x06,
	ESqlSrvProfilerStop			= ESqlSrvTestBase + 0x07,
	ESqlSrvProfilerReset		= ESqlSrvTestBase + 0x08,
	ESqlSrvProfilerSetRange		= ESqlSrvTestBase + 0x09,//not used
	ESqlSrvProfilerQuery	    = ESqlSrvTestBase + 0x0A,
	//Database functions
	ESqlSrvDbBase				= 					0x10,
	ESqlSrvDbCreate				= ESqlSrvDbBase + 	0x01,
	ESqlSrvDbCreateSecure		= ESqlSrvDbBase + 	0x02,
	ESqlSrvDbOpen				= ESqlSrvDbBase + 	0x03,
	ESqlSrvDbOpenFromHandle		= ESqlSrvDbBase + 	0x04,
	ESqlSrvDbClose				= ESqlSrvDbBase + 	0x05,
	ESqlSrvDbCopy				= ESqlSrvDbBase + 	0x06,
	ESqlSrvDbDelete				= ESqlSrvDbBase + 	0x07,
	//All operations with opcode > ESqlSrvDbDelete require valid database object (on the server side)
	ESqlSrvLastErrorMsg			= ESqlSrvDbBase + 	0x08,
	ESqlSrvDbExec8				= ESqlSrvDbBase + 	0x09,
	ESqlSrvDbExec16				= ESqlSrvDbBase + 	0x0A,
	ESqlSrvDbSetIsolationLevel	= ESqlSrvDbBase + 	0x0C,
	ESqlSrvDbGetSecurityPolicy  = ESqlSrvDbBase + 	0x0D,
	ESqlSrvDbAttach				= ESqlSrvDbBase + 	0x0E,
	ESqlSrvDbAttachFromHandle	= ESqlSrvDbBase + 	0x0F,
	ESqlSrvDbDetach				= ESqlSrvDbBase + 	0x10,
	ESqlSrvDbScalarFullSelect8	= ESqlSrvDbBase + 	0x11,
	ESqlSrvDbScalarFullSelect16	= ESqlSrvDbBase + 	0x12,
	ESqlSrvDbInTransaction		= ESqlSrvDbBase + 	0x13,
	ESqlSrvDbSize				= ESqlSrvDbBase + 	0x14,
	ESqlSrvDbSize2				= ESqlSrvDbBase + 	0x15,
	ESqlSrvDbBlobSource			= ESqlSrvDbBase + 	0x16,
	ESqlSrvDbLastInsertedRowId	= ESqlSrvDbBase + 	0x17,
	ESqlSrvDbCompact			= ESqlSrvDbBase + 	0x18,
	//Database - reserved drive space management
	ESqlSrvDbReserveDriveSpace	= ESqlSrvDbBase + 	0x19,
	ESqlSrvDbFreeReservedSpace	= ESqlSrvDbBase + 	0x1A,
	ESqlSrvDbGetReserveAccess	= ESqlSrvDbBase + 	0x1B,
	ESqlSrvDbReleaseReserveAccess=ESqlSrvDbBase + 	0x1C,
	//Statement functions
	ESqlSrvStmtBase				= 					0x50,
	ESqlSrvStmtPrepare8			= ESqlSrvStmtBase + 0x01,
	ESqlSrvStmtPrepare16		= ESqlSrvStmtBase + 0x02,
	ESqlSrvStmtClose			= ESqlSrvStmtBase + 0x03,
	ESqlSrvStmtReset			= ESqlSrvStmtBase + 0x04,
	ESqlSrvStmtExec				= ESqlSrvStmtBase + 0x05,
	ESqlSrvStmtAsyncExec		= ESqlSrvStmtBase + 0x06,
	ESqlSrvStmtBindExec			= ESqlSrvStmtBase + 0x07,
	ESqlSrvStmtAsyncBindExec	= ESqlSrvStmtBase + 0x09,
	ESqlSrvStmtNext				= ESqlSrvStmtBase + 0x0A,
	ESqlSrvStmtBindNext			= ESqlSrvStmtBase + 0x0B,
	ESqlSrvStmtColumnNames		= ESqlSrvStmtBase + 0x0C,
	ESqlSrvStmtParamNames		= ESqlSrvStmtBase + 0x0D,
	ESqlSrvStmtColumnSource		= ESqlSrvStmtBase + 0x0E,
	ESqlSrvStmtBinParamSink		= ESqlSrvStmtBase + 0x0F,
	ESqlSrvStmtTxtParamSink16	= ESqlSrvStmtBase + 0x11,
	ESqlSrvStmtBufFlat			= ESqlSrvStmtBase + 0x12,
	ESqlSrvStmtColumnValue		= ESqlSrvStmtBase + 0x13,
	ESqlSrvStmtDeclColumnTypes	= ESqlSrvStmtBase + 0x14,
	//Stream functions
	ESqlSrvStreamBase			= 					  0x70,
	ESqlSrvStreamRead			= ESqlSrvStreamBase + 0x01,
	ESqlSrvStreamWrite			= ESqlSrvStreamBase + 0x02,
	ESqlSrvStreamSize			= ESqlSrvStreamBase + 0x03,
	ESqlSrvStreamSynch			= ESqlSrvStreamBase + 0x04,
	ESqlSrvStreamClose			= ESqlSrvStreamBase + 0x05
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                  RMessage2, function code related macros                                 ////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQL server function mask.
The SQL server function code must be less than 0x100.
(it occupies the lower 8 bits of the 32-bit message code)

@see TSqlSrvFunction

@internalComponent
*/
const TUint KSqlSrvFunctionMask		= 0x000000FF;

/**
SQL server (statement or stream) handle mask.
The handle occupies 16 bits, right after the function code.

@see KSqlSrvHandleTypeMask
@see KSqlSrvHandleShiftBits

@internalComponent
*/
const TUint KSqlSrvHandleMask 		= 0x00FFFF00;

/**
SQL server (stream or statement) handle - shift bits.

@see KSqlSrvFunctionMask
@see KSqlSrvHandleMask

@internalComponent
*/
const TInt  KSqlSrvHandleShiftBits 	= 8;

/**
SQL server handle type bit.

@see KSqlSrvHandleMask
@see KSqlSrvHandleShiftBits

@internalComponent
*/
const TInt  KSqlSrvHandleTypeMask 	= 0x03000000;

/**
SQL server - handle type - statement or stream.

@internalComponent
*/
enum TSqlSrvHandleType
	{
	ESqlSrvNoHandle 		= 0x00000000,
	ESqlSrvStreamHandle 	= 0x01000000,
	ESqlSrvStatementHandle	= 0x02000000
	};

inline TInt MakeMsgCode(TSqlSrvFunction aFunction, TSqlSrvHandleType aHandleType, TInt aHandle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

TInt Sql2OsErrCode(TInt aSqlError, TInt aOsError);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                  Case insensitive string comparisons                                     ////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline TInt CompareNoCase8(const TDesC8& aLeft, const TDesC8& aRight);
inline TInt CompareNoCase16(const TDesC16& aLeft, const TDesC16& aRight);
#ifdef _UNICODE
#define CompareNoCase CompareNoCase16
#else
#define CompareNoCase CompareNoCase8
#endif//_UNICODE

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                      Buffer alignment functions                                     /////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline TInt AlignedLen8(TInt aLen);
#ifdef _DEBUG
inline TBool IsAligned8(TInt aLen);
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                      Compaction modes                                     ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Compaction modes.
@internalComponent
*/
enum TSqlCompactionMode
	{
	ESqlCompactionNotSet, 
	ESqlCompactionManual, 
	ESqlCompactionBackground, 
	ESqlCompactionAuto
	};

/**
SQLite vacuum modes.
@internalComponent
*/
enum TSqliteVacuumMode
	{
	ESqliteVacuumOff			= 0,
	ESqliteVacuumAuto			= 1, 
	ESqliteVacuumIncremental	= 2
	};


/**
Default compaction mode.
@internalComponent
*/
const TSqlCompactionMode KSqlDefaultCompactionMode = ESqlCompactionBackground;

/**
Exec free page threshold in Kb, after which the background compaction will be activated.
@internalComponent
*/
const TInt KSqlCompactFreePageThresholdKb = 50;

/**
One compaction step length in milliseconds.
@internalComponent
*/
const TInt KSqlCompactStepLengthMs = 50;

/**
Interval in milliseconds between the compaction steps.
@internalComponent
*/
const TInt KSqlCompactStepIntervalMs = 50;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The db config file version that is initially stored in the system settings table.
@internalComponent
*/
const TInt KSqlNullDbConfigFileVersion = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SqlUtil.inl"
	
#endif //__SQLUTIL_H__
