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
// EPOC Server providing access to Win32 stdin/stdout/stderr
// 
//

#include <e32std.h>
#include <e32base.h>
#include <estw32.h>
/**
@internalComponent
*/
enum TWin32Stream {
	EWin32Read,
	EWin32Write,
	EWin32Flush
	};

//
class CWin32Stream
/**
@internalComponent
*/
	{
public:
	static TInt StreamName(TInt aStream, TDes& aBuf);
	static void StartServer(TInt aStream);
	static TInt ThreadFunction(TAny* aPtr);
private:
	TInt Init(TInt aStream, const TDesC& aStreamName);
	void HandleMessage();
	TInt ReadStream(RMessage2& aMessage);
	TInt WriteStream(RMessage2& aMessage);
	void FlushStream();
	TInt MapWin32Error(TInt aDefault);

	RServer2 iServer;
	RMessage2 iMessage;
	TAny* iHandle;
	TAny* iLogHandle;	// used to log a copy of stream output, for debug purposes
	};

//
