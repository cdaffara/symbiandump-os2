/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __T_SQLITEPERF_H__
#define __T_SQLITEPERF_H__

typedef enum 
	{
	EPerfTestSqlMode,				//Symbian SQL performance tests
	
	EPerfTestSqliteSqlMode,			//SQLite C API performance tests. The SQLite configuration matches the
									//SQLite configuration used by the server build
									
	EPerfTestSqliteDefaultMode,		//SQLite C API performance tests. Default SQLite configuration
	//
	EPerfTestModeCnt
	} TPerfTestMode;
	
typedef enum 
	{
	EPerfTestMultiInsert,
	EPerfTestMultiUpdate,
	EPerfTestMultiDelete,
	EPerfTestMultiSelect,
	EPerfTestSingleInsert,
	EPerfTestSingleUpdate,
	EPerfTestSingleDelete,
	EPerfTestSingleSelect,
	//
	EPerfTestTypeCnt
	} TPerfTestType;

#ifdef __cplusplus
extern "C" {
#endif
	void SqliteInitialize(TPerfTestMode aMode);
	void SqliteFinalize(TPerfTestMode aMode);
	void SqliteMultiInsertTest(TPerfTestMode aPerfTestMode, const char aInsertSql[], int aInsertRecCnt);
	void SqliteMultiUpdateTest(TPerfTestMode aPerfTestMode, const char aUpdateSql[], int aUpdateRecIds[], int aUpdateRecCnt);
	void SqliteMultiDeleteTest(TPerfTestMode aPerfTestMode, const char aDeleteSql[], int aDeleteRecIds[], int aDeleteRecCnt);
	void SqliteMultiSelectTest(TPerfTestMode aPerfTestMode, const char aSelectSql[], int aSelectRecIds[], int aSelectRecCnt);
	void SqliteSingleInsertTest(TPerfTestMode aPerfTestMode, const char aSingleInsertSql[], TInt aInsertRecId);
	void SqliteSingleUpdateTest(TPerfTestMode aPerfTestMode, const char aSingleUpdateSql[], TInt aUpdateRecId);
	void SqliteSingleDeleteTest(TPerfTestMode aPerfTestMode, const char aSingleUpdateSql[], TInt aUpdateRecId);
	void SqliteSingleSelectTest(TPerfTestMode aPerfTestMode, const char aSingleSelectSql[], TInt aSelectRecId);
	void TestAbort(TInt aLine);
	const char* TestDbName(void);
	unsigned int FastCounterValue(void);
	void StorePerfTestResult(TPerfTestMode aMode, TPerfTestType aType, unsigned int aResult);
	void PrintS(const char* aFmt, const char* aMsg);
	void PrintI(const char* aFmt, int a1);
	void PrintIII(const char* aFmt, int a1, int a2, int a3);
#ifdef __cplusplus
}
#endif

#endif /* __T_SQLITEPERF_H__ */
