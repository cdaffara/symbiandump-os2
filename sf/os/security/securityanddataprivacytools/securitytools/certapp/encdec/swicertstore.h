#ifndef __SWICERTSTORE_H__
#define __SWICERTSTORE_H__/*
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


#include "filecertstore.h"
#include "capabilityset.h"

/**
 * @file
 * @internalComponent
 */

class SwiCertStoreEntry : public CertStoreEntry
	{
public:
	static EncDecContainerItem *Factory();
	SwiCertStoreEntry();
	virtual ~SwiCertStoreEntry();

	virtual void Encode(REncodeWriteStream &aWriteStream);
	virtual void Decode(RDecodeReadStream &aReadStream);

	SwiCertStoreEntry& operator= (const SwiCertStoreEntry& aRhs);
private:
	SwiCertStoreEntry(const SwiCertStoreEntry &aRef);
	EncDecObject<CapabilitySet> iCapabilitySet;
	EncDecObject<TUint8> iTmpFlags;
	EncDecEnum<TUint8> iMandatory;
	EncDecEnum<TUint8> iSystemUpgrade;
	};


#endif
