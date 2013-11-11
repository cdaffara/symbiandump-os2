// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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

#include <caf/streaming/keyassociation.h>
#include <e32cmn.h>
#include <e32def.h>

using namespace StreamAccess;

EXPORT_C CIpSecKeyAssociation* CIpSecKeyAssociation::NewL(TInt32 aSpi, HBufC8* aEncryptionKey, 
														  HBufC8* aAuthenticationKey)
{	
	CIpSecKeyAssociation* self = CIpSecKeyAssociation::NewLC(aSpi, aEncryptionKey, aAuthenticationKey);
	CleanupStack::Pop(self);
	return self;
}

EXPORT_C CIpSecKeyAssociation* CIpSecKeyAssociation::NewLC(TInt32 aSpi, HBufC8* aEncryptionKey, 
														   HBufC8* aAuthenticationKey)
{
	CIpSecKeyAssociation* self = new (ELeave) CIpSecKeyAssociation(aSpi, aEncryptionKey, aAuthenticationKey);
	CleanupStack::PushL(self);
	return self;	
}

CIpSecKeyAssociation::CIpSecKeyAssociation(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) : 
										   iSpi(aSpi), iEncryptionKey(aEncryptionKey),
										   iAuthenticationKey(aAuthenticationKey)
{
}


TInt32 CIpSecKeyAssociation::GetSpiL() const
{
	return iSpi;
}

const HBufC8* CIpSecKeyAssociation::GetEncryptionKeyL() const
{
	return iEncryptionKey;
}

const HBufC8* CIpSecKeyAssociation::GetAuthenticationKeyL() const
{
	return iAuthenticationKey;
}


