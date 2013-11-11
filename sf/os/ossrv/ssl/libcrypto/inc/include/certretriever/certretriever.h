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

Description:
*/

 
#ifndef CERT_RETRIEVER_H
#define CERT_RETRIEVER_H

#ifdef __cplusplus

#include <unifiedcertstore.h>
#include <mctwritablecertstore.h>
#include <x509cert.h>
#include <signed.h>
#include <openssl/x509.h>

#endif

#define ISSUER_NAME 0
#define SUBJECT_NAME 1

// CONSTANT
const TInt KCertMaxBuffer = 32;
const TInt KMaxNameLength = 256;
const TInt KMaxCertLength = 5120;

#ifdef __cplusplus
extern "C" 
{
#endif
int X509_add_symbian_certsL(X509_STORE * store);
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

NONSHARABLE_CLASS (CCertRetriever) : public CActive
	{
public:
	static CCertRetriever* NewLC(
				X509_STORE* aStore,
				TRequestStatus& aStatus, 
				const CActiveScheduler* aActiveScheduler);
	static CCertRetriever* NewL(
				X509_STORE* aStore,
				TRequestStatus& aStatus, 
				const CActiveScheduler* aActiveScheduler);
	~CCertRetriever();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	void RetriveCertificateL();
	CActiveSchedulerWait* activeSchedulerwait; //For nested wait loop;
	TBool OwnScheduler;
		
private:
	CCertRetriever(X509_STORE* aStore,
				TRequestStatus& aStatus, 
				const CActiveScheduler* aActiveScheduler);
	void ConstructL();
	
	void OpenUnifiedCertStoreL();
    void ListCertsL();
    void AppendCertsL();
    void ProcessCertsL();
    
private:
	X509_STORE* iStore;
	TRequestStatus& iFinStatus;
	const CActiveScheduler* iActiveScheduler;
	
	RFs iFs;
	CUnifiedCertStore * iCertStore;
 	CCertAttributeFilter * iCertFilter;
 	RMPointerArray< CCTCertInfo > iCerts;
 	
	enum TStateLists
	{		
		EInitializeCertStore,
		EListCerts,
		EAppendCerts,
		ENoCerts,
		EDone
	};  
						
	TStateLists iState;
	
	TInt iCertCount;	
	TPtr8 iCertPtr ;
	HBufC8 *iBuf;
	};


#endif

#endif //CERT_RETRIEVER_H
