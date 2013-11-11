// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef t_logservsession_h
#define t_logservsession_h

#include <e32std.h>

/**
Test LogServ session calls. Used in the test code to connect to the server, for example to simulate
heap failure.
@internalComponent
*/
class RLogTestSession : public RSessionBase
	{
public:
	TInt Connect();
	void Send(TInt aType, TIpcArgs& aParam, TRequestStatus& aStatus);
	TInt Send(TInt aType, TIpcArgs& aParam);
	};

#endif//t_logservsession_h
