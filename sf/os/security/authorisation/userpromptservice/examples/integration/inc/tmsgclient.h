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
* Example Code
*
*/


/**
 @file
 @internalComponent
 @test
*/

#ifndef TMSGCLIENT_H_
#define TMSGCLIENT_H_

#include <e32std.h>
#include <e32cmn.h>
#include "tmsgcommon.h"

class RMsgSession : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C void SendMsg(const TDesC& aTo, const TDesC& aBody, TRequestStatus& aStatus) const;
	IMPORT_C void CancelSendMsg() const;
	};

#endif /* TMSGCLIENT_H_*/
