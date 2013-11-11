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
* PKIXVALIDATIONRESULT.H
* Implementation of the class storing PKIX validation result information 
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __PKIXVALIDATIONRESULT_H__
#define __PKIXVALIDATIONRESULT_H__

#include <x509certchain.h>
#include <x509certext.h>

class RReadStream;
/**
 * Base class for CPKIXValidationResult.
 */
class CPKIXValidationResultBase : public CBase
	{
public:
	/** Creates a new PKIX Validation Result object.
	* 
	* @return	The new PKIX Validation Result object. */
	IMPORT_C static CPKIXValidationResultBase* NewL();
	
	/** Creates a new PKIX Validation Result object, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @return	The new PKIX Validation Result object. */
	IMPORT_C static CPKIXValidationResultBase* NewLC();
	
   /** Creates a new PKIX Validation Result object from a stream.
	* 
	* @return	The new PKIX Validation Result object. */
	IMPORT_C static CPKIXValidationResultBase* NewL(RReadStream& aStream);
	
	/** Creates a new PKIX Validation Result object from a stream, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @return	The new PKIX Validation Result object. */
	IMPORT_C static CPKIXValidationResultBase* NewLC(RReadStream& aStream);

	/** The destructor.
	* 
	* Frees all resources owned by the object. */
	IMPORT_C ~CPKIXValidationResultBase();
	
	/** Gets the error status of the operation.
	* 
	* Errors are considered fatal, i.e. validation has failed.
	* 
	* @return	The error status of the operation. */
	IMPORT_C const TValidationStatus Error() const;
	
	/** Gets a list of warnings generated.
	* 
	* The warnings may or may not be fatal, depending on the context, which the 
	* client is expected to provide.
	* 
	* @return	An array of any warnings generated. */
	IMPORT_C const CArrayFixFlat<TValidationStatus>& Warnings() const;
	
	/** Gets a list of all the certificate policies which have been accepted implicitly 
	* in the course of validation.
	* 
	* @return	An array of certificate policies. */
	IMPORT_C const CArrayPtrFlat<CX509CertPolicyInfo>& Policies() const;
	
	/** Resets the validation result object to its default values. 
	 * @internalTechnology
	 */
	void Reset();
	
	/** Sets the error status, and then leaves.
	* 
	* The function uses SetError() to set the error status.
	* 
	* @param aError	The error type that occurred when validating the certificate chain.
	* @param aCert	The index number identifying the certificate that gave rise to 
	* 				the error. 
    * @internalTechnology
	*/
	void SetErrorAndLeaveL(const TValidationError aError, const TInt aCert);
	
	/** Sets the error status.
	* 
	* @param aError	The error type that occurred when validating the certificate chain.
	* @param aCert	The index number identifying the certificate that gave rise to 
	* 				the error. 
	* @internalTechnology
	*/
	void SetError(const TValidationError aError, const TInt aCert);
	
	/** Adds a warning to the validation result.
	* 
	* @param aWarning	The warning to be added. 
	* @internalTechnology
	*/
	void AppendWarningL(TValidationStatus aWarning);
	
	/** Adds a policy to the validation result.
	* 
	* @param aPolicy	The policy to be added.
	* @internalTechnology
	*/
	void AppendPolicyL(CX509CertPolicyInfo& aPolicy);

	/** Removes all policies from the validation result.
	* 
	* It is used by the validation process to remove policies it has added when 
	* the computation cannot complete because of environmental conditions such as 
	* out of memory, file access failures, etc..
	* @internalTechnology
	*/
	// (not like signature validation...)
	void RemovePolicies();

	// Internalization/Externalization
	// Externalize. Writes the data out to a stream
	/** Externalises an object of this class to a write stream.
	* 
	* The presence of this function means that the standard templated operator<<() 
	* can be used to externalise objects of this class.
	* 
	* @param aStream	Stream to which the object should be externalised. */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	// Internalize. Reads the data from a stream 
	/** Internalises an object of this class from a read stream.
	* 
	* The presence of this function means that the standard templated operator>>() 
	* can be used to internalise objects of this class.
	* 
	* Note that this function has assignment semantics: it replaces the old value 
	* of the object with a new value read from the read stream.	
	* 
	* @param aStream	Stream from which the object should be internalised. */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	/** Returns a list of critical extensions encountered and warnings generated by
	* certificates during chain validation.
	* 
	* @return	An array of certificate warnings. */
	IMPORT_C const RPointerArray<CCertificateValidationWarnings>& ValidationWarnings() const;
	
	/** Adds a certificate warning to the validation result.
	* 
	* @param aCertWarning	The warning to be added. */
	IMPORT_C void AppendCertificateValidationObjectL(const CCertificateValidationWarnings& aCertWarning);
	
	/** Adds a critical extension OID warning.
	* 
	* @param aCriticalExt	The critical extension OID to be added. */
	IMPORT_C void AppendCriticalExtensionWarningL(TDesC& aCriticalExt);
	
protected:
	IMPORT_C CPKIXValidationResultBase();
	IMPORT_C void ConstructL();
	
private:
	TValidationStatus iError;
	CArrayFixFlat<TValidationStatus>* iWarnings;
	CArrayPtrFlat<CX509CertPolicyInfo>* iPolicies;
	RPointerArray<CCertificateValidationWarnings> iCertWarnings;
	};

/** Stores the information regarding the results of a validation of a PKIX 
* certificate chain. 
*
* @since v6.0 */
class CPKIXValidationResult : public CPKIXValidationResultBase
	{
public:
	/** Creates a new PKIX Validation Result object.
	* 
	* @return	The new PKIX Validation Result object. */
	IMPORT_C static CPKIXValidationResult* NewL();
	
	/** Creates a new PKIX Validation Result object, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @return	The new PKIX Validation Result object. */
	IMPORT_C static CPKIXValidationResult* NewLC();
	
	/** The destructor.
	* 
	* Frees all resources owned by the object. */
	IMPORT_C ~CPKIXValidationResult();
	
	/** Gets the error status of the operation.
	* 
	* Errors are considered fatal, i.e. validation has failed.
	* 
	* @return	The error status of the operation. */
	IMPORT_C const TValidationStatus Error() const;
	
	/** Gets a list of warnings generated.
	* 
	* The warnings may or may not be fatal, depending on the context, which the 
	* client is expected to provide.
	* 
	* @return	An array of any warnings generated. */
	IMPORT_C const CArrayFixFlat<TValidationStatus>& Warnings() const;
	
	/** Gets a list of all the certificate policies which have been accepted implicitly 
	* in the course of validation.
	* 
	* @return	An array of certificate policies. */
	IMPORT_C const CArrayPtrFlat<CX509CertPolicyInfo>& Policies() const;
	
	/** Returns a list of critical extensions encountered and warnings generated by
	* certificates during chain validation. A CCertificateWarning object is returned for
	* each certificate in the chain, even if no critical extensions or warnings were 
	* encountered. The array's data is in the same order as the certificate chain.
	* 
	* @return	An array of certificate warnings. Ownership is not transferred 
	* to the caller. */
	IMPORT_C const RPointerArray<CCertificateValidationWarnings>& ValidationWarnings() const;

private:
	CPKIXValidationResult();
	};

#endif
