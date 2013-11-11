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


/**
 @file
 @publishedPartner
 @released 
*/

#ifndef POLICY_H
#define POLICY_H

#include <e32base.h>
#include <e32cmn.h>

namespace UserPromptService 
	{
	/**
	 Defines a set of SID classes.
	 See Symbian Signed UID FAQ for the definition of SID classes.
	 */
	NONSHARABLE_CLASS(TSidClasses)
		{
	public:
		IMPORT_C TSidClasses(TUint16 aSidClasses);
		IMPORT_C TBool Contains(const TSecureId& aSid) const;
						
		const TUint16 iSidClasses; ///< A bitmask that defines the set SID classes.
		};

	/**
	 Class for a single policy record.
	 */
	NONSHARABLE_CLASS(CPolicy) : public CBase
		{
	public:
		/**
		   Defines the elements in the options field that determines 
		   whether a prompt should be displayed and if so what buttons are
		   available.

		   All other bits are reserved for future use and should be ignored
		   by the custom Policy Evaluators and Dialog Creators.
		 */
		typedef enum 
			{			
			EYes = 0x01,		///< The 'Yes' button should be displayed in the prompt.			
			ENo =  0x02,		///< The 'No' button should be displayed in the prompt.	
			ESession = 0x04,	///< The 'SessionYes' button should be displayed in the prompt.
			ESessionYes = 0x04,	///< The 'SessionYes' button should be displayed in the prompt.
			EAlways = 0x08,		///< The 'Always' button should be displayed in the prompt.			
			ENever = 0x10,		///< The 'Never' button should be displayed in the prompt.
			ESessionNo = 0x20   ///< The 'SessionNo' button should be displayed in the prompt.
			} TOptions;
			
		/**
		Allows policies to be matched according to whether the client process
		passed security check defined by the system server.
		Typically, this corresponds to whether the client has the correct capabilities
		for the requested service. However, system servers are free to use features
		other than capabilities in their security check.

		E.g. If the client has the correct capabilities for the requested service then
		the "Always" and "Never" options will be enabled in the policy; otherwise, a
		different policy will be matched where the prompt is limited to one-shot
		permissions ("Yes" and "No").
		*/
		typedef enum 
			{ 
			/**
			The policy applies regardless of whether the client process passed the
			system server's security check.
			*/
			ESystemServerSecurityPassedOrFailed = 0,
			/**
			The policy only applies if the client process failed the system server's
			security check.
			*/
			ESystemServerSecurityFailed = 1,
			/**
			The policy only applies if the client process passed the system server's
			security check.
			*/
			ESystemServerSecurityPassed = 2
			} TSystemServerSecurity;			
	
		IMPORT_C static CPolicy* NewL(const TSidClasses& aSidClasses, const RArray<TSecureId>& aSidList, 
				const TDesC& aDestination, TUint aOptions, const TUid& aPolicyEvaluator, 
				const TUid& aDialogCreator, TUint16 aFlags, 
				TSystemServerSecurity aSystemServerSecurity, 
				TUint16 aMajorVersion, TUint16 aMinorVersion, TBool aDefault = EFalse);
		
		IMPORT_C static CPolicy* NewLC(const TSidClasses& aSidClasses, const RArray<TSecureId>& aSidList, 
				const TDesC& aDestination, TUint aOptions, const TUid& aPolicyEvaluator, 
				const TUid& aDialogCreator, TUint16 aFlags,
				TSystemServerSecurity aSystemServerSecurity, 
				TUint16 aMajorVersion, TUint16 aMinorVersion, TBool aDefault = EFalse);

		IMPORT_C TBool Matches(const TSecureId& aClientSid, const TDesC& aDestination, TBool aSecurityResult) const;		
		IMPORT_C TBool PromptRequired() const;
	
		IMPORT_C TOptions Options() const;
		IMPORT_C const TUid& PolicyEvaluator() const;
		IMPORT_C const TUid& DialogCreator() const;
		IMPORT_C const TDesC& Destination() const;
		IMPORT_C const TSidClasses& SidClasses() const;
		IMPORT_C const RArray<TSecureId>& SidList() const;
		IMPORT_C TUint16 Flags() const;
		IMPORT_C TBool Default() const;
		IMPORT_C TSystemServerSecurity SystemServerSecurity() const;
		IMPORT_C TUint16 MajorVersion() const;
		IMPORT_C TUint16 MinorVersion() const;

		~CPolicy();
	private:
		CPolicy(const TSidClasses& aSidClasses, TUint aOptions, 
				const TUid& aPolicyEvaluator, const TUid& aDialogCreator,	
				TUint16 aFlags, TSystemServerSecurity aSystemServerSecurity, 
				TUint16 aMajorVersion, TUint16 aMinorVersion, TBool aDefault);

		void ConstructL(const RArray<TSecureId>& aSidList, const TDesC& aDescription);
		
		TSidClasses iSidClasses;	///< The clases of SIDs that this policy applies to.		
		RArray<TSecureId> iSidList;	///< The set of individual SIDs that this policy applies to.
		HBufC* iDestination;		///< Wildcard string to match against the request's destination.
		TUint iOptions;				///< Options for evaluator EYes|ENo|ESessionYes|EAlways|ENever.
		TUid iPolicyEvaluator;		///< Implementation UID of the associated policy evaluator.
		TUid iDialogCreator;		///< Implementation UID of the associated dialog creator.
		TUint16 iFlags;				///< Policy Evaluator specific flags
		/**	Whether this policy is specific to clients that passed/failed the system server's
		security check.	*/
		TSystemServerSecurity iSystemServerSecurity;
		TUint16 iMajorVersion;		///< Major version of the UPS policy file.
		TUint16 iMinorVersion;		///< Minor version of the UPS policy file.		
		TBool iDefault;				///< ETrue if no match was found in the policy file.
		};
	}

#endif // POLICY_H
