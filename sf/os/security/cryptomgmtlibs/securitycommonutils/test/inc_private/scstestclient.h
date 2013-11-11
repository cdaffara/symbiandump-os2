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
* Client-side API which tests the session count server.
*
*/


/**
 @internalComponent
 @test
 @file
*/


#ifndef SSCTESTCLIENT_H
#define SSCTESTCLIENT_H

#include <scs/scsclient.h>

_LIT(KDisableScsTestServerTimeout, "c:\\scstest_notimer");

class RScsTestSession : public RScsClientBase
/**
	Client-side handle to an SCS test session.
	Provides an API from which synchronous and asynchronous
	requests can be executed.
 */
	{
public:
	IMPORT_C RScsTestSession();
	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(const TVersion& aVersion);
	
	IMPORT_C TInt SendCustomFunction(TInt aFunction);
	
	IMPORT_C TInt NukeServer();
	IMPORT_C TInt Double(TInt& aValue);
	
	IMPORT_C void Treble(TDes8& aValue, TRequestStatus& aStatus);
	IMPORT_C void CancelTreble();
	};

class RScsTestSubsession : public RScsClientSubsessionBase
/**
	Provides an API from which synchronous and asynchronous
	requests can be executed.
 */
	{
public:
	IMPORT_C RScsTestSubsession();
	IMPORT_C TInt Create(RScsTestSession& aSession, TInt aValue);
	
	IMPORT_C TInt SendFunction(TInt aFunction);
	
	IMPORT_C TInt Quadruple(TInt& aResult);
	
	IMPORT_C void Treble(TDes8& aValue, TRequestStatus& aRequestStatus);
	IMPORT_C void CancelTreble();
	};

#endif	// #ifndef SSCTESTCLIENT_H

