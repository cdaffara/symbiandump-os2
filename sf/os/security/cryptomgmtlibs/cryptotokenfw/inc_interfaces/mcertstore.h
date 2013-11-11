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
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __MCERTINFO_H__
#define __MCERTINFO_H__

#include <ct/rmpointerarray.h>

// Forward declarations
class CCTCertInfo;
class CCertAttributeFilter;
class TCTTokenObjectHandle;


/**
 * 
 * Defines the interface for a read-only certificate store.
 *
 * This documentation describes the security policy that must be enforced by
 * implementations of the interface.
 */
class MCertStore
	{
public:
	/**
	 * Listing Certificates
	 */

	/**
	 * Get a list of all certificates that satisfy the supplied filter.
	 * 	
	 * This is an async function; all errors are reported by completing aStatus
	 * with the error value, and it can be cancelled with CancelList().
	 * 
	 * @param aCerts An array into which the returned certificates are placed.
	 * @param aFilter A filter to select which certificates should be included.
	 * @param aStatus A request status that will be completed when the operation completes.
	 */	
	virtual void List(RMPointerArray<CCTCertInfo>& aCerts, const CCertAttributeFilter& aFilter,
					  TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing List() operation. */
	virtual void CancelList() = 0;

	/**
	 * Getting a certificate given a handle.
	 */
	
	/**
	 * Get a certificate given its handle.
	 * 
	 * @param aCertInfo The returned certificate.
	 * @param aHandle The handle of the certificate to return.
	 * @param aStatus The request status object; contains the result of the
	 *     GetCert() request when complete. Set to KErrCancel if any outstanding
	 *     request is cancelled.
	 */
	virtual void GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle, 
						 TRequestStatus& aStatus) = 0;
		
	/** Cancel an ongoing GetCert() operation. */
	virtual void CancelGetCert() = 0;
	
	/**
	 * Querying the applications of a certificate.
	 */

	/**
	 * Get the list of the applications associcated with certificate.
	 * 
	 * Applications are represented by UIDs. Examples would be Software Install,
	 * TLS, WTLS, WMLScript, SignText, etc..
	 * 
	 * @param aCertInfo The certificate to return applications for.
	 * @param aAplications An array to save the applications in.
	 * @param aStatus The request status object; contains the result of the
	 *     Applications() request when complete. Set to KErrCancel if any
	 *     outstanding request is cancelled.
	 */
	virtual void Applications(const CCTCertInfo& aCertInfo, RArray<TUid>& aAplications,
							  TRequestStatus& aStatus) = 0;
		
	/** Cancels an ongoing Applications() operation. */
	virtual void CancelApplications() = 0;
	
	/**
	 * Tests if a certificate is applicable to a particular application.	
	 * 
	 * @param aCertInfo The certificate in question.
	 * @param aApplication The application.
	 * @param aIsApplicable Set to ETrue or EFalse by the function to return the result.
	 * @param aStatus The request status object; contains the result of the
	 *     IsApplicable() request when complete. Set to KErrCancel if any
	 *     outstanding request is cancelled.
	 */
	virtual void IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication, 
							  TBool& aIsApplicable, TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing IsApplicable() operation. */
	virtual void CancelIsApplicable() = 0;

	/**
	 * Trust querying
	 */
	
	/**
	 * Tests whether a certificate is trusted.
	 * 
	 * Trust is only meaningful for CA certificates where it means that the
	 * certificate can be used as a trust root for the purposes of certificate
	 * validation.
	 * 
	 * @param aCertInfo The certificate we are interested in.
	 * @param aTrusted Used to return the trust status.
	 * @param aStatus The request status object; contains the result of the
	 *     Trusted() request when complete. Set to KErrCancel if any outstanding
	 *     request is cancelled.
	 */
	virtual void Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted, 
						 TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing Trusted() operation. */
	virtual void CancelTrusted() = 0;

	/**
	 * Retrieving the actual certificate
	 */

	/**
	 * Retrieves the actual data of the certificate.	
	 *
	 * @param aCertInfo The certificate to retrieve.
	 * @param aEncodedCert A buffer to put the certificate in. It must be big
	 *     enough; the size is stored in aCertInfo.
	 * @param aStatus The request status object; contains the result of the
	 *     Retrieve()request when complete. Set to KErrCancel if any outstanding
	 *     request is cancelled.
	 *
	 * @capability ReadUserData This requires the ReadUserData capability when
	 *     applied to user certificates, as these may contain sensitive user data.
	 * @leave KErrPermissionDenied If called for a user certificate when the
	 *     caller doesn't have the ReadUserData capability.
	 */
	virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert, 
						  TRequestStatus& aStatus) = 0;
		
	/** Cancels an ongoing Retrieve() operation. */
	virtual void CancelRetrieve() = 0;
		
	};


#endif
