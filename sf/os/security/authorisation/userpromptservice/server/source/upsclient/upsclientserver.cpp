/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements an interface to keep and access decision record fields.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
#include <s32strm.h>
#include <ups/upstypes.h>
#include <ups/upsconst.h>
#include "upslog.h"
#include <ups/upserr.h>
#include "upscommon.h"

namespace UserPromptService {

CDecisionRecord::CDecisionRecord()
/**
	Constructor for the decision record. 
 */								 
	{
	
	}

CDecisionRecord::CDecisionRecord(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
								 const TUint8 aResult, const TUint16& aMajorPolicyVersion, const TUint32 aEvaluatorInfo, const TUint32& aRecordId)
/**
	Constructor for the decision record. Assigns the ID values. 
 */								 
	:iClientSid(aClientSid),iEvaluatorId(aEvaluatorId),
	 iServiceId(aServiceId),iServerSid(aServerSid),
	 iResult(aResult),iMajorPolicyVersion(aMajorPolicyVersion),
	 iRecordId(aRecordId),iEvaluatorInfo(aEvaluatorInfo)
	{
	
	}


CDecisionRecord::~CDecisionRecord()
/**
	Destructor for the decision record object
 */
	{
	iFingerprint.Close();		
	iClientEntity.Close();
	iDescription.Close();
	}
	
EXPORT_C CDecisionRecord* CDecisionRecord::NewL(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
								 	            const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TDesC& aDescription, const TUint8 aResult, 
								 	            const TUint16& aMajorPolicyVersion, const TUint32 aEvaluatorInfo, const TUint32& aRecordId)
/**
	Creates a decision record object. The function leaves, if creation of the record object fails.
	 
	@param aClientSid	  Secure Id of client application
	@param aEvaluatorId	  UID for the policy evaluator DLL
	@param aServiceId 	  UID for service e.g. sms, mms, telephony, gprs
	@param aServerSid  	  Secure Id for the system server that the decision applies to
	@param aFingerprint   Hash of the destination and/or opaque data. Maximum length is 32 bytes.
	@param aClientEntity  The name of the entity within the client that requested the service. Maximum length is 32 bytes. 
	@param aDescription	  A copy of description and/or opaque. 
	@param aResult		  Whether the request should be approved (=1) or denied (=0)
	@param aMajorPolicyVersion The major version of the policy file.
	@param aEvaluatorInfo Policy evaluator specific data
	@param aRecordId	  An auto-incrementing record number.
	
	@return    			  A pointer to the newly allocated decision record object, if creation is successful.
 */								 	   
	{
	CDecisionRecord* self = CDecisionRecord::NewLC(aClientSid,aEvaluatorId,aServiceId,aServerSid,aFingerprint,aClientEntity,
												   aDescription,aResult,aMajorPolicyVersion,aEvaluatorInfo,aRecordId);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CDecisionRecord* CDecisionRecord::NewLC(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
								 	    		 const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TDesC& aDescription, const TUint8 aResult, 
								 	    		 const TUint16& aMajorPolicyVersion, const TUint32 aEvaluatorInfo, const TUint32& aRecordId)
/**
	Creates a decision record object. The function leaves, if creation of the record object fails.
	
	@return    			  A pointer to the newly allocated decision record object, if creation is successful.
				  		  The pointer is also put onto the cleanup stack.
	@see CDecisionRecord::NewL()
 */									 	   
	{
	CDecisionRecord* self = new (ELeave) CDecisionRecord(aClientSid,aEvaluatorId,aServiceId,aServerSid,aResult,
														 aMajorPolicyVersion,aEvaluatorInfo,aRecordId);
	CleanupStack::PushL(self);
	self->ConstructL(aFingerprint,aClientEntity,aDescription);
	return self;
	}	

EXPORT_C CDecisionRecord* CDecisionRecord::NewLC()
/**
	Creates a decision record object. The function leaves, if creation of the record object fails.
	
	@return    			  A pointer to the newly allocated decision record object, if creation is successful.
				  		  The pointer is also put onto the cleanup stack.
 */									 	   
	{
	CDecisionRecord* self = new (ELeave) CDecisionRecord();
	CleanupStack::PushL(self);
	return self;
	}	
	
void CDecisionRecord::ConstructL(const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TDesC& aDescription)
/**
	Second phase constructor for the decision record. Sets members allocated on the heap.
	The function leaves, if any buffer allocation fails.
	@see CDecisionRecord::NewL()
 */
	{
		
	iFingerprint.CreateL(aFingerprint);
	
	iClientEntity.CreateL(aClientEntity);
	
	iDescription.CreateL(aDescription);

	ValidateL();
	}

void CDecisionRecord::ValidateL()
/**
	Check both the fingerprint and client entity are not too long.
	An absent fingerprint or client entity is represent by an empty string.
*/
	{
	if(iFingerprint.Length()  > (TInt)KUpsMaxFingerprintLength)
	    {
	    DEBUG_PRINTF2(_L8("The fingerprint length (%d) has not been satisfied during decision construction!"),iFingerprint.Length());
	    User::Leave(KErrUpsBadFingerprintLength);
	    }
	    	
	if(iClientEntity.Length() > (TInt)KUpsMaxClientEntityLength)
		{
		DEBUG_PRINTF2(_L8("The client entity length (%d) has not been satisfied during decision construction!"),iClientEntity.Length());
		User::Leave(KErrUpsBadClientEntityLength);
		}
	}

EXPORT_C void CDecisionRecord::ExternalizeL(RWriteStream& aStream) const
/**
	Externalise this CDecisionRecord object to the specified stream.
*/
	{
	aStream.WriteUint32L(iClientSid.iId);
	aStream.WriteInt32L(iEvaluatorId.iUid);
	aStream.WriteInt32L(iServiceId.iUid);
	aStream.WriteUint32L(iServerSid.iId);
	aStream << iFingerprint;
	aStream << iClientEntity;
	aStream << iDescription;
	aStream << iResult;
	aStream << iMajorPolicyVersion;
	aStream << iRecordId;
	aStream << iEvaluatorInfo;
	}

EXPORT_C void CDecisionRecord::InternalizeL(RReadStream& aStream)
/**
	Internalise this CDecisionRecord object from the specified stream.
*/
	{
	iClientSid.iId= aStream.ReadUint32L();
	iEvaluatorId.iUid = aStream.ReadInt32L();
	iServiceId.iUid = aStream.ReadInt32L();
	iServerSid.iId = aStream.ReadUint32L();

	iFingerprint.Close();
	iFingerprint.CreateL(aStream, KMaskDesLength8);

	iClientEntity.Close();
	iClientEntity.CreateL(aStream, KMaskDesLength8);

	iDescription.Close();
	iDescription.CreateL(aStream, KMaskDesLength16);

	aStream >> iResult;
	aStream >> iMajorPolicyVersion;
	aStream >> iRecordId;
	aStream >> iEvaluatorInfo;

	ValidateL();
	}

CDecisionFilter::CDecisionFilter()
/**
	Constructor for an empty decision filter.
 */
	{
	
	}

	
CDecisionFilter::CDecisionFilter(const TSecureId& aClientSid, const TUid& aEvaluatorId, 
	const TUid& aServiceId, const TSecureId& aServerSid, const TUint16& aVersion)
	:iClientSid(aClientSid),iEvaluatorId(aEvaluatorId),iServiceId(aServiceId),
	iServerSid(aServerSid),iMajorPolicyVersion(aVersion)
/**
	Constructor for the decision filter. Assigns the ID values.
 */
	{
	iSetFlag[KPosClientSid]	  = KSetClientSid	| UserPromptService::EEqual;
	iSetFlag[KPosEvaluatorId] = KSetEvaluatorId | UserPromptService::EEqual;
	iSetFlag[KPosServiceId]	  = KSetServiceId	| UserPromptService::EEqual;
	iSetFlag[KPosServerSid]	  = KSetServerSid	| UserPromptService::EEqual;
	iSetFlag[KPosMajorPolicyVersion] = KSetMajorPolicyVersion | UserPromptService::EEqual;
	}
		
				
CDecisionFilter::~CDecisionFilter()
/**
	Destructor for the decision filter
 */
	{
	delete iFingerprint;
	delete iClientEntity;	
	}
	
	
EXPORT_C CDecisionFilter* CDecisionFilter::NewL()
/**
	Creates an empty filter object. The function is used to define any number of filter keys.
	It leaves, if the creation of the filter object fails.
		
	@return	   A pointer to the newly allocated filter object, if creation is successful. 
 */
	{
	CDecisionFilter* self = CDecisionFilter::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
	
EXPORT_C CDecisionFilter* CDecisionFilter::NewLC()
/**
	Creates an empty filter object. The function is used to define any number of filter keys.
	It leaves, if the creation of the filter object fails.

	@return	   A pointer to the newly allocated filter object, if creation is successful. 
			   The pointer is also put onto the cleanup stack.
 */
	{
	CDecisionFilter* self = new (ELeave) CDecisionFilter();
	CleanupStack::PushL(self);
	return self;
	}
	
	
EXPORT_C CDecisionFilter* CDecisionFilter::NewL(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
												const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TUint16& aVersion)
/**
	Creates a filter object and assigns all filter keys. The function leaves, if creation of the filter object fails.
	 
	@param aClientSid	 Secure Id of client application
	@param aEvaluatorId	 UID for the policy evaluator DLL
	@param aServiceId	 UID for service e.g. sms, mms, telephony, gprs
	@param aServerSid	 Secure Id for the system server that the decision applies to
	@param aFingerprint	 Hash of the destination and/or opaque data.
	@param aClientEntity The name of the entity within the client that requested the service. 
	@param aVersion      Major version of policy file.
	@return				 A pointer to the newly allocated filter object, if creation is successful.
 */									
	{
	CDecisionFilter* self = CDecisionFilter::NewLC(aClientSid,aEvaluatorId,aServiceId,aServerSid,aFingerprint,aClientEntity,aVersion);
	CleanupStack::Pop(self);
	return self;
	}
	
	
EXPORT_C CDecisionFilter* CDecisionFilter::NewLC(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
												 const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TUint16& aVersion)
/**
	Creates a filter object and assigns all filter keys. The function leaves, if creation of the filter object fails.
	 
	@param aClientSid	 Secure Id of client application
	@param aEvaluatorId	 UID for the policy evaluator DLL
	@param aServiceId	 UID for service e.g. sms, mms, telephony, gprs
	@param aServerSid	 Secure Id for the system server that the decision applies to
	@param aFingerprint	 Hash of the destination and/or opaque data.
	@param aClientEntity The name of the entity within the client that requested the service.
	@param aVersion      Major version of policy file.
	@return				 A pointer to the newly allocated filter object, if creation is successful.
						 The pointer is also put onto the cleanup stack.
 */												 
	{
	CDecisionFilter* self = new (ELeave) CDecisionFilter(aClientSid,aEvaluatorId,aServiceId,aServerSid,aVersion);
	CleanupStack::PushL(self);
	self->ConstructL(aFingerprint,aClientEntity);
	return self;
	}	


void CDecisionFilter::ConstructL(const TDesC8& aFingerprint, const TDesC8& aClientEntity)
/**
	Second phase constructor for the decision filter. Sets members allocated on the heap.
	The function leaves, if any buffer allocation fails.
 */
	{
		
	iFingerprint = aFingerprint.AllocL();
	iSetFlag[KPosFingerprint] |= KSetFingerprint;
	
	iClientEntity = aClientEntity.AllocL();
	iSetFlag[KPosClientEntity] |= KSetClientEntity;

	ValidateL();
	}
	
void CDecisionFilter::ValidateL()
/**
	Check fingerprint and client entity lengths are valid.
*/
	{
	if((iSetFlag[KPosFingerprint] & KSetFingerprint) &&
		((iFingerprint == 0) || (iFingerprint->Length()	> KUpsMaxFingerprintLength)))
		{
		DEBUG_PRINTF2(_L8("The fingerprint length (%d) has not been satisfied during filter construction!"),(iFingerprint) ? (iFingerprint->Length()) : (0));
		User::Leave(KErrUpsBadFingerprintLength);
		}
		
	if((iSetFlag[KPosClientEntity] & KSetClientEntity) &&
		((iClientEntity == 0) || (iClientEntity->Length() > KUpsMaxClientEntityLength)))
		{
		DEBUG_PRINTF2(_L8("The client entity length (%d) has not been satisfied during filter construction!"),(iClientEntity) ? (iClientEntity->Length()) : (0));
		User::Leave(KErrUpsBadClientEntityLength);
		}
	}
	
EXPORT_C void CDecisionFilter::SetClientSid(const TSecureId& aSid, TComparisonOp aOp)
/**
	Sets the Secure ID of the client application.

	@param aUid		SID for the client application
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iClientSid = aSid;
	iSetFlag[KPosClientSid] |= KSetClientSid;
	iSetFlag[KPosClientSid] |= aOp;
	}
	
	
EXPORT_C void CDecisionFilter::SetEvaluatorId(const TUid& aUid, TComparisonOp aOp)
/**
	Sets the UID of the policy evaluator DLL.

	@param aUid		UID for the policy evaluator DLL
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iEvaluatorId = aUid;
	iSetFlag[KPosEvaluatorId] |= KSetEvaluatorId;
	iSetFlag[KPosEvaluatorId] |= aOp;
	}


EXPORT_C void CDecisionFilter::SetServiceId(const TUid& aUid, TComparisonOp aOp)
/**
	Sets the Secure ID of the service.

	@param aUid		UID for the service
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iServiceId = aUid;
	iSetFlag[KPosServiceId] |= KSetServiceId;
	iSetFlag[KPosServiceId] |= aOp;
	}
	
		
EXPORT_C void CDecisionFilter::SetServerSid(const TSecureId& aSid, TComparisonOp aOp)
/**
	Sets the Secure ID of the system server.
	
	@param aUid		UID for the system server
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iServerSid = aSid;
	iSetFlag[KPosServerSid] |= KSetServerSid;
	iSetFlag[KPosServerSid] |= aOp;
	}
	
	
EXPORT_C void CDecisionFilter::SetFingerprintL(const TDesC8& aFingerprint, TComparisonOp aOp)
/**
	Sets the fingerprint. A buffer is allocated on the heap and aFingerprint copied into it.
	The function leaves, if the buffer allocation fails.
	 
	@param aFingerprint		Hash of the destination and/or opaque data. (Maximum length is 32 bytes)
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	//Fingerprint's length can not be longer than KUpsMaxFingerprintLength
	if(aFingerprint.Length() > KUpsMaxFingerprintLength)
		{
		User::Leave(KErrUpsBadFingerprintLength);
		}

	delete iFingerprint;
	iFingerprint = 0;
			
	iFingerprint = aFingerprint.AllocL();
	iSetFlag[KPosFingerprint] |= KSetFingerprint;
	iSetFlag[KPosFingerprint] |= aOp;
	}
	
	
EXPORT_C void CDecisionFilter::SetClientEntityL(const TDesC8& aClientEntity, TComparisonOp aOp)
/**
	Sets the client entity. A buffer is allocated on the heap and aClientEntity copied into it.
	The function leaves, if the buffer allocation fails.
	 
	@param aClientEntity	The name of the entity within the client. (Maximum length is 32 bytes) 
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	//ClientEntity's length can not be longer than KUpsMaxClientEntityLength
	if(aClientEntity.Length() > KUpsMaxClientEntityLength)
		{
		User::Leave(KErrUpsBadClientEntityLength);
		}
		
	delete iClientEntity;
	iClientEntity = 0;
		
	iClientEntity = aClientEntity.AllocL();
	iSetFlag[KPosClientEntity] |= KSetClientEntity;
	iSetFlag[KPosClientEntity] |= aOp;
	}


EXPORT_C void CDecisionFilter::SetMajorPolicyVersion(const TUint16& aVersion, TComparisonOp aOp)
/**
	Sets the major version of the policy file.
	
	@param aVersion		Major policy version.
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iMajorPolicyVersion = aVersion;
	iSetFlag[KPosMajorPolicyVersion] |= KSetMajorPolicyVersion;
	iSetFlag[KPosMajorPolicyVersion] |= aOp;
	}


EXPORT_C void CDecisionFilter::SetRecordId(const TUint32& aId, TComparisonOp aOp)
/**
	Sets the unique Id number of the decision record which is searched.
	
	@param aId	A unique record Id.
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iRecordId = aId;
	iSetFlag[KPosRecordId] |= KSetRecordId;
	iSetFlag[KPosRecordId] |= aOp;
	}


EXPORT_C void CDecisionFilter::SetDescriptionL(const TDesC& aDescription, const TComparisonOp aOp)
/**
	Sets the description field. A buffer is allocated on the heap and aDescription copied into it.
	The function leaves, if the buffer allocation fails.
	 
	@param aDescription	A copy of description and/or opaque. (Maximum length is 32 bytes) 
	@param aOp			Comparision operator which is used to create SQL statement.
 */
	{	
	delete iDescription;
	iDescription = 0;
		
	iDescription = aDescription.AllocL();
	iSetFlag[KPosDescription] |= KSetDescription;
	iSetFlag[KPosDescription] |= aOp;
	}
	
	
EXPORT_C void CDecisionFilter::SetResult(const TUint8& aResult, const TComparisonOp aOp)
/**
	Sets the result field of the decision record which is searched.
	
	@param aResult	Whether the request should be approved.
	@param aOp		Comparision operator which is used to create SQL statement.
 */
	{
	iResult = aResult;
	iSetFlag[KPosResult] |= KSetResult;
	iSetFlag[KPosResult] |= aOp;
	}
	
	
EXPORT_C void CDecisionFilter::SetEvaluatorInfo(const TUint32& aEvaluatorInfo, const TComparisonOp aOp)
/**
	Sets the evaluator info field of the decision record which is searched.
	
	@param aEvaluatorInfo	Policy evaluator specific data
	@param aOp				Comparision operator which is used to create SQL statement.
 */
	{
	iEvaluatorInfo = aEvaluatorInfo;
	iSetFlag[KPosEvaluatorInfo] |= KSetEvaluatorInfo;
	iSetFlag[KPosEvaluatorInfo] |= aOp;
	}
	
	
EXPORT_C void CDecisionFilter::ExternalizeL(RWriteStream& aStream) const
/**
	Externalise this CDecisionFilter object to the specified stream.
*/
	{
	aStream << iClientSid.iId;		// TSecureId 
	aStream << iEvaluatorId.iUid;	// TUid 
	aStream << iServiceId.iUid;		// TUid 
	aStream << iServerSid.iId;		// TSecureId 

	aStream << ((iFingerprint) ? (*iFingerprint) : (KNullDesC8())); 	// HBufC8* 
	aStream << ((iClientEntity) ? (*iClientEntity) : (KNullDesC8()));	// HBufC8* 

	for(int i = 0; i < KFilterKeysNumber; ++i)
		{
		aStream.WriteUint16L(iSetFlag[i]);// TUint16 
		}

	aStream << iMajorPolicyVersion;// TUint16 
	aStream << iRecordId;// TUint32 
	}
EXPORT_C void CDecisionFilter::InternalizeL(RReadStream& aStream)
/**
	Internalise this CDecisionFilter object from the specified stream.
*/
	{
	iClientSid.iId= aStream.ReadUint32L();		// TSecureId 
	iEvaluatorId.iUid= aStream.ReadInt32L();	// TUid 
	iServiceId.iUid= aStream.ReadInt32L();		// TUid 
	iServerSid.iId= aStream.ReadUint32L();		// TSecureId 

	// iFingerprint is always present in stream, so internalise it and then delete it if it is not setup.
	delete iFingerprint;
	iFingerprint = 0;
	iFingerprint = HBufC8::NewL(aStream, KMaskDesLength8);

	// iClientEntity is always present in stream, so internalise it and then delete it if it is not setup.
	delete iClientEntity;
	iClientEntity = 0;
	iClientEntity = HBufC8::NewL(aStream, KMaskDesLength8);
	
	for(int i = 0; i < KFilterKeysNumber; ++i)
		{
		iSetFlag[i] = aStream.ReadUint16L();// TUint16 
		}

	// Delete iFingerprint if not setup
	if((iSetFlag[KPosFingerprint] & KSetFingerprint) == 0)
		{
		delete iFingerprint;
		iFingerprint = 0;
		}

	// Delete iClientEntity if not setup
	if((iSetFlag[KPosClientEntity] & KSetClientEntity) == 0)
		{
		delete iClientEntity;
		iClientEntity = 0;
		}

	iMajorPolicyVersion = aStream.ReadUint16L();// TUint16 
	iRecordId = aStream.ReadUint32L();// TUint32 

	ValidateL();
	}

} // End of  UserPromptService namespace
// End of file
