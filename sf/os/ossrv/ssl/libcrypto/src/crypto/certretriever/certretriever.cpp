/*
Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
  may be used to endorse or promote products derived from this software 
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Description:  Contains implementation for x509_add_symbian_cert - to use certificates installed in Symbian with OpenSSL code.
*/

 
#include "certretriever.h"
#include "createx509.h"

#include <securitydefsconst.h>

#ifdef __cplusplus
extern "C" 
{
#endif
int X509_add_symbian_certsL(X509_STORE * store)
{
	CActiveScheduler* activeScheduler;
	CActiveScheduler* CurrentActiveScheduler = CActiveScheduler::Current();
	if(CurrentActiveScheduler == NULL)
	{
		activeScheduler = new (ELeave) CActiveScheduler;
		CleanupStack::PushL(activeScheduler);
		
		CActiveScheduler::Install(activeScheduler);
	}
	
		
	TRequestStatus status;
	CCertRetriever* certRetriever;
	TRAPD(error, certRetriever = CCertRetriever::NewL(store, status, CActiveScheduler::Current()));
	
	// create CActiveSchedulerWait
	if (CurrentActiveScheduler)
	{
		certRetriever->activeSchedulerwait = new (ELeave) CActiveSchedulerWait; 
		certRetriever->OwnScheduler = EFalse;	
	}
	
	
	if(error != KErrNone)
	{
		CleanupStack::PopAndDestroy();	// activeScheduler
		return 0;
	}
	
	CleanupStack::PushL(certRetriever);	
	
	TRAP(error,certRetriever->RetriveCertificateL());
	if(error != KErrNone)
	{
		if(CurrentActiveScheduler == NULL)
		{
			CleanupStack::PopAndDestroy(2);	// activeScheduler, certRetriever
		}
		else
			CleanupStack::PopAndDestroy();	// certRetriever
		return 0;
	}
		
				
	if(CurrentActiveScheduler == NULL)
	{
		activeScheduler->Start();
		 CleanupStack::PopAndDestroy(2); // If you destroy the object it will not be there
										   // in the iActiveQ, and hence stray signal.
		
		//CleanupStack::Pop(2);	   // So just pop it. // activeScheduler, certRetriever						   
	}
	else
	{
	//	CurrentActiveScheduler->Start();// If you are using CActiveScheduler::Current();
										  // Why u want to start it again?	
        
        // CleanupStack::PopAndDestroy(); // If you destroy the object it will not be there
										  // in the iActiveQ, and hence stray signal.	
										  									   
		// should wait here untill it finish loading certificates, ths API is synchronous
		certRetriever->activeSchedulerwait->Start();
		   
		CleanupStack::Pop(); 		  // So just pop it. // certRetriever
	}
	
	
		
	if(status == KErrNone)
		return 1;
	else
		return 0;
}
#ifdef __cplusplus
}
#endif

CCertRetriever::CCertRetriever(X509_STORE* aStore, 
				TRequestStatus& aStatus,
				const CActiveScheduler* aActiveScheduler)
			: CActive(CActive::EPriorityHigh), 
			  iStore(aStore),
			  iFinStatus ( aStatus ),
			  iActiveScheduler(aActiveScheduler),
			  iCertPtr(0,0)			  
			  
	{
	
	OwnScheduler = ETrue;
	if(iActiveScheduler)
		iActiveScheduler->Add(this);
	}

CCertRetriever::~CCertRetriever()
	{
		Cancel();
 		delete iBuf;
 		delete iCertFilter;
 		
 	 	iCerts.Close();	
 		delete iCertStore;
 		iFs.Close();
 		if(!OwnScheduler)
 		  delete activeSchedulerwait;
 	}
	
CCertRetriever* CCertRetriever::NewLC(X509_STORE* aStore,
						TRequestStatus& aStatus, 
						const CActiveScheduler* aActiveScheduler)
	{
	CCertRetriever* self = new (ELeave) CCertRetriever(aStore, aStatus, aActiveScheduler);
	CleanupStack::PushL(self);
	TRAPD(err,self->ConstructL());
	if(err != KErrNotFound)
		return self;
	
	CleanupStack::PopAndDestroy(self);	
	return NULL;
	}

CCertRetriever* CCertRetriever::NewL(X509_STORE* aStore,
						TRequestStatus& aStatus, 
						const CActiveScheduler* aActiveScheduler)
	{
	CCertRetriever* self = CCertRetriever::NewLC(aStore, aStatus, aActiveScheduler);
	if(self)
		CleanupStack::Pop();
	return self;
	}

void CCertRetriever::ConstructL()
	{
		TInt err = iFs.Connect();
		if(err != KErrNone)
			User::Leave(err);
		
 		iState = EInitializeCertStore;
 		iBuf = HBufC8::NewL(KMaxCertLength);
	}


void CCertRetriever::RunL()
	{
	// 1. All certificates retrieved.
	// 2. yes. check iActiveScheduler. if null then call User::RequestComplete(iStatus) else iActiveScheduler->Stop();
	User::LeaveIfError(iStatus.Int());
		
		switch(iState)
		{
		case EInitializeCertStore:
					OpenUnifiedCertStoreL();		
					break;
		
		case EListCerts:
					ListCertsL();	
					break;
		case EAppendCerts:
					if (!iCerts.Count()) // no certificate in store.
					{
						iState = ENoCerts;												
					}
					else
					{
						AppendCertsL();
						break;	
					}			
		       		
		case EDone:
					if (iState != ENoCerts)
					{
						ProcessCertsL();	//Process the last certificate
						iCertCount = 0;	
					}					
		case ENoCerts:
								
					if(iActiveScheduler)
					{
						if(OwnScheduler)
							iActiveScheduler->Stop();
						else
							activeSchedulerwait->AsyncStop();
						
						iFinStatus = iStatus;
					}
					else
					{
						TRequestStatus *s = &iFinStatus;
						User::RequestComplete(s, KErrNone);	
					}
					
					
					break;
		
		default:
				User::Leave(KErrNotFound);
				break;
		}
	}


void CCertRetriever::DoCancel()
	{
	}

TInt CCertRetriever::RunError(TInt aError)
	{
		//Can do some error handling here
		if(iActiveScheduler)
		{
			iActiveScheduler->Stop();
			iFinStatus = iStatus;
		}
		else
		{
			TRequestStatus *s = &iFinStatus;
			User::RequestComplete(s, aError);	
		}
		
		return KErrNone; 
	}

void CCertRetriever::RetriveCertificateL()
	{
		OpenUnifiedCertStoreL();
	}
	
	
void CCertRetriever::OpenUnifiedCertStoreL()
    {
    
    	iState = EListCerts;
    	delete iCertStore;
    	iCertStore = NULL;
    	iCertStore = CUnifiedCertStore::NewL(iFs, EFalse);
    	iCertStore->Initialize(iStatus);
    	SetActive();
    }
    
void CCertRetriever::ListCertsL()
{
    // Create filter object
   
   delete iCertFilter;
   iCertFilter = NULL;
   iCertFilter = CCertAttributeFilter::NewL();    
   iCertFilter->SetFormat(EX509Certificate);
   iCertFilter->SetOwnerType(ECACertificate);
   iCertFilter->SetUid(KTlsApplicabilityUid);

    iStatus = KRequestPending;
    SetActive();
    iCertStore->List(iCerts, *iCertFilter, iStatus);
    iState = EAppendCerts;
}

void CCertRetriever::AppendCertsL()
{
	if(iCertCount>0)	
		ProcessCertsL();
	
	CCTCertInfo *cert = iCerts[iCertCount];
		
	SetActive();			
	iStatus == KRequestPending;
		
	iCertPtr.Set( iBuf->Des() );
    iCertStore->Retrieve((*cert),iCertPtr,iStatus);
	
	iCertCount++;
	
		if(iCertCount == iCerts.Count())
			iState = EDone;	
}


void CCertRetriever::ProcessCertsL()
{
	CX509Certificate *X509Cert;
	TRAPD(error, X509Cert = CX509Certificate::NewL( iCertPtr ));
	if(error !=KErrNone)
		return;
		
	CleanupStack::PushL(X509Cert);

	X509* x509 = CX509_Initializer::CreateX509L(X509Cert);

	if(x509)
	{
		X509_STORE_add_cert(iStore,x509);
		X509_free(x509);
	}
	
	CleanupStack::PopAndDestroy(); //X509Cert
	
}
