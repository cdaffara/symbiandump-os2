#ifndef __CERTINFO_H__
#define __CERTINFO_H__/*
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
#include "x509utils.h"

/**
 * @file
 * @internalComponent
 */
/**
 * The owner of a certificate.
 * 
 */
enum TCertificateOwnerType
	{
	ECACertificate,
	EUserCertificate,
	EPeerCertificate
	};

const TUint32 KMaxCertLabelLength = 64;
typedef TBuf<KMaxCertLabelLength> TCertLabel;
void EncodeHuman(REncodeWriteStream& aStream,const TCertLabel &aLabel);
void DecodeHuman(RDecodeReadStream& aStream,TCertLabel &aLabel);

class CertInfo
	{
public:
	CertInfo(bool aSwiMode);

	void Encode(REncodeWriteStream &aWriteStream);
	void Decode(RDecodeReadStream &aReadStream);

	TUint32 CertSize() const;
	void SetCertSize(TUint32 aSize);

	const TCertLabel &Label() const;
	TCertLabel &Label();

	TCertificateFormat CertificateFormat() const;

	KeyIdentifierObject &SubjectKeyId();
	const KeyIdentifierObject &SubjectKeyId() const;
	KeyIdentifierObject &IssuerKeyId();
	const KeyIdentifierObject &IssuerKeyId() const;


	TUint32 OutputCertificateId() const;
	void SetOutputCertificateId(TUint32 aId);

	TCertificateOwnerType CertificateOwnerType() const;
	
private:
	EncDecObject<TUint8> iTmpCombinedDeletableAndFormat;
	EncDecEnum<TUint8> iDeletable;
	EncDecEnum<TUint8> iFormat; // TCertificateFormat

	EncDecObject<TInt> iSize;
	EncDecObject<TCertLabel> iLabel;
	EncDecObject<TInt> iReadCertificateId; // Read from binary, written to human
	EncDecObject<TInt> iWriteCertificateId; // Written to binary
	EncDecEnum<TUint8> iCertificateOwnerType; // TCertificateOwnerType
	EncDecObject<KeyIdentifierObject> iSubjectKeyId;
	EncDecObject<KeyIdentifierObject> iIssuerKeyId;

	bool iSwiMode;
	};

#endif
