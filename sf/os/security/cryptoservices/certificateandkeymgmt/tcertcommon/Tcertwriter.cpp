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
*
*/


#include "tcertwriter.h"
#include <x509keys.h>
#include <asn1dec.h>

EXPORT_C CertWriter::CertWriter(Output* aOut)
	:iOut(aOut)
	{
	}

EXPORT_C void CertWriter::SetOut(Output* aOut)
	{
	iOut = aOut;
	}

EXPORT_C void CertWriter::WriteCert(const CX509Certificate& aCertificate)
	{

	CX509Certificate* cert = CX509Certificate::NewLC(aCertificate);
//	iOut->writeString(_L("\n Certificate Version = ")):
//	iOut->writeNum(cert->Version());
//	iOut.writeNewLine();
	WriteEncodings(*cert);
	iOut->writeNewLine();	
	ShowSerialNumber(cert->SerialNumber());
	ShowValidityPeriod(*cert);
	iOut->writeString(_L("Issuer Name = "));
	iOut->writeNewLine();	
	ShowDN(cert->IssuerName());	
	iOut->writeString(_L("Subject Name = "));
	iOut->writeNewLine();	
	ShowDN(cert->SubjectName());
	ShowSigningAlgorithm(cert->SigningAlgorithm());
	ShowExtensions(*cert);
	ShowPublicKey(*cert);
	iOut->writeNewLine();	
	ShowFingerprint(*cert);
	ShowSignature(*cert);

/*	CalculateSignature(*cert);
	if (IsSelfSigned(*cert))
		{
		iOut->writeString(_L("\nSelf-signed\n"));
		}
	else
		{
		iOut->writeString(_L("\nNot self-signed\n"));
		}
*/
	
	iOut->writeString(_L("Short Issuer Name = "));
	HBufC* issuer = cert->IssuerL();
	iOut->writeString(*issuer);
	iOut->writeNewLine();
	delete issuer;

	iOut->writeString(_L("Short Subject Name = "));
	HBufC* subject = cert->SubjectL();
	iOut->writeString(*subject);
	iOut->writeNewLine();
	delete subject;

	iOut->writeNewLine();
	CleanupStack::PopAndDestroy();//cert
	}

EXPORT_C void CertWriter::ShowPublicKey(const CX509Certificate& aCertificate)
	{
	const CSubjectPublicKeyInfo& spki = aCertificate.PublicKey();
	switch(spki.AlgorithmId())
		{
		case ERSA:
			ShowRSAKey(spki);
			break;
			
		case EDSA:
			ShowDSAKey(spki);
			break;
		
		case EDH:
			ShowDHKey(spki);
			break;
			
		default:
			User::Panic(_L("CertWriter"), 1);
			break;
		}
	}


void CertWriter::ShowRSAKey(const CSubjectPublicKeyInfo& aSpki)
	{
	CX509RSAPublicKey* key = CX509RSAPublicKey::NewLC(aSpki.KeyData());
	iOut->writeString(_L("RSA Key:"));
	iOut->writeNewLine();
	iOut->writeSpaces(4);
	iOut->writeString(_L("Modulus = "));
	iOut->writeNewLine();
	const TInteger& mod = key->N();
	HBufC8* mBuf = mod.BufferLC();
	iOut->writeOctetStringL(mBuf->Des());
	iOut->writeNewLine();	

	iOut->writeSpaces(4);
	iOut->writeString(_L("Exponent = "));
	iOut->writeNewLine();
	const TInteger& exp = key->E();
	HBufC8* eBuf = exp.BufferLC();
	iOut->writeOctetStringL(eBuf->Des());
	iOut->writeNewLine();	
	CleanupStack::PopAndDestroy(eBuf);
	CleanupStack::PopAndDestroy(mBuf);
	CleanupStack::PopAndDestroy(key);
	}

void CertWriter::ShowDSAKey(const CSubjectPublicKeyInfo& aSpki)
	{
	const TDesC8& encParams = aSpki.EncodedParams();
	if (encParams.Length() != 0)
		{
		CDSAPublicKey* key = CX509DSAPublicKey::NewLC(encParams, aSpki.KeyData());
		iOut->writeString(_L("DSA Key:"));
		iOut->writeNewLine();
		iOut->writeSpaces(4);
		iOut->writeString(_L("Y = "));
		iOut->writeNewLine();
		const TInteger& y = key->Y();
		HBufC8* yBuf = y.BufferLC();
		iOut->writeOctetStringL(yBuf->Des());
		iOut->writeNewLine();	

		iOut->writeSpaces(4);
		iOut->writeString(_L("Params = "));
		iOut->writeNewLine();

		iOut->writeSpaces(8);
		iOut->writeString(_L("P = "));
		iOut->writeNewLine();
		const TInteger& p = key->P();
		HBufC8* pBuf = p.BufferLC();
		iOut->writeOctetStringL(pBuf->Des());
		iOut->writeNewLine();

		iOut->writeSpaces(8);
		iOut->writeString(_L("Q = "));
		iOut->writeNewLine();
		const TInteger& q = key->Q();
		HBufC8* qBuf = q.BufferLC();
		iOut->writeOctetStringL(qBuf->Des());
		iOut->writeNewLine();

		iOut->writeSpaces(8);
		iOut->writeString(_L("G = "));
		iOut->writeNewLine();
		const TInteger& g = key->G();
		HBufC8* gBuf = g.BufferLC();
		iOut->writeOctetStringL(gBuf->Des());
		iOut->writeNewLine();

		CleanupStack::PopAndDestroy(5);
		}
	}

void CertWriter::ShowDHKey(const CSubjectPublicKeyInfo& aSpki)
	{
	TASN1DecInteger encInt;
	TInt pos = 0;
	RInteger keyVal = encInt.DecodeDERLongL(aSpki.KeyData(), pos);
	CleanupStack::PushL(keyVal);
	HBufC8* keyValBuf = keyVal.BufferLC();
	iOut->writeString(_L("Y = "));
	iOut->writeNewLine();
	iOut->writeOctetStringL(keyValBuf->Des());
	CleanupStack::PopAndDestroy(2, &keyVal);

	CX509DHDomainParams* params = CX509DHDomainParams::NewLC(aSpki.EncodedParams());
	const TInteger& p = params->P();
	const TInteger& g = params->G();
	const TInteger& q = params->Q();
	const TInteger& j = params->J();
	const CX509DHValidationParams* valParams = params->ValidationParams();

	iOut->writeNewLine();
	iOut->writeString(_L("Params = "));
	iOut->writeNewLine();

	iOut->writeString(_L("P = "));
	iOut->writeNewLine();
	HBufC8* pBuf = p.BufferLC();
	iOut->writeOctetStringL(pBuf->Des());
	iOut->writeNewLine();
	CleanupStack::PopAndDestroy();

	iOut->writeString(_L("G = "));
	iOut->writeNewLine();
	HBufC8* gBuf = g.BufferLC();
	iOut->writeOctetStringL(gBuf->Des());
	iOut->writeNewLine();
	CleanupStack::PopAndDestroy();

	iOut->writeString(_L("Q = "));
	iOut->writeNewLine();
	HBufC8* qBuf = q.BufferLC();
	iOut->writeOctetStringL(qBuf->Des());
	iOut->writeNewLine();
	CleanupStack::PopAndDestroy();

	iOut->writeString(_L("J = "));
	iOut->writeNewLine();
	HBufC8* jBuf = j.BufferLC();
	iOut->writeOctetStringL(jBuf->Des());
	iOut->writeNewLine();
	CleanupStack::PopAndDestroy();
		
	if (valParams)
		{
		iOut->writeString(_L("Seed = "));
		iOut->writeNewLine();
		iOut->writeOctetStringL(valParams->Seed());
		iOut->writeNewLine();
		const TInteger& pGC = valParams->PGenCounter();
		HBufC8* pgBuf = pGC.BufferLC();
		iOut->writeString(_L("pGenCounter = "));
		iOut->writeNewLine();
		iOut->writeOctetStringL(pgBuf->Des());
		iOut->writeNewLine();
		CleanupStack::PopAndDestroy();
		}
	CleanupStack::PopAndDestroy();
	}


EXPORT_C void CertWriter::ShowSigningAlgorithm(const CSigningAlgorithmIdentifier& aSigningAlgorithm)
	{
	iOut->writeString(_L("Signed using: "));
	iOut->writeNewLine();
	iOut->writeSpaces(4);
	iOut->writeString(_L("Asymmetric algorithm = "));
	const CAlgorithmIdentifier& algId = aSigningAlgorithm.AsymmetricAlgorithm();
	switch(algId.Algorithm())
		{
		case ERSA:
			iOut->writeString(_L("RSA"));
			break;
		
		case EDSA:
			iOut->writeString(_L("DSA"));
			break;
		
		case EDH:
			iOut->writeString(_L("DH"));
			break;
						
		default:
			User::Panic(_L("CertWriter"), 1);
			break;
		}
	iOut->writeNewLine();
	iOut->writeSpaces(4);
	iOut->writeString(_L("Digest algorithm = "));
	const CAlgorithmIdentifier& digestId = aSigningAlgorithm.DigestAlgorithm();
	switch(digestId.Algorithm())
		{
		case EMD2:
			iOut->writeString(_L("MD2"));
			break;
		
		case EMD5:
			iOut->writeString(_L("MD5"));
			break;
		
		case ESHA1:
			iOut->writeString(_L("SHA1"));
			break;
		
		default:
			User::Panic(_L("CertWriter"), 1);
			break;
		}
	iOut->writeNewLine();
	}

EXPORT_C void CertWriter::ShowSerialNumber(const TPtrC8& aSerialNumber)
	{
	iOut->writeString(_L("Serial Number = "));
	iOut->writeOctetStringL(aSerialNumber);
	iOut->writeNewLine();
	}

//dn display code
EXPORT_C void CertWriter::ShowAVA(const CX520AttributeTypeAndValue& aAva)
	{
	iOut->writeString(aAva.Type());
	HBufC* val = aAva.ValueL();
	CleanupStack::PushL(val);
	iOut->writeString(_L(" = "));
	iOut->writeString(val->Des());
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CertWriter::ShowDN(const CX500DistinguishedName& aName)
	{
	iOut->writeNewLine();
	TInt count = aName.Count();
	for (TInt i = 0; i < count; i++)
		{
  		const CX520AttributeTypeAndValue& ava = aName.Element(i);
		iOut->writeSpaces(4);
		ShowAVA(ava);
		iOut->writeNewLine();
		}
	}

EXPORT_C void CertWriter::ShowAKI(const CX509Certificate& aCert)
	{
	const CX509CertExtension* akiExt = aCert.Extension(KAuthorityKeyId);
	if (akiExt)
		{
		iOut->writeString(_L("Authority Key ID = "));
		iOut->writeNewLine();
		const CX509AuthorityKeyIdExt* ext = CX509AuthorityKeyIdExt::NewLC(akiExt->Data());
		const CArrayPtrFlat<CX509GeneralName>& authorityName = ext->AuthorityName();
		TInt count = authorityName.Count();
		if (count > 0)
			{
			iOut->writeSpaces(4);
			iOut->writeString(_L("Authority name = "));
			iOut->writeNewLine();
			for (TInt i = 0; i < count; i++)
				{
				ShowGN(*(authorityName.At(i)));
				}
			}
		if (ext->AuthorityCertSerialNumber().Length() > 0)
			{
			iOut->writeSpaces(4);
			iOut->writeString(_L("Authority cert serial no = "));
			iOut->writeOctetStringL(ext->AuthorityCertSerialNumber());
			iOut->writeNewLine();
			}
		iOut->writeSpaces(4);
		iOut->writeString(_L("Key Id = "));
		iOut->writeOctetStringL(ext->KeyId());
		iOut->writeNewLine();
		CleanupStack::PopAndDestroy();
		}
	}

EXPORT_C void CertWriter::ShowSKI(const CX509Certificate& aCert)
	{
	const CX509CertExtension* skiExt = aCert.Extension(KSubjectKeyId);
	if (skiExt)
		{
		iOut->writeString(_L("Subject Key ID = "));
		iOut->writeNewLine();
		const CX509SubjectKeyIdExt* ext = CX509SubjectKeyIdExt::NewLC(skiExt->Data());
		iOut->writeSpaces(4);
		iOut->writeString(_L("Key Id = "));
		iOut->writeOctetStringL(ext->KeyId());
		iOut->writeNewLine();
		CleanupStack::PopAndDestroy();
		}
	}

EXPORT_C void CertWriter::ShowGN(const CX509GeneralName& aName)
	{
	iOut->writeSpaces(4);
	switch(aName.Tag())
		{
		case 1:
			{
			//rfc822
			CX509RFC822NameSubtree* email = CX509RFC822NameSubtree::NewLC(aName.Data());
			iOut->writeString(email->Name());
			iOut->writeNewLine();
			CleanupStack::PopAndDestroy();//email
			break;
			}
		case 2:
			{
			//dns name
			CX509DNSNameSubtree* dns = CX509DNSNameSubtree::NewLC(aName.Data());
			iOut->writeString(dns->Name());
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
			iOut->writeString(uri->Name());
			iOut->writeNewLine();
			iOut->writeString(_L("Host="));
			iOut->writeString(uri->Host().Name());
			iOut->writeNewLine();
			CleanupStack::PopAndDestroy();//uri
			break;
			}
		case 7:
			{
			CX509IPAddress* ip = CX509IPAddress::NewLC(aName.Data());
			TPtrC8 addressStr(ip->Address());
			// IPv6 output not implemented yet
			iOut->write(_L("IP=%d.%d.%d.%d"), addressStr[0], addressStr[1], addressStr[2], addressStr[3]);
			iOut->writeNewLine();			
			CleanupStack::PopAndDestroy();
			break;
			}
		}
	}


EXPORT_C void CertWriter::ShowFingerprint(const CX509Certificate& aCertificate)
	{
	iOut->writeString(_L("Fingerprint:"));
	iOut->writeNewLine();
	iOut->writeOctetString(aCertificate.Fingerprint());
	iOut->writeNewLine();
	}

EXPORT_C void CertWriter::ShowValidityPeriod(const CX509Certificate& aCertificate)
	{
	const CValidityPeriod& vp = aCertificate.ValidityPeriod();
	iOut->writeString(_L("Validity Period = "));
	iOut->writeNewLine();
	const TTime& start = vp.Start();
	const TTime& finish = vp.Finish();
	TBuf<30> dateString1;
	start.FormatL(dateString1,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));

	iOut->writeSpaces(4);
	iOut->writeString(_L("Valid From = "));
	iOut->writeString(dateString1);
	iOut->writeNewLine();

	TBuf<30> dateString2;
	finish.FormatL(dateString2,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));

	iOut->writeSpaces(4);
	iOut->writeString(_L("Valid Until = "));

	iOut->writeString(dateString2);
	iOut->writeNewLine();
	}

//extensions
EXPORT_C void CertWriter::ShowExtensions(const CX509Certificate& aCertificate)
	{
	const CArrayPtrFlat<CX509CertExtension>& exts = aCertificate.Extensions();
	TInt count = exts.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509CertExtension* ext = exts.At(i);
		iOut->writeString(_L("extension"));
		iOut->writeNum(i);
		if (!(ext->Critical()))
			{
			iOut->writeString(_L(" not"));
			}
		iOut->writeString(_L(" critical"));
		iOut->writeString(_L(" ID = "));
		TPtrC id(ext->Id());
		iOut->writeString(id);
		iOut->writeNewLine();
		if (id == KBasicConstraints)
			{
			ShowBC(*ext);
			}
		if (id == KSubjectAltName)
			{
			iOut->writeString(_L("SubjectAltName: "));
			iOut->writeNewLine();
			ShowAltName(*ext);
			}
		if (id == KIssuerAltName)
			{
			iOut->writeString(_L("IssuerAltName: "));
			iOut->writeNewLine();
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

EXPORT_C void CertWriter::ShowBC(const CX509CertExtension& aExt)
	{
	iOut->writeString(_L("Basic Constraints:"));
	iOut->writeNewLine();
	CX509BasicConstraintsExt* ext = CX509BasicConstraintsExt::NewLC(aExt.Data());
	if (ext->IsCA())
		{
		iOut->writeSpaces(4);
		iOut->writeString(_L("CA cert"));
		iOut->writeNewLine();
		if (ext->MaxChainLength() < KMaxTInt)
			{
			iOut->writeSpaces(4);
			iOut->writeString(_L("Max Chain Length = "));
			iOut->writeNum(ext->MaxChainLength());
			iOut->writeNewLine();
			}
		}
	else
		{
		iOut->writeSpaces(4);
		iOut->writeString(_L("EE cert"));
		iOut->writeNewLine();
		}
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CertWriter::ShowAltName(const CX509CertExtension& aExt)
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

EXPORT_C void CertWriter::ShowKU(const CX509CertExtension& aExt)
	{
	iOut->writeString(_L("Key Usage:"));
	iOut->writeNewLine();
	CX509KeyUsageExt* ext = CX509KeyUsageExt::NewLC(aExt.Data());
		if (ext->IsSet(EX509DigitalSignature))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KDigSig);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509NonRepudiation))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KNonRep);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509KeyEncipherment))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KKeyEnc);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509DataEncipherment))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KDataEnc);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509KeyAgreement))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KKeyAgree);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509KeyCertSign))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KCertSign);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509CRLSign))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KCRLSign);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509EncipherOnly))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KEnciph);
			iOut->writeNewLine();
			}
		if (ext->IsSet(EX509DecipherOnly))
			{
			iOut->writeSpaces(4);
			iOut->writeString(KDeciph);
			iOut->writeNewLine();
			}		
	CleanupStack::PopAndDestroy();//ext
	}

EXPORT_C void CertWriter::ShowSubtrees(const CArrayPtrFlat<CX509GeneralSubtree>& aSubtrees)
	{
	TInt count = aSubtrees.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509GeneralSubtree* subtree = aSubtrees.At(i);
		ShowGN(subtree->Name());
		///!!!!!don't write these outFile either!!
		}
	}

EXPORT_C void CertWriter::ShowNC(const CX509CertExtension& aExt)
	{//!!!!don't write these outFile

	CX509NameConstraintsExt* ext = CX509NameConstraintsExt::NewLC(aExt.Data());
	ShowSubtrees(ext->ExcludedSubtrees());
	ShowSubtrees(ext->PermittedSubtrees());
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CertWriter::ShowPC(const CX509CertExtension& /*aExt*/)
	{//!!!!don't write these outFile
//	CX509PolicyConstraintsExt* ext = CX509PolicyConstraintsExt::NewLC(aExt.Data());
//	TX509PolicyConstraint required = ext->ExplicitPolicyRequired();
//	TX509PolicyConstraint mapping = ext->InhibitPolicyMapping();
//	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CertWriter::ShowCP(const CX509CertExtension& aExt)
	{
	iOut->writeString(_L("Cert Policies = "));
	iOut->writeNewLine();

	CX509CertPoliciesExt* ext = CX509CertPoliciesExt::NewLC(aExt.Data());
	const CArrayPtrFlat<CX509CertPolicyInfo>& policies = ext->Policies();
	TInt count = policies.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509CertPolicyInfo* policy = policies.At(i);
		
		iOut->writeSpaces(4);
		iOut->writeString(_L("Policy ID = "));
		iOut->writeString(policy->Id());
		iOut->writeNewLine();
		const CArrayPtrFlat<CX509PolicyQualifierInfo>& qualifiers = policy->Qualifiers();
		TInt qCount = qualifiers.Count();

		if (qCount > 0)
			{

			iOut->writeSpaces(4);
			iOut->writeString(_L("Qualified by: "));
			iOut->writeNewLine();
			}

		for (TInt i = 0; i < qCount; i++)
			{
			iOut->writeSpaces(8);
			iOut->writeString(_L("Qualifier ID = "));
			iOut->writeString(policy->Id());
			iOut->writeNewLine();
			}
		}
	CleanupStack::PopAndDestroy();//ext
	}

EXPORT_C void CertWriter::ShowPM(const CX509CertExtension& aExt)
	{
	iOut->writeString(_L("Policy Mappings = "));
	CX509PolicyMappingExt* ext = CX509PolicyMappingExt::NewLC(aExt.Data());
	const CArrayPtrFlat<CX509PolicyMapping>& mappings = ext->Mappings();
	TInt count = mappings.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509PolicyMapping* mapping = mappings.At(i);
		iOut->writeString(_L("Issuer Policy = "));
		iOut->writeString(mapping->IssuerPolicy());
		iOut->writeNewLine();
		iOut->writeString(_L("Subject Policy = "));
		iOut->writeString(mapping->SubjectPolicy());
		iOut->writeNewLine();
		}
	iOut->writeNewLine();
	CleanupStack::PopAndDestroy();
	}


EXPORT_C void CertWriter::ShowEKU(const CX509CertExtension& aExt)
	{
	iOut->writeString(_L("Extended Key Usage = "));
	iOut->writeNewLine();

	CX509ExtendedKeyUsageExt* ext = CX509ExtendedKeyUsageExt::NewLC(aExt.Data());
	const CArrayPtrFlat<HBufC>& usages = ext->KeyUsages();
	TInt count = usages.Count();
	for (TInt i = 0; i < count; i++)
		{
		iOut->writeSpaces(4);
		iOut->writeString(usages.At(i)->Des());
		iOut->writeNewLine();
		}
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CertWriter::ShowSignature(const CX509Certificate& aCert)
	{
	iOut->writeString(_L("Signature:"));
	iOut->writeNewLine();
	const TPtrC8 sig = aCert.Signature();
	iOut->writeOctetStringL(sig);
	iOut->writeNewLine();
	}

void CertWriter::CalculateSignature(const CX509Certificate& aCert)
	{
	TBool res = EFalse;;
	TRAPD(err, res = aCert.VerifySignatureL(aCert.PublicKey().KeyData()));
	if (err != KErrNone)
		{
		iOut->writeString(_L("\nsignature verification could not be performed\n"));
		if (err != KErrArgument)
			{
			User::Leave(err);
			}
		}
	else
		{
		if (res)
			{
			iOut->writeString(_L("\nself-signed\n"));
			}
		else
			{
			iOut->writeString(_L("\n not self-signed\n"));
			}
		}
	}

EXPORT_C TBool CertWriter::IsSelfSigned(const CX509Certificate& aCert)
	{
	TBool res = EFalse;
	const CX500DistinguishedName& subject = aCert.SubjectName();
	if (subject.Count() > 0)
		{
		res = subject.ExactMatchL(aCert.IssuerName());
		}
	else
		{
		const CX509CertExtension* subjectExt = aCert.Extension(KIssuerAltName);
		const CX509CertExtension* issuerExt = aCert.Extension(KSubjectAltName);
		if ((!subjectExt) || (!issuerExt))
			{
			}
		else
			{
			const CX509AltNameExt* issuerAltName = CX509AltNameExt::NewLC(subjectExt->Data());
			const CX509AltNameExt* subjectAltName = CX509AltNameExt::NewLC(issuerExt->Data());
			if (subjectAltName->Match(*issuerAltName))
				{
				res = ETrue;
				}
			CleanupStack::PopAndDestroy(2);//subjectAltName, issuerAltName
			}		
		}
	return res;
	}
/*		EVersionNumber = 0,
		ESerialNumber = 1,
		EAlgorithmId = 2,
		EIssuerName = 3,
		EValidityPeriod = 4,
		ESubjectName = 5,
		ESubjectPublicKeyInfo = 6,
		EIssuerUID = 7,
		ESubjectUID = 8,
		EExtensionList = 9
*/
void CertWriter::WriteEncodings(const CX509Certificate& aCertificate)
	{
	iOut->writeString(_L("Version:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EVersionNumber);
	iOut->writeNewLine();

	iOut->writeString(_L("Serial Number:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESerialNumber);
	iOut->writeNewLine();

	iOut->writeString(_L("Algorithm:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EAlgorithmId);
	iOut->writeNewLine();

	iOut->writeString(_L("Issuer:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EIssuerName);
	iOut->writeNewLine();

	iOut->writeString(_L("Validity:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EValidityPeriod);
	iOut->writeNewLine();

	iOut->writeString(_L("Subject:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESubjectName);
	iOut->writeNewLine();

	iOut->writeString(_L("Public Key:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESubjectPublicKeyInfo);
	iOut->writeNewLine();

	iOut->writeString(_L("Issuer ID:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EIssuerUID);
	iOut->writeNewLine();
	
	iOut->writeString(_L("Subject ID:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::ESubjectUID);
	iOut->writeNewLine();

	iOut->writeString(_L("Extensions:"));
	iOut->writeNewLine();
	WriteEncoding(aCertificate, CX509Certificate::EExtensionList);
	iOut->writeNewLine();
	}

void CertWriter::WriteEncoding(const CX509Certificate& aCertificate, const TUint aIndex)
	{
	if (aCertificate.DataElementEncoding(aIndex))
		{
		iOut->writeOctetStringL(*(aCertificate.DataElementEncoding(aIndex)));
		}
	}
