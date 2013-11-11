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
// EPOC Server for redirecting stdin/stdout/stderr to a stream-like object 
// (console, file or serial port)
// 
//

#ifndef _REDIRCLIINTERNAL_H_
#define _REDIRCLIINTERNAL_H_

#include <e32std.h>
#include <e32base.h>
#include <redirstr.h>


/**
Client side session for Redirection server

@internalAll
*/
class RRedirSession2 : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C void Read(TRequestStatus& aStatus, TDes8& aDes);
	IMPORT_C void Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength);
	IMPORT_C void Write(TRequestStatus& aStatus, const TDesC8& aDes);
	IMPORT_C void Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength);
	IMPORT_C void Flush(TRequestStatus& aStatus);
private:
//TO DO: do we really need this function??
	TInt CheckEOF(TRequestStatus& aStatus);
	};


#endif
