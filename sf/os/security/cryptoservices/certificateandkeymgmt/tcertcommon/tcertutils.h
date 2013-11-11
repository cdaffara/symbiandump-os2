/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tcertuils.h
*
*/


/**
 @file 
 @internalTechnology
*/


#ifndef __TCERTUTILS_H__
#define __TCERTUTILS_H__

#include <e32base.h>
#include <x509cert.h>
#include <unifiedcertstore.h>
#include <pkixcertchain.h>
#include "t_output.h"

/**
CCertUtils provides various functions often used in tests.
This class has changed because the functions are asynchronous now. 
The function are no longer static as we need an instance to serve as
an active object.
*/
class CCertUtils : public CActive
	{
public:
	/**
	 * This enumeration enumerates all the states used for the
	 * RunL function.
	 */
	enum TState
		{
		EAddCACerts,

		EAddCert,
		EAddCACertGetCAEntry,
		EAddCACertSetApplications,
		EAddCACertSetTrust,
		EAddCACertFinished,

		ERemoveCertsGetCACerts,
		ERemoveCertsCACertsRetrieved,
		ERemoveCertsRemoveCACerts,
		ERemoveCertsGetUserCerts,
		ERemoveCertsUserCertsRetrieved,
		ERemoveCertsRemoveUserCerts,
		ERemoveCertsFinished
		};	
	
public:
	IMPORT_C static CCertUtils* NewL(RFs& aFs);
	IMPORT_C static CCertUtils* NewLC(RFs& aFs);
	/**
	The destructor destroys all the members of CCertUtils but only destroys
	iStoreManager if the CCertStoreManager instance was created by this CCertUtils
	instance. It doesn't destroy it if it was passed as an argument in the 
	constructor.
	*/
	IMPORT_C virtual ~CCertUtils();


	/**
	Certificate handling functions
	*/
	IMPORT_C void AddCertL(const TDesC& aLabel, 
							TCertificateFormat aFormat,
							TCertificateOwnerType aCertificateOwnerType, 
							TInt aTrustedUsage, 
							const TDesC& aCertificatePath, 
							const TDesC& aCertificateFileName, 
							TRequestStatus& aStatus);
							
	IMPORT_C void AddCert(const TDesC& aLabel, 
							TCertificateFormat aFormat,
							TCertificateOwnerType aCertificateOwnerType, 
							TInt aTrustedUsage, 
							const TDesC& aCertificatePath, 
							const TDesC& aCertificateFileName, 
							CUnifiedCertStore& aUnifiedCertStore, 
							TRequestStatus& aStatus);
						
	IMPORT_C void AddCACertsL(const CDesCArray& aRoots, 
							const CDesCArray& aLabels,
							TCertificateFormat aFormat, 
							TInt aTrustedUsage, 
							const TDesC& aPath, 
							TRequestStatus& aStatus);
							
	IMPORT_C void AddCACertsL(const CDesCArray& aRoots, 
							const CDesCArray& aLabels,
							TCertificateFormat aFormat, 
							TInt aTrustedUsage, 
							const TDesC& aPath, 
							CUnifiedCertStore& aUnifiedCertStore, 
							TRequestStatus& aStatus);
							
	IMPORT_C void RemoveCertsL(TRequestStatus& aStatus);
	IMPORT_C void RemoveCertsL(CUnifiedCertStore& aUnifiedCertStore, 
							TRequestStatus& aStatus);

	IMPORT_C static CCertificate* CertFromFileLC(const TDesC& aFilename, 
							const TDesC& aPathname, 
							RFs& aFs, 
							TCertificateFormat aFormat);
							
	IMPORT_C static CCertificate* CertFromFileL(const TDesC& aFilename,
							const TDesC& aPathname, 
							RFs& aFs, 
							TCertificateFormat aFormat);
							
	IMPORT_C static void WriteError(TValidationError aError, Output& aOut);
	IMPORT_C static TPtrC MapError(TValidationError aError);
	IMPORT_C static TValidationError MapError(const TDesC& aError);
	IMPORT_C static TPtrC ParseElement(const TDesC& aBuf, const TDesC& aStart, const TDesC& aEnd, TInt& aPos, TInt& aError);

	IMPORT_C HBufC* DiagnosticLC() const;
	IMPORT_C void AddApplicationL(const TDesC& aName, TUid aUid) const;
	IMPORT_C void RemoveApplicationL(TUid aUid) const;

private:
	/**
	This constructor constructs a CCertUtils instance without initializing
	iStoreManager. A store manager will be created if one is needed and
	will be destroyed by the destructor.
	@param aFs A file server session. The file server session must have opened.
	*/
	CCertUtils(RFs& aFs);
	void ConstructL();

	//Virtual from CActive
	void RunL();
	void DoCancel();
    TInt RunError(TInt aError);


private:
	// The following functions handle the different RunL states
	void HandleEAddCACertsL();

	void HandleEAddCACertL();
	void HandleEAddCACertGetCAEntry();
	void HandleEAddCACertSetApplicationsL();
	void HandleEAddCACertSetTrust();
	void HandleEAddCACertFinishedL();
	
	void HandleERemoveCertsGetCACerts();
	void HandleERemoveCertsCACertsRetrieved();
	void HandleERemoveCertsRemoveCACerts();
	void HandleERemoveCertsGetUserCerts();
	void HandleERemoveCertsUserCertsRetrieved();
	void HandleERemoveCertsRemoveUserCerts();
	void HandleERemoveCertsFinished();


private:
	/**
	File Server Sessioin
	*/
	RFs& iFs;

	/**
	The state used to know what RunL should do
	*/
	TState iState;
	
	/**
	The store managers will be used for certificate store operations,
	*/
	CUnifiedCertStore* iUnifiedCertStore;
	CUnifiedCertStore* iCreatedUnifiedCertStore;
	
	/**
	These members are used to store the arguments of the functions
	because most of the things are handled in a RunL.
	*/
	const CDesCArray* iRoots;					// Used by AddCertsL
	const CDesCArray* iLabels;					// Used by AddCertsL
	TCertificateFormat iFormat;
	TInt iTrustedUsage;							// Used by AddCertsL, AddCert
	RArray<TUid> iTrusters;						// Used by AddCertsL, AddCert
	const TDesC* iPath;							// Used by AddCertsL, AddCert
	TRequestStatus* iOriginalRequestStatus;		// Used by AddCertsL, AddCert
	TInt iIndex;								// Used by AddCertsL to know 
												// which certificate to add next
												// Used by RemoveCerts
	CCertUtils* iSecondCertUtils;				// Used by AddCertsL to add each individual
												// certificate
	const TDesC* iLabel;						// Used by AddCert
	TCertificateOwnerType iCertificateOwnerType;
	const TDesC* iCertificateFileName;			// Used by AddCert 
	CCertificate* iCertificate;					// Used by AddCACert
	MCTWritableCertStore *iStore;				// Used by AddCACert
	TPtrC8 iEncoding;							// Used by AddCACert

	HBufC* iCertData;
	HBufC* iLabelData;

	/**
	This is a filter used by the GetCACerts function. We use the filter constructed
	by the constructor with no argument, the filter doesn't filter anything.
	*/
	CCertAttributeFilter* iCAFilter;				// Used by RemoveCerts
	RMPointerArray<CCTCertInfo> iCACertStoreEntries;	// Used by RemoveCerts
	
	/**
	This is a filter used by the GetUserCerts function. We use the filter constructed
	by the constructot with no argument, the filter doesn't filter anything.
	*/
	CCertAttributeFilter* iUserFilter;			// Used by RemoveCerts
	RMPointerArray<CCTCertInfo> iUserCertStoreEntries; // Used by RemoveCerts

public:
	/**
	A diagnostic variable for when an error occurs. It stores the state at which
	the error occurred.
	*/
	TState iDiagnosticState;
	
	/** 
	A diagnostic variable for when an error occurs. It is an info message set
	to whatever is relevant for the error.
	*/
	TBuf<400> iDiagnosticMessage;
	};

#endif
