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

#ifndef __ACMSERVERCLIENT_H__
#define __ACMSERVERCLIENT_H__

#include <e32base.h>

/**
Client-side interface to the ACM server to create new ACM functions.
This class MUST NOT be used outside Symbian.
*/
NONSHARABLE_CLASS(RAcmServerClient) : public RSessionBase
	{
public:
	RAcmServerClient();
	~RAcmServerClient();

public:
	TInt Connect();
	TVersion Version() const;

public:
	TInt CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName);
	TInt DestroyFunctions(const TUint aNoAcms);

private:
	void ConnectL();
	};

#endif // __ACMSERVERCLIENT_H__
