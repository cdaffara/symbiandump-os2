#ifndef __ESTW32_H__
#define __ESTW32_H__

// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EPOC Client interface for accessing Win32 stdin/stdout/stderr
// 
//

/**
 @file
 @internalComponent
*/

#include <e32std.h>

const TInt Kstdin=0;
const TInt Kstdout=1;
const TInt Kstderr=2;

class RWin32Stream : public RSessionBase
	{
public:
	IMPORT_C static void StartServer();
	IMPORT_C TInt Open(TInt aStream);
	IMPORT_C void Read(TRequestStatus& aStatus, TDes8& aDes);
	IMPORT_C void Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength);
	IMPORT_C void Write(TRequestStatus& aStatus, const TDesC8& aDes);
	IMPORT_C void Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength);
	IMPORT_C void Flush(TRequestStatus& aStatus);
private:
	TInt CheckEOF(TRequestStatus& aStatus);
	};

//
#endif

