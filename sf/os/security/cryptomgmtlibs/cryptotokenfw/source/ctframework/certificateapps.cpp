/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mctcertapps.h>

////////////////////////////////////////////////////////////////
//	TCertificateAppInfo
////////////////////////////////////////////////////////////////

EXPORT_C TCertificateAppInfo::TCertificateAppInfo(const TUid& aUid, const TName& aName)
	:iUid(aUid), iName(aName)
	{
	}

EXPORT_C TCertificateAppInfo::TCertificateAppInfo()
	{
	}

EXPORT_C TCertificateAppInfo& TCertificateAppInfo::operator = (const TCertificateAppInfo& aClient)
	{
	if (this == &aClient)
		return *this;
	iUid = aClient.iUid;
	iName = aClient.iName;
	return *this;
	}

EXPORT_C const TUid& TCertificateAppInfo::Id() const
	{
	return iUid;
	}

EXPORT_C const TName& TCertificateAppInfo::Name() const
	{
	return iName;
	}

EXPORT_C void TCertificateAppInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iUid;
	aStream << iName;
	}

EXPORT_C void TCertificateAppInfo::InternalizeL(RReadStream& aStream)
	{
	aStream >> iUid;
	aStream >> iName;
	}
