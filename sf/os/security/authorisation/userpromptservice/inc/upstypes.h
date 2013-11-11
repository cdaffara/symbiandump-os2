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
* Types required to use the UPS server.
*
*/


/**
 @file
 @publishedAll
 @released
*/


#ifndef UPSTYPES_H
#define UPSTYPES_H
#include <e32base.h>

#ifdef __WINS__
// Work around broken TRAP handling.......... See DEF108105
#undef TRAP_INSTRUMENTATION_END
#define TRAP_INSTRUMENTATION_END (void)User::Allocator()
#endif

class RWriteStream;
class RReadStream;

enum TUpsDecision
/**
	Decisions which can be returned by the UPS server to the
	system server.
 */
	{
	/**
		The action can be performed.  This response does not distinguish
		between a one-shot or an always approval.
	 */
	EUpsDecYes,
	/**
		The action cannot be performed.
	 */
	EUpsDecNo,
	/**
		The action can be performed for a session, as defined by the
		system server.
	 */
	EUpsDecSessionYes,
	/**
		The action can not be performed for a session, as defined by the
		system server.
	 */
	EUpsDecSessionNo

	};

/**
	Convenience typedef wraps up an instance of TUpsDecision which
	can be supplied to a server.  Note the APIs themselves take TDes8&
	arguments, not specifically references to an instance of this type.
 */
typedef TPckgBuf<TUpsDecision> TUpsDecisionPckgBuf;

/**
Uniquely identifies a service. UIDs should be reserved via Symbian Signed.
 */
typedef TUid TServiceId;

namespace UserPromptService
{

//Number of columns exist in a filter key
static const TUint8  KFilterKeysNumber			= 0x0B;

/** Comparision types used in the querying of the decision database. */
//4 bytes on the right side are used to indicate whether a columns is set.
enum TComparisonOp
	{
	EEqual  			= 0x00010000,
	ENotEqual  			= 0x00020000,
	ELessThan  			= 0x00040000,
	EGreaterThan  		= 0x00080000,
	ELessThanOrEqual    = 0x00100000,
	EGreaterThanOrEqual = 0x00200000
	};

NONSHARABLE_CLASS(CDecisionFilter):public CBase
/**
   Stores the filter key values that used to query the database. 
   A filter object can be created in one phase by supplying all required parameters, 
   or an empty filter is first created and then required filter keys are set separately.
 */
	{	
	friend class CDecisionDb;
	friend class CDecisionDbW;
	
public:
	IMPORT_C static CDecisionFilter *NewL ();	
	IMPORT_C static CDecisionFilter *NewLC();
	IMPORT_C static CDecisionFilter *NewL (const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
										   const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TUint16& aVersion);										  
	IMPORT_C static CDecisionFilter *NewLC(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,
										   const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TUint16& aVersion);		
	~CDecisionFilter();
	
	IMPORT_C void SetClientSid(const TSecureId& aSid, const TComparisonOp aOp);	
	IMPORT_C void SetEvaluatorId(const TUid& aUid, const TComparisonOp aOp);
	IMPORT_C void SetServiceId(const TUid& aUid, const TComparisonOp aOp);
	IMPORT_C void SetServerSid(const TSecureId& aSid, const TComparisonOp aOp);
	IMPORT_C void SetFingerprintL(const TDesC8& aFingerprint, const TComparisonOp aOp);
	IMPORT_C void SetClientEntityL(const TDesC8& aClientEntity, const TComparisonOp aOp);
	IMPORT_C void SetMajorPolicyVersion(const TUint16& aVersion, const TComparisonOp aOp);
	IMPORT_C void SetRecordId(const TUint32& aId, const TComparisonOp aOp);
	IMPORT_C void SetDescriptionL(const TDesC& aDescription, const TComparisonOp aOp);
	IMPORT_C void SetResult(const TUint8& aResult, const TComparisonOp aOp);
	IMPORT_C void SetEvaluatorInfo(const TUint32& aEvaluatorInfo, const TComparisonOp aOp);

	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:
	CDecisionFilter();
	CDecisionFilter(const TSecureId& aClientSid, const TUid& aEvaluatorId, const TUid& aServiceId, const TSecureId& aServerSid, const TUint16& aVersion);	
	
	void ConstructL(const TDesC8& aFingerprint, const TDesC8& aClientEntity);
	void ValidateL();
	
private:
	/**
	   Secure Id for client application
	 */
	TSecureId iClientSid;	
	/**
	   UID for the policy evaluator DLL
	 */
	TUid iEvaluatorId;	
	/**
	   UID for service
	 */
	TUid iServiceId;	
	/**
	   Secure Id for system server
	 */	
	TSecureId iServerSid;	
	/**
	   Fingerprint.
	   NULL pointer if not specified.
	 */
	HBufC8* iFingerprint; 	
	/**
	   Name of the entity within client
	   NULL pointer if not specified.
	 */
	HBufC8* iClientEntity;	
	/**
	   An array of flags which shows which attributes are set and what operator(=,!=,<,>, etc.) is assigned.
	   NULL pointer if not specified.
	 */
	TUint32 iSetFlag[KFilterKeysNumber];
	/**
		Major version of policy file.
	 */
	TUint16 iMajorPolicyVersion;
	/**
		Unique decision record Id.
	 */
	TUint32 iRecordId;
	/**
	   A copy of description and/or opaque
	 */
	HBufC* iDescription;
	/**
	   Whether the request should be approved
	 */
	TUint8 iResult;	
	/**
	   Policy evaluator specific data
	 */
	TUint32 iEvaluatorInfo;	
	};


NONSHARABLE_CLASS(CDecisionRecord):public CBase
/**
	Stores all the field values in a decision database row. All member variables of the class 
    are constant and public. Therefore, the members are assigned during construction and then 
    they can be accessible directly, however, it is not possible to modify them.
 */
	{
public:
	IMPORT_C static CDecisionRecord *NewL(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,const TDesC8& aFingerprint,
								 		  const TDesC8& aClientEntity,const TDesC& aDescription, const TUint8 aResult,const TUint16& aMajorPolicyVersion,
								 		  const TUint32 aEvaluatorInfo, const TUint32& aRecordId=0);
							  							  
	IMPORT_C static CDecisionRecord *NewLC(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,const TDesC8& aFingerprint,
 								 		   const TDesC8& aClientEntity,const TDesC& aDescription, const TUint8 aResult,const TUint16& aMajorPolicyVersion,
 								 		   const TUint32 aEvaluatorInfo, const TUint32& aRecordId=0);

	IMPORT_C static CDecisionRecord *NewLC();
	~CDecisionRecord();

	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);

private:
	CDecisionRecord();
	CDecisionRecord(const TSecureId& aClientSid,const TUid& aEvaluatorId,const TUid& aServiceId,const TSecureId& aServerSid,const TUint8 aResult,
				    const TUint16& aMajorPolicyVersion, const TUint32 aEvaluatorInfo, const TUint32& aRecordId);
	
	void ConstructL(const TDesC8& aFingerprint,const TDesC8& aClientEntity,const TDesC& aDescription);
	void ValidateL();

public:
	/**
	   Secure Id of client application
	 */
	TSecureId iClientSid;
	/**
	   UID for the policy evaluator DLL
	 */
	TUid iEvaluatorId;	
	/**
	   UID for service
	 */
	TUid iServiceId;	
	/**
	   Secure Id for system server
	 */	
	TSecureId iServerSid;	
	/**
	   Fingerprint
	 */
	RBuf8 iFingerprint; 	
	/**
	   Name of the entity within client
	 */
	RBuf8 iClientEntity;	
	/**
	   A copy of description and/or opaque
	 */
	RBuf iDescription;	
	/**
	   Whether the request should be approved
	 */
	TUint8 iResult;	
	/**
		Major policy version which this decision record possesses to.
	 */	
	TUint16 iMajorPolicyVersion;
	/**
		Unique decision record Id.
	 */
	TUint32 iRecordId;
	/**
	   Policy evaluator specific data
	 */
	TUint32 iEvaluatorInfo;
	};

} // End of UserPromptService namespace


#endif	// #ifndef UPSTYPES_H

