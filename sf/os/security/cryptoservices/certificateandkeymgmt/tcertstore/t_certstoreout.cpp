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
* TCERTSTOREOUT.CPP
*
*/


#include "t_certstoreout.h"

X509CertWriter::X509CertWriter(Output& aOut)
	:iOut(aOut)
	{
	}

void X509CertWriter::WriteCert(const CX509Certificate& aCertificate)
	{
#ifdef SHOW_EXTENDED
	ShowSerialNumber(aCertificate.SerialNumber());
	ShowValidityPeriod(aCertificate);
	iOut.writeString(_L("Issuer Name = "));
	iOut.writeNewLine();	
	ShowDN(aCertificate.IssuerName());	
	iOut.writeString(_L("Subject Name = "));
	iOut.writeNewLine();	
	ShowDN(aCertificate.SubjectName());
	ShowSigningAlgorithm(aCertificate.SigningAlgorithm());
	ShowExtensions(aCertificate);
	WriteEncodings(aCertificate);
#endif // SHOW_EXTENDED
	iOut.writeString(_L("\t\tShort Issuer Name = "));
	HBufC* issuer = aCertificate.IssuerL();
	iOut.writeString(*issuer);
	iOut.writeNewLine();
	delete issuer;

	iOut.writeString(_L("\t\tShort Subject Name = "));
	HBufC* subject = aCertificate.SubjectL();
	iOut.writeString(*subject);
	iOut.writeNewLine();
	delete subject;
	}

// The rest of these are only compiled of SHOW_EXTENDED defined - this 
// eliminates warnings from WINS/UREL build
#ifdef SHOW_EXTENDED
void X509CertWriter::ShowSigningAlgorithm(const CSigningAlgorithmIdentifier& aSigningAlgorithm)
	{
	iOut.writeString(_L("Signed using: "));
	iOut.writeNewLine();
	iOut.writeSpaces(4);
	iOut.writeString(_L("Asymmetric algorithm = "));
	const CAlgorithmIdentifier& algId = aSigningAlgorithm.AsymmetricAlgorithm();
	switch(algId.Algorithm())
	{
	case ERSA:
		{
		iOut.writeString(_L("RSA"));
		break;
		}
	case EDSA:
		{
		iOut.writeString(_L("DSA"));
		break;
		}
	case EDH:
		{
		iOut.writeString(_L("DH"));
		break;
		}
	default:
		iOut.writeString(_L("Unknown"));
		break;
	}
	iOut.writeNewLine();
	iOut.writeSpaces(4);
	iOut.writeString(_L("Digest algorithm = "));
	const CAlgorithmIdentifier& digestId = aSigningAlgorithm.DigestAlgorithm();
	switch(digestId.Algorithm())
	{
	case EMD2:
		{
		iOut.writeString(_L("MD2"));
		break;
		}
	case EMD5:
		{
		iOut.writeString(_L("MD5"));
		break;
		}
	case ESHA1:
		{
		iOut.writeString(_L("SHA1"));
		break;
		}
	default:
		iOut.writeString(_L("Unknown"));
	}
	iOut.writeNewLine();
	}

void X509CertWriter::ShowSerialNumber(const TPtrC8& aSerialNumber)
	{
	iOut.writeString(_L("Serial Number = "));
	iOut.writeOctetString(aSerialNumber);
	iOut.writeNewLine();
	}

//dn display code
void X509CertWriter::ShowAVA(const CX520AttributeTypeAndValue& aAva)
	{
	iOut.writeString(aAva.Type());
	HBufC* val = aAva.ValueL();
	CleanupStack::PushL(val);
	iOut.writeString(_L(" = "));
	iOut.writeString(val->Des());
	CleanupStack::PopAndDestroy();
	}

void X509CertWriter::ShowDN(const CX500DistinguishedName& aName)
	{
	TInt count = aName.Count();
	for (TInt i = 0; i < count; i++)
		{
  		const CX520AttributeTypeAndValue& ava = aName.Element(i);
		iOut.writeSpaces(4);
		ShowAVA(ava);
		iOut.writeNewLine();
		}
	}

void X509CertWriter::ShowAKI(const CX509Certificate& aCert)
	{
	const CX509CertExtension* akiExt = aCert.Extension(KAuthorityKeyId);
	if (akiExt)
		{
		iOut.writeString(_L("Authority Key ID = "));
		iOut.writeNewLine();
		const CX509AuthorityKeyIdExt* ext = CX509AuthorityKeyIdExt::NewLC(akiExt->Data());
		const CArrayPtrFlat<CX509GeneralName>& authorityName = ext->AuthorityName();
		TInt count = authorityName.Count();
		if (count > 0)
			{
			iOut.writeSpaces(4);
			iOut.writeString(_L("Authority name = "));
			iOut.writeNewLine();
			for (TInt i = 0; i < count; i++)
				{
				ShowGN(*(authorityName.At(i)));
				}
			}
		if (ext->AuthorityCertSerialNumber().Length() > 0)
			{
			iOut.writeSpaces(4);
			iOut.writeString(_L("Authority cert serial no = "));
			iOut.writeOctetString(ext->AuthorityCertSerialNumber());
			iOut.writeNewLine();
			}
		iOut.writeSpaces(4);
		iOut.writeString(_L("Key Id = "));
		iOut.writeOctetString(ext->KeyId());
		iOut.writeNewLine();
		CleanupStack::PopAndDestroy();
		}
	}

void X509CertWriter::ShowSKI(const CX509Certificate& aCert)
	{
	const CX509CertExtension* skiExt = aCert.Extension(KSubjectKeyId);
	if (skiExt)
		{
		iOut.writeString(_L("Subject Key ID = "));
		iOut.writeNewLine();
		const CX509SubjectKeyIdExt* ext = CX509SubjectKeyIdExt::NewLC(skiExt->Data());
		iOut.writeSpaces(4);
		iOut.writeString(_L("Key Id = "));
		iOut.writeOctetString(ext->KeyId());
		iOut.writeNewLine();
		CleanupStack::PopAndDestroy();
		}
	}

void X509CertWriter::ShowGN(const CX509GeneralName& aName)
	{
	iOut.writeSpaces(4);
	switch(aName.Tag())
		{
		case 1:
			{
			//rfc822
			CX509RFC822NameSubtree* email = CX509RFC822NameSubtree::NewLC(aName.Data());
			const RArray<TPtrC>& rep = email->Rep();
			TInt count = rep.Count();
			for (TInt i = 0; i < count; i++)
				{
				iOut.writeString(rep[i]);
				iOut.writeSpaces(1);
				}
			iOut.writeNewLine();
			CleanupStack::PopAndDestroy();//email
			break;
			}
		case 2:
			{
			//dns name
			CX509DNSNameSubtree* dns = CX509DNSNameSubtree::NewLC(aName.Data());
			const RArray<TPtrC>& rep = dns->Rep();
			TInt count = rep.Count();
			for (TInt i = 0; i < count; i++)
				{
				iOut.writeString(rep[i]);
				iOut.writeSpaces(1);
				}
			CleanupStack::PopAndDestroy();//dns
			break;
			}
		case 4:
			{
			//DN
			CX500DistinguishedName* dN = CX500DistinguishedName::NewLC(aName.Data());
			ShowDN(*dN);
			CleanupStack::PopAndDestroy();
			break;
			}
		case 6:
			{
			//uri
			CX509IPBasedURI* uri = CX509IPBasedURI::NewLC(aName.Data());
			iOut.writeString(uri->Name());
			iOut.writeNewLine();
			CleanupStack::PopAndDestroy();//uri
			break;
			}
		case 7:
			{
			//ip address//!!!!not done for writing to file yet!!!
			CX509IPSubnetMask* ip = CX509IPSubnetMask::NewLC(aName.Data());
			TPtrC8 pBA(ip->BaseAddress());
			TInt counter;
			counter = pBA.Length();
			TPtrC8 pM(ip->Mask());
			counter = pM.Length();
			CleanupStack::PopAndDestroy();
			break;
			}
		}
	}


void X509CertWriter::ShowFingerprint(const CX509Certificate& aCertificate)
	{
	iOut.writeOctetString(aCertificate.Fingerprint());
	iOut.writeNewLine();
	}

void X509CertWriter::ShowValidityPeriod(const CX509Certificate& aCertificate)
	{
	const CValidityPeriod& vp = aCertificate.ValidityPeriod();
	iOut.writeString(_L("Validity Period = "));
	iOut.writeNewLine();
	const TTime& start = vp.Start();
	const TTime& finish = vp.Finish();
	TBuf<30> dateString1;
	start.FormatL(dateString1,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));

	iOut.writeSpaces(4);
	iOut.writeString(_L("Valid From = "));
	iOut.writeString(dateString1);
	iOut.writeNewLine();

	TBuf<30> dateString2;
	finish.FormatL(dateString2,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));

	iOut.writeSpaces(4);
	iOut.writeString(_L("Valid Until = "));

	iOut.writeString(dateString2);
	iOut.writeNewLine();
	}

//extensions
void X509CertWriter::ShowExtensions(const CX509Certificate& aCertificate)
	{
	const CArrayPtrFlat<CX509CertExtension>& exts = aCertificate.Extensions();
	TInt count = exts.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509CertExtension* ext = exts.At(i);
		iOut.writeString(_L("extension"));
		iOut.writeNum(i);
		if (!(ext->Critical()))
			{
			iOut.writeString(_L(" not"));
			}
		iOut.writeString(_L(" critical"));
		iOut.writeString(_L(" ID = "));
		TPtrC id(ext->Id());
		iOut.writeString(id);
		iOut.writeNewLine();
		if (id == KBasicConstraints)
			{
			ShowBC(*ext);
			}
		if (id == KSubjectAltName)
			{
			iOut.writeString(_L("SubjectAltName: \n"));
			ShowAltName(*ext);
			}
		if (id == KIssuerAltName)
			{
			iOut.writeString(_L("IssuerAltName: \n"));
			ShowAltName(*ext);
			}
		if (id == KKeyUsage)
			{
			ShowKU(*ext);
			}
		if (id == KNameConstraints)
			{
			ShowNC(*ext);
			}
		if (id == KPolicyConstraints)
			{
			ShowPC(*ext);
			}
		if (id == KCertPolicies)
			{
			ShowCP(*ext);
			}
		if (id == KPolicyMapping)
			{
			ShowPM(*ext);
			}
		if (id == KAuthorityKeyId)
			{
			ShowAKI(aCertificate);
			}
		if (id == KSubjectKeyId)
			{
			ShowSKI(aCertificate);
			}
		if (id == KExtendedKeyUsage)
			{
			ShowEKU(*ext);
			}
		}
	}

void X509CertWriter::ShowBC(const CX509CertExtension& aExt)
	{
	iOut.writeString(_L("Basic Constraints:"));
	iOut.writeNewLine();
	CX509BasicConstraintsExt* ext = CX509BasicConstraintsExt::NewLC(aExt.Data());
	if (ext->IsCA())
		{
		iOut.writeSpaces(4);
		iOut.writeString(_L("CA cert"));
		iOut.writeNewLine();
		if (ext->MaxChainLength() < KMaxTInt)
			{
			iOut.writeSpaces(4);
			iOut.writeString(_L("Max Chain Length = "));
			iOut.writeNum(ext->MaxChainLength());
			iOut.writeNewLine();
			}
		}
	else
		{
		iOut.writeSpaces(4);
		iOut.writeString(_L("EE cert"));
		iOut.writeNewLine();
		}
	CleanupStack::PopAndDestroy();
	}

void X509CertWriter::ShowAltName(const CX509CertExtension& aExt)
	{
	CX509AltNameExt* ext = CX509AltNameExt::NewLC(aExt.Data());
	const CArrayPtrFlat<CX509GeneralName>& names = ext->AltName();
	TInt count = names.Count();
	for (TInt i = 0; i < count; i++)
		{
		ShowGN(*(names.At(i)));
		}
	CleanupStack::PopAndDestroy();
	}


_LIT(KDigSig, "digital signature");
_LIT(KNonRep, "non-repudiation");
_LIT(KKeyEnc, "key encipherment");
_LIT(KDataEnc, "data encipherment");
_LIT(KKeyAgree, "key agreement");
_LIT(KCertSign, "key cert sign");
_LIT(KCRLSign, "crl sign");
_LIT(KEnciph, "encipher only");
_LIT(KDeciph, "decipher only");

void X509CertWriter::ShowKU(const CX509CertExtension& aExt)
	{
	iOut.writeString(_L("Key Usage:"));
	iOut.writeNewLine();
	CX509KeyUsageExt* ext = CX509KeyUsageExt::NewLC(aExt.Data());
		if (ext->IsSet(EX509DigitalSignature))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KDigSig);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509NonRepudiation))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KNonRep);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509KeyEncipherment))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KKeyEnc);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509DataEncipherment))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KDataEnc);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509KeyAgreement))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KKeyAgree);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509KeyCertSign))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KCertSign);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509CRLSign))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KCRLSign);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509EncipherOnly))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KEnciph);
			iOut.writeNewLine();
			}
		if (ext->IsSet(EX509DecipherOnly))
			{
			iOut.writeSpaces(4);
			iOut.writeString(KDeciph);
			iOut.writeNewLine();
			}		
	CleanupStack::PopAndDestroy();//ext
	}

void X509CertWriter::ShowSubtrees(const CArrayPtrFlat<CX509GeneralSubtree>& aSubtrees)
	{
	TInt count = aSubtrees.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509GeneralSubtree* subtree = aSubtrees.At(i);
		ShowGN(subtree->Name());
		///!!!!!don't write these outFile either!!
		}
	}

void X509CertWriter::ShowNC(const CX509CertExtension& aExt)
	{//!!!!don't write these outFile

	CX509NameConstraintsExt* ext = CX509NameConstraintsExt::NewLC(aExt.Data());
	ShowSubtrees(ext->ExcludedSubtrees());
	ShowSubtrees(ext->PermittedSubtrees());
	CleanupStack::PopAndDestroy();
	}

void X509CertWriter::ShowPC(const CX509CertExtension& /*aExt*/)
	{//!!!!don't write these outFile
	//CX509PolicyConstraintsExt* ext = CX509PolicyConstraintsExt::NewLC(aExt.Data());
	//TX509PolicyConstraint required = ext->ExplicitPolicyRequired();
	//TX509PolicyConstraint mapping = ext->InhibitPolicyMapping();
	//CleanupStack::PopAndDestroy();
	}

void X509CertWriter::ShowCP(const CX509CertExtension& aExt)
	{
	iOut.writeString(_L("Cert Policies = "));
	iOut.writeNewLine();

	CX509CertPoliciesExt* ext = CX509CertPoliciesExt::NewLC(aExt.Data());
	const CArrayPtrFlat<CX509CertPolicyInfo>& policies = ext->Policies();
	TInt count = policies.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509CertPolicyInfo* policy = policies.At(i);
		
		iOut.writeSpaces(4);
		iOut.writeString(_L("Policy ID = "));
		iOut.writeString(policy->Id());
		iOut.writeNewLine();
		const CArrayPtrFlat<CX509PolicyQualifierInfo>& qualifiers = policy->Qualifiers();
		TInt qCount = qualifiers.Count();

		if (qCount > 0)
			{

			iOut.writeSpaces(4);
			iOut.writeString(_L("Qualified by: "));
			iOut.writeNewLine();
			}

		for (TInt i = 0; i < qCount; i++)
			{
			CX509PolicyQualifierInfo* qualifier = qualifiers.At(i);

			iOut.writeSpaces(8);
			iOut.writeString(_L("Qualifier ID = "));
			iOut.writeString(qualifier->Id());
			iOut.writeNewLine();
			}
		}
	CleanupStack::PopAndDestroy();//ext
	}

void X509CertWriter::ShowPM(const CX509CertExtension& aExt)
	{//!!!!we don't write this one outFile either 
	CX509PolicyMappingExt* ext = CX509PolicyMappingExt::NewLC(aExt.Data());
	const CArrayPtrFlat<CX509PolicyMapping>& mappings = ext->Mappings();
	TInt count = mappings.Count();
	for (TInt i = 0; i < count; i++)
		{
		//const CX509PolicyMapping* mapping = mappings.At(i);
		}
	CleanupStack::PopAndDestroy();
	}


void X509CertWriter::ShowEKU(const CX509CertExtension& aExt)
	{
	iOut.writeString(_L("Extended Key Usage = "));
	iOut.writeNewLine();

	CX509ExtendedKeyUsageExt* ext = CX509ExtendedKeyUsageExt::NewLC(aExt.Data());
	const CArrayPtrFlat<HBufC>& usages = ext->KeyUsages();
	TInt count = usages.Count();
	for (TInt i = 0; i < count; i++)
		{
		iOut.writeSpaces(4);
		iOut.writeString(usages.At(i)->Des());
		iOut.writeNewLine();
		}
	CleanupStack::PopAndDestroy();
	}

void X509CertWriter::ShowSignature(const CX509Certificate& /*aCert*/)
	{
/*	HBufC8* sig = aCert.SignatureL();
	CleanupStack::PushL(sig);
	User::LeaveIfError(outFile.Write(sig->Des()));
	User::LeaveIfError(outFile.Flush()); // Commit data
	CleanupStack::PopAndDestroy();*/
	}

void X509CertWriter::WriteEncodings(const CX509Certificate& aCertificate)
	{
	iOut.writeString(_L("Version:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EVersionNumber);
	iOut.writeNewLine();

	iOut.writeString(_L("Serial Number:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESerialNumber);
	iOut.writeNewLine();

	iOut.writeString(_L("Algorithm:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EAlgorithmId);
	iOut.writeNewLine();

	iOut.writeString(_L("Issuer:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EIssuerName);
	iOut.writeNewLine();

	iOut.writeString(_L("Validity:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EValidityPeriod);
	iOut.writeNewLine();

	iOut.writeString(_L("Subject:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESubjectName);
	iOut.writeNewLine();

	iOut.writeString(_L("Public Key:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESubjectPublicKeyInfo);
	iOut.writeNewLine();

	iOut.writeString(_L("Issuer ID:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EIssuerUID);
	iOut.writeNewLine();
	
	iOut.writeString(_L("Subject ID:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESubjectUID);
	iOut.writeNewLine();

	iOut.writeString(_L("Extensions:"));
	iOut.writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EExtensionList);
	iOut.writeNewLine();
	}

void X509CertWriter::WriteEncoding(const CX509Certificate& aCertificate, const TUint aIndex)
	{
	if (aCertificate.DataElementEncoding(aIndex))
		{
		iOut.writeOctetString(*(aCertificate.DataElementEncoding(aIndex)));
		}
	}

#endif // SHOW_EXTENDED

/**********************************************************/

WTLSCertWriter::WTLSCertWriter(Output& aOut)
	:iOut(aOut)
	{
	}

void WTLSCertWriter::WriteCert(const CWTLSCertificate& aCertificate)
	{
	iOut.writeString(_L("\t\tIssuer Name = "));
	HBufC* issuer = aCertificate.IssuerL();
	iOut.writeString(*issuer);
	delete issuer;
	iOut.writeNewLine();
	iOut.writeString(_L("\t\tSubject Name = "));
	HBufC* subject = aCertificate.SubjectL();
	iOut.writeString(*subject);
	delete subject;
	iOut.writeNewLine();
	}

void WTLSCertWriter::ShowName(const CWTLSName& /*aName*/)
	{
//	iOut.writeString(aName.Name());
	}


