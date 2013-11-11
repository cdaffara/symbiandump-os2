/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_filter.h"
#include "t_input.h"
#include "t_output.h"
#include "t_certstoredefs.h"

#include <ccertattributefilter.h>
#include <x509cert.h>

CCertAttributeFilter* CFilter::NewL(Output& aOut, 
									const TDesC8& aFilter)
	{
	CFilter* self = new(ELeave) CFilter(aOut);
	CleanupStack::PushL(self);
	
	iFilter = CCertAttributeFilter::NewL();
	CleanupStack::PushL(iFilter);
	
	self->ConstructL(aFilter);
	
	CleanupStack::Pop(iFilter);
	CleanupStack::PopAndDestroy(self);
	return iFilter;
	}

 CFilter::CFilter(Output& aOut): iOut(aOut)
 	{}
 
 CFilter::~CFilter()
 	{}
 
void CFilter::ConstructL(const TDesC8& aFilter)
	{
	
	// Set label filter
	SetLabelFilter(Input::ParseElement(aFilter, KCertLabelStart));
	
	// Set uid filter
	SetUIDFilter(Input::ParseElement(aFilter, KUidStart));
	
	// Set format filter
	SetFormatFilterL(Input::ParseElement(aFilter, KFormatFlagsStart));	
	
	// Set ownertype filter
	SetOwnerTypeFilterL(Input::ParseElement(aFilter, KOwnerTypeStart));
	
	// Set keyusage filter
	TInt pos = 0;
	TInt err = KErrNone;
	
	while( KErrNone == err )
		{
		const TDesC8& keyUsageDesc = Input::ParseElement(aFilter, KKeyUsageDescStart, KKeyUsageDescEnd,pos,err);
		if(KErrNone != err)
			{
			break;
			}
		// as by default the filter object has all usage. 
		iFilter->SetKeyUsage(EX509UsageNone);
		SetKeyUsageFilterL(keyUsageDesc);
		}
	
	// Set subjetkeyid filter
	SetSubjectKeyIdFilter(Input::ParseElement(aFilter, KSubjectKeyIdStart));
	
	// Set subjetkeyid filter
	SetIssuerKeyIdFilter(Input::ParseElement(aFilter, KIssuerKeyIdStart));

	// Set subjetkeyid using the certificate details
	SetSubjectKeyIdFromCertFilterL(Input::ParseElement(aFilter, KUseSubjectKeyIdStart));

	}

void CFilter::SetLabelFilter(const TDesC8& aLabel)
	{
	if (aLabel != KNullDesC8)
		{
		TCertLabel certLabel;
		certLabel.Copy(aLabel);
		iFilter->SetLabel(certLabel);
		}
	}

void CFilter::SetUIDFilter(const TDesC8& aUid)
	{
	if (aUid != KNullDesC8)
		{
		TLex8 uidString(aUid);
		TInt32 val;
		uidString.Val(val);
		iFilter->SetUid(TUid::Uid(val));
		}
	}

void CFilter::SetFormatFilterL(const TDesC8& aFormatString)
	{
	if (aFormatString != KNullDesC8)
		{
		if (aFormatString == KWTLS)
			{
			iFilter->SetFormat(EWTLSCertificate);
			}
		else if (aFormatString == KX509)
			{
			iFilter->SetFormat(EX509Certificate);
			}
		else if (aFormatString == KWTLSURL)
			{
			iFilter->SetFormat(EWTLSCertificateUrl);
			}
		else if (aFormatString == KX509URL)
			{
			iFilter->SetFormat(EX509CertificateUrl);
			}
		else
			{
			iOut.write(_L("Unknown cert format: "));
			iOut.writeString(aFormatString);
			iOut.writeNewLine();		   
			User::Leave(KErrNotSupported);
			}
		}
	}

void CFilter::SetOwnerTypeFilterL(const TDesC8& aOwnerType)
	{
	if (aOwnerType != KNullDesC8)
		{
		if(aOwnerType == KCACert)
			{
			iFilter->SetOwnerType(ECACertificate);
			}
		else if(aOwnerType == KUserCert)
			{
			iFilter->SetOwnerType(EUserCertificate);
			}
		else if(aOwnerType == KPeerCert)
			{
			iFilter->SetOwnerType(EPeerCertificate);
			}
		else
			{
			iOut.write(_L("Unknown owner type: "));
			iOut.writeString(aOwnerType);
			iOut.writeNewLine();		   
			User::Leave(KErrNotSupported);
			}
		}
	}

void CFilter::SetKeyUsageFilterL(const TDesC8& aKeyUsageDescription)
	{
	TInt pos = 0;
	TInt err = KErrNone;
	
	const TDesC8& keyUsageScheme = Input::ParseElement(aKeyUsageDescription, KKeyUsageSchemeStart,KKeyUsageSchemeEnd,pos,err);
		
	if(keyUsageScheme == KKeyUsageX509)
		{
		// Set key usage filter
		TInt err = KErrNone;
		for(;;)
			{
			const TDesC8& keyusage = Input::ParseElement(aKeyUsageDescription, KKeyUsageStart, KKeyUsageEnd, pos, err);
			if(err != KErrNone)
				{
				break;
				}
			TKeyUsageX509 x509KeyUsage  = ParseKeyUsageL(keyusage);
			iFilter->SetKeyUsage(iFilter->iKeyUsage | x509KeyUsage);
			}
		}
	else if(keyUsageScheme == KKeyUsagePKCS)
		{
		// Set key usage filter
		TInt err = KErrNone;
		const TDesC8& keytype = Input::ParseElement(aKeyUsageDescription, KKeyTypeStart, KKeyTypeEnd, pos, err);
		
		for(;;)
			{
			const TDesC8& keyusage = Input::ParseElement(aKeyUsageDescription, KKeyUsageStart, KKeyUsageEnd, pos, err);	
			if(err != KErrNone)
				{
				break;
				}
			TKeyUsageX509 x509KeyUsage  = ParseKeyUsageL(keyusage,keytype);
			iFilter->SetKeyUsage(iFilter->iKeyUsage | x509KeyUsage);
			}
		}
	else
		{
		iOut.write(_L("Unknown scheme specified: "));
		iOut.writeString(keyUsageScheme);
		iOut.writeNewLine();		   
		User::Leave(KErrNotSupported);
		}
	}

TKeyUsageX509 CFilter::ParseKeyUsageL(const TDesC8& keyUsage)
	{
	TKeyUsageX509 x509KeyUsage = EX509UsageNone;
	
	if (keyUsage == KDigitalSignature || keyUsage == KDS)
		{
		x509KeyUsage = EX509UsageDigitalSignature;
		}
	else if (keyUsage == KNonRepudiation || keyUsage == KNR)
		{
		x509KeyUsage = EX509UsageNonRepudiation;
		}
	else if (keyUsage == KKeyEncipherment || keyUsage == KKE)
		{
		x509KeyUsage = EX509UsageKeyEncipherment;
		}
	else if (keyUsage == KDataEncipherment || keyUsage == KDE)
		{
		x509KeyUsage = EX509UsageDataEncipherment;
		}
	else if (keyUsage == KKeyAgreement || keyUsage == KKA)
		{
		x509KeyUsage = EX509UsageKeyAgreement;
		}
	else if (keyUsage == KKeyCertSign || keyUsage == KKCS)
		{
		x509KeyUsage = EX509UsageKeyCertSign;
		}
	else if (keyUsage == KCRLSign || keyUsage == KCRLS)
		{
		x509KeyUsage = EX509UsageCRLSign;
		}
	else if (keyUsage == KEncipherOnly || keyUsage == KE)
		{
		x509KeyUsage = EX509UsageEncipherOnly;
		}
	else if (keyUsage == KDecipherOnly || keyUsage == KD)
		{
		x509KeyUsage = EX509UsageDecipherOnly;
		}
	else if (keyUsage.Length() != 0)
		{
		iOut.writeString(_L("WARNING: unknown key usage: "));
		iOut.writeString(keyUsage);
		iOut.writeNewLine();
		}
	return x509KeyUsage;
	}
	
TKeyUsageX509 CFilter::ParseKeyUsageL(const TDesC8& aKeyUsage, const TDesC8& keyType)
	{
	TKeyUsageX509 x509KeyUsage = EX509UsageNone;
	if( keyType != KNullDesC8 && (keyType ==KPublic || keyType == KAll))
		{
		if (aKeyUsage == KEncrypt)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageEncrypt);
			}
		else if (aKeyUsage == KVerify)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageVerify);
			}
		else if (aKeyUsage == KVerifyRecover)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageVerifyRecover);
			}
		else if (aKeyUsage == KDerive)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageDerive);
			}
		else if (aKeyUsage == KWrap)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageWrap);
			}
		else if (aKeyUsage == KNonRepudiation || aKeyUsage == KNR)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageNonRepudiation);
			}
		else if (aKeyUsage.Length() != 0)
			{
			iOut.writeString(_L("WARNING: unknown key usage: "));
			iOut.writeString(aKeyUsage);
			iOut.writeNewLine();
			}
		}
	else if( keyType != KNullDesC8 && (keyType == KPrivate || keyType == KAll) )
		{
		// individual key usages
		if(aKeyUsage == KDecrypt)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageDecrypt);
			}
		else if (aKeyUsage == KSign)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageSign);
			}
		else if (aKeyUsage == KSignRecover)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageSignRecover);
			}
		else if (aKeyUsage == KDerive)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageDerive);
			}
		else if (aKeyUsage == KUnwrap)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageUnwrap);
			}
		else if (aKeyUsage == KNonRepudiation || aKeyUsage == KNR)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageNonRepudiation);
			}
		
		// common combinations of private keys 
		else if (aKeyUsage == KSigning)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageSignSignRecover);
			}
		else if (aKeyUsage == KEncipherAndSign)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageSignSignRecover | EPKCS15UsageUnwrap);
			}
		else if (aKeyUsage == KAllKeyUsagesButNR)
			{
			x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageSignSignRecover | EPKCS15UsageDecryptUnwrap); // | EPKCS15UsageDerive);
			}
		
		else if (aKeyUsage.Length() != 0)
			{
			iOut.writeString(_L("WARNING: unknown key usage: "));
			iOut.writeString(aKeyUsage);
			iOut.writeNewLine();
			}
		}
	else if(keyType == KNullDesC8)
		{
		x509KeyUsage = KeyUsagePKCS15ToX509(EPKCS15UsageNonRepudiation);
		}
	
	return x509KeyUsage;
	}

void CFilter::SetSubjectKeyIdFilter(const TDesC8& aSubjectKeyId)
	{
	if (aSubjectKeyId != KNullDesC8)
		{
		TKeyIdentifier id;
		id.Zero();
		
		for (TInt i = 0; i < aSubjectKeyId.Length() ; i += 2)
			{
			TInt a = (aSubjectKeyId[i+1] >= 'a') ? (aSubjectKeyId[i+1] - 'a' + 10) : (aSubjectKeyId[i+1] - '0');
			TInt b = (aSubjectKeyId[i] >= 'a') ? (aSubjectKeyId[i] - 'a' + 10) : (aSubjectKeyId[i] - '0');
			id.Append(a  + b * 16);
			}

		iFilter->SetSubjectKeyId(id);
		}
	}

void CFilter::SetIssuerKeyIdFilter(const TDesC8& aIssuerKeyId)
	{
	if (aIssuerKeyId != KNullDesC8)
		{
		TKeyIdentifier id;
		id.Zero();
		
		for (TInt i = 0; i < aIssuerKeyId.Length() ; i += 2)
			{
			TInt a = (aIssuerKeyId[i+1] >= 'a') ? (aIssuerKeyId[i+1] - 'a' + 10) : (aIssuerKeyId[i+1] - '0');
			TInt b = (aIssuerKeyId[i] >= 'a') ? (aIssuerKeyId[i] - 'a' + 10) : (aIssuerKeyId[i] - '0');
			id.Append(a  + b * 16);
			}

		iFilter->SetIssuerKeyId(id);
		}
	}

void CFilter::SetSubjectKeyIdFromCertFilterL(const TDesC8& aCertPath)
	{
	if(aCertPath != KNullDesC8)
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TFileName filename;
		filename.Copy(aCertPath);
		HBufC8* buf = Input::ReadFileL(filename, fs);
		CleanupStack::PushL(buf);
		CX509Certificate* cert = CX509Certificate::NewLC(buf->Des());
		TKeyIdentifier id = cert->SubjectKeyIdL();
		CleanupStack::PopAndDestroy(3, &fs); // buf, cert, fs
		iFilter->SetSubjectKeyId(id);
		}
	}


CCertAttributeFilter* CFilter::iFilter = NULL;
