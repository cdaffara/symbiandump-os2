/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "certclients.h"
#include "stringconv.h"
#include <s32file.h>

EncDecContainerItem *CertificateAppInfo::Factory()
{
	return new CertificateAppInfo;
}

EXPORT_C CertificateAppInfo::CertificateAppInfo()
	: iUid("Uid"), iName("Name")
	{
	}

EXPORT_C CertificateAppInfo& CertificateAppInfo::operator = (const CertificateAppInfo& aClient)
	{
BULLSEYE_OFF
	if (this == &aClient) return *this;
BULLSEYE_RESTORE
	iUid = aClient.iUid;
	iName = aClient.iName;
	return *this;
	}

EXPORT_C const TUid& CertificateAppInfo::Id() const
{
	return iUid.Value();
}

EXPORT_C const TName& CertificateAppInfo::Name() const
{
	return iName.Value();
}

CertificateAppInfo::~CertificateAppInfo()
{
}

const char *CertificateAppInfo::ItemType() const
{
	return 0;
}



void CertificateAppInfo::Encode(REncodeWriteStream &aWriteStream)
{
	aWriteStream << iUid;
	aWriteStream << iName;
}

void CertificateAppInfo::Decode(RDecodeReadStream &aReadStream)
{
	aReadStream >> iUid;
	aReadStream >> iName;
}

// End of file
