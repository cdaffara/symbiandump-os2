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

#ifndef T_SQLCMDLINEUTIL_H
#define T_SQLCMDLINEUTIL_H

#include <f32file.h>

class RTest;

struct TCmdLineParams
	{
	enum TDbEncoding
		{
		EDbUtf8,
		EDbUtf16
		};
	
	inline TCmdLineParams(TDbEncoding aDefaultEncoding = EDbUtf16,
						  TInt aDefaultPageSize = 1024,
						  TInt aDefaultCacheSize = 1000) :	
		iDefaultEncoding(aDefaultEncoding),
	    iDefaultPageSize(aDefaultPageSize),
	    iDefaultCacheSize(aDefaultCacheSize)
		{
		SetDefaults();
		}
	
	inline void SetDefaults()
		{
		iDbEncoding = iDefaultEncoding;
		iPageSize = iDefaultPageSize;
		iCacheSize = iDefaultCacheSize;
		iDriveName.Copy(_L("c:"));
		iSoftHeapLimitKb = 0;
		}
	
	const TDbEncoding	iDefaultEncoding;
	const TInt			iDefaultPageSize;
	const TInt			iDefaultCacheSize;
	
	TDbEncoding	iDbEncoding;
	TInt		iPageSize;
	TInt 		iCacheSize;
	TDriveName	iDriveName;
	TInt		iSoftHeapLimitKb;
	};

void GetCmdLineParamsAndSqlConfigString(RTest& aTest, const TDesC& aTestName, TCmdLineParams& aCmdLineParams, TDes8& aConfigStr);
void PrepareDbName(const TDesC& aDeafultDbName, const TDriveName& aDriveName, TDes& aDbName);
void SetSoftHeapLimit(TInt aSoftHeapLimit);
void ResetSoftHeapLimit();

#endif//T_SQLCMDLINEUTIL_H
