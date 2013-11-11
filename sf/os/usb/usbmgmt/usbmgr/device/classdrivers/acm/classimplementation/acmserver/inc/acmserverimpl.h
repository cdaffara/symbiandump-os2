/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
 @file
 @internalTechnology
*/

#ifndef __ACMSERVERIMPL_H__
#define __ACMSERVERIMPL_H__

#include <e32base.h>
#include <c32comm.h>
#include "acmserverclient.h"

/**
This is the 'body' of the RAcmServer - CAcmServerImpl handle-body system. 
Handle-body enables us to alter the implementation of ACM control whilst 
maintaining the same binary interface with clients.
This class MUST NOT be used outside Symbian.
Specifically, it is responsible for making sure that the ECACM CSY is loaded 
(= ACM server running) and connecting an IPC channel to the ACM server.
*/
NONSHARABLE_CLASS(CAcmServerImpl) : public CBase
	{
public:
	static CAcmServerImpl* NewL();
	~CAcmServerImpl();

public:
	TInt CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName);
	TInt DestroyFunctions(const TUint aNoAcms);

private:
	CAcmServerImpl();
	void ConstructL();

private:
	RCommServ iCommServ;
	RAcmServerClient iAcmServerClient;
	};

#endif // __ACMSERVERIMPL_H__
