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


#include "tactionsetreadpfs.h"
#include "t_input.h"
#include <cryptostrength.h>
#include <securityerr.h>
#include <pbedata.h>
#include <f32file.h>
#include <s32file.h>
#include <stdlib.h>
#include <s32mem.h>
#include <s32std.h>

_LIT8(KReadPFSStart, "<readpfs>");
_LIT8(KReadPFSEnd, "</readpfs>");
_LIT8(KStrengthStart, "<strength>");
_LIT8(KStrengthEnd, "</strength>");
_LIT8(KInputStart, "<input>");
_LIT8(KInputEnd, "</input>");
_LIT8(KPasswdStart, "<passwd>");
_LIT8(KPasswdEnd, "</passwd>");
_LIT8(KStrong, "strong");
_LIT16(KWeakFileName, "\\tpbe\\weak.dat");
_LIT16(KStrongFileName, "\\tpbe\\strong.dat");

CTestAction* CActionSetReadPFS::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionSetReadPFS::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionSetReadPFS::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionSetReadPFS* self = new(ELeave) CActionSetReadPFS(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionSetReadPFS::~CActionSetReadPFS()
	{
	delete iBody;
	}

CActionSetReadPFS::CActionSetReadPFS(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionSetReadPFS::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	
	}

void CActionSetReadPFS::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 encryptElement = Input::ParseElement(*iBody, KReadPFSStart,
		KReadPFSEnd, pos, err);

	pos = 0;
	TPtrC8 strengthTemp = Input::ParseElement(encryptElement, KStrengthStart, 
		KStrengthEnd, pos, err);
		
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	if (strengthTemp.CompareF(KStrong))
		{
		iFileName = sysDrive.Name();
		iFileName.Append(KStrongFileName);
		}
	
	else
		{
		iFileName = sysDrive.Name();
		iFileName.Append(KWeakFileName);
		}

	pos = 0;		
	TPtrC8 passwdTemp = Input::ParseElement(encryptElement, KPasswdStart, 
		KPasswdEnd, pos, err);
	iPasswd = HBufC::NewL(passwdTemp.Length());
	TPtr16 passwdTemp3( iPasswd->Des());
	passwdTemp3.Copy(passwdTemp);

	pos = 0;
	TPtrC8 inputTemp = Input::ParseElement(encryptElement, KInputStart, 
		KInputEnd, pos, err);
	iInput = HBufC8::NewL(inputTemp.Length());
	*iInput = inputTemp;

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CActionSetReadPFS::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	delete iPasswd;
	delete iInput;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CActionSetReadPFS::DoReportAction(void)
	{
	}

void CActionSetReadPFS::DoCheckResult(TInt)
	{

	}

void CActionSetReadPFS::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;
	
	//Change the password by appending the letter 'a' to it
	HBufC* newPasswordTemp = HBufC::NewMaxLC(iPasswd->Length()+1);
	TPtr newPassword = newPasswordTemp->Des();
	newPassword.Copy(*iPasswd);
	newPassword.Append('a');

	//prepare to read the streams back in, creating a new TPBEncryptionData
	RStoreReadStream read;
	// open the next PFS
	CFileStore *store = CPermanentFileStore::OpenLC(iFs, iFileName, EFileRead | EFileWrite);
	TStreamId dataStreamId(2); // we know it was the second stream written
	read.OpenLC(*store, dataStreamId);
	CleanupStack::Pop();
	//read in Encryption data
	CPBEncryptionData* data = CPBEncryptionData::NewL(read);
	read.Close();
	CleanupStack::PushL(data);

	//read in encrypted master key
	TStreamId keyStreamId(1); // we know it was the first stream written
	read.OpenLC(*store, keyStreamId);
	CleanupStack::Pop();
	HBufC8* encryptedMasterKey = HBufC8::NewLC(read, 10000); //some large number
	read.Close();
	//create a new set encryption class
	CPBEncryptSet* set = CPBEncryptSet::NewLC(*data, *encryptedMasterKey, newPassword);

	//read in ciphertext key
	TStreamId cipherId(3); // we know it was the third stream written
	read.OpenLC(*store, cipherId);
	CleanupStack::Pop();
	HBufC8* ciphertextTemp = HBufC8::NewLC(read, 10000); //some large number
	read.Close();
	TPtr8 ciphertext = ciphertextTemp->Des();
	
	HBufC8* plaintextTemp = HBufC8::NewLC(ciphertext.Length());
	TPtr8 plaintext = plaintextTemp->Des();	

	// weak crypto should fail if trying to decrypt strong
	TRAPD(err, 
		{
		CPBDecryptor* decryptor = set->NewDecryptLC();
		decryptor->Process(ciphertext, plaintext);

		//this Mid call is due to get rid of the decrypted padding at the end
		if ((plaintext.Mid(0,iInput->Length()) == *iInput) &&
		!((TCrypto::Strength() == TCrypto::EWeak) && (iFileName == KStrongFileName)))
			{
			iResult = ETrue;
			}
		CleanupStack::PopAndDestroy(decryptor);
		});

	if ((err == KErrKeyNotWeakEnough) && 
	(TCrypto::Strength() == TCrypto::EWeak) && (iFileName == KStrongFileName))
		{
		iResult = ETrue;
		}

	CleanupStack::PopAndDestroy(plaintextTemp);
	CleanupStack::PopAndDestroy(ciphertextTemp);
	CleanupStack::PopAndDestroy(set);
	CleanupStack::PopAndDestroy(encryptedMasterKey);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(store);
	CleanupStack::PopAndDestroy(newPasswordTemp);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

void CActionSetReadPFS::Hex(HBufC8& aString)
    {
    TPtr8 ptr=aString.Des();
    if (aString.Length()%2)
        {
        ptr.SetLength(0);
        return;
        }
    TInt i;
    for (i=0;i<aString.Length();i+=2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
        ptr[i/2]=tmp;
        }
    ptr.SetLength(aString.Length()/2);
    }
