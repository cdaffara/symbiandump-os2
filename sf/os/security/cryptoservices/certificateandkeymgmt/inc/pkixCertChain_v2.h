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
* PKIXCERTCHAIN.H
* PKIX certificate chain implementation
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __PKIXCERTCHAIN_H__
#define __PKIXCERTCHAIN_H__

#include <e32std.h>
#include <x509certchain.h>
#include <pkixvalidationresult.h>

//implements key validation according to RFC 2459 (PKIX cert/CRL profile), section 6
class CPKIXValidationState;
class CPKIXChainBuilder;
class CPKIXCertChainAO;
class CPKIXCertChainHelper;
class MCertStore;

/**
 * Base class for CPKIXCertChain
 */
class CPKIXCertChainBase : public CX509CertChain
	{
public:
	//constructors
	/** Creates a certificate chain using the binary data in aEncodedCerts.
	*
	* @param aCertStore		The certificate store to use when looking for root certificates.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	*						The first certificate will be interpreted as the end entity certificate to 
	*						be validated; subsequent certificates may be in any order and may be used 
	*						by the chain as intermediate certificates, but not root certificates. The 
	*						individual certificates can be retrieved since each one contains its own length.
	* @param aClient		The Uid identifying the purpose for which the chain will be used. 
	* 						This value will be used to select a subset of stored certificates, by way of their trust 
	*						settings, to be used as candidate root certificates. */
	IMPORT_C static CPKIXCertChainBase* NewL(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, 
		const TUid aClient);
	
	/** Creates a certificate chain using the binary data in aEncodedCerts, and puts 
	* a pointer to the new object onto the cleanup stack.
	*
	* @param aCertStore		The certificate store to use when looking for root certificates.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	* 						The first certificate will be interpreted as the end entity certificate to 
	* 						be validated; subsequent certificates may be in any order and may be used 
	* 						by the chain as intermediate certificates, but not root certificates. The 
	* 						individual certificates can be retrieved since each one contains its own length.
	* @param aClient		The Uid identifying the purpose for which the chain will be used. 
	* 						This value will be used to select a subset of stored certificates, by way of their trust 
	*						settings, to be used as candidate root certificates. */
	IMPORT_C static CPKIXCertChainBase* NewLC(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, 
		const TUid aClient);
		
	/** Creates a certificate chain using the binary data in aEncodedCerts.
	*
	* @param aCertStore		The certificate store to use when looking for root certificates.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	* 						The first certificate will be interpreted as the end entity certificate to 
	* 						be validated; subsequent certificates may be in any order and may be used 
	* 						by the chain as intermediate certificates, but not root certificates. Any 
	* 						self signed certificates supplied here after the first one will be discarded, 
	* 						as self signed certificates cannot by definition be intermediate certificates. 
	* 						The individual certificates can be retrieved since each one contains its own 
	* 						length.
	* @param aRootCerts		An array of certificates which the chain will treat as candidate root 
	* 						certificates. If one of these overloads is used, the chain will not look in 
	* 						stores for root certificates, but will only use the certificates supplied here. */
	IMPORT_C static CPKIXCertChainBase* NewL(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, 
		const RPointerArray<CX509Certificate>& aRootCerts);
		
	/** Creates a certificate chain using the binary data in aEncodedCerts and puts 
	* a pointer to the new object onto the cleanup stack.
	*
	* @param aCertStore		The certificate store to use when looking for root certificates.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	* 						The first certificate will be interpreted as the end entity certificate to 
	* 						be validated; subsequent certificates may be in any order and may be used 
	* 						by the chain as intermediate certificates, but not root certificates. Any 
	* 						self signed certificates supplied here after the first one will be discarded 
	* 						as self signed certificates cannot by definition be intermediate certificates. 
	* 						The individual certificates can be retrieved since each one contains its own 
	* 						length.
	* @param aRootCerts		An array of certificates which the chain will treat as candidate root 
	* 						certificates. If one of these overloads is used, the chain will not look in 
	* 						stores for root certificates, but will only use the certificates supplied here. */
	IMPORT_C static CPKIXCertChainBase* NewLC(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, 
		const RPointerArray<CX509Certificate>& aRootCerts);
		
	/** Destructor.
	*
	* Frees all resources owned by the object. */
	IMPORT_C ~CPKIXCertChainBase();
		//validation

	/** Validates the chain.
	*
	* @param aValidationResult	On completion, this contains the result of the validation.
	* @param aValidationTime	The time that should be presumed to be the current time when checking timestamps.
	* @param aStatus			An asynchronous request status object. */
	IMPORT_C void ValidateL(CPKIXValidationResultBase& aValidationResult, 
		const TTime& aValidationTime, TRequestStatus& aStatus);
	
	/** Validates the chain.
	*
	* @param aValidationResult	On completion, this contains the result of the validation.
	* @param aValidationTime	The time that should be presumed to be the current time when checking timestamps.
	* @param aInitialPolicies	The policies we want to be present in the certificate chain.
	* @param aStatus 			An asynchronous request status object. */
	IMPORT_C void ValidateL(CPKIXValidationResultBase& aValidationResult, 
		const TTime& aValidationTime, const CArrayPtr<HBufC>& aInitialPolicies, 
		TRequestStatus& aStatus);

	/** Cancels an asynchronous ValidateL() operation. */
	IMPORT_C void CancelValidate();

	/** Adds one or more intermediate certificates to use when building the chain .
	*
	* Any self signed certs are ignored.
	*
	* @param aEncodedCerts	The concatenation of one or more DER encoded X.509 certificates. */
	IMPORT_C void AddCertL(const TPtrC8& aEncodedCerts);
	
	/** Tests whether the root certificate of the chain is locatable.
	*
	* Note that the value is only significant after a successful call to ValidateL().
	* 
	* @return	ETrue if the chain has a root; EFalse, otherwise. */
	IMPORT_C TBool ChainHasRoot() const;	
	
	/** Returns a list of the critical extension OIDs that are supported by the 
	* chain validator. If a critical extension is encountered in a certificate 
	* chain whose OID matches an element in this set then the chain validator 
	* shall treat this as a warning instead of an error. 
	* 
	* If CPKIXCertChain::SetSupportedCriticalExtensionsL() has not been called, this 
	* list will return the default set of supported critical extensions which 
	* includes the X.509 standard and Symbian specific SIS file critical extensions. 
	* These extensions may change in the future and should not be relied upon.
	* 
	* @return	The current list of supported critical extension OIDs. Ownership is not 
	* transferred to the caller. */
	IMPORT_C const RPointerArray<TDesC>& SupportedCriticalExtensions() const;
	
	/** Adds one or more critical extension OIDs to the list of supported critical 
	* extensions. Duplicate OID values are not added.
	* 
	* @param aCriticalExtOids	A list of the critical extensions OIDs to append to the supported 
	* list. Ownership is not transferred from the caller. */
	IMPORT_C void AddSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids);
	
	/** Removes one or more critical extension OIDs from the list of supported critical extensions.
	* 
	* @param aCriticalExts	A list of the critical extensions OIDs to remove from the supported list. 
	* Ownership is with the original caller. Oids will not be destroyed. */
	IMPORT_C void RemoveSupportedCriticalExtensions(const RPointerArray<TDesC>& aCriticalExtOids);

	/** Completely replaces the set of supported critical extensions for certificate validation. If a critical 
	* extension is encountered matching one of these OIDs then its occurrence is treated as a warning rather 
	* than an error.  The results of which can be queried through a call to CPKIXValidationResult::ValidationWarnings().
	* 
	* @param aCriticalExtOids	A list of the critical extensions OIDs for the class to support. Ownership is
	* not transferred from the caller. */
	IMPORT_C void SetSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids);

	/** Resets the current list of supported critical extensions and re-populates it with the default set
	* which includes the X.509 standard and Symbian specific SIS file critical extensions. These extensions 
	* may change in the future and should not be relied upon. */
	IMPORT_C void ResetSupportedCriticalExtsToDefaultL();
	
	/** Specify if a failed check on the certificate validity date is treated as an error or a warning.
	*
	* @param aIsFatal  ETrue for reporting as an error; EFalse for a warning.*/
	IMPORT_C void SetValidityPeriodCheckFatal(TBool aIsFatal);

	/** Returns whether or not validity period check failures will be reported as an error or a warning.
	*
	* @param aIsFatal  ETrue if failure is reported as an error; EFalse for a warning.*/
	IMPORT_C TBool ValidityPeriodCheckFatal() const;

protected:
	IMPORT_C CPKIXCertChainBase();
	IMPORT_C void ConstructL(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, TUid aClient);
	IMPORT_C void ConstructL(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, const RPointerArray<CX509Certificate>& aRootCerts);

public:
	// Public non-exported methods called by CPKIXCertChainAO
	CArrayPtrFlat<CX509Certificate>& Chain();
	const RPointerArray<CX509Certificate>& IntermediateCerts();	
	TBool ChainHasRoot();
	void RemoveLastCerts(TInt aNumberOfCertsToRemove);	
	void SetChainHasRoot(TBool aHasRoot);

private:
	void DoConstructL(const TPtrC8& aEncodedCerts);

	/**
	 * This function adds certificates to the chain but only the ones that are not
	 * self-signed.
	 *
	 * @param aEncodedCerts	The encoded certificates.
	 */
	void AddIntermediateCertsL(const TPtrC8& aEncodedCerts);

private:
	/**
	 * Holds a list of candiate intermediate certs - these come from the encoded
	 * certs passed at construction time, and also any added with AddCertL().
	 */
	RPointerArray<CX509Certificate> iIntermediateCerts;

	/**
	 * This is ETrue if the chain has a root and EFalse if it hasn't.  The value
	 * is only significant after a successfull call to ValidateL().
	 */
	TBool iChainHasRoot;

	/**
	 * Most of the fucntionality of the class is asynchronous and is in fact
	 * delegated to iActiveObject which will deal with all the asynchronous
	 * functions.
	 */
	CPKIXCertChainAO* iActiveObject;
	
	/**
	 * Holds a list of supported critical extensions set by the client.
	 */
	RPointerArray<TDesC> iSupportedCriticalExts;
	
	/**
	 * When true (the defaut) indicates that a failed check on the validity period of a
	 * certificate will result in a fatal error.  When false this instead results in a
	 * warning.
	 */
	TBool iDateTimeCheckFatal;
	};


/**
 * This class implements a PKIX certificate chain.
 *
 * @since v6.0
 */
class CPKIXCertChain : public CPKIXCertChainBase
	{
public:
	
		//constructors
	/** Creates a certificate chain using the binary data in aEncodedCerts.
	*
	* @param aFs			An open file server session.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	*						The first certificate will be interpreted as the end entity certificate to 
	*						be validated; subsequent certificates may be in any order and may be used 
	*						by the chain as intermediate certificates, but not root certificates. The 
	*						individual certificates can be retrieved since each one contains its own length.
	* @param aClient		The Uid identifying the purpose for which the chain will be used. 
	* 						This value will be used to select a subset of stored certificates, by way of their trust 
	*						settings, to be used as candidate root certificates. */
	IMPORT_C static CPKIXCertChain* NewL(RFs& aFs, const TPtrC8& aEncodedCerts, 
		const TUid aClient);
	
	/** Creates a certificate chain using the binary data in aEncodedCerts, and puts 
	* a pointer to the new object onto the cleanup stack.
	*
	* @param aFs			An open file server session
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	* 						The first certificate will be interpreted as the end entity certificate to 
	* 						be validated; subsequent certificates may be in any order and may be used 
	* 						by the chain as intermediate certificates, but not root certificates. The 
	* 						individual certificates can be retrieved since each one contains its own length.
	* @param aClient		The Uid identifying the purpose for which the chain will be used. 
	* 						This value will be used to select a subset of stored certificates, by way of their trust 
	*						settings, to be used as candidate root certificates. */
	IMPORT_C static CPKIXCertChain* NewLC(RFs& aFs, const TPtrC8& aEncodedCerts, 
		const TUid aClient);
		
	/** Creates a certificate chain using the binary data in aEncodedCerts.
	*
	* @param aFs			An open file server session.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	* 						The first certificate will be interpreted as the end entity certificate to 
	* 						be validated; subsequent certificates may be in any order and may be used 
	* 						by the chain as intermediate certificates, but not root certificates. Any 
	* 						self signed certificates supplied here after the first one will be discarded, 
	* 						as self signed certificates cannot by definition be intermediate certificates. 
	* 						The individual certificates can be retrieved since each one contains its own 
	* 						length.
	* @param aRootCerts		An array of certificates which the chain will treat as candidate root 
	* 						certificates. If one of these overloads is used, the chain will not look in 
	* 						stores for root certificates, but will only use the certificates supplied here. */
	IMPORT_C static CPKIXCertChain* NewL(RFs& aFs, const TPtrC8& aEncodedCerts, 
		const RPointerArray<CX509Certificate>& aRootCerts);
		
	/** Creates a certificate chain using the binary data in aEncodedCerts and puts 
	* a pointer to the new object onto the cleanup stack.
	*
	* @param aFs			An open file server session.
	* @param aEncodedCerts	One or more concatenated DER encoded X.509 certificates. 
	* 						The first certificate will be interpreted as the end entity certificate to 
	* 						be validated; subsequent certificates may be in any order and may be used 
	* 						by the chain as intermediate certificates, but not root certificates. Any 
	* 						self signed certificates supplied here after the first one will be discarded 
	* 						as self signed certificates cannot by definition be intermediate certificates. 
	* 						The individual certificates can be retrieved since each one contains its own 
	* 						length.
	* @param aRootCerts		An array of certificates which the chain will treat as candidate root 
	* 						certificates. If one of these overloads is used, the chain will not look in 
	* 						stores for root certificates, but will only use the certificates supplied here. */
	IMPORT_C static CPKIXCertChain* NewLC(RFs& aFs, const TPtrC8& aEncodedCerts, 
		const RPointerArray<CX509Certificate>& aRootCerts);
		
		//destructor
	/** Destructor.
	*
	* Frees all resources owned by the object. */
	IMPORT_C ~CPKIXCertChain();
		//validation

	/** Validates the chain.
	*
	* @param aValidationResult	On completion, this contains the result of the validation.
	* @param aValidationTime	The time that should be presumed to be the current time when checking timestamps.
	* @param aStatus			An asynchronous request status object. */
	IMPORT_C void ValidateL(CPKIXValidationResult& aValidationResult, 
		const TTime& aValidationTime, TRequestStatus& aStatus);
	
	/** Validates the chain.
	*
	* @param aValidationResult	On completion, this contains the result of the validation.
	* @param aValidationTime	The time that should be presumed to be the current time when checking timestamps.
	* @param aInitialPolicies	The policies we want to be present in the certificate chain.
	* @param aStatus 			An asynchronous request status object. */
	IMPORT_C void ValidateL(CPKIXValidationResult& aValidationResult, 
		const TTime& aValidationTime, const CArrayPtr<HBufC>& aInitialPolicies, 
		TRequestStatus& aStatus);

	/** Cancels an asynchronous ValidateL() operation. */
	IMPORT_C void CancelValidate();

	/** Adds a certificate (if it is not self-signed) to the chain .
	*
	* @param aEncodedCerts	A DER encoded X.509 certificate. */
	IMPORT_C void AddCertL(const TPtrC8& aEncodedCerts);
	
	/** Tests whether the root certificate of the chain is locatable.
	*
	* Note that the value is only significant after a successfull call to ValidateL().
	* 
	* @return	ETrue if the chain has a root; EFalse, otherwise. */
	IMPORT_C TBool ChainHasRoot() const;

	/** Returns a list of the critical extension OIDs that are supported by the 
	* chain validator. If a critical extension is encountered in a certificate 
	* chain whose OID matches an element in this set then the chain validator 
	* shall treat this as a warning instead of an error. 
	* 
	* If CPKIXCertChain::SetSupportedCriticalExtensionsL() has not been called, this 
	* list will return the default set of supported critical extensions which 
	* includes the X.509 standard and Symbian specific SIS file critical extensions. 
	* These extensions may change in the future and should not be relied upon.
	* 
	* @return	The current list of supported critical extension OIDs. Ownership is not 
	* transferred to the caller. */
	IMPORT_C const RPointerArray<TDesC>& SupportedCriticalExtensions() const;
	
	/** Adds one or more critical extension OIDs to the list of supported critical 
	* extensions. Duplicate OID values are not added.
	* 
	* @param aCriticalExtOids	A list of the critical extensions OIDs to append to the supported 
	* list. Ownership is not transferred from the caller. */
	IMPORT_C void AddSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids);
	
	/** Removes one or more critical extension OIDs from the list of supported critical extensions.
	* 
	* @param aCriticalExts	A list of the critical extensions OIDs to remove from the supported list. 
	* Ownership is with the original caller. Oids will not be destroyed. */
	IMPORT_C void RemoveSupportedCriticalExtensions(const RPointerArray<TDesC>& aCriticalExtOids);

	/** Completely replaces the set of supported critical extensions for certificate validation. If a critical 
	* extension is encountered matching one of these OIDs then its occurrence is treated as a warning rather 
	* than an error.  The results of which can be queried through a call to CPKIXValidationResult::ValidationWarnings().
	* 
	* @param aCriticalExtOids	A list of the critical extensions OIDs for the class to support. Ownership is
	* not transferred from the caller. */
	IMPORT_C void SetSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids);

	/** Resets the current list of supported critical extensions and re-populates it with the default set
	* which includes the X.509 standard and Symbian specific SIS file critical extensions. These extensions 
	* may change in the future and should not be relied upon. */
	IMPORT_C void ResetSupportedCriticalExtsToDefaultL();

	/** Specify if a failed check on the certificate validity date is treated as an error or a warning.
	*
	* @param aIsFatal  ETrue for reporting as an error; EFalse for a warning.*/
	IMPORT_C void SetValidityPeriodCheckFatal(TBool aIsFatal);

 private:
	CPKIXCertChain();
	void ConstructL(RFs& aFs, const TPtrC8& aEncodedCerts, TUid aClient);
	void ConstructL(RFs& aFs, const TPtrC8& aEncodedCerts,
					const RPointerArray<CX509Certificate>& aRootCerts);

 private:
	CPKIXCertChainHelper* iHelper;
	};

#endif
