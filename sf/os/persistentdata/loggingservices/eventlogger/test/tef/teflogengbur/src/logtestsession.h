// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration of test class to send client requests to log engine server
// Identical declaration exists in logeng\ongoing\test\src\test.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef LOGTESTSESSION_H
#define LOGTESTSESSION_H

#include <e32std.h>

class RLogTestSession : public RSessionBase
	{
public:
	TInt Connect();
	void Send(TInt aType, TIpcArgs& aParam, TRequestStatus& aStatus);
	TInt Send(TInt aType, TIpcArgs& aParam);
	};

#endif // #ifndef LOGTESTSESSION_H
