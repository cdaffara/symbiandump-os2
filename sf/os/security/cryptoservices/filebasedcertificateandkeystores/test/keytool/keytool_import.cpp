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


#include <mctkeystore.h>
#include <asn1dec.h>
#include <asnpkcs.h>
#include <unifiedkeystore.h>
#include <securityerr.h>

#include "keytool_commands.h"
#include "keytool_controller.h"
#include "controller.h"

#include <keytool.rsg>

/*static*/ CKeytoolImport* CKeytoolImport::NewLC(CController* aController)
	{
	CKeytoolImport* self = new (ELeave) CKeytoolImport(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

/*static*/ CKeytoolImport* CKeytoolImport::NewL(CController* aController)
	{
	CKeytoolImport* self = CKeytoolImport::NewLC(aController);
	CleanupStack::Pop(self);
	return self;	
	}

CKeytoolImport::~CKeytoolImport()
	{
	Cancel();	
	delete iKeyData;
	if (iKeyInfo != NULL)
		{
		iKeyInfo->Release();
		}	
	}

void CKeytoolImport::ConstructL()
	{
	CActiveScheduler::Add(this);		
	}

void CKeytoolImport::Pkcs8PreprocessorL()
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());	

	RFile file;
	CleanupClosePushL(file);	
	TInt r = file.Open(fs, *(iParams->iDefault), EFileRead);
	if (r != KErrNone)
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_OPENFAIL);				
		User::Leave(r);
		}
        
	TInt fileSize = 0;
	file.Size(fileSize);

	if (fileSize <= 0)
	{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_EMPTYFILE);	
		User::Leave(KErrCorrupt);
	}

	iKeyData = HBufC8::NewMaxL(fileSize);
	TPtr8 data(iKeyData->Des());
	data.FillZ();
	User::LeaveIfError(file.Read(data, fileSize));
	
	CleanupStack::PopAndDestroy(&file); 
	CleanupStack::PopAndDestroy(&fs); 
	}


void CKeytoolImport::DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam)
	{
	Cancel();	
	iParams = aParam;
	
	TRAPD(err, Pkcs8PreprocessorL());
	if (err != KErrNone)
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_PKCS8);		
		User::Leave(err);
		}
			
	// keystore index, key data, usage, label, access type, start, end, keyinfo, status
	// must get proper keystore
	if (aParam->iKeystoreIndex == -1)
		{
		aParam->iKeystoreIndex = 0;
		}
	if (aParam->iKeystoreIndex >= aKeyStore.KeyStoreCount())
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_NOTEXIST);	
		User::Leave(KErrCorrupt);
		}

	if (!iParams->iLabel)
		{
		iParams->iLabel	= (iParams->iDefault)->AllocL();
		}
	
	aKeyStore.ImportKey(iParams->iKeystoreIndex, *iKeyData, iParams->iUsage, *iParams->iLabel, 
						iParams->iAccess, TTime(0), iParams->iEndDate, iKeyInfo, iStatus);

	SetActive();
	}

void CKeytoolImport::RunL()
	{	
	if (iStatus.Int() != KErrNone)
		{
		// A problem occured. Handle gracefully.
		User::Leave(iStatus.Int());
		}
	iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_IMPORTSUCCESS);
	iController->DisplayKeyL(*iKeyInfo, ETrue, iParams->iPageWise);

	CActiveScheduler::Stop();		
	}

TInt CKeytoolImport::RunError(TInt aError)
	{
	CActiveScheduler::Stop();
	switch (aError)
		{
		case KErrBadPassphrase:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_PASSPHRASE, iStatus.Int()));
			}
			break;
		case KErrKeyNotWeakEnough:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_WEAK, iStatus.Int()));			
			}
			break;			
		case KErrKeyUsage:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_USAGE, iStatus.Int()));						
			}
			break;
		default:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_IMPORT, iStatus.Int()));			
			}
		}	
	return KErrNone;
	}


void CKeytoolImport::DoCancel()
	{
	
	}
	
CKeytoolImport::CKeytoolImport(CController* aController) : CKeyToolCommand(aController)
	{	
	}

