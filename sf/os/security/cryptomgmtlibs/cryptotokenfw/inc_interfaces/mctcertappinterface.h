/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* mctcertappinterface.h
*
*/


/**
 @file 
 @internalAll
*/
 
#ifndef MCTCERTAPPINTERFACE_H
#define MCTCERTAPPINTERFACE_H

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <ct/mcttokeninterface.h>

/** The UID of certificate application token type */
const TInt KTokenTypeCertApps = 0x101f7a37;

/** The UID of certificate application interface */
const TInt KInterfaceCertApps = 0x101f7a38;

class TCertificateAppInfo;

/**
 * Interface for certificate applications manager token.
 */
class MCTCertApps : public MCTTokenInterface
	{
public:
	virtual void AddL(const TCertificateAppInfo& aClient) = 0;
	virtual void RemoveL(const TUid& aUid) = 0;
	virtual TInt ApplicationCountL() const = 0;
	virtual void ApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const = 0;
	virtual void ApplicationL(const TUid& aUid, TCertificateAppInfo& aInfo) const = 0;
	};


#endif // MCTCERTAPPINTERFACE_H
