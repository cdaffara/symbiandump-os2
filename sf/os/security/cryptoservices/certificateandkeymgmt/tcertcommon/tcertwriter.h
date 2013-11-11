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
* CertWriter class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TCERTWRITER_H__
#define __TCERTWRITER_H__

#include "t_output.h"
#include <x509cert.h>
#include <x509gn.h>
#include <x509certext.h>

class CertWriter
	{
public:
	IMPORT_C CertWriter(Output* aOut);
	IMPORT_C void SetOut(Output* aOut);
	IMPORT_C void WriteCert(const CX509Certificate& aCertificate);
	IMPORT_C void ShowSigningAlgorithm(const CSigningAlgorithmIdentifier& aSigningAlgorithm);
	IMPORT_C void ShowPublicKey(const CX509Certificate& aCertificate);
	IMPORT_C void ShowSerialNumber(const TPtrC8& aSerialNumber);
	IMPORT_C void ShowAVA(const CX520AttributeTypeAndValue& aAva);
	IMPORT_C void ShowDN(const CX500DistinguishedName& aName);
	IMPORT_C void ShowAKI(const CX509Certificate& aCert);
	IMPORT_C void ShowSKI(const CX509Certificate& aCert);
	IMPORT_C void ShowGN(const CX509GeneralName& aName);
	IMPORT_C void ShowFingerprint(const CX509Certificate& aCertificate);
	IMPORT_C void ShowExtensions(const CX509Certificate& aCertificate);
	IMPORT_C void ShowValidityPeriod(const CX509Certificate& aCertificate);
	IMPORT_C void ShowBC(const CX509CertExtension& aExt);
	IMPORT_C void ShowAltName(const CX509CertExtension& aExt);
	IMPORT_C void ShowKU(const CX509CertExtension& aExt);
	IMPORT_C void ShowSubtrees(const CArrayPtrFlat<CX509GeneralSubtree>& aSubtrees);
	IMPORT_C void ShowNC(const CX509CertExtension& aExt);
	IMPORT_C void ShowPC(const CX509CertExtension& aExt);
	IMPORT_C void ShowCP(const CX509CertExtension& aExt);
	IMPORT_C void ShowPM(const CX509CertExtension& aExt);
	IMPORT_C void ShowEKU(const CX509CertExtension& aExt);
	IMPORT_C void ShowSignature(const CX509Certificate& aCert);
	IMPORT_C TBool IsSelfSigned(const CX509Certificate& aCert);
private:
	void WriteEncodings(const CX509Certificate& aCertificate);
	void WriteEncoding(const CX509Certificate& aCertificate, const TUint aIndex);
	void ShowRSAKey(const CSubjectPublicKeyInfo& aSpki);
	void ShowDSAKey(const CSubjectPublicKeyInfo& aSpki);
	void ShowDHKey(const CSubjectPublicKeyInfo& aSpki);
	void CalculateSignature(const CX509Certificate& aCert);
	Output* iOut;
	};

#endif

