/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <e32cons.h>

#include "keytool_utils.h"
#include "securityerr.h"
#include "compiledsecuritypolicy.h"

_LIT(space, " ");

_LIT(newline, "\n");
 
_LIT(KExtractable, "Extractable");
_LIT(KSensitive, "Sensitive");
_LIT(KAlwaysSensitive, "AlwaysSensitive");
_LIT(KNeverExtractable, "NeverExtractable");
_LIT(KLocal, "Local");
_LIT(KEllipsis, "...");
 
_LIT(x509, "X509");
_LIT(allusage, "AllUsage");
_LIT(nousage, "NoUsage");
_LIT(encrypt, "Encrypt");
_LIT(decrypt, "Decrypt");
_LIT(sign, "Sign");
_LIT(signrecover, "SignRecover");
_LIT(wrap, "Wrap");
_LIT(unwrap, "Unwrap");
_LIT(verify, "Verify");
_LIT(verifyrecover, "VerifyRecover");
_LIT(derive, "Derive");
_LIT(nonrepudiation, "NonRepudiation");

const TInt KCursorOffset		= 2;
static const TInt KMaxLabel = 32;
CConsoleBase* KeyToolUtils::iConsole = NULL;       
RFile* KeyToolUtils::iFile = NULL;

/*static*/ void KeyToolUtils::WriteErrorL(TInt aError)
	{
	
	if ((aError <= -12000) && (aError >= -12100))
		{
		PrintInfoL(_L("You have tsecdlg.dll in z:\\system\\notifiers."));		
		PrintInfoL(_L("This notifier is for *testing only* please remove it and try again!"));
		return;		
		}
	
	switch (aError)
		{
	case KErrNone:	// 0
		PrintInfoL(_L("KErrNone"));
		break;

	case KErrNotFound:	// -1
		PrintInfoL(_L("KErrNotFound"));
		break;

	case KErrNotSupported:	// -5
		PrintInfoL(_L("KErrNotSupported"));
		break;

	case KErrInUse:	// -14
		PrintInfoL(_L("KErrInUse"));
		break;

	case KErrNotReady:	// -18
		PrintInfoL(_L("KErrNotReady"));
		break;

	case KErrKeyAlgorithm:  //-11006
		PrintInfoL(_L("KErrKeyAlgorithm"));
		break;		

	case KErrKeyUsage:  //-11007
		PrintInfoL(_L("KErrKeyUsage"));
		break;
		
	case KErrKeyValidity:  //-11008
		PrintInfoL(_L("KErrKeyValidity"));
		break;		

	case KErrKeySize:  //-11009
		PrintInfoL(_L("KErrKeySize"));
		break;

	case KErrKeyAccess:  //-11010
		PrintInfoL(_L("KErrKeyAccess"));
		break;

	case KRequestPending:
		PrintInfoL(_L("KRequestPending"));
		break;

	case KErrAlreadyExists:
		PrintInfoL(_L("KErrAlreadyExists"));
		break;

	case KErrArgument:
		PrintInfoL(_L("KErrArgument"));
		break;

	case KErrBadName: // -28
		PrintInfoL(_L("KErrBadName"));
		break;

	case KErrCorrupt: // -20
		PrintInfoL(_L("KErrCorrupt"));
		break;		
	case KErrPrivateKeyNotFound: //-11011				
		PrintInfoL(_L("KErrPrivateKeyNotFound"));
		break;	

	default:
		{
		TBuf<64> msg;
		msg.Format(_L("Unknown (Code %d)"), aError);
		PrintInfoL(msg);		
		}
		break;
		}
	}
        
/*static*/ void KeyToolUtils::WriteOctetStringL(const TDesC8& aString)
	{
	TInt index;
   	TInt counter;
 	counter = aString.Length();
	HBufC* buf = HBufC::NewLC(counter*6);
	TPtr pBuf = buf->Des();
   	 for (index = 0; index < counter; index++)
		{
		pBuf.AppendFormat(_L("%01x"),aString[index]>>4);
		pBuf.AppendFormat(_L("%01x"),aString[index]&0xf);
		pBuf.Append(_L(" "));
		}
	PrintInfoL(pBuf);
	CleanupStack::PopAndDestroy();//buf
	}
	       
	        
/*static*/ void KeyToolUtils::PrintUsageL(TUint aUsage)
	{
	PrintInfoL(_L("\t\tUsage : "));
	if (aUsage & 0x0000ffff)  // In this range we are talking PKCS15
		{
		PrintInfoL(_L("PKCS15 "));
		}
	if (aUsage & 0xffff0000)  // In this range we are talking X509
		{
		PrintInfoL(x509);
		}
	if (aUsage == (TUint)EPKCS15UsageAll)
		{
		PrintInfoL(allusage);		
		return;
		}	
	if (aUsage == EPKCS15UsageNone)
		{
		PrintInfoL(nousage);		
		return;
		}	
	if (aUsage & EPKCS15UsageEncrypt)
		{
		PrintInfoL(encrypt);		
		}
	if (aUsage & EPKCS15UsageDecrypt)
		{
		PrintInfoL(decrypt);		
		}
	if (aUsage & EPKCS15UsageSign)
		{
		PrintInfoL(sign);		
		}
	if (aUsage & EPKCS15UsageSignRecover)
		{
		PrintInfoL(signrecover);		
		}
	if (aUsage & EPKCS15UsageWrap)
		{
		PrintInfoL(wrap);		
		}
	if (aUsage & EPKCS15UsageUnwrap)
		{
		PrintInfoL(unwrap);		
		}
	if (aUsage & EPKCS15UsageVerify)
		{
		PrintInfoL(verify);		
		}
	if (aUsage & EPKCS15UsageVerifyRecover)
		{
		PrintInfoL(verifyrecover);		
		}
	if (aUsage & EPKCS15UsageDerive)
		{
		PrintInfoL(derive);		
		}
	if (aUsage & EPKCS15UsageNonRepudiation)
		{
		PrintInfoL(nonrepudiation);		
		}
	if (aUsage & (TUint)EX509UsageDigitalSignature)
		{
		PrintInfoL(_L("DigitalSignature "));		
		}
	if (aUsage & EX509UsageNonRepudiation)
		{
		PrintInfoL(_L("NonRepudiation "));		
		}
	if (aUsage & EX509UsageKeyEncipherment)
		{
		PrintInfoL(_L("KeyEncipherment "));		
		}
	if (aUsage & EX509UsageDataEncipherment )
		{
		PrintInfoL(_L("DataEncipherment "));		
		}
	if (aUsage & EX509UsageKeyAgreement )
		{
		PrintInfoL(_L("KeyAgreement "));		
		}
	if (aUsage & EX509UsageKeyCertSign )
		{
		PrintInfoL(_L("KeyCertSign "));		
		}
	if (aUsage & EX509UsageCRLSign )
		{
		PrintInfoL(_L("CRLSign "));		
		}
	if (aUsage & EX509UsageEncipherOnly )
		{
		PrintInfoL(_L("EncipherOnly  "));		
		}
	if (aUsage & EX509UsageDecipherOnly)
		{
		PrintInfoL(_L("DecipherOnly  "));		
		}

	TBuf<32> buf;	
	_LIT(KKeyUsage, "\tCode: 0x%x ");
	buf.Format(KKeyUsage, aUsage);
	PrintInfoL(buf);
	PrintInfoL(newline);	
	}
	
	
/*static*/ HBufC* KeyToolUtils::KeyAccessDesLC(TInt aAccess)
	{
	TBuf<128> access;
	
	if (aAccess & CCTKeyInfo::ESensitive)
		{
		access.Append(KSensitive);
		access.Append(space);
		}

	if (aAccess & CCTKeyInfo::EExtractable)
		{ 
		access.Append(KExtractable);
		access.Append(space);		
		}

	if (aAccess & CCTKeyInfo::EAlwaysSensitive)
		{
		access.Append(KAlwaysSensitive);
		access.Append(space);
		}

	if (aAccess & CCTKeyInfo::ENeverExtractable)
		{ 
		access.Append(KNeverExtractable);
		access.Append(space);
		}

	if (aAccess & CCTKeyInfo::ELocal)	
		{ 
		access.Append(KLocal);
		access.Append(space);
		}
	return access.AllocLC();	
	}
	
	
/*static*/ HBufC* KeyToolUtils::AlgorithmDesLC(CCTKeyInfo::EKeyAlgorithm aAlgorithm)
	{
	TBuf<32> alg;

	switch (aAlgorithm)
		{
		case CCTKeyInfo::EInvalidAlgorithm:
			{
			alg = _L("Invalid Algorithm");
			}
			break;
		case CCTKeyInfo::ERSA:
			{
			alg = _L("RSA");
			}
			break;
		case CCTKeyInfo::EDSA:
			{
			alg = _L("DSA");
			}
			break;
		case CCTKeyInfo::EDH :
			{
			alg = _L("Diffie Hellman");
			}
			break;
		default:
			{
			alg = _L("Unsupported algorithm!");			
			}
		}	
	return alg.AllocLC();
	}


        
/*static*/ void KeyToolUtils::PrintKeyInfoL(const CCTKeyInfo& aKey, TBool aIsDetailed,TBool aPageWise)
	{
	HBufC* alg = KeyToolUtils::AlgorithmDesLC(aKey.Algorithm());
	TFileName buf;

	MCTToken& token = aKey.Token();
	TFileName msg,msg1;
	const TDesC& tmp = token.Label();
	msg.Format(_L("Store Label: %S"), &tmp);	
	
	const TDesC& tmp1 = token.Information(MCTToken::EVersion);
	const TDesC& tmp2 = token.Information(MCTToken::ESerialNo);
	const TDesC& tmp3 = token.Information(MCTToken::EManufacturer);

	msg1.Format(_L("Version: %S \t Serial Number: %S \t Manufacturer: %S Store Type: Read only"), &tmp1, &tmp2, &tmp3);	
	
	if (!aIsDetailed)
		{
		PrintInfoL(msg, aPageWise);
		PrintInfoL(newline, aPageWise);
		PrintInfoL(aKey.Label().Left(KMaxLabel));
		if (aKey.Label().Length() > KMaxLabel)
			{
			PrintInfoL(KEllipsis);
			}
		PrintInfoL(_L(" is a "));			
		_LIT(KKeySizeShort, "%d");
		buf.Format(KKeySizeShort,  aKey.Size());
		PrintInfoL(buf);
		PrintInfoL(_L(" bits "));
		PrintInfoL(*alg);
		PrintInfoL(_L(" key "));	
		CleanupStack::PopAndDestroy(alg);	
		PrintInfoL(newline, aPageWise);	
		return;
		}
	
	PrintInfoL(msg, aPageWise);
	PrintInfoL(newline, aPageWise);
	PrintInfoL(msg1, aPageWise);
	PrintInfoL(newline, aPageWise);

	PrintInfoL(_L("\t\tAlgorithm: "));	
	PrintInfoL(*alg);
	CleanupStack::PopAndDestroy(alg);			

	_LIT(KKeySize, "\tSize: %d ");
	buf.Format(KKeySize,  aKey.Size());
	PrintInfoL(buf);
	PrintInfoL(_L(" bits"));
	PrintInfoL(newline, aPageWise);

	KeyToolUtils::PrintUsageL(aKey.Usage());

	_LIT(KKeyOwner, "\t\tOwner : 0x%x ");
	_LIT(KKeyUser, "\t\tUser : 0x%x ");
	TCompiledSecurityPolicy managementPolicy = aKey.ManagementPolicy();
	TCompiledSecurityPolicy usePolicy = aKey.UsePolicy();
	
	TUint32 mSecureid = managementPolicy.SecureId();
	if (mSecureid == 0xffffffff)
		{
		TCapability cab = managementPolicy.Capability(0);
		_LIT(KKeyOwnerCab, "\t\tOwner : %S ");
		// currently only possible for keyowner other than secure id.
		if (cab == ECapabilityWriteDeviceData)
			{
			_LIT(KCab, "WriteDeviceData");
			buf.Format(KKeyOwnerCab, &KCab);
			PrintInfoL(buf);	
			}
		else
			{
			_LIT(KCab, "Unknown");
			buf.Format(KKeyOwnerCab, &KCab);
			PrintInfoL(buf);
			}
		}
	else
		{
		buf.Format(KKeyOwner, mSecureid);
		PrintInfoL(buf);		
		}
	PrintInfoL(newline, aPageWise);

	TUint32 uSecureid = usePolicy.SecureId();
	if (uSecureid == 0xffffffff)
		{
		TSecurityPolicy::TType utype = usePolicy.Type();
		_LIT(KKeyUserType, "\t\tUser : %S ");
		// currently only possible for keyuser other than secure id
		if (utype == TSecurityPolicy::ETypePass)
			{
			_LIT(KType, "ALL");
			buf.Format(KKeyUserType, &KType);
			PrintInfoL(buf);	
			}
		else
			{
			_LIT(KType, "Unknown");
			buf.Format(KKeyUserType, &KType);
			PrintInfoL(buf);
			}
		}
	else
		{
		buf.Format(KKeyUser, uSecureid);
		PrintInfoL(buf);
		if (uSecureid == KUidSecurityKeytool)
			{
			_LIT(KUser, " (KeyTool)");
			PrintInfoL(KUser);
			}
		}
	PrintInfoL(newline, aPageWise);


	_LIT(KKeySens, "\t\tAccess flags: ");
	PrintInfoL(KKeySens);
	
	HBufC* access = KeyToolUtils::KeyAccessDesLC(aKey.AccessType());
	PrintInfoL(*access);
	CleanupStack::PopAndDestroy(access);
	
	PrintInfoL(newline, aPageWise);

	_LIT(KKeyID, "\t\tID: ");
	PrintInfoL(KKeyID);
	KeyToolUtils::WriteOctetStringL(aKey.ID());
	PrintInfoL(newline, aPageWise);
				
	_LIT(KKeyLabel, "\t\tLabel: ");
	PrintInfoL(KKeyLabel);
	PrintInfoL(aKey.Label().Left(KMaxLabel));
	if (aKey.Label().Length() > KMaxLabel)
		{
		PrintInfoL(KEllipsis);
		}
	PrintInfoL(newline, aPageWise);

	_LIT(KKeyNative, "\t\tNative: Yes ");
	_LIT(KKeyNotNative, "\t\tNative: No ");
	if ( aKey.Native())
		{
		PrintInfoL(KKeyNative);	
		}
	else
		{
		PrintInfoL(KKeyNotNative);			
		}
	PrintInfoL(newline, aPageWise);

	_LIT(KTimeFormat, "%1/%2/%3 %H:%T:%S");
	 
	_LIT(KKeyStartDate, "\t\tStart date: ");
	PrintInfoL(KKeyStartDate);
	if (aKey.StartDate().Int64() == 0)
		{
		PrintInfoL(_L("not set"));		
		}
	else
		{
		aKey.StartDate().FormatL(buf, KTimeFormat);
		PrintInfoL(buf);
		}

	_LIT(KKeyEndDate, "\tEnd date: ");
	PrintInfoL(KKeyEndDate);
	
	if (aKey.EndDate().Int64() == 0)
		{
		PrintInfoL(_L("not set"));		
		}
	else
		{
		aKey.EndDate().FormatL(buf, KTimeFormat);
		PrintInfoL(buf);
		}
	PrintInfoL(newline, aPageWise);
	}
	
/*static*/ TKeyUsagePKCS15 KeyToolUtils::ParseKeyUsage(TPtrC aUsage)
	{
	TKeyUsagePKCS15 usage = EPKCS15UsageNone;

	if (aUsage.Compare(allusage)==0)
		{
		return EPKCS15UsageAll;
		}
	if (aUsage.Compare(nousage)==0)
		{
		return EPKCS15UsageNone;
		}
	if (aUsage.Compare(encrypt)==0)
		{
		return EPKCS15UsageEncrypt;
		}				
	if (aUsage.Compare(decrypt)==0)
		{
		return EPKCS15UsageDecrypt;
		}
	if (aUsage.Compare(sign)==0)
		{
		return EPKCS15UsageSign;
		}
	if (aUsage.Compare(signrecover)==0)
		{
		return EPKCS15UsageSignRecover;
		}
	if (aUsage.Compare(wrap)==0)
		{
		return EPKCS15UsageWrap;
		}							
	if (aUsage.Compare(unwrap)==0)
		{
		return EPKCS15UsageUnwrap;
		}							
	if (aUsage.Compare(verify)==0)
		{
		return EPKCS15UsageVerify;
		}	
	if (aUsage.Compare(verifyrecover)==0)
		{
		return EPKCS15UsageVerifyRecover;
		}	
	if (aUsage.Compare(derive)==0)
		{
		return EPKCS15UsageDerive;
		}	
	if (aUsage.Compare(nonrepudiation)==0)
		{
		return EPKCS15UsageNonRepudiation;
		}															
	return usage;
	}
	
/*static*/ CCTKeyInfo::EKeyAccess KeyToolUtils::ParseKeyAccess(TPtrC aAccess)
	{
	CKeyInfoBase::EKeyAccess access = CKeyInfoBase::EInvalidAccess;

	if (aAccess.Compare(KSensitive)==0)
		{
		access = CCTKeyInfo::ESensitive;
		}

	if (aAccess.Compare(KExtractable)==0)
		{ 
		access = CCTKeyInfo::EExtractable;
		}

	if (aAccess.Compare(KAlwaysSensitive)==0)
		{
		access = CCTKeyInfo::EAlwaysSensitive;
		}

	if (aAccess.Compare(KNeverExtractable)==0)
		{ 
		access = CCTKeyInfo::ENeverExtractable;
		}

	if (aAccess.Compare(KLocal)==0)	
		{ 
		access = CCTKeyInfo::ELocal;
		}
	
	return access;
	}


/*static*/ RArray<CCTKeyInfo*> KeyToolUtils::MatchKey(RMPointerArray<CCTKeyInfo>& aKeyList, TDesC& aLabel)
	{
	RArray<CCTKeyInfo*>	ret;
	
	for (TInt j = 0; j < aKeyList.Count(); j++)
		{
		if (aKeyList[j]->Label().Match(aLabel) != KErrNotFound) 
			{
			ret.Append(aKeyList[j]);
			}
		}
	return ret;
	}

/*static*/ CCTKeyInfo* KeyToolUtils::findKey(RMPointerArray<CCTKeyInfo>& aKeyList, TDesC& aLabel)
	{
	TInt keyIndex = -1;
	// Select the key with the label we want!
	for (TInt j = 0; j < aKeyList.Count(); j++)
		{
		if (aKeyList[j]->Label() == aLabel) 
			{
			keyIndex = j;
			break;
			}
		}

	if (keyIndex == -1)
		{
		return NULL;
		}
	return aKeyList[keyIndex];
	}

/*static*/ const TDesC& KeyToolUtils::Uid2Des(TUid aUid)
	{
	if (aUid.iUid == swinstalluid)
		{
		return KSWInstall; 
		}
	else if (aUid.iUid == swinstallocspuid) 
		{
		return KSWInstallOCSP;
		}
	else if (aUid.iUid == midletinstalluid)
		{
		return KMidletInstall;
		}
	else if (aUid.iUid ==  tlsuid) 
		{
		return KTls;
		}
  	else if (aUid.iUid ==  tocspuid) 
  		{
  		return KTOcsp;
  		}
  
 	return KUnknown;
	}	


/*static*/ void KeyToolUtils::FilterCertsL(RMPointerArray<CCTCertInfo>& aCertList, TDesC& aLabel)
  	{
  	TInt j = 0;
  	while (j < aCertList.Count())
  		{
  		if ( aCertList[j]->Label().Match(aLabel) == KErrNotFound) 
  			{
  			CCTCertInfo* tmp = aCertList[j];
  			tmp->Release();
  			aCertList.Remove(j);
  			}
  		else
  			{
  			j++;
  			}	
  		}
  	if (j ==0)	
  		{
  		PrintInfoL(_L("Cannot find the specified Certificate"));
  		PrintInfoL(newline);	
  		}
  	}

/*static*/ void KeyToolUtils::FilterCertsL(RMPointerArray<CCTCertInfo>& aCertList, TCertificateOwnerType& aOwnerType)
	{
	TInt j = 0;
  	while (j < aCertList.Count())
  		{
  		if ( aCertList[j]->CertificateOwnerType() != aOwnerType) 
  			{
  			CCTCertInfo* tmp = aCertList[j];
  			tmp->Release();
  			aCertList.Remove(j);
  			}
  		else
  			{
  			j++;
  			}	
  		}
  	if (j ==0)	
  		{
  		PrintInfoL(_L("No Certificate Exist"));	
  		PrintInfoL(newline);
  		}
	}

/*static*/ void KeyToolUtils::PrintCertInfoL(CCTCertInfo& aCertInfo, CCertificate& aCertificate,RArray<TUid> aApps, TBool aTrusted, TBool aIsDetailed,TBool aPageWise)
	{
	TFileName certFormat,ownerType;

	switch (aCertInfo.CertificateFormat())
		{
		case EX509Certificate :
			{
			certFormat.Copy(_L("X509"));
			}
			break;
		case EWTLSCertificate :
			{
			certFormat.Copy(_L("WTLS"));
			}
			break;
		case EX968Certificate :
			{
			certFormat.Copy(_L("X968"));
			}
			break;		
		default :
			{
			certFormat.Copy(_L("Unknown certificate format!"));
			}
		}	

	switch (aCertInfo.CertificateOwnerType())
		{
		case EPeerCertificate :
			{
			ownerType.Copy(_L("Peer"));	
			}
			break;
		case EUserCertificate :
			{
			ownerType.Copy(_L("User"));	
			}
			break;
		case ECACertificate :
			{
			ownerType.Copy(_L("Root (CA)"));	
			}
			break;
		default :
			{
			ownerType.Copy(_L("Unknown"));			
			}
		}
	
	MCTToken& token = aCertInfo.Token();
	TFileName msg,msg1;
	const TDesC& tmp = token.Label();
	msg.Format(_L("Store Label: %S"), &tmp);	
	
	const TDesC& tmp1 = token.Information(MCTToken::EVersion);
	const TDesC& tmp2 = token.Information(MCTToken::ESerialNo);
	const TDesC& tmp3 = token.Information(MCTToken::EManufacturer);

	msg1.Format(_L("Version: %S \t Serial Number: %S \t Manufacturer: %S "), &tmp1, &tmp2, &tmp3);	
						
	if (!aIsDetailed)
		{
		PrintInfoL(msg, aPageWise);
		PrintInfoL(newline, aPageWise);
		PrintInfoL(aCertInfo.Label());
		PrintInfoL(_L(" is a "));
		PrintInfoL(certFormat);
		PrintInfoL(_L(" format which is a "));
		PrintInfoL(ownerType);
		PrintInfoL(_L(" type "));
		PrintInfoL(newline, aPageWise);
		return;
		}

	PrintInfoL(msg, aPageWise);
	PrintInfoL(newline, aPageWise);		
	PrintInfoL(msg1, aPageWise);
	PrintInfoL(newline, aPageWise);		
	PrintInfoL(_L("\tLabel: "));
	PrintInfoL(aCertInfo.Label());

	PrintInfoL(_L("\t\t"));
	
	PrintInfoL(_L("Format: "));
	PrintInfoL(certFormat);
	PrintInfoL(_L("\t"));
	PrintInfoL(_L("Owner Type: "));	
	PrintInfoL(ownerType);
	PrintInfoL(newline, aPageWise);
	PrintInfoL(_L("\tIssuer Name: "));
	HBufC* issuer = aCertificate.IssuerL();
	PrintInfoL(*issuer);	
	delete issuer;
	HBufC* subj = aCertificate.SubjectL();
	PrintInfoL(_L("\t\tSubject Name: "));
	PrintInfoL(*subj);
	delete subj;

	PrintInfoL(newline, aPageWise);
	CValidityPeriod vp = aCertificate.ValidityPeriod();

	const TTime& start = vp.Start();
	const TTime& finish = vp.Finish();
	TBuf<30> dateString1;
	start.FormatL(dateString1,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));

	PrintInfoL(_L("\tValid From  "));
	PrintInfoL(dateString1, aPageWise);

	TBuf<30> dateString2;
	finish.FormatL(dateString2,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));

	PrintInfoL(_L("\tValid Until "));
	PrintInfoL(dateString2, aPageWise);
	PrintInfoL(newline, aPageWise);	
	PrintInfoL(_L("\tTrusted for Applications: "));
	for (TInt k = 0; k < aApps.Count(); k++)
		{
		PrintInfoL(Uid2Des(aApps[k]), aPageWise);
		PrintInfoL(_L("  "));
		}
	PrintInfoL(newline, aPageWise);
	PrintInfoL(_L("\tMarked as trusted: "));
	PrintInfoL( aTrusted ? _L("Yes"): _L("No"));
	PrintInfoL(newline, aPageWise);
	PrintInfoL(_L("\tMarked as Deletable : "));
	PrintInfoL( aCertInfo.IsDeletable() ? _L("Yes"): _L("No"));

	PrintInfoL(newline, aPageWise);
	}
	
void KeyToolUtils::SetConsole(CConsoleBase *aConsole )
	{
	iConsole = aConsole;
	}

void KeyToolUtils::SetFile(RFile* aFile)
	{
	iFile = aFile;
	}

TBool KeyToolUtils::DoesFileExistsL(const RFs& aFs, const TDesC& aFileName)
	{
	TBool exists = EFalse;
	TEntry* e = new(ELeave) TEntry();		
	TInt err = aFs.Entry(aFileName, *e);
	if (err == KErrNone && ! e->IsDir())
		{
		exists = ETrue;			
		}
	delete e;
	return exists;		
	}
	
/*static*/ void KeyToolUtils::PrintInfoL(const TDesC& aValue, TBool aPageWise)
	{
	if (iConsole)
		{
		TSize screenSize = iConsole->ScreenSize();
		if(aPageWise && (iConsole->WhereY() >= screenSize.iHeight - KCursorOffset))
			{
 			iConsole->Printf(_L("\nPress a key to continue..."));
			iConsole->Getch();
			iConsole->ClearScreen();
			iConsole->SetPos(0);
			}
		iConsole->Printf(aValue);	
		}
	else
		{
		TBuf8<256> buf8;
		buf8.Copy(aValue);
		TInt seekPos = 0;
		iFile->Seek(ESeekEnd, seekPos);
		User::LeaveIfError(iFile->Write(buf8));
		}
	}
	

