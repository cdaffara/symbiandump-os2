/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32uid.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <x509keys.h>
#include <securityerr.h>

#ifdef SYMBIAN_AUTH_SERVER
#include <keystore_errs.h>
#include <authserver/auth_srv_errs.h>
#include <authserver/authclient.h>
#endif // SYMBIAN_AUTH_SERVER

static void TestKeyStorePanic(TKSPanicCode aPanicCode)
{
	_LIT(KTestKeyStorePanic, "TKeyStorePanic");
	User::Panic(KTestKeyStorePanic, aPanicCode);
}


//	CSharedKeyStores static singleton which maintains unfied key store instances

/*static*/ CSharedKeyStores* CSharedKeyStores::iTheKeyStores = NULL;


/*static*/ void CSharedKeyStores::InitialiseKeyStoresL()
{
	if (!iTheKeyStores)
	{
		iTheKeyStores = new (ELeave) CSharedKeyStores();
	}
}

/*static*/ void CSharedKeyStores::DestroyKeyStores()
{
	if (iTheKeyStores)
	{	
		delete iTheKeyStores;
		iTheKeyStores = NULL;
	}
}

/*static*/ RUnifiedKeyStoreArray& CSharedKeyStores::TheUnifiedKeyStores()
{
	return (iTheKeyStores->iUnifiedKeyStores);
}

CSharedKeyStores::~CSharedKeyStores()
{
	iUnifiedKeyStores.ResetAndDestroy();
	iUnifiedKeyStores.Close();
}


/////////////////////////////////////////////////////////////////////////////////
//CKeyStoreTestAction base class
/////////////////////////////////////////////////////////////////////////////////
CKeyStoreTestAction::~CKeyStoreTestAction()
	{
	delete iLabel;
	iExpectedDialogs.ResetAndDestroy();
	iKeys.Close();
#ifdef SYMBIAN_AUTH_SERVER	
	delete iAuthExpression;
#endif // SYMBIAN_AUTH_SERVER

	}


CKeyStoreTestAction::CKeyStoreTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CTestAction(aConsole, aOut), 
	iFs(aFs),
	iDisableCheckDialog(0),
	iKeyStoreImplIndex(0)
#ifdef SYMBIAN_AUTH_SERVER
	,iUseNewApi(EFalse),
	iFreshness(0)
#endif // SYMBIAN_AUTH_SERVER
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	,iHardwareType(-1)
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
	{
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
	}

void CKeyStoreTestAction::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	
	
	iExpectedResult = SetExpectedResultL(Input::ParseElement(aTestActionSpec.iActionResult, KReturnStart));
	TInt pos = 0;
	
	while (SetKeyUsageL(Input::ParseElement(aTestActionSpec.iActionBody, KKeyUsageStart, KKeyUsageEnd, pos)))
		/* do nothing */;
	
	SetKeyLabel(Input::ParseElement(aTestActionSpec.iActionBody, KKeyLabelStart));
	SetKeyAlgorithm(Input::ParseElement(aTestActionSpec.iActionBody, KKeyAlgorithmStart));
	SetKeyType(Input::ParseElement(aTestActionSpec.iActionBody, KOpenStart, KOpenEnd));
	
	pos = 0;
	while (SetKeyAccessType(Input::ParseElement(aTestActionSpec.iActionBody, KKeyAccessTypeStart, KKeyAccessTypeEnd, pos)))
		/* do nothing */;
	pos = 0;
	while (AddExpectedDialogL(Input::ParseElement(aTestActionSpec.iActionBody, KSecDlg, KSecDlgEnd, pos)))
		/* do nothing */;

	iKeystore = Input::ParseIntElement(aTestActionSpec.iActionBody, KKeystoreStart, KKeystoreEnd);
	
	iDisableCheckDialog = Input::ParseIntElement(aTestActionSpec.iActionBody, KDisableDialogStart, KDisableDialogEnd, pos);

	pos = 0;
	iKeyStoreImplLabel.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KUseKeyStoreStart, KUseKeyStoreEnd, pos));
	
#ifdef SYMBIAN_AUTH_SERVER
	TPtrC8 authExpr;
	TInt authExprPresent = KErrNone;
	authExpr.Set(Input::ParseElement(aTestActionSpec.iActionBody, KAuthExpressionStart, KAuthExpressionEnd, pos , authExprPresent));
	if(authExpr.Compare(_L8("null")) != 0)
		{
		TInt size = authExpr.Size();
		if (size != 0)
			{
			iAuthExpression = HBufC::NewMaxL(size);
			TPtr authExp(iAuthExpression->Des());
			authExp.FillZ();
			authExp.Copy(authExpr);
			}
		}
	TInt freshnessPresent = KErrNone;
	TInt negativeFreshness = Input::ParseIntElement(aTestActionSpec.iActionBody, KNegativeFreshnessStart, KNegativeFreshnessEnd);
	if(negativeFreshness == 1)
		{
		iFreshness = -1;
		}
	else
		{
		iFreshness = Input::ParseIntElement(aTestActionSpec.iActionBody, KFreshnessStart, KFreshnessEnd, pos, freshnessPresent);
		}
	if(authExprPresent == KErrNone && freshnessPresent == KErrNone)
		{
		iUseNewApi = ETrue;
		}
	
	iDeauthenticate = Input::ParseIntElement(aTestActionSpec.iActionBody, KAuthenticateStart, KAuthenticateEnd);
		
#endif // SYMBIAN_AUTH_SERVER
	
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	iHardwareType = Input::ParseIntElement(aTestActionSpec.iActionBody, KHwTypeStart, KHwTypeEnd, pos);
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
	
	}

TBool CKeyStoreTestAction::AddExpectedDialogL(const TDesC8& aData)
	{
	if (aData.Length() == 0)
		return EFalse;

	HBufC* data = HBufC::NewMaxLC(aData.Length());
	TPtr ptr = data->Des();
	ptr.Copy(aData);
	User::LeaveIfError(iExpectedDialogs.Append(data));
	CleanupStack::Pop(data);
	return ETrue;
	}

void CKeyStoreTestAction::WriteExpectedDialogDataL()
	{
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	
	TBuf<128> fileName (sysDriveName);
	fileName.Append(KTSecDlgOutputFile);
	TInt err = fileMan->Delete(fileName);
	if (err != KErrNotFound)
		User::LeaveIfError(err);
	
	fileName.Copy (sysDriveName);
	fileName.Append(KTSecDlgInputFile);
	err = fileMan->Delete(fileName);
	if (err != KErrNotFound)
		User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(fileMan);
	
	RFileWriteStream stream;
	err = stream.Open(iFs, fileName, EFileWrite | EFileShareExclusive);
	if (err == KErrNotFound)
		{
		err = stream.Create(iFs, fileName, EFileWrite | EFileShareExclusive);
		}
	User::LeaveIfError(err);
	stream.PushL();
	
	for (TInt index = 0 ; index < iExpectedDialogs.Count() ; ++index)
		{		
		TLex lex(*iExpectedDialogs[index]);
		TPtrC token(lex.NextToken());

		if (token.Length() == 0)
			User::Leave(KErrCorrupt);
	
		if (token == KSecDlgImport)
			{
			TPtrC passphrase(lex.NextToken());
			if (passphrase.Length() == 0)
				User::Leave(KErrCorrupt);
			WriteDialogRecordL(stream, EEnterPIN, _L("Passphrase of the imported key file"), passphrase, KNullDesC);
			}
		else if (token == KSecDlgExport)
			{
			TPtrC passphrase(lex.NextToken());
			if (passphrase.Length() == 0)
				User::Leave(KErrCorrupt);
			WriteDialogRecordL(stream, EEnterPIN, _L("Passphrase of the exported key file"), passphrase, KNullDesC);			
			}
		else if (token == KSecDlgChange)
			{
			TPtrC oldPassphrase(lex.NextToken());
			if (oldPassphrase.Length() == 0)
				User::Leave(KErrCorrupt);
			TPtrC newPassphrase(lex.NextToken());
			if (newPassphrase.Length() == 0)
				User::Leave(KErrCorrupt);
			WriteDialogRecordL(stream, EChangePIN, KNullDesC, oldPassphrase, newPassphrase);
			}
		else if (token == KSecDlgCreate)
			{
			TPtrC passphrase(lex.NextToken());
			if (passphrase.Length() == 0)
				User::Leave(KErrCorrupt);
			WriteDialogRecordL(stream, EEnterPIN, _L("New key store passphrase"), passphrase, KNullDesC);
			}
		else
			{
			WriteDialogRecordL(stream, EEnterPIN, _L("Key store passphrase"), token, KNullDesC);
			}
		}

	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	}


void CKeyStoreTestAction::WriteDialogRecordL(RFileWriteStream& aStream, TSecurityDialogOperation aOp, const TDesC& aLabelSpec,
											 const TDesC& aResponse1, const TDesC& aResponse2)
	{

	MStreamBuf* streamBuf = aStream.Sink();
	streamBuf->SeekL(MStreamBuf::EWrite, EStreamEnd);

	aStream.WriteInt32L(aOp);
	aStream.WriteInt32L(aLabelSpec.Length());
	aStream.WriteL(aLabelSpec);
	aStream.WriteInt32L(aResponse1.Length());
	aStream.WriteL(aResponse1);
	aStream.WriteInt32L(aResponse2.Length());
	aStream.WriteL(aResponse2);
	}

TInt CKeyStoreTestAction::ReadDialogCountL()
	{
	RFileReadStream stream;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> secDlgOutputFile (sysDrive.Name());
	secDlgOutputFile.Append(KTSecDlgOutputFile);
	
	TInt err = stream.Open(iFs, secDlgOutputFile, EFileRead);
	// If the file doesn't exist yet just return zero
	if (err == KErrNotFound)
		{
		return 0;
		}
	User::LeaveIfError(err);
	stream.PushL();
	TInt index = stream.ReadInt32L();
	CleanupStack::PopAndDestroy(); // stream
	return index;
	}


TInt CKeyStoreTestAction::SetExpectedResultL(const TDesC8& aResult)
{
	if (aResult==KErrNoneString)
		return (KErrNone);
	else if (aResult==KErrAccessDeniedString)
		return (KErrAccessDenied);
	else if (aResult==KErrAlreadyExistsString)
		return (KErrAlreadyExists);
	else if (aResult==KErrInUseString)
		return (KErrInUse);
	else if (aResult==KErrNotFoundString)
		return (KErrNotFound);
	else if (aResult==KErrBadNameString)
		return (KErrBadName);
	else if (aResult==KErrArgumentString)
		return (KErrArgument);
	else if (aResult==KErrNotReadyString)
		return (KErrNotReady);
	else if (aResult==KErrCancelString)
		return (KErrCancel);
	else if (aResult==KErrCorruptString)
		return (KErrCorrupt);
	else if (aResult==KErrNotSupportedString)
		return (KErrNotSupported);
	else if (aResult==KErrOverflowString)
		return (KErrOverflow);
	else if (aResult==KErrBadPassphraseString)
		return (KErrBadPassphrase);
	else if (aResult==KErrGeneralString)
		return (KErrGeneral);
	else if (aResult==KErrKeyAlgorithmString)
		return KErrKeyAlgorithm;
	else if (aResult==KErrKeyUsageString)
		return KErrKeyUsage;
	else if (aResult==KErrKeyValidityString)
		return KErrKeyValidity;
	else if (aResult==KErrKeySizeString)
		return KErrKeySize;
	else if (aResult==KErrKeyAccessString)
		return KErrKeyAccess;
	else if (aResult==KErrPrivateKeyNotFoundString) 
		return KErrPrivateKeyNotFound;
	else if (aResult==KErrPermissionDeniedString)
		return KErrPermissionDenied;
	else if (aResult==KErrNoMemoryString)
			return KErrNoMemory;
#ifdef SYMBIAN_AUTH_SERVER
	else if (aResult==KErrAuthFailureString)
		return KErrAuthenticationFailure;
	else if (aResult==KErrUnknownAuthStrengthAliasString)
		return KErrUnknownAuthStrengthAlias;
#endif // SYMBIAN_AUTH_SERVER
	
	else
		{
		iOut.writeString(_L("Unknown expected result: "));
		iOut.writeString(aResult);
		iOut.writeNewLine();
		User::Leave(KErrNotFound);
		return KErrNotFound; // Keep compiler happy 
		}
}

TBool CKeyStoreTestAction::SetKeyUsageL(const TDesC8& aKeyUsage)
	{
	if (aKeyUsage.Length() == 0)
		return EFalse;
	
	TBool ret = ETrue;
	if (aKeyUsage.Compare(KAllKeyUsages)==0)
		iUsage = EPKCS15UsageAll;
	else if (aKeyUsage.Compare(KKeyUsageNone)==0)
		iUsage = EPKCS15UsageNone;
	else if (aKeyUsage.Compare(KAllKeyUsagesExceptNR)==0)
		iUsage |= EPKCS15UsageSign | EPKCS15UsageSignRecover | EPKCS15UsageDecrypt;
	else if (aKeyUsage.Compare(KDSAUsage)==0)
		iUsage |= EPKCS15UsageSign | EPKCS15UsageSignRecover;
	else if (aKeyUsage.Compare(KEncryptUsage)==0)
		iUsage |= EPKCS15UsageEncrypt;
	else if (aKeyUsage.Compare(KDecryptUsage)==0)
		iUsage |= EPKCS15UsageDecrypt;
	else if (aKeyUsage.Compare(KSignUsage)==0)
		iUsage |= EPKCS15UsageSign;
	else if (aKeyUsage.Compare(KSignRecoverUsage)==0)
		iUsage |= EPKCS15UsageSignRecover;
	else if (aKeyUsage.Compare(KWrapUsage)==0)
		iUsage |= EPKCS15UsageWrap;
	else if (aKeyUsage.Compare(KUnwrapUsage)==0)
		iUsage |= EPKCS15UsageUnwrap;
	else if (aKeyUsage.Compare(KVerifyUsage)==0)
		iUsage |= EPKCS15UsageVerify;
	else if (aKeyUsage.Compare(KVerifyRecoverUsage)==0)
		iUsage |= EPKCS15UsageVerifyRecover;
	else if (aKeyUsage.Compare(KDeriveUsage)==0)
		iUsage |= EPKCS15UsageDerive;
	else if (aKeyUsage.Compare(KNR)==0)
		iUsage |= EPKCS15UsageNonRepudiation;
	else
		{
		iOut.writeString(_L("Unknown key usage: "));
		iOut.writeString(aKeyUsage);
		iOut.writeNewLine();		
		User::Leave(KErrNotFound);
		}
	return ret;
	}

void CKeyStoreTestAction::SetKeyLabel(const TDesC8& aKeyLabel)
{
	iLabel = HBufC::NewMax(aKeyLabel.Size());
	if (iLabel)
	{
		TPtr theLabel(iLabel->Des());
		theLabel.FillZ();
		theLabel.Copy(aKeyLabel);
	}
}

void CKeyStoreTestAction::SetKeyAlgorithm(const TDesC8& aKeyAlgorithm)
{
	if (aKeyAlgorithm.Compare(KAlgRSA)==0)
	{
		iAlgorithm = CCTKeyInfo::ERSA;
	}
	else if (aKeyAlgorithm.Compare(KAlgDSA)==0)
	{
		iAlgorithm = CCTKeyInfo::EDSA;
	}
	else if (aKeyAlgorithm.Compare(KAlgDH)==0)
	{
		iAlgorithm = CCTKeyInfo::EDH;
	}
	else if (aKeyAlgorithm.Compare(KAlgBAD)==0)
	{
		iAlgorithm = static_cast<CCTKeyInfo::EKeyAlgorithm>(-1);
	}
	else
	{
		iAlgorithm = CCTKeyInfo::EInvalidAlgorithm;
	}
}


TBool CKeyStoreTestAction::SetKeyAccessType(const TDesC8& aKeyAccessType)
	{
	TBool ret = ETrue;
	if (aKeyAccessType.Compare(KExtractable)==0)
		{
		iAccessType |= CCTKeyInfo::EExtractable;
		}
	else if (aKeyAccessType.Compare(KSensitive)==0)
		{
		iAccessType |= CCTKeyInfo::ESensitive;
		}
	else if (aKeyAccessType.Compare(KAlwaysSensitive)==0)
		{
		iAccessType |= CCTKeyInfo::EAlwaysSensitive;
		}
	else if (aKeyAccessType.Compare(KNeverExtractable)==0)
		{
		iAccessType |= CCTKeyInfo::ENeverExtractable;
		}
	else if (aKeyAccessType.Compare(KLocal)==0)
		{
		iAccessType |= CCTKeyInfo::ELocal;
		}
	else
		ret = EFalse;
	return ret;
	}

void CKeyStoreTestAction::SetKeyStoreIndex(CUnifiedKeyStore* aUnifiedKeyStore)
	{
	TInt keyStoreCount = aUnifiedKeyStore->KeyStoreManagerCount();
	for(int index = 0 ; index<keyStoreCount; ++index)
		{
		MCTKeyStoreManager& mctKeyStoreMngr = aUnifiedKeyStore->KeyStoreManager(index);
		const TDesC16& label = mctKeyStoreMngr.Token().Label();
		TBuf<50> keyStoreLabel;
		keyStoreLabel.Copy(iKeyStoreImplLabel);
		if(label.Compare(keyStoreLabel) == 0 )
			{
			iKeyStoreImplIndex = index;
			break;
			}
		}
	
	}


void CKeyStoreTestAction::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	#ifdef SYMBIAN_AUTH_SERVER
	if(iDeauthenticate == 1)
		{
		AuthServer::RAuthClient authClient;
		authClient.Connect();
		TRAP(err,authClient.DeauthenticateL());
		authClient.Close();
		}
	#endif // SYMBIAN_AUTH_SERVER
	
	TRAP(err, WriteExpectedDialogDataL());
	
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CKeyStoreTestAction::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TInt dialogCount = -1;
	
	if(!iDisableCheckDialog)
		{
		TRAPD(err, dialogCount = ReadDialogCountL());
	
		if (err != KErrNone)
			{
			iOut.writeString(_L("Error reading dialog count: "));
			iOut.writeNum(err);
			iOut.writeNewLine();
			iResult = EFalse;
			}
		else 
			{
			iOut.writeString(_L("Dialog count: got "));
			iOut.writeNum(dialogCount);
			iOut.writeString(_L(", expected "));
			iOut.writeNum(iExpectedDialogs.Count());
			iOut.writeNewLine();
			if (dialogCount != iExpectedDialogs.Count())
				iResult = EFalse;
			}
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, aStatus.Int());
	iFinished = ETrue;
	}

void CKeyStoreTestAction::AfterOOMFailure()
	{
	// Reset t_secdlg
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> secDlgOutputFile (sysDrive.Name());
	secDlgOutputFile.Append(KTSecDlgOutputFile);
	
	TInt r = iFs.Delete(secDlgOutputFile);
	if (r != KErrNone && r != KErrNotFound)
		{
		User::Leave(r); // shouldn't leave!
		}	
	}

// Truncate labels after this many characters
static const TInt KMaxLabel = 32;
_LIT(KEllipsis, "...");

void CKeyStoreTestAction::PrintKeyInfoL(const CCTKeyInfo& aKey)
	{
	TBuf<256> buf;
	_LIT(KKeyAlg, "\t\tAlg: %d ");
	buf.Format(KKeyAlg, aKey.Algorithm());
	iOut.writeString(buf);
	iOut.writeNewLine();

	_LIT(KKeySize, "\t\tSize: %d ");
	buf.Format(KKeySize,  aKey.Size());
	iOut.writeString(buf);
	iOut.writeNewLine();

	_LIT(KKeyUsage, "\t\tUsage: 0x%x ");
	buf.Format(KKeyUsage, aKey.Usage());
	iOut.writeString(buf);
	iOut.writeNewLine();


	_LIT(KKeyUsePolicy, "\t\tUse policy: ");
	iOut.writeString(KKeyUsePolicy);
	iOut.writeSecurityPolicyL(aKey.UsePolicy());
	iOut.writeNewLine();

	_LIT(KKeyManagementPolicy, "\t\tManagement policy: ");
	iOut.writeString(KKeyManagementPolicy);
	iOut.writeSecurityPolicyL(aKey.ManagementPolicy());
	iOut.writeNewLine();


	_LIT(KKeySens, "\t\tAccess flags: ");
	_LIT(KSpace, " ");
	iOut.writeString(KKeySens);
	TInt flags = aKey.AccessType();
	if (flags & CCTKeyInfo::ESensitive)
		{ iOut.writeString(KSpace); iOut.writeString(KSensitive); }
	if (flags & CCTKeyInfo::EExtractable)
		{ iOut.writeString(KSpace); iOut.writeString(KExtractable); }
	if (flags & CCTKeyInfo::EAlwaysSensitive)
		{ iOut.writeString(KSpace); iOut.writeString(KAlwaysSensitive); }
	if (flags & CCTKeyInfo::ENeverExtractable)
		{ iOut.writeString(KSpace); iOut.writeString(KNeverExtractable); }
	if (flags & CCTKeyInfo::ELocal)
		{ iOut.writeString(KSpace); iOut.writeString(KLocal); }
	iOut.writeNewLine();

	_LIT(KKeyID, "\t\tID: ");
	iOut.writeString(KKeyID);
	iOut.writeOctetString(aKey.ID());	//	id
	iOut.writeNewLine();
				
	_LIT(KKeyLabel, "\t\tLabel: ");
	iOut.writeString(KKeyLabel);
	iOut.writeString(aKey.Label().Left(KMaxLabel));
	if (aKey.Label().Length() > KMaxLabel)
		{
		iOut.writeString(KEllipsis);
		}
	iOut.writeNewLine();

	_LIT(KKeyNative, "\t\tNative: %d ");
	buf.Format(KKeyNative, aKey.Native());
	iOut.writeString(buf);
	iOut.writeNewLine();

	_LIT(KTimeFormat, "%1/%2/%3 %H:%T:%S");
	 
	_LIT(KKeyStartDate, "\t\tStart date: ");
	iOut.writeString(KKeyStartDate);
	if (aKey.StartDate().Int64() == 0)
		iOut.writeString(_L("not set"));
	else
		{
		aKey.StartDate().FormatL(buf, KTimeFormat);
		iOut.writeString(buf);
		}
	iOut.writeNewLine();

	_LIT(KKeyEndDate, "\t\tEnd date: ");
	iOut.writeString(KKeyEndDate);
	if (aKey.EndDate().Int64() == 0)
		iOut.writeString(_L("not set"));
	else
		{
		aKey.EndDate().FormatL(buf, KTimeFormat);
		iOut.writeString(buf);
		}
	iOut.writeNewLine();
	}

void CKeyStoreTestAction::SetKeyType(const TDesC8& aKeyType)
{
	if (aKeyType.Compare(KAlgRSA)==0)
	{
		iType = ERSASign;
	}
	else if (aKeyType.Compare(KAlgDSA)==0)
	{
		iType = EDSASign;
	}
	else if (aKeyType.Compare(KDecryptUsage)==0)
	{
		iType = EDecrypt;
	}
	else if (aKeyType.Compare(KAlgDH)==0)
	{
		iType = EDH;
	}
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    else if (aKeyType.Compare(KAlgEcc)==0)
        {
        iType = EECC;
        }
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
}

/////////////////////////////////////////////////////////////////
//CInitialiseKeyStore::
///////////////////////////////////////////////////////////////////
CTestAction* CInitialiseKeyStore::NewL(	RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut,
										const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CInitialiseKeyStore::NewLC(aFs, aConsole, aOut,	aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CInitialiseKeyStore::NewLC(RFs& aFs,
										CConsoleBase& aConsole, 
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
{
	CInitialiseKeyStore* self = new(ELeave) CInitialiseKeyStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CInitialiseKeyStore::~CInitialiseKeyStore()
	{
	delete iNewUnifiedKeyStore;
	if (iKeyStoreLabel)
	    {
	    delete iKeyStoreLabel;
	    }
}

CInitialiseKeyStore::CInitialiseKeyStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(ENew)
	{
	}

void CInitialiseKeyStore::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 ptr = Input::ParseElement(aTestActionSpec.iActionBody, KModeStart, KModeEnd, pos, err);
	

	if (ptr == KNullDesC8)
		{
		User::Leave(KErrNotFound);
		}

	if (ptr == _L8("user"))
		{
		iKeyManager = EFalse;
		}
	else if (ptr == _L8("manager"))
		{
		iKeyManager = ETrue;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
		
	iEnableStoreCountCheck = Input::ParseIntElement(aTestActionSpec.iActionBody, KCheckStoreCountStart, KCheckStoreCountEnd); 
	
	if(iEnableStoreCountCheck)
		{
		iKeyStoreManagerCount = Input::ParseIntElement(aTestActionSpec.iActionBody, KManagerCountStart, KManagerCountEnd);
		iKeyStoreCount = Input::ParseIntElement(aTestActionSpec.iActionBody, KStoreCountStart, KStoreCountEnd);
		}
	
	iEnableKeyStoreLabelCheck = Input::ParseIntElement(aTestActionSpec.iActionBody, KCheckKeyStoreLabelStart, KCheckKeyStoreLabelEnd); 
	if(iEnableKeyStoreLabelCheck)
		{
		iKeyStoreIndex = Input::ParseIntElement(aTestActionSpec.iActionBody, KKeyStoreIndexStart, KKeyStoreIndexEnd); 
		TPtrC8 keyLabel = Input::ParseElement(aTestActionSpec.iActionBody, KKeyStoreLabelStart, KKeyStoreLabelEnd);
		iKeyStoreLabel = HBufC8::NewL(keyLabel.Length());
		iKeyStoreLabel->Des().Copy(keyLabel);
		}
}

void CInitialiseKeyStore::PerformAction(TRequestStatus& aStatus)
{
	switch (iState)
	{
		case ENew:
		{
			__ASSERT_ALWAYS(!iNewUnifiedKeyStore, TestKeyStorePanic(EKSTestNotInitialised));
			TRAPD(err, iNewUnifiedKeyStore = CUnifiedKeyStore::NewL(iFs));
            
			if (err != KErrNone)
			{
				iState = EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, err);
			}
			else
			{
				iState = EInit;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, err);
			}

			break;
		}


		case EInit:
		{
			iNewUnifiedKeyStore->Initialize(aStatus);
			iState = EAnalyseResults;
			break;
		}
		case EAnalyseResults:
		{
			switch(aStatus.Int())
			{
				case KErrNone:
				{
					iState = EAppend;
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, aStatus.Int());
				}
				break;

				default:
				{
					iConsole.Printf(_L("iNewUnifiedKeyStore->Initialize() returned %X\n"), aStatus.Int());
					iState = EFinished;
					iFinished = ETrue;
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, aStatus.Int());
				}
				break;
			}

		
		}
		break;
		case EAppend:
		{
			TInt result = CSharedKeyStores::TheUnifiedKeyStores().Append(iNewUnifiedKeyStore);
			if (KErrNone==result)
			{// Shared key stores has taken ownership
			TInt managerCount = iNewUnifiedKeyStore->KeyStoreManagerCount();
			TInt keyStoreCount = iNewUnifiedKeyStore->KeyStoreCount();
			
				if(iEnableStoreCountCheck)
					{
					if(iKeyStoreManagerCount != managerCount)
						{
						iOut.writeString(_L("KeyStoreManagerCount = "));
						iOut.writeNum(managerCount);
						iOut.writeString(_L(", expected = "));
						iOut.writeNum(iKeyStoreManagerCount);
						iOut.writeNewLine();						
						result = KErrGeneral;
						}
					
					if(iKeyStoreCount != keyStoreCount)
						{
						iOut.writeString(_L("KeyStoreCount = "));
						iOut.writeNum(keyStoreCount);
						iOut.writeString(_L(", expected = "));
						iOut.writeNum(iKeyStoreCount);
						iOut.writeNewLine();						
						result = KErrGeneral;
						}
					}
				
					
				if(iEnableKeyStoreLabelCheck)
					{
					if(iKeyManager)
						{
						if( iKeyStoreIndex > managerCount-1 )
							{
							iOut.writeString(_L("Invalid index = "));
							iOut.writeNum(iKeyStoreIndex);
							iOut.writeString(_L(", expected range: 0 to "));
							iOut.writeNum(managerCount-1);
							iOut.writeNewLine();						
							result = KErrGeneral;
							}
						else
							{
							MCTKeyStoreManager& mctKeyStoreMngr = iNewUnifiedKeyStore->KeyStoreManager(iKeyStoreIndex);
							const TDesC16& label = mctKeyStoreMngr.Token().Label();
							TBuf<50> keyStoreLabel;
							keyStoreLabel.Copy(iKeyStoreLabel->Des());
						
							if(label != keyStoreLabel )
								{
								iOut.writeString(_L("KeyStoreLabel = "));
								iOut.writeString(label);
								iOut.writeString(_L(", expected = "));
								iOut.writeString(iKeyStoreLabel->Des());
								iOut.writeNewLine();						
								result = KErrGeneral;
								}
							}
						} // if
						
					else
						{ 
						if( iKeyStoreIndex > keyStoreCount-1 )
							{
							iOut.writeString(_L("Invalid index = "));
							iOut.writeNum(iKeyStoreIndex);
							iOut.writeString(_L(", expected range: 0 to "));
							iOut.writeNum(keyStoreCount-1);
							iOut.writeNewLine();						
							result = KErrGeneral;
							}
						else
							{
							MCTKeyStore& mctKeyStore = iNewUnifiedKeyStore->KeyStore(iKeyStoreIndex);
							const TDesC16& label = mctKeyStore.Token().Label();
							TBuf<50> keyStoreLabel;
							keyStoreLabel.Copy(iKeyStoreLabel->Des());
						
							if(label != keyStoreLabel )
								{
								iOut.writeString(_L("KeyStoreLabel = "));
								iOut.writeString(label);
								iOut.writeString(_L(", expected = "));
								iOut.writeString(iKeyStoreLabel->Des());
								iOut.writeNewLine();						
								result = KErrGeneral;
								}
							}
						} // else
					}
				
				iNewUnifiedKeyStore = NULL;
				TRequestStatus* status = &aStatus;
  				User::RequestComplete(status, result);

				iState = EFinished;
			}
			else
			{
				delete iNewUnifiedKeyStore;
				iNewUnifiedKeyStore = NULL;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, result);
			}
		}
			break;

		case EFinished:
		{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
			{
				iResult = ETrue;
			}
			else
			{
				iResult = EFalse;
			}
			
			iActionState = EPostrequisite;
			break;
		}			
	}
}

void CInitialiseKeyStore::PerformCancel()
    {
    if (iState == EAnalyseResults) // ie, we're initialising
        {
		iNewUnifiedKeyStore->CancelInitialize();
        }
    }

void CInitialiseKeyStore::Reset()
    {
	iState = ENew;
	delete iNewUnifiedKeyStore;
	iNewUnifiedKeyStore = NULL;
    }

void CInitialiseKeyStore::DoReportAction()
{
	_LIT(KInitialiseString, "Initializing keystore manager...");
	iOut.writeString(KInitialiseString);
	iOut.writeNewLine();
}

void CInitialiseKeyStore::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccess, "\tStore initialised successfully\n");
			iConsole.Write(KSuccess);
			iOut.writeString(KSuccess);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else if (aError == KErrInUse)
		{
			_LIT(KFailed, "\tStore initialised error : KErrInUse\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// CDeleteKeyStore
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CDeleteKeyStore::NewL(RFs& aFs, 
									CConsoleBase& aConsole, 
									Output& aOut,
									const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CDeleteKeyStore::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CDeleteKeyStore::NewLC(RFs& aFs,
									CConsoleBase& aConsole, 
									Output& aOut,
									const TTestActionSpec& aTestActionSpec)
{
	CDeleteKeyStore* self = new(ELeave) CDeleteKeyStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CDeleteKeyStore::~CDeleteKeyStore()
	{
	if (CSharedKeyStores::TheUnifiedKeyStores().Count() == 0)
		CSharedKeyStores::TheUnifiedKeyStores().Close();
	}

CDeleteKeyStore::CDeleteKeyStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut)

:	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EDelete)
{}

void CDeleteKeyStore::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
}

void CDeleteKeyStore::PerformAction(TRequestStatus& aStatus)
{
	switch (iState)
	{
		case EDelete:
		{
		    iState = EFinished;
			TRequestStatus* status = &aStatus;

			TInt count = CSharedKeyStores::TheUnifiedKeyStores().Count();
			if (--count >= 0)
			{
			    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](count);
				CSharedKeyStores::TheUnifiedKeyStores().Remove(count);
				delete keystore;
				User::RequestComplete(status, KErrNone);
			}
			else
			{
			    User::RequestComplete(status, KErrNotFound);
			}
		}
			break;

		case EFinished:
		{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
			{
				iResult = ETrue;
			}
			else
			{
				iResult = EFalse;
			}
			
			iActionState = EPostrequisite;
		}
			break;
	}
}

void CDeleteKeyStore::PerformCancel()
    {
    // nothing to cancel
    }

void CDeleteKeyStore::Reset()
	{
	iState = EDelete;
	}

void CDeleteKeyStore::DoReportAction()
{
	_LIT(KDeleting, "Deleting keystore manager...");
	iOut.writeString(KDeleting);
	iOut.writeNewLine();
}

void CDeleteKeyStore::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccess, "\tStore deleted successfully\n");
			iConsole.Write(KSuccess);
			iOut.writeString(KSuccess);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			_LIT(KFailed, "\tStore delete failed\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}
