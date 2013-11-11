#ifndef __FILECERTSTORE_H__
#define __FILECERTSTORE_H__/*
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
#include <s32file.h>
#include "certinfo.h"

/**
 * @file
 * @internalComponent
 */

class AppUidListEntry : public EncDecContainerItem
	{
public:
	static EncDecContainerItem *Factory();
	AppUidListEntry(const EnumEntry *aEnumEntries);
	virtual ~AppUidListEntry();

	virtual const char *ItemType() const;
	virtual void Encode(REncodeWriteStream &aWriteStream);
	virtual void Decode(RDecodeReadStream &aReadStream);

private:
	EncDecEnum<TUint32> iUid;                    
	};

class CertStoreEntry : public EncDecContainerItem
	{
public:
	static EncDecContainerItem *Factory();
	CertStoreEntry();
	CertStoreEntry(bool aSwiMode);
	virtual ~CertStoreEntry();

	const TCertLabel &Label() const;

	CertInfo &Info();
	const CertInfo &Info() const;

	virtual const char *ItemType() const;
	virtual std::string ItemName() const;
	virtual void SetItemName(const std::string &aName);
	virtual void Encode(REncodeWriteStream &aWriteStream);
	virtual void Decode(RDecodeReadStream &aReadStream);

	CertStoreEntry& operator= (const CertStoreEntry& aRhs);

	const TUint8 * CertData() const;
	const std::string &CertSubject() const;


private:
	CertStoreEntry(const CertStoreEntry &aRef);

	CertInfo iCertInfo;

	EncDecContainer iCertApps; 	// RArray of certifcate apps
	
	EncDecEnum<TUint8> iTrusted;

	EncDecObject<TStreamId> iReadDataStreamId;
	EncDecObject<TStreamId> iWriteDataStreamId;
	EncDecObject<TName> iDataFileName;
	std::string iCertData;

private:
	std::string iCertSubject;
	bool iSwiMode;
	};

#endif
