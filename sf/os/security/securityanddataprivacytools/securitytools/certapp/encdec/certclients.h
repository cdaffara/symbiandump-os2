#ifndef __CERTCLIENTS_H__
#define __CERTCLIENTS_H__/*
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


#include "encdec.h"

/**
 * @file
 * @internalComponent
 */

class CertificateAppInfo : public EncDecContainerItem
	{
public:
	static EncDecContainerItem *Factory();
	virtual ~CertificateAppInfo();
	CertificateAppInfo& operator= (const CertificateAppInfo& aClient);
	CertificateAppInfo();
	const TUid& Id() const;
	const TName& Name() const;

	virtual const char *ItemType() const;
	virtual void Encode(REncodeWriteStream &aWriteStream);
	virtual void Decode(RDecodeReadStream &aReadStream);

private:
	EncDecObject<TUid> iUid;                    
	EncDecObject<TName> iName;
	};

#endif
