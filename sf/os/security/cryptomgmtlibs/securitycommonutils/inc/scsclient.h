/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Defines the client-side classes which other client-side handles
* derive from to use the SCS functionality.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SCSCLIENT_H
#define SCSCLIENT_H

#include <e32std.h>


class RScsClientBase : public RSessionBase
/**
	Provides functionality to connect to the server, starting
	the server process if necessary.  This class cannot be used
	directly - the SCS implementor must define a subclass.
 */
	{
public:
	IMPORT_C void Close();
	IMPORT_C TInt SetServerHeapFail(TInt aRate);
	IMPORT_C TInt ResetServerHeapFail();

	IMPORT_C TInt ShutdownServer();
	
protected:
	IMPORT_C RScsClientBase();
	IMPORT_C TInt Connect(
		const TDesC& aSvrName, const TVersion& aReqVer, const TDesC& aExeName, const TUidType& aFullExeUid);

	IMPORT_C TInt CallSessionFunction(TInt aFunction) const;
	IMPORT_C TInt CallSessionFunction(TInt aFunction, const TIpcArgs& aArgs) const;
	IMPORT_C void CallSessionFunction(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const;
	IMPORT_C void CancelSessionFunction(TInt aFunction) const;
	
private:
	TInt StartServerProcess(const TDesC& aExeName, const TUidType& aFullExeUid);	
	};

class RScsClientSubsessionBase : public RSubSessionBase
/**
	This class is used to send messages to server-side SCS subsessions.
	This class cannot be used directly - the implementor must define a
	subclass.
 */
	{
public:
	IMPORT_C void Close();

protected:
	IMPORT_C RScsClientSubsessionBase();
	IMPORT_C TInt CreateSubsession(const RScsClientBase& aSession, TInt aFunction, const TIpcArgs& aArgs);

	IMPORT_C TInt CallSubsessionFunction(TInt aFunction) const;
	IMPORT_C TInt CallSubsessionFunction(TInt aFunction, const TIpcArgs& aArgs) const;
	IMPORT_C void CallSubsessionFunction(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const;
	IMPORT_C void CancelSubsessionFunction(TInt aFunction) const;
	};

#endif	// #ifndef SCSCLIENT_H

