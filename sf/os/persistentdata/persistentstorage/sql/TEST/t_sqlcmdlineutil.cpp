// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32test.h>
#include "t_sqlcmdlineutil.h"

static void GetCmdLine(RTest& aTest, const TDesC& aTestName, TDes& aCmdLine)
	{
	User::CommandLine(aCmdLine);
	aCmdLine.TrimAll();
	if(aCmdLine.Length() == 0)
		{
		aTest.Printf(_L("Usage: %S [ [/enc=<16/8>] /drv=<drive letter>:] [/page=<512/1024/2048/4096/8192/16384/32768>] ] [/cache=<number>] [/hlimit=<Kb>]\r\n"), &aTestName);
		return;
		}
	aCmdLine.Append(TChar('/'));
	}

static void ExtractCmdLineParams(TDes& aCmdLine,  RArray<TPtrC>& aPrmNames, RArray<TPtrC>& aPrmValues)
	{
	aPrmNames.Reset();	
	aPrmValues.Reset();	
	
	enum TState{EWaitPrmStart, EReadPrmName, EReadPrmValue};
	TState state = EWaitPrmStart;
	TInt startPos = -1;
	TPtr prmName(0, 0);
	TPtr prmValue(0, 0);
	
	aCmdLine.Append(TChar('/'));
	
	for(TInt i=0;i<aCmdLine.Length();++i)
		{
		switch(state)
			{
			case EWaitPrmStart:
				if(aCmdLine[i] == TChar('/'))
					{
					startPos = i + 1;
					prmName.Zero();
					state = EReadPrmName;
					}
				break;
			case EReadPrmName:
				if(aCmdLine[i] == TChar('='))
					{
					TPtr p = aCmdLine.MidTPtr(startPos, i - startPos);
					prmName.Set(p);
					prmName.TrimRight();
					startPos = i + 1;
					prmValue.Zero();
					state = EReadPrmValue;
					}
				break;
			case EReadPrmValue:
				if(aCmdLine[i] == TChar('/'))
					{
					TPtr p = aCmdLine.MidTPtr(startPos, i - startPos);
					prmValue.Set(p);
					prmValue.Trim();
					startPos = i + 1;
					aPrmNames.Append(prmName);
					aPrmValues.Append(prmValue);
					prmName.Zero();
					prmValue.Zero();
					state = EReadPrmName;
					}
				break;
			default:
				break;
			}
		}
	}

static void ExtractParamNamesAndValues(const RArray<TPtrC>& aPrmNames, const RArray<TPtrC>& aPrmValues, TCmdLineParams& aCmdLineParams)
	{
	__ASSERT_ALWAYS(aPrmNames.Count() == aPrmValues.Count(), User::Invariant());
	
	aCmdLineParams.SetDefaults();
	
	for(TInt i=0;i<aPrmNames.Count();++i)
		{
		if(aPrmNames[i].CompareF(_L("enc")) == 0)
			{
			TLex lex(aPrmValues[i]);
			TInt enc = 0;
			TInt err = lex.Val(enc);
			if(err == KErrNone)
				{
				if(enc == 8)
					{
					aCmdLineParams.iDbEncoding = TCmdLineParams::EDbUtf8;
					}
				else if(enc == 16)
					{
					aCmdLineParams.iDbEncoding = TCmdLineParams::EDbUtf16;
					}
				}
			}
		else if(aPrmNames[i].CompareF(_L("drv")) == 0)
			{
			if(aPrmValues[i].Length() == 2 && aPrmValues[i][1] == TChar(':'))
				{
				TChar ch(aPrmValues[i][0]);
				ch.LowerCase();
				if(ch >= TChar('a') && ch <= TChar('z'))
					aCmdLineParams.iDriveName.Copy(aPrmValues[i]);
				}
			}
		else if(aPrmNames[i].CompareF(_L("page")) == 0)
			{
			TLex lex(aPrmValues[i]);
			TInt pageSize = 0;
			TInt err = lex.Val(pageSize);
			if(err == KErrNone && (pageSize == 512 || pageSize == 1024 || pageSize == 2048 ||
			   pageSize == 4096 || pageSize == 8192 || pageSize == 16384 || pageSize == 32768))
				{
				aCmdLineParams.iPageSize = pageSize;
				}
			}
		else if(aPrmNames[i].CompareF(_L("cache")) == 0)
			{
			TLex lex(aPrmValues[i]);
			TInt cacheSize = 0;
			TInt err = lex.Val(cacheSize);
			if(err == KErrNone && (cacheSize > 0 && cacheSize < 1000000000))
				{
				aCmdLineParams.iCacheSize = cacheSize;
				}
			}
		else if(aPrmNames[i].CompareF(_L("hlimit")) == 0)
			{
			TLex lex(aPrmValues[i]);
			TInt softHeapLimit = 0;
			TInt err = lex.Val(softHeapLimit);
			if(err == KErrNone && (softHeapLimit >= 0 && softHeapLimit < 1000000000))
				{
				aCmdLineParams.iSoftHeapLimitKb = softHeapLimit;
				}
			}
		}
	}

static void PrepareSqlConfigString(RTest& aTest, const TCmdLineParams& aCmdLineParams, TDes8& aConfigStr)
	{
	aConfigStr.Zero();
	
	if(aCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf8)
		{
		aTest.Printf(_L("--PRM--Database Encoding: UTF8\r\n"));
		aConfigStr.Append(_L8("encoding=\"UTF-8\";"));
		}
	else
		{
		aTest.Printf(_L("--PRM--Database Encoding: UTF16\r\n"));
		aConfigStr.Append(_L8("encoding=\"UTF-16\";"));
		}
	
	aTest.Printf(_L("--PRM--Database page size: %d\r\n"), aCmdLineParams.iPageSize);
	TBuf8<20> pageSizeBuf;
	pageSizeBuf.Format(_L8("page_size=%d;"), aCmdLineParams.iPageSize);
	aConfigStr.Append(pageSizeBuf);

	aTest.Printf(_L("--PRM--Database cache size: %d\r\n"), aCmdLineParams.iCacheSize);
	TBuf8<20> cacheSizeBuf;
	cacheSizeBuf.Format(_L8("cache_size=%d;"), aCmdLineParams.iCacheSize);
	aConfigStr.Append(cacheSizeBuf);
	
	aTest.Printf(_L("--PRM--Database drive: %S\r\n"), &aCmdLineParams.iDriveName);

	if(aCmdLineParams.iSoftHeapLimitKb > 0)
		{
		aTest.Printf(_L("--PRM--Soft heap limit: %d Kb\r\n"), aCmdLineParams.iSoftHeapLimitKb);
		}
	else
		{
		aTest.Printf(_L("--PRM--Soft heap limit: default\r\n"));
		}
	}

#ifdef SQL_SOFT_HEAP_LIMIT_TEST	

static TInt KillProcess(const TDesC& aProcessName)
	{
	TFullName name;
	TBuf<64> pattern(aProcessName);
	TInt length = pattern.Length();
	pattern += _L("*");
	TFindProcess procFinder(pattern);

	while (procFinder.Next(name) == KErrNone)
		{
		if (name.Length() > length)
			{//If found name is a string containing aProcessName string.
			TChar c(name[length]);
			if (c.IsAlphaDigit() ||
				c == TChar('_') ||
				c == TChar('-'))
				{
				// If the found name is other valid application name
				// starting with aProcessName string.
				continue;
				}
			}
		RProcess proc;
		if (proc.Open(name) == KErrNone)
			{
			proc.Kill(0);
			}
		proc.Close();
		}
	return KErrNone;
	}

_LIT(KSqlSrvName, "sqlsrv.exe");
_LIT(KSqlSrvConfigFile, "c:\\test\\t_sqlserver.cfg");

static void ReplaceConfigFile(const TDesC16& aConfig)
	{
	RFs fs;
	TInt err = fs.Connect();
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	
	(void)KillProcess(KSqlSrvName);
	
	(void)fs.MkDirAll(KSqlSrvConfigFile);
	(void)fs.Delete(KSqlSrvConfigFile);
	
	RFile file;
	err = file.Create(fs, KSqlSrvConfigFile, EFileRead | EFileWrite);
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	
	TPtrC8 p((const TUint8*)aConfig.Ptr(), aConfig.Length() * sizeof(TUint16));
	err = file.Write(p);
	file.Close();
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	
	fs.Close();
	}

static void DeleteConfigFile()
	{
	RFs fs;
	TInt err = fs.Connect();
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	
	(void)KillProcess(KSqlSrvName);
	
	(void)fs.MkDirAll(KSqlSrvConfigFile);
	(void)fs.Delete(KSqlSrvConfigFile);
	
	fs.Close();
	}

#endif //SQL_SOFT_HEAP_LIMIT_TEST	

void GetCmdLineParamsAndSqlConfigString(RTest& aTest, const TDesC& aTestName, TCmdLineParams& aCmdLineParams, TDes8& aConfigStr)
	{
	TBuf<200> cmdLine;
	GetCmdLine(aTest, aTestName, cmdLine);
	RArray<TPtrC> prmNames;
	RArray<TPtrC> prmValues;
	ExtractCmdLineParams(cmdLine, prmNames, prmValues);
	ExtractParamNamesAndValues(prmNames, prmValues, aCmdLineParams);
	prmValues.Close();
	prmNames.Close();
	PrepareSqlConfigString(aTest, aCmdLineParams, aConfigStr);
	}

void PrepareDbName(const TDesC& aDeafultDbName, const TDriveName& aDriveName, TDes& aDbName)
	{
	TParse parse;
	parse.Set(aDriveName, &aDeafultDbName, 0);
	const TDesC& dbFilePath = parse.FullName();
	aDbName.Copy(dbFilePath);
	}

void SetSoftHeapLimit(TInt aSoftHeapLimit)
	{
	if(aSoftHeapLimit > 0)
		{
#ifdef SQL_SOFT_HEAP_LIMIT_TEST	
		TBuf<50> configBuf;
		configBuf.Format(_L("soft_heap_limit_kb=%d"), aSoftHeapLimit);
		ReplaceConfigFile(configBuf);
#else
		RDebug::Print(_L("The soft heap limit cannot be set if \"SQL_SOFT_HEAP_LIMIT_TEST\" macro is not defined!\r\n"));
#endif
		}
	else if(aSoftHeapLimit < 0)
		{
		RDebug::Print(_L("Soft heap limit of %d Kb cannot be set!\r\n"), aSoftHeapLimit);
		}
	}

void ResetSoftHeapLimit()
	{
#ifdef SQL_SOFT_HEAP_LIMIT_TEST	
	DeleteConfigFile();
#endif
	}

