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
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <x509keys.h>


const TInt KKeyStoreEmpty = -1199;

/////////////////////////////////////////////////////////////////////////////////
// CExportPublicKey
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CExportPublicKey::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CExportPublicKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CExportPublicKey::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
	{
	CExportPublicKey* self = new (ELeave) CExportPublicKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CExportPublicKey::~CExportPublicKey()
	{
	}

CExportPublicKey::CExportPublicKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CExportKey(aFs, aConsole, aOut)
	{
	}

void CExportPublicKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CExportKey::ConstructL(aTestActionSpec);
	}
	
void CExportPublicKey::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
	case EExportKey:
		{
		CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);

		TCTKeyAttributeFilter filter;
		filter.iUsage = EPKCS15UsageAll;

		keyStore->List(iKeys, filter, aStatus);
				
		iState = EListing;
		}
		
		break;
	case EListing:
		{
		TInt keyIndex = 0; // The index of the key we want to export
		
		if (iKeys.Count() == 0)
			{
			iConsole.Printf(_L("keyStore empty!!"));
			iState = EFinished;
			TRequestStatus *status = &aStatus;
			User::RequestComplete(status, KKeyStoreEmpty);			
			break;
			}
			
		// Select the key with the label we want!
		for (TInt j = 0; j < iKeys.Count(); j++)
			{
			if (iKeys[j]->Label() == *iLabel) 
				{
				keyIndex = j;
				break;
				}
			}
		
		switch(aStatus.Int())
			{				
			case KErrNone:
			{	 
				CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);

				keyStore->ExportPublic(iKeys[keyIndex]->Handle(), iKeyData, aStatus);									
	
				iState = EIntermediate;
				}
				break;
			
			default:
				{
				iConsole.Printf(_L("keyStore->ExportPublicKey returned: %d\n"), aStatus.Int());
				iState = EFinished;
				TRequestStatus *status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}
				break;
			}
		break;	
		}
		
	case EIntermediate:
		{		
		switch(aStatus.Int())
			{				
			case KErrNone:
				{	
					iState = EFinished;            // All is well let's end this
					TRAPD(err, WriteToFileL());                  // Write exported key to file
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, err);
				}
				break;
			
			default:
				{
				iConsole.Printf(_L("keyStore->ExportPublicKey returned: %d\n"), aStatus.Int());
				iState = EFinished;
				TRequestStatus *status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}
			break;			
			}
		break;
		}

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
		iFinished = ETrue;
		}
		break;
		}
	}

void CExportPublicKey::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EListing:
            keystore->CancelList();
            break;

        case EIntermediate:
            keystore->CancelExportKey();
            break;

        default:
            break;
        }
    }
