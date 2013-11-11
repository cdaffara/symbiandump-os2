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
* TCERTSTOREOUT.H
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_CERTSTOREOUT_H__
#define __T_CERTSTOREOUT_H__
 
#include <f32file.h>
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>

#include <x509cert.h>
#include <x509certext.h>

#include <wtlscert.h>

#include "t_output.h"

class X509CertWriter
	{
public:
	X509CertWriter(Output& aOut);
	void WriteCert(const CX509Certificate& aCertificate);
	void ShowSigningAlgorithm(const CSigningAlgorithmIdentifier& aSigningAlgorithm);
	void ShowPublicKey(const CX509Certificate& aCertificate);
	void ShowSerialNumber(const TPtrC8& aSerialNumber);
	void ShowAVA(const CX520AttributeTypeAndValue& aAva);
	void ShowDN(const CX500DistinguishedName& aName);
	void ShowAKI(const CX509Certificate& aCert);
	void ShowSKI(const CX509Certificate& aCert);
	void ShowGN(const CX509GeneralName& aName);
	void ShowFingerprint(const CX509Certificate& aCertificate);
	void ShowExtensions(const CX509Certificate& aCertificate);
	void ShowValidityPeriod(const CX509Certificate& aCertificate);
	void ShowBC(const CX509CertExtension& aExt);
	void ShowAltName(const CX509CertExtension& aExt);
	void ShowKU(const CX509CertExtension& aExt);
	void ShowSubtrees(const CArrayPtrFlat<CX509GeneralSubtree>& aSubtrees);
	void ShowNC(const CX509CertExtension& aExt);
	void ShowPC(const CX509CertExtension& aExt);
	void ShowCP(const CX509CertExtension& aExt);
	void ShowPM(const CX509CertExtension& aExt);
	void ShowEKU(const CX509CertExtension& aExt);
	void ShowSignature(const CX509Certificate& aCert);
private:
	void WriteEncodings(const CX509Certificate& aCertificate);
	void WriteEncoding(const CX509Certificate& aCertificate, const TUint aIndex);
	void ShowRSAKey(const CSubjectPublicKeyInfo& aSpki);
	void ShowDSAKey(const CSubjectPublicKeyInfo& aSpki);
	void ShowDHKey(const CSubjectPublicKeyInfo& aSpki);
	Output& iOut;
	};

class WTLSCertWriter
	{
public:
	WTLSCertWriter(Output& aOut);
	void WriteCert(const CWTLSCertificate& aCertificate);
	void ShowName(const CWTLSName& aName);
private:
	Output& iOut;
	};


#endif
