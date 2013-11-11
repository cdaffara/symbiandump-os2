// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Global strings, whic are declared once in this header file and used in the SQL server source files.
// Make sure that __SQLSRV_STRINGS__ macro is defined only once in SqlSrvStrings.cpp file, before the
// include of "SqlSrvStrings.h", which ensures that only one copy of the strings is included in the
// executable server image.
// 
//

#ifndef __SQLSRVSTRINGS_H__
#define __SQLSRVSTRINGS_H__

#include <e32std.h>

#ifdef __SQLSRV_STRINGS__
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

//System table names
//If you need to change the system tables name prefix, please check SqlSrvDbSysSettings.cpp file, 
//where this prefix is used in sql statements and modify the statements.
CONST_LIT8(KSqlitePrefix8,			"sqlite_");
CONST_LIT8(KSystemPrefix8,			"symbian_");
CONST_LIT8(KMasterTable8,			"sqlite_master");
CONST_LIT8(KSecurityTable8,			"symbian_security");
CONST_LIT8(KSettingsTable8,			"symbian_settings");

CONST_LIT16(KSqlitePrefix16,		"sqlite_");
CONST_LIT16(KSystemPrefix16,		"symbian_");
CONST_LIT16(KMasterTable16,			"sqlite_master");
CONST_LIT16(KSecurityTable16,		"symbian_security");
CONST_LIT16(KSettingsTable16,		"symbian_settings");

// config consts
#if defined SYSLIBS_TEST || defined SQL_SOFT_HEAP_LIMIT_TEST
CONST_LIT16(KSqlSrvDefaultConfigFile, "c:\\test\\t_sqlserver.cfg");
CONST_LIT16(KSqlSrvDbConfigFileFormat, "c:cfg*");
#else
CONST_LIT16(KSqlSrvDefaultConfigFile, "z:SqlServer.cfg");
CONST_LIT16(KSqlSrvDbConfigFileFormat, "z:cfg*");
#endif
CONST_LIT8(KSoftHeapLimitKb,"soft_heap_limit_kb");
CONST_LIT8(KCompactionMode,"compaction");
CONST_LIT8(KFreePageThresholdKb,"free_space_threshold_kb");
CONST_LIT8(KUTF8,"UTF-8");
CONST_LIT8(KUTF8Q,"\"UTF-8\"");
CONST_LIT8(KUTF16,"UTF-16");
CONST_LIT8(KUTF16Q,"\"UTF-16\"");
CONST_LIT8(KManual,"manual");
CONST_LIT8(KBackground,"background");
CONST_LIT8(KAuto,"auto");
CONST_LIT8(KSynchronous,"synchronous");

//System database names
CONST_LIT16(KMainDb16,	"main");
CONST_LIT8(KMainDb8,	"main");
CONST_LIT8(KTempDb8,	"temp");

//Setting isolation level SQL statements (zero-terminated strings)
CONST_LIT8(KReadUncommittedPragma, "PRAGMA read_uncommitted = True;\x0");
CONST_LIT8(KSerializablePragma,	 "PRAGMA read_uncommitted = False;\x0");

//Format of the file name when sending file handles in it
CONST_LIT16(KFileHandleFmt,		"|%d%08X%S%08X%S|\x0");
const TUint KFileHandleSeparator = '|';

//SQlite Pragma Commands
CONST_LIT8(KAutoVacuum,				"auto_vacuum");			
CONST_LIT8(KCacheSize, 				"cache_size");			
CONST_LIT8(KCaseSensitiveLike, 		"case_sensitive_like");		
CONST_LIT8(KCountChanges, 			"count_changes");		
CONST_LIT8(KDefaultCacheSize, 		"default_cache_size");			
CONST_LIT8(KEmptyResultCallbacks, 	"empty_result_callbacks");			
CONST_LIT8(KEncoding, 				"encoding");				
CONST_LIT8(KFullColumnNames, 		"full_column_names");					
CONST_LIT8(KFullfsync, 				"fullfsync");				
CONST_LIT8(KIncrementalVacuum, 		"incremental_vacuum");				
CONST_LIT8(KJournalMode, 			"journal_mode");			
CONST_LIT8(KJournalSizeLimit, 		"journal_size_limit");			
CONST_LIT8(KLegacyFileFormat, 		"legacy_file_format");		
CONST_LIT8(KLockingMode, 			"locking_mode");			
CONST_LIT8(KPageSize, 				"page_size");			
CONST_LIT8(KMaxPageCount, 			"max_page_count");				
CONST_LIT8(KReadUncommitted, 		"read_uncommitted");					
CONST_LIT8(KShortColumnNames, 		"short_column_names");					
CONST_LIT8(KSynchronousFlag, 		"synchronous");					
CONST_LIT8(KTempStore, 				"temp_store");					
CONST_LIT8(KTempStoreDirectory, 	"temp_store_directory");				
CONST_LIT8(KDatabaseList, 			"database_list");					
CONST_LIT8(KForeignKeyList, 		"foreign_key_list");								
CONST_LIT8(KFreelistCount, 			"freelist_count");				
CONST_LIT8(KIndexInfo, 				"index_info");					
CONST_LIT8(KIndexIist, 				"index_list");					
CONST_LIT8(KPageCount, 				"page_count");		
CONST_LIT8(KTableInfo, 				"table_info");			
CONST_LIT8(KSchemaVersion, 			"schema_version");			
CONST_LIT8(KUserVersion, 			"user_version");
CONST_LIT8(KIntegrityCheck, 		"integrity_check");
CONST_LIT8(KParserTrace, 			"parser_trace");
CONST_LIT8(KVdbeTrace, 				"vdbe_trace");
CONST_LIT8(KdbeListing, 			"vdbe_listing");


#endif//__SQLSRVSTRINGS_H__
