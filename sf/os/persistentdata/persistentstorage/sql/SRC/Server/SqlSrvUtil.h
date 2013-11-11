// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVUTIL_H__
#define __SQLSRVUTIL_H__

#include <f32file.h>		//RFs - used in SqlSrvUtil.inl
#include "SqlSrvStrings.h"	//KSystemPrefix16, KSystemPrefix8, KSqlitePrefix16, KSqlitePrefix8 - used in SqlSrvUtil.inl
#include "SqlUtil.h"		//CompareNoCase() - used in SqlSrvUtil.inl

inline TBool IsSystemTableName(const TDesC16& aTableName);
inline TBool IsSystemTableName(const TDesC8& aTableName);
inline TBool IsSqliteTableName(const TDesC8& aTableName);

#ifdef _SQL_AUTHORIZER_TRACE_ENABLED
void PrintAuthorizerArguments(TInt aDbOpType, 
                              const char* aDbObjName1, 
                              const char* aDbObjName2, 
							  const char* aDbName, 
							  const char* aTrgOrViewName);
#endif

TBool UTF16ToUTF8(const TDesC& aIn, TDes8& aOut);
TBool UTF16ZToUTF8Z(const TDesC& aFileName, TDes8& aFileNameDestBuf);
TBool UTF16ToUTF8Z(const TDesC& aFileName, TDes8& aFileNameDestBuf);
inline TBool FileExists(RFs& aFs, const TDesC& aFileName);
TBool IsReadOnlyFileL(RFs& aFs, const TDesC& aDbFileName);

#include "SqlSrvUtil.inl"

#endif//__SQLSRVUTIL_H__
