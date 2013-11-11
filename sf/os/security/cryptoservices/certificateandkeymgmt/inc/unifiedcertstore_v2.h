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
* UNIFIEDCERTSTORE.H
* The unified certificate store implementation
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __UNIFIEDCERTSTORE_H__
#define __UNIFIEDCERTSTORE_H__

class CCertificate;

#include <signed.h>
#include <e32std.h>
#include <f32file.h>
#include <e32property.h>
#include <sacls.h>
#include <mcertstore.h>
#include <ct/rmpointerarray.h>
#include <ct/rcpointerarray.h>




/**  
 *
 * Publish and Subscribe - UnifiedCertSTore Category
 * Aliased here to System Category to prevent SC break.
 *
 */
const TUid KUnifiedCertStorePropertyCat = {KUidSystemCategoryValue};

/**  
 *
 * Publish and subscribe key for UnifiedCertSTore.
 */
enum TUnifiedCertStoreKey
	{
	/**  
 	 *
 	 * The Publish and subscribe key for the certstore changes.
	 * If the client of the UnifiedCertstore needs to be notified when
	 * certificate addition, removal and trust&application setting occurs,
	 * the client needs to subscribe to KUnifiedCertStorePropertyCat and
	 * EUnifiedCertStoreFlag.
	 *
	 * Aliased here to KUidUnifiedCertstore flag to avoid SC break.
 	 */
 	EUnifiedCertStoreFlag = KUidUnifiedCertstoreFlag
 	};

/**
 *
 * The Unique ID for unknown hardware certstore, used as the input parameter when it is to be filtered.
 * @deprecated. Used only for Data compatibility.
 */

const TInt KUnknownHardwareCertStore = 0;


/**
 *
 * The Unique ID for mutable software certstore, used as the input parameter when it is to be filtered.
 *
 */

const TInt KThirdPartyCertStore = 1;


/**
 *
 * The Unique ID for SIM certstore, used as the input parameter when it is to be filtered.
 *
 */

const TInt KSIMCertStore  = 2;

/**
 *
 * The Unique ID for WIM certstore, used as the input parameter when it is to be filtered.
 *  
 */

const TInt KWIMCertStore  = 3;

/**  
 *
 * The Unique ID for UICC certstore, used as the input parameter when it is to be filtered.
 *  
 */
const TInt KUICCCertStore = 4;

/**  
 *
 * The Unique ID for immutable software certstore, used as the input parameter when it is to be filtered.
 *  
 */

const TInt KManufactureCertStore = 5;

// Forward declarations
class MCTCertStore;
class MCTWritableCertStore;
class MCTTokenInterface;
class MCTToken;
class MCTTokenType;
class CCTCertInfo;
class CCertAttributeFilter;
class CCTTokenTypeInfo;
class TCTTokenObjectHandle;
class CCheckedCertStore;

// This class is forward declared to avoid including its definition in this
// exported header file because it must only be used internally.
class CUnifiedCertStoreWorkingVars;
class CX500DistinguishedName;

/**
 * The unified certificate store.
 * 
 * This class provides a certificate store whose contents are the sum of the
 * contents of all certificate store implementations on the device.  It is
 * intended as the single point of access for clients wishing to use certificate
 * stores.
 *
 * Since this class is intended for widespread use, capability checks relating
 * to certificate access are documented here even though the checks are actually
 * made in the individual cert store implementations.
 * 
 */
NONSHARABLE_CLASS(CUnifiedCertStore) : public CActive, public MCertStore
	{
public:
	/** 
	 * Creates a new CUnifiedCertStore
	 *
	 * @param aFs			A file server session. It must already be open.
	 * @param aOpenForWrite	ETrue if the stores must be opened with write access
	 *						(e.g. for adding certificates) and EFalse if the user 
	 *						only needs read-only access.
	 * @return				A pointer to an instance of the CUnifiedCertStore class.
	 */
	IMPORT_C static CUnifiedCertStore* NewL(RFs& aFs, TBool aOpenForWrite);
	
	/** 
	 * Creates a new CUnifiedCertStore and pushes it on the cleanup stack.
	 *
	 * @param aFs			A file server session. It must already be open.
	 * @param aOpenForWrite	ETrue if the stores must be opened with write access
	 *						(e.g. for adding certificates) and EFalse if the user
	 *						only needs read-only access.
	 * @return				A pointer to an instance of the CUnifiedCertStore class.
	 */
	IMPORT_C static CUnifiedCertStore* NewLC(RFs& aFs, TBool aOpenForWrite);
 	/** 
 	 * Creates a new CUnifiedCertStore with the sequence filter, so that multiple certstores that are managed
	 * by it will be filtered and ordered.
 	 *
 	 * @param aFs			A file server session. It must already be open.
 	 * @param aOpenForWrite	ETrue if the stores must be opened with write access
 	 *						(e.g. for adding certificates) and EFalse if the user
 	 *						only needs read-only access. Ownership is taken.
 	 * @param aOrderFilter  An array of the unique sequence IDs specifying CertStore ordering.
 	 * @return				A pointer to an instance of the CUnifiedCertStore class.
 	 */
 	IMPORT_C static CUnifiedCertStore* NewL(RFs& aFs, 
 	                                        TBool aOpenForWrite,
 	                                        RArray<TInt>& aOrderFilter);
 	/** 
 	 * Creates a new CUnifiedCertStore with the sequence filter, so that multiple certstores that are managed
	 * by it will be filtered and ordered, and it is pushed on the cleanup stack.
 	 *
 	 * @param aFs			A file server session. It must already be open.
 	 * @param aOpenForWrite	ETrue if the stores must be opened with write access
 	 *						(e.g. for adding certificates) and EFalse if the user
 	 *						only needs read-only access. Ownership is taken.
 	 * @param aOrderFilter  An array of the unique sequence IDs specifying CertStore ordering.
 	 * @return				A pointer to an instance of the CUnifiedCertStore class.
 	 */
 	IMPORT_C static CUnifiedCertStore* NewLC(RFs& aFs, 
 	                                         TBool aOpenForWrite,
 	                                         RArray<TInt>& aOrderFilter);
	
	/**
	 * The destructor destroys all the resources owned by this object.
	 */
	IMPORT_C ~CUnifiedCertStore();
	
	/**
	 * Initializes the manager. 
	 * 
	 * It must be called after the manager has been constructed
	 * and before any call to the manager functions.
	 * 
	 * This is an asynchronous request.
	 * 
	 * @param aStatus	The request status object; contains the result of the Initialize() 
	 * 					request when complete. Set to KErrCancel if any outstanding request is cancelled.
	 */
	IMPORT_C void Initialize(TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing Initialize() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelInitialize();

public:	// Implementation of MCertStore interface

	/** Lists all certificates that satisfy the supplied filter.
	*
	* @param aCertInfos	An array that the returned certificates are added to .
	* @param aFilter	A filter to restrict which certificates are returned.
	* @param aStatus	The request status object.
	* 
	*/
	virtual void List(RMPointerArray<CCTCertInfo>& aCertInfos,
					  const CCertAttributeFilter& aFilter, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle, 
						 TRequestStatus& aStatus);
	virtual void CancelGetCert();

	/** Gets the list of applications . Applications are represented by UIDs .
	* 	
	* @param aCertInfos		An array of certificates .
	* @param aApplications	An array that the returned application UIDs are added to.
	* @param aStatus		The request status object.
	*
	*/
	virtual void Applications(const CCTCertInfo& aCertInfo, 
							  RArray<TUid>& aApplications, TRequestStatus &aStatus);
	virtual void CancelApplications();
	virtual void IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication, 
							  TBool& aIsApplicable, TRequestStatus& aStatus);
	virtual void CancelIsApplicable();
	virtual void Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted, 
						 TRequestStatus& aStatus);
	virtual void CancelTrusted();
	virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert,
						  TRequestStatus& aStatus);
	virtual void CancelRetrieve();

public:	// Functions defined in MCTWritableCertStore except Add functions

	/**
	 * Removes a certificate.
	 * 
	 * @param aCertInfo				The certificate to be removed.
	 * @param aStatus				The request status object; contains the result of the Remove() 
	 * 								request when complete. Set to KErrCancel if an outstanding request is cancelled.
	 *
	 * @capability WriteUserData	This requires the WriteUserData capability when
	 *								applied to user certificates.
	 * @capability WriteDeviceData	This requires the WriteDeviceData capability
	 *								when applied to CA certificates.
	 * @leave KErrPermissionDenied	If the caller doesn't have the required
	 *								capabilities.
	 */
	IMPORT_C void Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
	
	/** 
	 * Cancels an ongoing Remove() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelRemove();
	
	/**
	 * Replaces the current applicability settings with the settings in the
	 * supplied array.
	 * 
	 * This should only be called for CA certificates - it has no meaning for
	 * user certificates.
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
	IMPORT_C void SetApplicability(const CCTCertInfo& aCertInfo, 
		const RArray<TUid>& aApplications, TRequestStatus &aStatus);
	
	/** 
	 * Cancels an ongoing SetApplicability() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelSetApplicability();
	
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
	 * @capability WriteDeviceData This requires the WriteDeviceData capability.
	 * @leave KErrPermissionDenied If the caller doesn't have the required capabilities.
	 */
	IMPORT_C void SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted, 
		TRequestStatus& aStatus);
	
	/** 
	 * Cancels an ongoing SetTrust() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelSetTrust();

public:
	/**
	 * Lists all certificates that have a particular subject DN.
	 * 
	 * @param aCertInfos	An array that the returned certificates are added to
	 * @param aFilter		A filter to restrict which certificates are returned.
	 * @param aIssuer		Only certificates with this issuer DN will be returned
	 * @param aStatus		Asynchronous request status.
	 */
	IMPORT_C void List(RMPointerArray<CCTCertInfo>& aCertInfos,
					   const CCertAttributeFilter& aFilter, 
					   const TDesC8& aIssuer, 
					   TRequestStatus& aStatus);

	/**
	 * Lists all certificates that have a particular issuer.
	 * 
	 * @param aCertInfos	An array that the returned certificates are added to
	 * @param aFilter		A filter to restrict which certificates are returned.
	 * @param aIssuers		Only certificates with this issuer will be returned
	 * @param aStatus		Asynchronous request status.
	 */
	IMPORT_C void List(RMPointerArray<CCTCertInfo>& aCertInfos,
					   const CCertAttributeFilter& aFilter, 
					   RPointerArray<const TDesC8> aIssuers, 
					   TRequestStatus& aStatus);

	/**
	 * Retrieves a certificate as a parsed object.
	 *
	 * This will only work for certificates that have a CCertificate-derived
	 * representation, in other words X509 and WTLS certificates.  If called for
	 * a URL certificate, KErrNotSupported is returned.
	 * 
	 * @param aCertInfo	The certificate to retrieve
	 * @param aCert		The returned certificate.  This object can safely be up-cast
	 *					to a CX509Certificate or CWTLSCertificate if it's known that
	 *					that is the certificate format.
	 * @param aStatus	Asynchronous request status.
	 * 
	 * @capability ReadUserData		This requires the ReadUserData capability when
	 *								applied to user certificates, as these may contain
	 *								sensitive user data.
	 * @leave KErrPermissionDenied	If called for a user certificate when the
	 *								caller doesn't have the ReadUserData capability.
	 */
	IMPORT_C void Retrieve(const CCTCertInfo& aCertInfo, CCertificate*& aCert,
						   TRequestStatus& aStatus);
	
	/** 
	 * Gets the number of certificate stores.
	 *
	 * @return	The total number of certificate stores.
	 */
	IMPORT_C TInt CertStoreCount() const;
	
	/**
	 * Gets a particular certificate store.
	 * 
	 * @param aIndex	The index of the required certificate store.
	 *					A number between 0 and CertStoreCount() - 1.
	 * @return			The certificate store.
	 */
	IMPORT_C MCTCertStore& CertStore(TInt aIndex);
	
	/** 
	 * Gets the number of writeable certificate stores.
	 *
	 * @return	The number of writeable certificate stores.
	 */
	IMPORT_C TInt WritableCertStoreCount() const;
	
	/**
	 * Gets a particular writeable certificate store.
	 *
	 * @param aIndex	The index of the required certificate store.
	 *					A number between 0 and WriteableCertStoreCount() - 1.
	 * @return			The writeable certificate store.
	 */
	IMPORT_C MCTWritableCertStore& WritableCertStore(TInt aIndex);
	
	/** 
	 * Gets the number of read-only certificate stores.
	 *
	 * @return	The number of read-only certificate stores.
	 */
	IMPORT_C TInt ReadOnlyCertStoreCount() const;
	
	/**
	 * Gets a particular read-only certificate store.
	 * 
	 * @param aIndex	The index of the required certificate store.
	 *					A number between 0 and ReadOnlyCertStoreCount() - 1.
	 * @return			The read-only certificate store.
	 */
	IMPORT_C MCTCertStore& ReadOnlyCertStore(TInt aIndex);

private:
	enum TState
		{
		EIdle,

		EInitializeGetTokenList,
		EInitializeGetToken,
		EInitializeGetWritableInterface,
		EInitializeGetReadableInterface,
		EInitializeGetReadableInterfaceFinished,
		EInitializeFinished,

		EList,
		ERetrieve,
		ERetrieveForList,

		EGetCert,
		EApplications,
		EIsApplicable,
		ETrusted,
		ERetrieveData,
		ERemove,
		ESetApplicability,
		ESetTrust
		};
	enum TCompareResults
		{
		ENo,
		EYes,
		EMaybe
		};
private:
	CUnifiedCertStore(RFs& aFs, TBool aOpenForWrite);
	void ConstructL(RArray<TInt>& aOrderFilter);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	// Implementations for asynchronous operations
	void InitializeL();
	void ListL(RMPointerArray<CCTCertInfo>& aCertInfos,
			   const CCertAttributeFilter& aFilter);
	void ListL(RMPointerArray<CCTCertInfo>& aCertInfos,
			  const CCertAttributeFilter& aFilter, 
			  RPointerArray<const TDesC8> aIssuers);
	void RetrieveL(const CCTCertInfo& aCertInfo, CCertificate*& aCert);
	void GetCertL(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle);
	void ApplicationsL(const CCTCertInfo& aCertInfo, RArray<TUid>& aApplications);
	void IsApplicableL(const CCTCertInfo& aCertInfo, TUid aApplication, 
					   TBool& aIsApplicable);
	void TrustedL(const CCTCertInfo& aCertInfo, TBool& aTrusted);
	void RetrieveDataL(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert);
	void RemoveL(const CCTCertInfo& aCertInfo);
	void SetApplicabilityL(const CCTCertInfo& aCertInfo,
						   const RArray<TUid>& aApplications);
	void SetTrustL(const CCTCertInfo& aCertInfo, TBool aTrusted);

	// Helper functions
	void AllocWorkingVarsL();
	void BeginAsyncOp(TRequestStatus& aStatus, TState aState);	
	void DestroyTemporaryMembers();
	MCTCertStore* GetCertStore(const TCTTokenObjectHandle& aHandle);
	void FindCertStoreL(const TCTTokenObjectHandle& aHandle);
	void FindWritableCertStoreL(const TCTTokenObjectHandle& aHandle);
	TCompareResults CompareCertInfoDN(const CCTCertInfo* aCertInfo);
	TBool MatchL(const CX500DistinguishedName& aName) const;
	void Complete(TInt aError);
	void CancelOutstandingRequest();
 	
 	// Filters CertStores according to specified order.
 	void ApplyOrderingL(RCPointerArray<CCTTokenTypeInfo>& aTokenTypes);
 	
 	void FilterTokenTypesL(RCPointerArray<CCTTokenTypeInfo>& aSearchTokenTypes,
						RCPointerArray<CCTTokenTypeInfo>& aTempTokenTypes,
						TInt aOrderAttribute);
 
private:
	/**
	 * A file server session, this is not logically a part of this class
	 * but is needed for the client store and the file certstore.
	 */
	RFs& iFs;

	TBool iOpenedForWrite;

	RPointerArray<CCheckedCertStore> iReadOnlyCertStores;
	RPointerArray<CCheckedCertStore> iWritableCertStores;
	RPointerArray<CCheckedCertStore> iCertStores;

	TBool iCurrentlyDoingReadOnly;

	/**
	 * This a TokenType retrieved from the iTokenTypes array.
	 * We use this to get a list of Tokens and to open these Tokens.
	 */
	MCTTokenType* iTokenType;

	/**
	 * This is the list of Tokens for one of the Token Types of iTokenTypes.
	 */
	RCPointerArray<HBufC> iTokens;

	/** All the UIDs of hardware token types */
	RArray<TUid> iHardwareTypeUids;
	/**
	 * This is used as an index for the iTokens array when we try 
	 * to get an interface to each of the tokens.
	 */
	TInt iIndexTokens;

	/**
	 * A Token interface. We will use the interface to get a readable or writable
	 * certstore interface. The value is updated at EInitializeGetToken and used
	 * at EInitializeGetWritableInterface.
	 */
	MCTToken* iToken;

	/**
	 * We use this to (temporarily) store the interface we obtained from iToken.
	 * It will be inserted in iWritableCertStores or iCertStores.
	 */
	MCTTokenInterface* iTokenInterface;

 	/**
 	The index of the plugin certstore that is being processed
 	*/
	TInt iIndex;

	/**
	 * This is the status of the caller of an asynchronous function. It must be set
	 * to KRequestPending by the function while doing the processing.
	 */
	TRequestStatus* iClientStatus;

	TState iState;

	TBool iIsInitialized;

	/**
	 * This member holds all the variables that are only used to store temporary results
	 * while performing a given operation. It must be initialized at the start of the
	 * operation and deleted at the end of it whether the opeartion completes successfully
	 * or not. When no operation is being performed it must be 0.
	 */
	CUnifiedCertStoreWorkingVars* iWorkingVars;

	/**
	 * The cert store in use by an outstanding async operation.
	 */
	MCTCertStore *iCurrentCertStore;

	/**
	 * The writable cert store in use by an outstanding async operation.
	 */
	MCTWritableCertStore *iCurrentWritableCertStore;

	// Padding to keep class size constant
	TInt32 iUnused1;
	TInt32 iUnused2;
	                  
 	// An array of Uids specifying Token Type ordering
 	RArray<TInt> iOrderAttributes;
 	
 	// Publish and subscribe property which is used to notify the 
	// cerificate addition,removal and application&trust setting.
 	RProperty iPSCertstoreChangeProperty;

	};

#endif
