/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Makes SendReceive requests on the filetokens server
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TOKENTYPECLIENTSESSION_H__
#define __TOKENTYPECLIENTSESSION_H__

#include "fstokencliserv.h"
#include <e32base.h>

//	THIS FILE IS NOT TO BE EDITED FOR TEST CODE PURPOSES!!!

NONSHARABLE_CLASS(RFileStoreClientSession) : public RSessionBase
	{
public:
	RFileStoreClientSession();
public:	
	TInt Connect(ETokenEnum aTokenEnum);
public:	
	TInt SendRequest(TFSTokenMessages aRequest, const TIpcArgs& aArgs) const;
	void SendAsyncRequest(TFSTokenMessages aRequest, const TIpcArgs& aArgs, TRequestStatus* aStatus) const;
	};

#endif	//	__TOKENTYPECLIENTSESSION_H__
