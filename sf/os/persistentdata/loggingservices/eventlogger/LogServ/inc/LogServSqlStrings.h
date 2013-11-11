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
//
// Description:
//

#ifndef __LOGSERVSQLSTRINGS_H__
#define __LOGSERVSQLSTRINGS_H__

#include <e32std.h>

#ifdef __LOGSRV_STRINGS__
	#define CONST_LIT8(var, val) extern const TLitC8<sizeof(val)> var = {sizeof(val) - 1, val}
	#define CONST_LIT16(var, val) extern const TLitC16<sizeof(L##val)/2> var = {sizeof(L##val)/2 - 1, L##val}
	#define CONST_TTEXT(var, val) extern const TText var = val
#else
	#define CONST_LIT8(var, val) extern const TLitC8<sizeof(val)> var
	#define CONST_LIT16(var, val) extern const TLitC16<sizeof(L##val)/2> var
	#define CONST_TTEXT(var, val) extern const TText var
#endif

#ifdef _UNICODE
	#define CONST_LIT(var, val) CONST_LIT16(var, val)
#else
	#define CONST_LIT(var, val) CONST_LIT8(var, val)
#endif

const  TInt KMaxSqlStringLength = 1024;
extern TBuf<KMaxSqlStringLength> TheSql;

/////////// Create tables
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
CONST_LIT(KLogTableEventString, "CREATE TABLE Event (Id INTEGER NOT NULL, EType UNSIGNED TINYINT NOT NULL, Remote CHAR(%d), Direction SMALLINT, ETime DATE NOT NULL, DType TINYINT NOT NULL, Duration UNSIGNED INTEGER, Status SMALLINT, Subject CHAR(%d), Number CHAR(%d), Contact INTEGER, Link UNSIGNED INTEGER, Data LONG VARBINARY, Recent TINYINT, Duplicate INTEGER, Flag1 BIT NOT NULL, Flag2 BIT NOT NULL, Flag3 BIT NOT NULL, Flag4 BIT NOT NULL, SimId UNSIGNED INTEGER)");
#else
CONST_LIT(KLogTableEventString, "CREATE TABLE Event (Id INTEGER NOT NULL, EType UNSIGNED TINYINT NOT NULL, Remote CHAR(%d), Direction SMALLINT, ETime DATE NOT NULL, DType TINYINT NOT NULL, Duration UNSIGNED INTEGER, Status SMALLINT, Subject CHAR(%d), Number CHAR(%d), Contact INTEGER, Link UNSIGNED INTEGER, Data LONG VARBINARY, Recent TINYINT, Duplicate INTEGER, Flag1 BIT NOT NULL, Flag2 BIT NOT NULL, Flag3 BIT NOT NULL, Flag4 BIT NOT NULL)");
#endif
CONST_LIT(KLogTableTypeString, "CREATE TABLE Type (Id SMALLINT NOT NULL, UId UNSIGNED INTEGER NOT NULL, Description CHAR(%d) NOT NULL, Enabled BIT)");
CONST_LIT(KLogTableStringString, "CREATE TABLE String (Id SMALLINT NOT NULL, TEXT CHAR(%d) NOT NULL)");
CONST_LIT(KLogTableConfigString, "CREATE TABLE Config (Size UNSIGNED SMALLINT NOT NULL, Recent UNSIGNED TINYINT NOT NULL, Age UNSIGNED INTEGER NOT NULL)");
/////////// Config table: operations 
CONST_LIT(KLogSqlInsertConfigString, "INSERT INTO Config VALUES (%d, %d, %d)") ;
/////////// Type table: operations
CONST_LIT(KLogSqlUpdateTypeString, "UPDATE Type SET Description = '%S', Enabled = %d WHERE UId = %d") ;
CONST_LIT(KLogSqlDeleteTypeString, "DELETE FROM Type WHERE UId = %d") ;
//
CONST_LIT(KLogSqlSelectOldestString, "SELECT Id FROM Event WHERE ETime < #%S#") ;
CONST_LIT(KLogSqlDeleteOldestString, "DELETE FROM Event WHERE ETime < #%S#") ;
CONST_LIT(KLogSqlSelectAllRecent,    "SELECT Id, Recent, Duplicate FROM Event") ;
CONST_LIT(KLogSqlSelectDuplicateString, "SELECT Id, Duplicate FROM EVENT WHERE Recent = %d %S AND NOT Id = %d ORDER BY Id DESC") ;
CONST_LIT(KLogSqlGetRecent, "SELECT Id FROM Event WHERE Recent = %d AND Duplicate IS NULL ORDER BY Id DESC") ;
CONST_LIT(KLogSqlRemoveDuplicateEvents, "UPDATE Event SET Recent = NULL, Duplicate = NULL WHERE ");
//The next 4 queries must have exactly the same set of columns in the list => see CLogServViewBase::InitializeColumnsL()
_LIT(KLogViewSelectColList, "SELECT Id, EType, Flag1, Flag2, Flag3, Flag4 FROM Event");
_LIT(KLogSqlEventViewString,        "%S %S ORDER BY Id DESC") ;
_LIT(KLogSqlAllRecentViewString,    "%S WHERE Recent IS NOT NULL AND Duplicate IS NULL %S ORDER BY Id DESC") ;
_LIT(KLogSqlRecentViewString,       "%S WHERE Recent=%d AND Duplicate IS NULL %S ORDER BY Id DESC") ;
_LIT(KLogSqlDuplicateViewString2,   "%S WHERE Duplicate=%d %S ORDER BY Id DESC");
// Selects all duplicate events for a particular recent list
_LIT(KLogSqlSelectAllDuplicatesString, "SELECT Id, Recent, Duplicate FROM Event WHERE Recent = %d AND Duplicate IS NOT NULL");
// Get duplicate events
CONST_LIT(KLogSqlDuplicateViewString, "SELECT Id, EType, Duplicate, Flag1, Flag2, Flag3, Flag4 FROM Event WHERE Duplicate = %d %S ORDER BY Id DESC");
// Remove duplicate event
CONST_LIT(KLogSqlRemoveDuplicateString, "UPDATE Event SET Recent = NULL, Duplicate = NULL WHERE Id = %d AND Duplicate = %d");
//Field names
CONST_LIT(KLogNameEventString, "Event") ;
CONST_LIT(KLogNameTypeString, "Type");
CONST_LIT(KLogNameStringString, "String");
CONST_LIT(KLogNameConfigString, "Config");
CONST_LIT(KLogFieldIdString, "Id");
CONST_LIT(KLogFieldEventTypeString, "EType");
CONST_LIT(KLogFieldEventRemoteString, "Remote");
CONST_LIT(KLogFieldEventDirectionString, "Direction");
CONST_LIT(KLogFieldEventTimeString, "ETime");
CONST_LIT(KLogFieldEventDTypeString, "DType");
CONST_LIT(KLogFieldEventDurationString, "Duration");
CONST_LIT(KLogFieldEventStatusString,  "Status");
CONST_LIT(KLogFieldEventSubjectString,  "Subject");
CONST_LIT(KLogFieldEventNumberString, "Number");
CONST_LIT(KLogFieldEventContactString, "Contact");
CONST_LIT(KLogFieldEventLinkString, "Link");
CONST_LIT(KLogFieldEventDataString, "Data");
CONST_LIT(KLogFieldEventRecentString, "Recent");
CONST_LIT(KLogFieldEventDuplicateString, "Duplicate");
CONST_LIT(KLogFieldEventFlagString, "Flag%d");
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
CONST_LIT(KLogFieldEventSimId, "SimId");
#endif
CONST_LIT(KLogFieldTypeIdString, "Id");
CONST_LIT(KLogFieldTypeUidString, "UId");
CONST_LIT(KLogFieldTypeEnabledString, "Enabled");
CONST_LIT(KLogFieldTypeDescriptionString, "Description");
CONST_LIT(KLogFieldStringIdString, "Id");
CONST_LIT(KLogFieldStringTextString, "Text");
CONST_LIT(KLogFieldConfigSizeString, "Size");
CONST_LIT(KLogFieldConfigRecentString, "Recent");
CONST_LIT(KLogFieldConfigAgeString, "Age");

CONST_LIT(KLogNameEventIdx1, "Index1") ;
CONST_LIT(KLogFlagString, "Flag");
CONST_LIT(KIdEqStr, "Id=");
CONST_LIT(KDuplicateEqStr, "Duplicate=");
CONST_LIT(KLogOpenBracket, "(");
CONST_LIT(KLogCloseBracket, ")");
CONST_LIT(KLogAnd, " AND ");
CONST_LIT(KLogOr, " OR ");
CONST_LIT(KLogWhere, " WHERE ");
CONST_LIT(KLogStringPredicate, "%S='%S'");
CONST_LIT(KLogNumberPredicate, "%S=%d");
CONST_LIT(KLogUNumberPredicate, "%S=%u");
CONST_LIT(KLogFlagPredicate, "%S%d=%d");
CONST_LIT(KLogNullPredicate, "%S IS NULL");
CONST_LIT(KLogLikePredicate, "%S LIKE '*%S'");
CONST_LIT(KLogDateBeforePredicate, "%S<=#%S#");
CONST_LIT(KLogDateAfterPredicate, "%S>=#%S#");

//
CONST_LIT(KLogUnknownString, "Unknown") ;

CONST_LIT(KLogServ, "LogServ");

#ifdef SYSLIBS_TEST
//Complex recent list with a lot of fields. These are the fields values.
CONST_LIT(KLogRctTListRemoteParty, "TestRemParty");
CONST_LIT(KLogRctTListDirection, "TestDir");
CONST_LIT(KLogRctTListStatus, "TestStatus");
CONST_LIT(KLogRctTListNumber, "11111999");
const TInt KLogRctTListDurationType = 123;
const TUint KLogRctTListFlags = 0x9;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
const TUint KLogRctTListSimId = 891711;
#endif
const TInt KLogRctTListId = 97;
#endif//SYSLIBS_TEST

#endif
