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
* TWTLSOUT.CPP
*
*/


#include "CertWriter.h"

WTLSCertWriter::WTLSCertWriter(Output* aOut)
	:iOut(aOut)
	{
	}

void WTLSCertWriter::SetOut(Output* aOut)
	{
	iOut = aOut;
	}

void WTLSCertWriter::WriteCert(const CWTLSCertificate& aCertificate)
	{
	CWTLSCertificate* cert = CWTLSCertificate::NewL(aCertificate);
	CleanupStack::PushL(cert);
	WriteEncodings(*cert);
	iOut->writeNewLine();
	iOut->writeString(_L("Version = "));
	iOut->writeNum(cert->Version());	
	iOut->writeNewLine();	
	ShowValidityPeriod(*cert);
	iOut->writeString(_L("Issuer Name = "));
	iOut->writeNewLine();	
	ShowName(cert->IssuerName());	
	iOut->writeNewLine();	
	iOut->writeString(_L("Subject Name = "));
	iOut->writeNewLine();	
	ShowName(cert->SubjectName());
	iOut->writeNewLine();	
	ShowSigningAlgorithm(cert->SigningAlgorithm());
	ShowPublicKey(aCertificate);

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

	iOut->writeString(_L("Full structured issuer & subject names"));
	iOut->writeNewLine();
	CWTLSStructuredText* sIssuer = NULL;
	TRAPD(err, sIssuer = CWTLSStructuredText::NewL(cert->IssuerName().NameData()));
	if (err != KErrNone)
		{
		if (err == KErrNoMemory)
			{
			User::Leave(err);
			}
		}
	else
		{
		const TWTLSStructuredTextField* field = sIssuer->FieldByName(KWTLSCountryName);
		if (field)
			{
			iOut->writeString(field->Type());
			iOut->writeString(_L(" = "));
			iOut->writeString(field->Value());
			iOut->writeNewLine();
			__ASSERT_ALWAYS((field->Value() == sIssuer->Country()),User::Panic(_L("CERT"),0));
			}

		field = sIssuer->FieldByName(KWTLSOrganizationName);
		if (field)
			{
			iOut->writeString(field->Type());
			iOut->writeString(_L(" = "));
			iOut->writeString(field->Value());
			iOut->writeNewLine();
			__ASSERT_ALWAYS((field->Value() == sIssuer->Organization()),User::Panic(_L("CERT"),0));
			}
		field = sIssuer->FieldByName(KWTLSServiceName);
		if (field)
			{
			iOut->writeString(field->Type());
			iOut->writeString(_L(" = "));
			iOut->writeString(field->Value());
			iOut->writeNewLine();
			__ASSERT_ALWAYS((field->Value() == sIssuer->ServiceName()),User::Panic(_L("CERT"),0));
			}
		field = sIssuer->FieldByName(KWTLSTitle);
		if (field)
			{
			iOut->writeString(field->Type());
			iOut->writeString(_L(" = "));
			iOut->writeString(field->Value());
			iOut->writeNewLine();
			}
		field = sIssuer->FieldByName(KWTLSCommonName);
		if (field)
			{
			iOut->writeString(field->Type());
			iOut->writeString(_L(" = "));
			iOut->writeString(field->Value());
			iOut->writeNewLine();
			}
		delete sIssuer;
		}

	CWTLSStructuredText* sSubject = NULL;
	TRAP(err, sSubject = CWTLSStructuredText::NewL(cert->SubjectName().NameData()));
	if (err != KErrNone)
		{
		if (err == KErrNoMemory)
			{
			User::Leave(err);
			}
		}
	else
		{
		TInt count = sSubject->Count();
		for (TInt i = 0; i < count; i++)
			{
			const TWTLSStructuredTextField& field = sSubject->FieldByIndex(i);
			iOut->writeString(field.Type());
			iOut->writeString(_L(" = "));
			iOut->writeString(field.Value());
			iOut->writeNewLine();
			}
		delete sSubject;
		}

	if (cert->IsSelfSignedL())
		{
		const CWTLSName& subject = cert->SubjectName();
		const CWTLSName& issuer = cert->IssuerName();
		if ((subject.NameType() == EWTLSText) && (issuer.NameType() == EWTLSText))
			{
			CWTLSText* sText = CWTLSText::NewLC(subject.NameData());
			CWTLSText* iText = CWTLSText::NewLC(issuer.NameData());
			__ASSERT_ALWAYS((sText->ExactMatchL(*iText)),User::Panic(_L("CERT"),0));
			CleanupStack::PopAndDestroy(2);
			}

		}


	CleanupStack::PopAndDestroy();
	}

void WTLSCertWriter::ShowName(const CWTLSName& aName)
	{
	if (aName.NameType() == EWTLSText)
		{
		CWTLSText* text = CWTLSText::NewL(aName.NameData());
		CleanupStack::PushL(text);
		iOut->writeString(_L("Char set = "));
		iOut->writeNum(text->CharacterSet());
		iOut->writeNewLine();
		iOut->writeString(_L("Text = "));
		iOut->writeString(text->Name());
		iOut->writeNewLine();	
		CleanupStack::PopAndDestroy();
		}
//	iOut->writeString(aName.Name());
//	iOut->writeNewLine();
	iOut->writeOctetStringL(aName.NameData());
	}

void WTLSCertWriter::ShowPublicKey(const CWTLSCertificate& aCertificate)
	{
	const CSubjectPublicKeyInfo& spki = aCertificate.PublicKey();
	switch(spki.AlgorithmId())
		{
		case ERSA:
			{
			ShowRSAKey(spki);
			break;
			}
		default:
			//ignore any other algorithm
			break;
		}
	}

void WTLSCertWriter::ShowRSAKey(const CSubjectPublicKeyInfo& aSpki)
	{
	CRSAPublicKey* key = CWTLSRSAPublicKey::NewLC(aSpki.KeyData());
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
	iOut->writeOctetString(eBuf->Des());
	iOut->writeNewLine();	
	CleanupStack::PopAndDestroy(3);
	}

void WTLSCertWriter::ShowSigningAlgorithm(const CSigningAlgorithmIdentifier& aSigningAlgorithm)
	{
	iOut->writeString(_L("Signed using: "));
	iOut->writeNewLine();
	iOut->writeSpaces(4);
	iOut->writeString(_L("Asymmetric algorithm = "));
	const CAlgorithmIdentifier& algId = aSigningAlgorithm.AsymmetricAlgorithm();
	switch(algId.Algorithm())
	{
	case ERSA:
		{
		iOut->writeString(_L("RSA"));
		break;
		}
	case EDSA:
		{
		iOut->writeString(_L("DSA"));
		break;
		}
	case EDH:
		{
		iOut->writeString(_L("DH"));
		break;
		}
	default:
		{
		iOut->writeString(_L("Unknown"));
		break;
		}
	}
	iOut->writeNewLine();
	iOut->writeSpaces(4);
	iOut->writeString(_L("Digest algorithm = "));
	const CAlgorithmIdentifier& digestId = aSigningAlgorithm.DigestAlgorithm();
	switch(digestId.Algorithm())
	{
	case EMD2:
		{
		iOut->writeString(_L("MD2"));
		break;
		}
	case EMD5:
		{
		iOut->writeString(_L("MD5"));
		break;
		}
	case ESHA1:
		{
		iOut->writeString(_L("SHA1"));
		break;
		}
	default:
		{
		iOut->writeString(_L("Unknown"));
		break;
		}
	}
	iOut->writeNewLine();
	}

void WTLSCertWriter::ShowSerialNumber(const TPtrC8& aSerialNumber)
	{
	iOut->writeString(_L("Serial Number = "));
	iOut->writeOctetString(aSerialNumber);
	iOut->writeNewLine();
	}

void WTLSCertWriter::ShowFingerprint(const CWTLSCertificate& aCertificate)
	{
	iOut->writeOctetString(aCertificate.Fingerprint());
	iOut->writeNewLine();
	}

void WTLSCertWriter::ShowValidityPeriod(const CWTLSCertificate& aCertificate)
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

void WTLSCertWriter::WriteEncodings(const CWTLSCertificate& aCertificate)
	{
	iOut->writeString(_L("Version:"));
	iOut->writeNewLine();
	const TPtrC8* encoding = Encoding(aCertificate, CWTLSCertificate::EVersionNumber);
	if (encoding)
		{
		iOut->writeOctetString(*encoding);
		}
	iOut->writeNewLine();

	iOut->writeString(_L("Algorithm:"));
	iOut->writeNewLine();
	encoding = Encoding(aCertificate, CWTLSCertificate::EAlgorithmId);
	if (encoding)
		{
		CWTLSSigningAlgorithmIdentifier* algId = CWTLSSigningAlgorithmIdentifier::NewL(*encoding);
		CleanupStack::PushL(algId);
		__ASSERT_ALWAYS((*algId==aCertificate.SigningAlgorithm()),User::Panic(_L("CERT"),0));
		iOut->writeOctetString(*encoding);
		CleanupStack::PopAndDestroy();
		}
	iOut->writeNewLine();

	iOut->writeString(_L("Issuer:"));
	iOut->writeNewLine();
	encoding = Encoding(aCertificate, CWTLSCertificate::EIssuerName);
	if (encoding)
		{
		CWTLSName* name = CWTLSName::NewL(*encoding);
		CleanupStack::PushL(name);
		__ASSERT_ALWAYS(	(name->ExactMatchL(aCertificate.IssuerName())	),User::Panic(_L("CERT"),0));
		iOut->writeOctetStringL(*encoding);
		CleanupStack::PopAndDestroy();
		}
	iOut->writeNewLine();

	iOut->writeString(_L("Validity:"));
	iOut->writeNewLine();
	encoding = Encoding(aCertificate, CWTLSCertificate::EValidityPeriod);
	if (encoding)
		{
		CValidityPeriod* val  = CWTLSValidityPeriod::NewL(*encoding);
		CleanupStack::PushL(val);
		__ASSERT_ALWAYS(((val->Start() == aCertificate.ValidityPeriod().Start()) && (val->Finish() == aCertificate.ValidityPeriod().Finish())),User::Panic(_L("CERT"),0));
		iOut->writeOctetString(*encoding);
		CleanupStack::PopAndDestroy();
		}
	iOut->writeNewLine();

	iOut->writeString(_L("Subject:"));
	iOut->writeNewLine();
	encoding = Encoding(aCertificate, CWTLSCertificate::ESubjectName);
	if (encoding)
		{
		CWTLSName* name = CWTLSName::NewL(*encoding);
		CleanupStack::PushL(name);
		__ASSERT_ALWAYS(	(name->ExactMatchL(aCertificate.SubjectName())	),User::Panic(_L("CERT"),0));
		iOut->writeOctetStringL(*encoding);
		CleanupStack::PopAndDestroy();
		}
	iOut->writeNewLine();

	iOut->writeString(_L("Public Key:"));
	iOut->writeNewLine();
	encoding = Encoding(aCertificate, CWTLSCertificate::ESubjectPublicKeyInfo);
	if (encoding)
		{
		CSubjectPublicKeyInfo* info  = CWTLSSubjectPublicKeyInfo::NewL(*encoding);
		CleanupStack::PushL(info);
		__ASSERT_ALWAYS((info->KeyData() == aCertificate.PublicKey().KeyData()),User::Panic(_L("CERT"),0));
		iOut->writeOctetStringL(*encoding);
		CleanupStack::PopAndDestroy();
		}
	iOut->writeNewLine();
	}

const TPtrC8* WTLSCertWriter::Encoding(const CWTLSCertificate& aCertificate, const TUint aIndex)
	{
	if (aCertificate.DataElementEncoding(aIndex))
		{
		return aCertificate.DataElementEncoding(aIndex);
		}
	return NULL;
	}

