/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "securitydefs.h"

////////////////////////////////////////////////////////////////////////////////
// TKeyUsagePKCS15 & TKeyUsageX509
////////////////////////////////////////////////////////////////////////////////

//	x509									PKCS15 Public	PKCS15 Private

//	DataEncipherment						Encrypt			Decrypt
//	DigitalSignature, keyCertSign, cRLSign	Verify			Sign
//	DigitalSignature, keyCertSign, cRLSign	VerifyRecover	SignRecover
//	KeyAgreement							Derive			Derive
//	KeyEncipherment							Wrap			Unwrap
//	NonRepudiation							NonRepudiation	NonRepudiation

EXPORT_C TKeyUsagePKCS15 KeyUsageX509ToPKCS15Private(TKeyUsageX509 aUsage)
	{
	TKeyUsagePKCS15 result = EPKCS15UsageNone;

	if (EX509UsageAll == aUsage)
		{
		result = EPKCS15UsageAll;
		}
	else
		{
		if (aUsage & EX509UsageDataEncipherment)
			{
			result |= EPKCS15UsageDecrypt;
			}
		if (aUsage & (EX509UsageDigitalSignature | EX509UsageKeyCertSign | EX509UsageCRLSign))
			{
			result |= EPKCS15UsageSign | EPKCS15UsageSignRecover;
			}
		if (aUsage & EX509UsageKeyAgreement)
			{
			result |= EPKCS15UsageDerive;
			}
		if (aUsage & EX509UsageKeyEncipherment)
			{
			result |= EPKCS15UsageUnwrap;
			}
		if (aUsage & EX509UsageNonRepudiation)
			{//	This shouldn't really happen, ENonRepudiation should be sole usage
			result |= EPKCS15UsageNonRepudiation;
			}
		}
	
	return result;
	}

EXPORT_C TKeyUsagePKCS15 KeyUsageX509ToPKCS15Public(TKeyUsageX509 aUsage)
	{
	TKeyUsagePKCS15 result = EPKCS15UsageNone;

	if (EX509UsageAll == aUsage)
		{
		result = EPKCS15UsageAll;
		}
	else
		{
		if (aUsage & EX509UsageDataEncipherment)
			{
			result |= EPKCS15UsageEncrypt;
			}
		if (aUsage & (EX509UsageDigitalSignature | EX509UsageKeyCertSign | EX509UsageCRLSign))
			{
			result |= EPKCS15UsageVerify | EPKCS15UsageVerifyRecover;
			}
		if (aUsage & EX509UsageKeyAgreement)
			{
			result |= EPKCS15UsageDerive;
			}
		if (aUsage & EX509UsageKeyEncipherment)
			{
			result |= EPKCS15UsageWrap;
			}
		if (aUsage & EX509UsageNonRepudiation)
			{//	This shouldn't really happen, ENonRepudiation should be sole usage
			result |= EPKCS15UsageNonRepudiation;
			}
		}
	
	return result;
	}

EXPORT_C TKeyUsageX509 KeyUsagePKCS15ToX509(TKeyUsagePKCS15 aUsage)
	{
	TKeyUsageX509 result = EX509UsageNone;

	if (EPKCS15UsageAll == aUsage)
		{
		result = EX509UsageAll;
		}
	else
		{
		if (aUsage & (EPKCS15UsageEncrypt | EPKCS15UsageDecrypt))
			{
			result |= EX509UsageDataEncipherment;
			}
		if (aUsage & (EPKCS15UsageVerify | EPKCS15UsageSign | EPKCS15UsageVerifyRecover | EPKCS15UsageSignRecover))
			{
			result |= EX509UsageDigitalSignature | EX509UsageKeyCertSign | EX509UsageCRLSign;
			}
		if (aUsage & EPKCS15UsageDerive)
			{
			result |= EX509UsageKeyAgreement;
			}
		if (aUsage & (EPKCS15UsageWrap | EPKCS15UsageUnwrap))
			{
			result |= EX509UsageKeyEncipherment;
			}
		if (aUsage & EPKCS15UsageNonRepudiation)
			{
			result |= EX509UsageNonRepudiation;
			}
		}
	
	return result;
	}
