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
*
*/


#include <ups/policy.h>

using namespace UserPromptService;

EXPORT_C TSidClasses::TSidClasses(TUint16 aSidClasses)
	: iSidClasses(aSidClasses)
/**
Constructor
@param aSidClasses A 32 bit integers that represents the set of SID classes.
*/
	{
	}

EXPORT_C TBool TSidClasses::Contains(const TSecureId& aSid) const
/**
Determines the set of SID classes contains a given SID.
@param aSid The SID to test.
@return ETrue if aSID lies within one of the SID classes.
*/
	{
	TInt sidClass = (aSid.iId & 0xf0000000) >> 28;	
	return (1 << sidClass) & iSidClasses;
	}

EXPORT_C CPolicy* CPolicy::NewL(const TSidClasses& aSidClasses, const RArray<TSecureId>& aSidList, 
		const TDesC& aDestination, TUint aOptions, const TUid& aPolicyEvaluator, 
		const TUid& aDialogCreator, TUint16 aFlags, TSystemServerSecurity aSystemServerSecurity, 
		TUint16 aMajorVersion, TUint16 aMinorVersion, TBool aDefault)
/**
Creates a new policy object.
			   
@param aSidClasses				The classes of SIDs that this policy applies to.
@param aSidList					A list of individual SIDs that this policy applies to. This overrides aSidClasses.
@param aDestination				The wildcard string to match against destination supplied by the system server.						   
@param aOptions					A bit field that defines whether the prompt should be displayed and if so,
								what buttons should be available.
@param aPolicyEvaluator 		The implementation UID of the policy evaluator ECOM plug-in to use with this policy.
@param aDialogCreator			The implementation UID of the dialog creator ECOM plug-in to use with this policy.
@param aFlags					Flags specific to an individual policy evaluator.
@param aSystemServerSecurity	Whether this policy is specific to clients that pass/fail 
								the system server's security check.
@param aMajorVersion			The major version of the UPS policy file.
@param aMinorVersion			The minor version of the UPS policy file.
@param aDefault					Set to ETrue if this is an automatically generated policy
								because no match was found in the policy file.

@return A pointer to the new policy object.
 */
	{
	CPolicy* self = CPolicy::NewLC(aSidClasses, aSidList, aDestination, aOptions, 
		aPolicyEvaluator, aDialogCreator, aFlags, aSystemServerSecurity, 
		aMajorVersion, aMinorVersion, aDefault);
	CleanupStack::Pop(self);
	return self;
	}		
	
EXPORT_C CPolicy* CPolicy::NewLC(const TSidClasses& aSidClasses, const RArray<TSecureId>& aSidList, 
	const TDesC& aDestination, TUint aOptions, const TUid& aPolicyEvaluator, 
	const TUid& aDialogCreator, TUint16 aFlags, TSystemServerSecurity aSystemServerSecurity, 
	TUint16 aMajorVersion, TUint16 aMinorVersion, TBool aDefault)
/**
Creates a new policy object and puts the pointer on the cleanup stack.

@param aSidClasses				The classes of SIDs that this policy applies to.
@param aSidList					A list of individual SIDs that this policy applies to. This overrides aSidClasses.
@param aDestination				The wildcard string to match against destination supplied by the system server.						   
@param aOptions					A bit field that defines whether the prompt should be displayed and if so,
								what buttons should be available.
@param aPolicyEvaluator 		The implementation UID of the policy evaluator ECOM plug-in to use with this policy.
@param aDialogCreator			The implementation UID of the dialog creator ECOM plug-in to use with this policy.
@param aFlags					Flags specific to an individual policy evaluator.
@param aSystemServerSecurity	Whether this policy is specific to clients that pass/fail 
								the system server's security check.
@param aMajorVersion			The major version of the UPS policy file.
@param aMinorVersion			The minor version of the UPS policy file.
@param aDefault					Set to ETrue if this is an automatically generated policy
								because no match was found in the policy file.

@return A pointer to the new policy object.
 */
	{
	CPolicy* self = new(ELeave) CPolicy(
		aSidClasses, aOptions, aPolicyEvaluator, 
		aDialogCreator, aFlags, aSystemServerSecurity, 
		aMajorVersion, aMinorVersion, aDefault);
	CleanupStack::PushL(self);
	self->ConstructL(aSidList, aDestination);
	return self;
	}

EXPORT_C TBool CPolicy::Matches(const TSecureId& aClientSid, const TDesC& aDestination, TBool aSecurityResult) const
/**
Determines whether a request matches this policy.
The request matches if and only if the following conditions are true.

- aClientSid matches an entry in the SID list or lies within one of the classes of SIDs
- aDestination matches the destination wildcard string in the policy.
- The result of the system server's securitiy check for this client matches the rule defined
  for this policy.
  - iSystemServerSecurity  == ESystemServerSecurityPassedOrFailed OR
  - aSecurityResult == ETrue and iSystemServerSecurity == ESystemServerSecurityPassed OR
  - aSecurityResult == EFalse and iSystemServerSecurity == ESystemServerSecurityFailed 

@param aClientSid		The SID of the client application
@param aDestination		A descriptor containing the destination supplied by the system server.
@param aSecurityResult	ETrue, if the client passed the system server's security check, otherwise, EFalse.
@return 				ETrue If the policy matches the request; otherwise EFalse is returned.
*/
	{	
	if (iSystemServerSecurity == CPolicy::ESystemServerSecurityPassed &&
		! aSecurityResult)
		{
		return EFalse;
		}
	else if (iSystemServerSecurity == CPolicy::ESystemServerSecurityFailed &&
		aSecurityResult)
		{
		return EFalse;
		}
	
	TBool sidMatch(EFalse);
	TInt sidListCount = iSidList.Count();
	if (sidListCount > 0)
		{
		for (TInt i = 0; i < sidListCount; ++i)
			{
			if (iSidList[i] == aClientSid.iId)
				{				
				sidMatch = ETrue;
				break;
				}
			}
		}
	else 
		{
		sidMatch = iSidClasses.Contains(aClientSid);
		}	
	
	return sidMatch && (aDestination.MatchF(*iDestination) != KErrNotFound);
	}

EXPORT_C const TDesC& CPolicy::Destination() const
/**
Gets the destination wildcard string.
@return A reference to a descriptor containing the wildcard string to
		match against the destination supplied by the system server.
 */
	{
	return *iDestination;
	}

EXPORT_C const TSidClasses& CPolicy::SidClasses() const 
/**
Gets the classes of SIDs that are applicable to this policy.
@return The set of SID classes that this policy applies to.
 */
	{
	return iSidClasses;
	}

EXPORT_C const RArray<TSecureId>& CPolicy::SidList() const 
/**
Gets the list of individual SIDs that are applicable to this policy.
The SID list takes precedent over the SID classes when matching policies.
 
@return The list of SIDs that this policy applies to as an array of 
		unsigned integers.
*/
	{
	return iSidList;
	}	

EXPORT_C CPolicy::TOptions CPolicy::Options() const 
/**
Gets the options field for this policy. 
@see CPolicy::TOptions
@return A 32bit integer containing the options.
*/
	{		
	return TOptions(iOptions);
	}

EXPORT_C const TUid& CPolicy::PolicyEvaluator() const
/**
Gets the implementation UID of the policy evaluator to use with this policy.
@return The implementation UID of the Policy Evaluator 
*/
	{
	return iPolicyEvaluator;
	}

EXPORT_C const TUid& CPolicy::DialogCreator() const
/**
Identifies the Dialog Creator to use with this policy.
@return The implementation UID of the Dialog Creator. 
*/
	{
	return iDialogCreator;
	}

EXPORT_C TUint16 CPolicy::Flags() const
/**
Gets the flags field. The contents of this field is specific to individual 
policy evaluator plug-ins.

@return The 16-bit flags field
*/
	{
	return iFlags;
	}

EXPORT_C TBool CPolicy::Default() const
/**
If a policy file is defined for a request but no policy matches the request
then a default policy object is created to allow the user to authorise a one-shot 
request.\n
This function allows the Policy Evaluator to check whether this policy 
is the default policy.
N.B. This can be overriden by simply defining a policy at the end of the file
that matches all SIDs and has '*' as the destination wildcard.

@return ETrue if this policy is the default (automatically generated) policy.
*/
	{
	return iDefault;
	}

EXPORT_C TBool CPolicy::PromptRequired() const
/**
Examines the set of options to determine whether the policy requires a prompt to be
displayed.

If the options fields only contains authorizations (EYes, ESessionYes or EAlways) or conversely 
the options field only contains rejections (ENo,ESessionNo or ENever) then this function will 
return EFalse.\n

@return ETrue if a prompt is required or EFalse if a prompt is not required
		(in which case the request will probably be silently accepted / rejected).
*/
	{
	// (a yes option) && (a no option)
	return ((iOptions & (EYes|ESessionYes|EAlways)) && (iOptions & (ENo|ESessionNo|ENever)));
	}	
	
EXPORT_C CPolicy::TSystemServerSecurity CPolicy::SystemServerSecurity() const
/**
Whether this policy is specific to clients's that pass or fail the system
server's securitiy check.
@return An enum that dictates whether this policy only applies if the client 
 		process passed/failed the system server's security check.
*/
	{
	return iSystemServerSecurity;
	}
	
EXPORT_C TUint16 CPolicy::MajorVersion() const 
/**
Gets the major version of the UPS policy file containing this policy.
@return A 16-bit unsigned number containing the major version number.
*/
	{
	return iMajorVersion;
	}

EXPORT_C TUint16 CPolicy::MinorVersion() const 
/**
Gets the minor version of the UPS policy file containing this policy.
@return A 16-bit unsigned number containing the minor version number.
*/
	{
	return iMinorVersion;
	}

CPolicy::CPolicy(const TSidClasses& aSidClasses, TUint aOptions,
		const TUid& aPolicyEvaluator, const TUid& aDialogCreator, 
		TUint16 aFlags, TSystemServerSecurity aSystemServerSecurity, 
		TUint16 aMajorVersion, TUint16 aMinorVersion, TBool aDefault) 
	  
/**
Constructor
@param aSidClasses				The classes of SIDs that this policy applies to.
@param aOptions					A bit field that defines whether the prompt should be displayed and if so,
								what buttons should be available.
@param aPolicyEvaluator 		The implementation UID of the policy evaluator ECOM plug-in to use with this policy.
@param aDialogCreator			The implementation UID of the dialog creator ECOM plug-in to use with this policy.
@param aFlags					Flags specific to an individual policy evaluator.
@param aSystemServerSecurity	Whether this policy is specific to clients that pass/fail 
								the system server's security check.
@param aMajorVersion			The major version of the UPS policy file.
@param aMinorVersion			The minor version of the UPS policy file.								
@param aDefault					Set to ETrue if this is an automatically generated policy
								because no match was found in the policy file.							 
*/
	: 	iSidClasses(aSidClasses), iOptions(aOptions),
		iPolicyEvaluator(aPolicyEvaluator), iDialogCreator(aDialogCreator),
		iFlags(aFlags), iSystemServerSecurity(aSystemServerSecurity), 
		iMajorVersion(aMajorVersion), iMinorVersion(aMinorVersion), iDefault(aDefault)
	{		
	}

void CPolicy::ConstructL(const RArray<TSecureId>& aSidList, const TDesC& aDestination)
/**
Second phase constructor
@param aSidList		A list (possibly empty) of SIDs that this policy applies to.
@param aDestination The destination wildcard string.
*/
	{
	TInt numSids = aSidList.Count();		
	for (TInt i = 0; i < numSids; ++i)
		{
		iSidList.AppendL(aSidList[i]);
		}
	iDestination = aDestination.AllocL();	
	}

CPolicy::~CPolicy()
/**
Destructor
*/
	{
	delete iDestination;
	iSidList.Close();
	}
