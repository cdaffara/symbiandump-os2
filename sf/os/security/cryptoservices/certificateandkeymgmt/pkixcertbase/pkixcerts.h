/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __PKIXCERTS_H__
#define __PKIXCERTS_H__

//pkixroots.h
#include <e32std.h>
#include <x509cert.h>
#include <x509certext.h>
#include <mcertstore.h>

/**
 * Base class for classes that help retrieving certificates from stores
 */
class MPKIXCertSource
	{
public:
	//copies cert into aCandidates, passes ownership of cert to calling code...
	virtual void CandidatesL(const CX509Certificate& aSubject, 
		RPointerArray<CX509Certificate>& aCandidates, TRequestStatus& aStatus) = 0;
	virtual void CancelCandidates() = 0;
	virtual void Release() = 0;

protected:
	virtual ~MPKIXCertSource();
	
protected:
	/**
	 * This function compares the issuer altname in aSubjectCert with the 
	 * subject altname in aIssuerCert
	 * @param aSubjectCert We will compare the issuer altname of this certificate.
	 * @param aIssuerCert We will compare the subject altname of this certificate.
	 * @return 
	 * <UL>
	 * <LI>ETrue if the issuer altname in aSubjectCert matches the subject altname in
	 * aIssuerCert</LI>
	 * <LI>EFalse otherwise</LI>
	 * </UL>
	 */
	TBool AltNameMatchL(const CX509Certificate& aSubjectCert, const CX509Certificate& aIssuerCert) const;
	};

/**
 * This class is used to retrieve the certificates from a store
 * It doesn't work with client base trust.
 */
class CPKIXCertsFromStore : public CActive, public MPKIXCertSource
	{
public:
	/**
	 * Constructs a new CPKIXCertsFromStore instance and adds it to the active scheduler
	 * Initialize must be called after this function
	 * @param aStore Reference to the cert store. The store is created with the default 
	 * filter intialized to retrieve certificate of CA type and of X509 format.
	 * @return Initialized instance of this class.
	 */
	static CPKIXCertsFromStore* NewL(MCertStore& aCertStore);
	static CPKIXCertsFromStore* NewLC(MCertStore& aCertStore);
	
	/**
	 * Constructs a new CPKIXCertsFromStore instance and adds it to the active scheduler
	 * Initialize must be called after this function
	 * @param aStore Reference to the cert store. The store is created with the default 
	 * filter intialized to retrieve certificate of CA type and of X509 format.
	 * @param aClient The UID for which the certificates are to be retrieved from the
	 * cert store, This UID is also passed to the filter for retrieving the certificates 
	 * specific to this client UID.
	 * @return Initialized instance of this class.
	 */
	
	static CPKIXCertsFromStore* NewL(MCertStore& aCertStore, TUid aClient);
	static CPKIXCertsFromStore* NewLC(MCertStore& aCertStore, TUid aClient);
	/**
	 * This function does the actual listing of certificates based on the filter created.
	 * It must be called after construction.
	 * @param aStatus Standard parameter for asynchronous calling convention. 
	 */
	void Initialize(TRequestStatus& aStatus);
	/**
	 * This function returns a list of CA certificates that authenticate the
	 * aSubject certificate.
	 * @param aCandidates On return, this array contains the list of CA certificates
	 * that can possibly be used to authenticate aSubject. The array owns the elements
	 * and must take care of deleting them.
	 */
	virtual void CandidatesL(const CX509Certificate& aSubject, 
		RPointerArray<CX509Certificate>& aCandidates, TRequestStatus& aStatus);
	virtual void CancelCandidates();
	virtual void Release();
	virtual ~CPKIXCertsFromStore();

private:
	CPKIXCertsFromStore(MCertStore& aCertStore);
	CPKIXCertsFromStore(MCertStore& aCertStore, TUid aClient);
	void ConstructL();
	void ConstructL(TUid aClient);

public:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	void HandleEGetCertificateL();
	void HandleEAddCandidateL();
	void HandleECheckTrusted();
	
	TBool IsDuplicateL(const CX509Certificate& aCertificate);

private:
	enum TState
	{
		EIdle = 0,
		EInitialize,
		ECheckTrusted,
		EGetCertificate,
		EAddCandidate,
		EEnd
	};

private:
	/**
	 * The state used to know what must be done when executing
	 * RunL().
	 */
	TState iState;

	/**
	 * The TRequestStatus that must be updated when the operation
	 * requested by a user of this class has been 
	 * completed
	 */
	TRequestStatus *iOriginalRequestStatus;

	TUid iClient;

	CCertAttributeFilter *iFilter;

	/**
	 * iRootName is used for CandidateL
	 */
	const CX500DistinguishedName* iRootName;

	/**
	 * We don't own this
	 */
	const CX509Certificate* iSubject;

	/**
	 * We don't own this
	 */
	RPointerArray<CX509Certificate>* iCandidates;

	/**
	 * iCertData is used for CandidateL
	 */
	HBufC8* iCertData;

	TPtr8* iCertPtr;

	/**
	 * iEntriesIndex is used for CandidateL
	 */
	TInt iEntriesIndex;
	
	/**
	 * Applies to certificate at iEntriesIndex - reflects trust setting
	 */
	TBool iIsTrusted;

	/**
	 * Used when listing certificates (filtered but not on trust).
	 */
	RMPointerArray<CCTCertInfo> iCertInfos;

	MCertStore& iCertStore;
	};

class CPKIXCertsFromClient : public MPKIXCertSource
	{
public:
	static CPKIXCertsFromClient* NewL(const RPointerArray<CX509Certificate>& aCerts);
	static CPKIXCertsFromClient* NewLC(const RPointerArray<CX509Certificate>& aCerts);
	virtual void CandidatesL(const CX509Certificate& aSubject,
		RPointerArray<CX509Certificate>& aCandidates, TRequestStatus& aStatus);
	virtual void CancelCandidates();
	virtual void Release();
	virtual ~CPKIXCertsFromClient();

private:
	CPKIXCertsFromClient(const RPointerArray<CX509Certificate>& aCerts);

private:
	const RPointerArray<CX509Certificate>& iCerts;
	};

#endif
