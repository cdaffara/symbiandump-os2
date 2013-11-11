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
* WTLSCertWriter class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CertWriter_H
#define __CertWriter_H


#include <f32file.h>
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>

#include <wtlscert.h>
#include <wtlskeys.h>
#include <bigint.h>
#include "t_output.h"

class WTLSCertWriter
	{
public:
	WTLSCertWriter(Output* aOut);
	void SetOut(Output* aOut);
	void WriteCert(const CWTLSCertificate& aCertificate);
	void ShowSigningAlgorithm(const CSigningAlgorithmIdentifier& aSigningAlgorithm);
	void ShowPublicKey(const CWTLSCertificate& aCertificate);
	void ShowSerialNumber(const TPtrC8& aSerialNumber);
	void ShowSignature(const CWTLSCertificate& aCert);
	void ShowValidityPeriod(const CWTLSCertificate& aCertificate);
	void ShowFingerprint(const CWTLSCertificate& aCertificate);
	void ShowName(const CWTLSName& aName);
private:
	void WriteFieldByName(TPtrC& aFieldName, const CWTLSStructuredText& aName);
	void WriteEncodings(const CWTLSCertificate& aCertificate);
	const TPtrC8* Encoding(const CWTLSCertificate& aCertificate, const TUint aIndex);
	void ShowRSAKey(const CSubjectPublicKeyInfo& aSpki);
	Output* iOut;
	};

#endif

