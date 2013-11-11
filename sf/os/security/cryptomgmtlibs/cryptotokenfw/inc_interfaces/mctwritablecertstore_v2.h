/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* MCTWritableCertStore.h (v.2)
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __MCTWRITABLECERTSTORE_H__
#define __MCTWRITABLECERTSTORE_H__

#include <mctcertstore.h>

/**
 * The UID of writeable certificate store interfaces.
 *
 * A token that supports this interface should also support the read-only certificate 
 * store interface.
 */
const TInt KInterfaceWritableCertStore = 0x102020FB; // new version, since 9.0

/**
 * Defines the interface for a writeable certificate store token.
 * 
 * This extends the read-only certificate store API in MCTCertStore by adding 
 * functions to add and delete certificates, and to set their applicability and 
 * trust settings. 
 *
 * This documentation describes the security policy that must be enforced by
 * implementations of the interface.
 * 
 * @publishedPartner
 * @released
 */
class MCTWritableCertStore : public MCTCertStore
	{
public:
	/**
	 * Adding a certificate
	 */

	/**
	 * Adds a certificate to the store.
	 * 
	 * This is an asynchronous request.	
	 * 
	 * @param aLabel				The label of the certificate to add.
	 * @param aFormat				The format of the certificate.
	 * @param aCertificateOwnerType	The owner type.
	 * @param aSubjectKeyId			The Subject key ID.
	 * @param aIssuerKeyId			The issuer key ID.
	 * @param aCert					The certificate to be added.
	 * @param aStatus				The request status object; contains the result of the Add() 
	 * 								request when complete. Set to KErrCancel, if an outstanding 
	 * 								request is cancelled.
	 *
	 * @capability WriteUserData	This requires the WriteUserData capability when
	 *								applied to user certificates.
	 * @capability WriteDeviceData	This requires the WriteDeviceData capability
	 *								when applied to CA certificates.
	 * @leave KErrPermissionDenied	If the caller doesn't have the required capabilities.
	 */
	virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
					 TCertificateOwnerType aCertificateOwnerType, 
					 const TKeyIdentifier* aSubjectKeyId,
					 const TKeyIdentifier* aIssuerKeyId,
					 const TDesC8& aCert, TRequestStatus& aStatus) = 0;
		
	/** Cancels an ongoing Add() operation. */
	virtual void CancelAdd() = 0;

	/**
	 * Removing Certificates
	 */
	
	/**
	 * Removes a certificate.
	 * 
	 * @param aCertInfo	The certificate to be removed.
	 * @param aStatus	The request status object; contains the result of the Remove() 
	 * 					request when complete. Set to KErrCancel, if an outstanding request is cancelled.
	 *
	 * @capability WriteUserData	This requires the WriteUserData capability when
	 *								applied to user certificates.
	 * @capability WriteDeviceData	This requires the WriteDeviceData capability
	 *								when applied to CA certificates.
	 * @leave KErrPermissionDenied	If the caller doesn't have the required capabilities.
	 */
	virtual void Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing Remove() operation. */
	virtual void CancelRemove() = 0;

	/**
	 * Setting applicability
	 */
		
	/**
	 * Replaces the current applicability settings with the settings in the
	 * supplied array.
	 * 
	 * This should only be called for CA certificates - it has no meaning for
	 * user certificates.
	 * 
	 * If this function is called by the unified certstore the given application
	 * uids array is guaranteed not to contain duplicates. However, client
	 * applications may bypass the unified certstore and call this function
	 * directly, in that case the array passed might contain duplicates.
	 * 
	 * @param aCertInfo		The certificate whose applicability should be updated.
	 * @param aApplications	The new applicability settings. Ownership of this
	 * 						remains with the caller, and it must remain valid for the
	 * 						lifetime of the call.
	 * @param aStatus		The request status object; contains the result of the SetApplicability() 
	 * 						request when complete. Set to KErrCancel, if an outstanding request is cancelled.
	 *
	 * @capability WriteDeviceData	This requires the WriteDeviceData capability.
	 * @leave KErrPermissionDenied	If the caller doesn't have the required capabilities.
	 */
	virtual void SetApplicability(const CCTCertInfo& aCertInfo, 
						  const RArray<TUid>& aApplications, TRequestStatus &aStatus) = 0;

	/** Cancels an ongoing SetApplicability() operation. */
	virtual void CancelSetApplicability() = 0;

	/**
	 * Changing trust settings
	 */

	/**
	 * Changes the trust settings.
	 * 
	 * A CA certificate is trusted if the user is willing to use it for authenticating 
	 * servers. It has no meaning with other types of certificates.
	 * 
	 * @param aCertInfo	The certificate to be updated.
	 * @param aTrusted	ETrue, if trusted; EFalse, otherwise.
	 * @param aStatus	The request status object; contains the result of the SetTrust() 
	 * 					request when complete. Set to KErrCancel, if an outstanding request is cancelled.
	 *
	 * @capability WriteDeviceData	This requires the WriteDeviceData capability.
	 * @leave KErrPermissionDenied	If the caller doesn't have the required capabilities.
	 */
	virtual void SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted, 
						  TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing SetTrust() operation. */
	virtual void CancelSetTrust() = 0;
			
	/**
	 * Adding a certificate
	 */
	
	/**
	 * Same as original Add() method above, but with additional parameter TBool aDeletable.
	 *
	 * @param aLabel				The label of the certificate to add.
	 * @param aFormat				The format of the certificate.
	 * @param aCertificateOwnerType	The owner type.
	 * @param aSubjectKeyId			The Subject key ID.
	 * @param aIssuerKeyId			The issuer key ID.
	 * @param aCert					The certificate to be added.
	 * 
	 * @param aDeletable			Sets the value for the certificate's deletable flag
	 * 									= true 	- means it is permitted to remove the
	 *												certificate from certstore
	 * 									= false - means the certificate is NOT deletable.
	 *
	 * @param aStatus				The request status object;
	 * 								contains the result of the Add() request when complete. 
	 *								Two of possible error values:
	 *									= KErrCancel, if an outstanding request is cancelled;
	 *									= KErrNotSupported (-5), if the method is called from a
	 *										child class that doesn't support implementation of
	 *										the new Add() method.
	 *
	 * @capability WriteUserData	This requires the WriteUserData capability when
	 *								applied to user certificates.
	 * @capability WriteDeviceData	This requires the WriteDeviceData capability
	 *								when applied to CA certificates.
	 * @leave KErrPermissionDenied	If the caller doesn't have the required capabilities.
	 */
	virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
					 TCertificateOwnerType aCertificateOwnerType, 
					 const TKeyIdentifier* aSubjectKeyId,
					 const TKeyIdentifier* aIssuerKeyId,
					 const TDesC8& aCert, 
					 const TBool aDeletable,
					 TRequestStatus& aStatus );
	
	};


#include <mctwritablecertstore.inl>

#endif
