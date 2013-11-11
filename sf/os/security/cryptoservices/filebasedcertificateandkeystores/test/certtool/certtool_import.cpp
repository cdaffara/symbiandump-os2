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
#include <mctwritablecertstore.h>

#include "certtool_commands.h"
#include "certtool_controller.h"
#include "keytool_commands.h"


const TInt KASN1SequenceTagValue = 0x30;
const TInt KWTLSCertificateVersionValue = 0x01;

/*static*/ CCertToolAdd* CCertToolAdd::NewLC(CCertToolController* aController)
	{
	CCertToolAdd* self = new (ELeave) CCertToolAdd(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

/*static*/ CCertToolAdd* CCertToolAdd::NewL(CCertToolController* aController)
	{
	CCertToolAdd* self = CCertToolAdd::NewLC(aController);
	CleanupStack::Pop(self);
	return self;	
	}

CCertToolAdd::~CCertToolAdd()
	{
	Cancel();	
	delete iCertData;	
	}

void CCertToolAdd::ConstructL()
	{
	//The Certificate added is deletable by default.
	iIsDeletable = ETrue;
	}


TCertificateFormat CCertToolAdd::DoRecognizeL(const TDesC8& iData)
	{
	// Ensure length is sufficient for checking type
	if ( iData.Size() >= 1 )
		{
		// First byte of X.509 certificate is an ANS.1 SEQUENCE tag
		if ( iData[0] == KASN1SequenceTagValue )
			{
			return EX509Certificate;
			}
		// First byte of WTLS certificate is version == 1
		else if ( iData[0] == KWTLSCertificateVersionValue )
			{
			return EWTLSCertificate;
			}
		}
	User::Leave(KErrEof);
	return EWTLSCertificate;
	}


/**
 * Inserts a certificate in the certstore. 
 * If a specific certstore implementation is given using
 * the -store command line parameter the certificate is inserted
 * there. If no implementation is specified the first one is used.
 * Initially we try to add the certificate as a *user* certificate
 * if this fails (no corresponding private key in the keystore)
 * the certificate is added a root (CA) certificate.
 * If the option -private was present then the private key is in the
 * keystore and the certificate will be a user certificate.
 */
void CCertToolAdd::DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam)
	{
	Cancel();	
	iParams = aParam;	
	iCertStore = &aCertStore;

	if (!aParam->iDefault)
		{
		iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_CERTFILE);			
		User::Leave(KErrNotFound);
		CActiveScheduler::Stop();		
		}

		// must get proper certstore, hard-coded 0 is no good!
		if (aParam->iCertstoreIndex == -1)
			{
			aParam->iCertstoreIndex = 0;
			}
		if (aParam->iCertstoreIndex >= iCertStore->WritableCertStoreCount())
			{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_NOTEXIST);	
			User::Leave(KErrCorrupt);
			}
		MCTWritableCertStore& wCertStore = iCertStore->WritableCertStore(aParam->iCertstoreIndex);

		if (!iParams->iLabel)
			{
			iParams->iLabel	= (iParams->iDefault)->AllocL();
			}
			
		// Get the certificate binary!
		RFs fs;
		CleanupClosePushL(fs);
		User::LeaveIfError(fs.Connect());	
	
		RFile file;
		CleanupClosePushL(file);	
		TInt r = file.Open(fs, *(iParams->iDefault), EFileRead);
		if (r != KErrNone)
			{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_OPENFAIL);				
			User::Leave(r);
			}
	        
		TInt fileSize = 0;
		file.Size(fileSize);

		if (fileSize <= 0)
		{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_EMPTYFILE);	
			User::Leave(KErrCorrupt);
		}

		iCertData = HBufC8::NewMaxL(fileSize);
		TPtr8 data(iCertData->Des());
		data.FillZ();
		User::LeaveIfError(file.Read(data, fileSize));

		// Use the recognizer to see what kind of certificate we have!
		TCertificateFormat format = DoRecognizeL(*iCertData);
						
		iState = EIntermediate;
		
		/**
		 * If the iIsDeletable variable of iParams is set, parse its value
		 * and set the iIsDeletable flag appropriately.
		 */
		if(iParams->iIsDeletable)
		    {
		    HBufC* lowerCaseString = HBufC::NewLC(iParams->iIsDeletable->Length());
		    TPtr ptr(lowerCaseString->Des());
		    
		    //Convert to lower case.
		    ptr.CopyLC(*iParams->iIsDeletable);
		    
		    if(ptr.CompareF(_L("n")) == 0 || ptr.CompareF(_L("no")) == 0 )
		        {
		        iIsDeletable = EFalse;
		        }
            else if (ptr.CompareF(_L("y")) != 0  && ptr.CompareF(_L("yes")) != 0)
                {
                //Wrong value passed.
                User::Leave(KErrArgument);     
                }

		    CleanupStack::PopAndDestroy(lowerCaseString);            
		    }

		SetActive();
		//wCertStore.Add(*iParams->iLabel, format, ECACertificate, NULL, NULL, *iCertData, iStatus);
		wCertStore.Add(*iParams->iLabel, format, EUserCertificate, NULL, NULL, *iCertData, iIsDeletable, iStatus);
		CleanupStack::PopAndDestroy(2, &fs);
	}


void CCertToolAdd::RunL()
	{	
	if ((iStatus.Int() != KErrNone) && (iStatus.Int() != KErrPrivateKeyNotFound))
		{
		// A problem occured. Handle gracefully.
		User::Leave(iStatus.Int());
		}
	switch (iState)
		{
		case EIntermediate :
			{
			if (iStatus.Int() == KErrPrivateKeyNotFound)
				{
				// Not an user certificate add as root!
				iState = EFinish;
				MCTWritableCertStore& wCertStore = iCertStore->WritableCertStore(iParams->iCertstoreIndex);
				TCertificateFormat format = DoRecognizeL(*iCertData);
				SetActive();				
				wCertStore.Add(*iParams->iLabel, format, ECACertificate, NULL, NULL, *iCertData, iIsDeletable, iStatus);				
				}
			else 
				{
				iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_IMPORTSUCCESS);			
				CActiveScheduler::Stop();				
				}
			}
			break;
		case EFinish : 
			{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_IMPORTSUCCESS);
			CActiveScheduler::Stop();			
			}	
			break;
		default :
			{
			}
		}
	
	}
	
TInt CCertToolAdd::RunError(TInt aError)
	{
	CActiveScheduler::Stop();
	switch (aError)
		{
		case KErrBadName:
			{
			// Most likely it was there already
			TRAP_IGNORE(iController->DisplayErrorL(_L("The given label is invalid, or already present in the certstore."), iParams->iPageWise));
			}
			break;
		case KErrKeyNotWeakEnough:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_WEAK, iStatus.Int()));			
			}
			break;			
		default:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_IMPORT, iStatus.Int()));			
			}
		}	
	return KErrNone;
	}
	
void CCertToolAdd::DoCancel()
	{
	
	}
	
CCertToolAdd::CCertToolAdd(CCertToolController* aController) : CCertToolCommand(aController)
	{
	CActiveScheduler::Add(this);	
	}

