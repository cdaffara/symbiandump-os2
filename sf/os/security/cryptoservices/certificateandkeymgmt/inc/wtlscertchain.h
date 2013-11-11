/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
 
#ifndef __WTLSCERTCHAIN_H__
#define __WTLSCERTCHAIN_H__

#include <e32std.h>
#include <unifiedcertstore.h>
#include <wtlscert.h>
#include <wtlsnames.h>

class TWTLSValidationStatus
/** The validation status.
*
* Some errors cannot be blamed on any single certificate, in which case the 
* iCert value is meaningless. The same structure is used for errors and for warnings. 
*
*/
	{
public:
	/** Creates a validation status object.
	* 
	* @param aError	The error type that occurred when validating the certificate chain.
	* @param aCert	The index number for the certificate that gave rise to the error. */
	IMPORT_C TWTLSValidationStatus(const TValidationError aError, const TInt aCert);
	
	/** The reason for the error. */
	TValidationError iReason;
	
	/** The index number for the certificate that gave rise to the error. */
	TInt iCert;
	};

class CWTLSValidationResult : public CBase
/** Encapsulates the results of the validation process.
* 
* It is returned to client code, which can examine it. Client code takes ownership of it. 
*
*/
	{
public:
	/** Creates a new CWTLSValidationResult object and puts a pointer to it on the 
	* cleanup stack.
	* 
	* @return	The new WTLS Validation Result object. */
	IMPORT_C static CWTLSValidationResult* NewLC();
	
	/** Creates a new CWTLSValidationResult object.
	* 
	* @return	The new WTLS Validation Result object. */
	IMPORT_C static CWTLSValidationResult* NewL();
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CWTLSValidationResult();
	
	/** Gets the error status of the operation.
	* 
	* Any errors here are considered fatal: validation has failed.
	* 
	* @return	The error status of the operation. */
	IMPORT_C const TWTLSValidationStatus Error() const;
	
	/** Gets an array of any warnings generated.
	* 
	* The warnings may or may not be fatal, depending on the context, which the 
	* client is expected to provide.
	* 
	* @return	An array of any warnings generated. */
	IMPORT_C const CArrayFixFlat<TWTLSValidationStatus>& Warnings() const;
	
	/** Resets the validation result object to its default values. 
	 * @internalAll
	 */
	void Reset();
	
	/** Sets the error.
	* 
	* @param aError	The error type that occurred when validating the certificate chain.
	* @param aCert	The index number for the certificate that gave rise to the error. 
	* @internalAll
	*/
	void SetError(const TValidationError aError, const TInt aCert);
	
	/** Adds a warning to the validation.
	* 
	* @param aWarning	The validation status object to be added. 
	* @internalAll
	*/
	void AppendWarningL(TWTLSValidationStatus aWarning);

private:
	CWTLSValidationResult();
	void ConstructL();
	TWTLSValidationStatus iError;
	CArrayFixFlat<TWTLSValidationStatus>* iWarnings;
	};

class CWTLSRootCerts;
class CWTLSCertChainAO;

class CWTLSCertChain : public CBase
/** Implements a WTLS certificate chain. 
*
*/
	{
	friend class CWTLSCertChainAO;

public:
	/** Creates a certificate chain using the binary data in aEncodedCerts.
	* 
	* @param aFs			An open file server session.
	* @param aEncodedCerts	One or more concatenated DER encoded WTLS certificates. 
	* 						The first certificate will be interpreted as the end entity 
	* 						certificate to be validated; subsequent certificates may be 
	* 						in any order and may be used by the chain as intermediate 
	* 						certificates, but not root certificates.
	* @param aClient		The uid of the client. It is a value identifying the application 
	* 						to the chain; this will be used to select a subset of stored 
	* 						certificates to use as candidate root certificates. */
	IMPORT_C static CWTLSCertChain* NewL(RFs& aFs, const TPtrC8& aEncodedCerts, 
		const TUid aClient);
	
	/** Creates a certificate chain using the binary data in aEncodedCerts and puts 
	* a pointer to the new object onto the cleanup stack.
	* 
	* @param aFs			An open file server session
	* @param aEncodedCerts	One or more concatenated DER encoded WTLS certificates. 
	* 						The first certificate will be interpreted as the end entity 
	* 						certificate to be validated; subsequent certificates may be 
	* 						in any order and may be used by the chain as intermediate 
	* 						certificates, but not root certificates.
	* @param aClient		The uid of the client. It is a value identifying the application 
	* 						to the chain; this will be used to select a subset of stored 
	* 						certificates to use as candidate root certificates. */
	IMPORT_C static CWTLSCertChain* NewLC(RFs& aFs, const TPtrC8& aEncodedCerts,
		const TUid aClient);
	
	/** Creates a certificate chain using the binary data in aEncodedCerts.
	* 
	* @param aFs			An open file server session.
	* @param aEncodedCerts	One or more concatenated DER encoded WTLS certificates. 
	* 						The first certificate will be interpreted as the end entity 
	* 						certificate to be validated; subsequent certificates may be 
	* 						in any order and may be used by the chain as intermediate 
	* 						certificates, but not root certificates. Any self signed 
	* 						certificates supplied here after the first one will be 
	* 						discarded, as self signed certificates cannot by definition 
	* 						be intermediate certificates.
	* @param aRootCerts		An array of certificates which the chain will treat as 
	* 						candidate root certificates. If one of these overloads is 
	* 						used, the chain will not look in stores for root certificates, 
	* 						but will only use the certificates supplied here. */
	IMPORT_C static CWTLSCertChain* NewL(RFs& aFs, const TPtrC8& aEncodedCerts,
		const CArrayPtr<CWTLSCertificate>& aRootCerts);
	
	/** Creates a certificate chain using the binary data in aEncodedCerts and puts 
	* a pointer to the new object onto the cleanup stack.
	* 
	* @param aFs			An open file server session.
	* @param aEncodedCerts	One or more concatenated DER encoded WTLS certificates. 
	* 						The first certificate will be interpreted as the end entity 
	* 						certificate to be validated; subsequent certificates may be 
	* 						in any order and may be used by the chain as intermediate 
	* 						certificates, but not root certificates. Any self signed 
	* 						certificates supplied here after the first one will be 
	* 						discarded as self signed certificates cannot by definition 
	* 						be intermediate certificates.
	* @param aRootCerts		An array of certificates which the chain will treat as 
	* 						candidate root certificates. If one of these overloads is 
	* 						used, the chain will not look in stores for root certificates, 
	* 						but will only use the certificates supplied here. */
	IMPORT_C static CWTLSCertChain* NewLC(RFs& aFs, const TPtrC8& aEncodedCerts,
		const CArrayPtr<CWTLSCertificate>& aRootCerts);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	IMPORT_C ~CWTLSCertChain();
	
	/** Validates the chain.
	* 
	* @param aValidationResult	On completion, this contains the result of the validation.
	* @param aValidationTime	The time for which validation should be performed, usually 
	* 							the current time.
	* @param aStatus			An asynchronous request status object. */
	IMPORT_C void ValidateL(CWTLSValidationResult& aValidationResult, 
		const TTime& aValidationTime, TRequestStatus& aStatus);
	
	/** Gets the number of WTLS certificates in the chain.
	* 
	* @return	The number of WTLS certificates in the chain. */
	IMPORT_C TInt Count() const;
	
	/** Gets the certificate at the specified index.
	* 
	* @param aIndex	The ordinal number representing the position of the certificate 
	* 				within the chain.
	* @return		The WTLS certificate at the specified index. */
	IMPORT_C const CWTLSCertificate& Cert(TInt aIndex) const;
	
	/** Tests whether the root certificate of the chain is locatable.
	* 
	* Note that the value is only significant after a successfull call to ValidateL().
	* 
	* @return	ETrue if the chain has a root; EFalse, otherwise. */
	IMPORT_C TBool ChainHasRoot() const;
	
	/** Appends the specified encoded certificate to the chain.
	* 
	* @param aEncodedCerts	One or more concatenated DER encoded WTLS certificates. 
	* 						These certificates will be used as candidates. The first 
	* 						certificate will be interpreted as the end entity certificate 
	* 						to be validated; subsequent certificates may be in any order 
	* 						and may be used by the chain as intermediate certificates, 
	* 						but not root certificates. */
	IMPORT_C void AppendCertsL(const TPtrC8& aEncodedCerts);

private:
	CWTLSCertChain(RFs& aFs);
	void ConstructL(const TPtrC8& aEncodedCerts, const TUid aClient);
	void ConstructL(const TPtrC8& aEncodedCerts, const CArrayPtr<CWTLSCertificate>& aRootCerts);
	void DoConstructL(const TPtrC8& aEncodedCerts);
		
private:
	RFs& iFs;
	CWTLSCertChainAO* iActiveObject;
	CArrayPtrFlat<CWTLSCertificate>* iChain;
	TBool iChainHasRoot;
	};

#endif

