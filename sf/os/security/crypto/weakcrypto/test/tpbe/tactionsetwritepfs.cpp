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


#include "tactionsetwritepfs.h"
#include "t_input.h"
#include <cryptostrength.h>
#include <pbedata.h>
#include <f32file.h>
#include <s32file.h>
#include <stdlib.h>
#include <s32mem.h>
#include <s32std.h>

_LIT8(KWritePFSStart, "<writepfs>");
_LIT8(KWritePFSEnd, "</writepfs>");
_LIT8(KInputStart, "<input>");
_LIT8(KInputEnd, "</input>");
_LIT8(KPasswdStart, "<passwd>");
_LIT8(KPasswdEnd, "</passwd>");
_LIT16(KWeakFileName, "\\tpbe\\weak.dat");
_LIT16(KStrongFileName, "\\tpbe\\strong.dat");

CTestAction* CActionSetWritePFS::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionSetWritePFS::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CActionSetWritePFS::NewLC(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	CActionSetWritePFS* self = new(ELeave) CActionSetWritePFS(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionSetWritePFS::~CActionSetWritePFS()
	{
	delete iBody;
	}

CActionSetWritePFS::CActionSetWritePFS(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionSetWritePFS::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	
	}

void CActionSetWritePFS::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 encryptElement = Input::ParseElement(*iBody, KWritePFSStart,
		KWritePFSEnd, pos, err);

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

void CActionSetWritePFS::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	delete iPasswd;
	delete iInput;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CActionSetWritePFS::DoReportAction(void)
	{
	}

void CActionSetWritePFS::DoCheckResult(TInt)
	{

	}

void CActionSetWritePFS::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;
	
	CPBEncryptSet* set = CPBEncryptSet::NewLC(*iPasswd);
	CPBEncryptor* encryptor = set->NewEncryptLC();

	HBufC8* ciphertextTemp = HBufC8::NewLC(encryptor->MaxFinalOutputLength(iInput->Length())); 

	TPtr8 ciphertext = ciphertextTemp->Des();	
	encryptor->ProcessFinalL(*iInput, ciphertext);

	//Change the password by appending the letter 'a' to it
	HBufC* newPasswordTemp = HBufC::NewMaxLC(iPasswd->Length()+1);
	TPtr newPassword = newPasswordTemp->Des();
	newPassword.Copy(*iPasswd);
	newPassword.Append('a');

	set->ChangePasswordL(newPassword);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<128> pfsFileName (driveName);

	if (TCrypto::Strength() == TCrypto::EStrong)
		pfsFileName.Append(KStrongFileName);
	else
		pfsFileName.Append(KWeakFileName);

	RStoreWriteStream write;
	
	CFileStore *store = CPermanentFileStore::ReplaceLC(iFs, pfsFileName, EFileRead | EFileWrite);
	store->SetTypeL(store->Layout());
	
	//write the encrypted master key to a new stream
	write.CreateLC(*store);
	write << set->EncryptedMasterKey();
	write.CommitL();
	CleanupStack::PopAndDestroy(); //CreateLC()

	//write the encryption data to a new stream
	write.CreateLC(*store);
	set->EncryptionData().ExternalizeL(write);
	write.CommitL();
	CleanupStack::PopAndDestroy(); //CreateLC()

	//write the cyphertext to a new stream
	write.CreateLC(*store);
	write << ciphertext;
	write.CommitL();
	CleanupStack::PopAndDestroy(); //CreateLC()

	store->Commit();

	// Finished writing the PFS
	iResult = ETrue;
	CleanupStack::PopAndDestroy(store);
	CleanupStack::PopAndDestroy(newPasswordTemp);
	CleanupStack::PopAndDestroy(ciphertextTemp);
	CleanupStack::PopAndDestroy(encryptor);
	CleanupStack::PopAndDestroy(set);
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

void CActionSetWritePFS::Hex(HBufC8& aString)
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
