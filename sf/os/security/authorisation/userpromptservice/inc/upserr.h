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
* Error defined by the User Prompt Service.
* System servers MUST interpret any error returned by the User Prompt Service as EDecNo 
* and deny the client applications request.
*
*/


/**
 @file
 @publishedAll
 @released. 
*/

#ifndef UPSERR_H
#define UPSERR_H

#include <e32base.h>
#include <e32cmn.h>

/// A fingerprint was either empty or exceeded KUpsMaxFingerprintLength bytes.
const TInt KErrUpsBadFingerprintLength = -5703;

/// The policy evaluator ECOM plug-in specified in the policy file was not found.
const TInt KErrUpsMissingPolicyEvaluator = -5704;

/// The dialog creator ECOM plug-in specified in the policy file was not found.
const TInt KErrUpsMissingDialogCreator = -5705;

/// An error occured whilst parsing a policy file.		
const TInt KErrUpsBadPolicyFile = -5706;

/// No policy file was found for the requested server secure id and service uid
const TInt KErrUpsMissingPolicyFile = -5707;

/// A file in the policy directory did not conform to the policy file name format
/// -  ups_SystemServerSid_ServiceUid.rsc
const TInt KErrUpsInvalidPolicyFileName = -5708;

//One of the required arguments was not found
const TInt KErrUpsMissingArgument		= -5709;

//The client entity was exceeded the maximum allowed length
const TInt KErrUpsBadClientEntityLength	 = -5710;

//The client thread ID is invalid (either bad, or has since terminated)
const TInt KErrUpsBadClientThreadId	 = -5711;

//The client process ID is invalid (either bad, or has since terminated)
const TInt KErrUpsBadClientProcessId	 = -5712;

/// UPS policy library panic category
_LIT(KUpsPoliciesPanicCat, "UPS-Policies");
/// UPS policy library panic codes
enum TUpsPoliciesPanicCat 
	{
	/// A corrupt policy file on the Z drive was encountered
	EUpsPoliciesCorruptRomPolicy = 0
	};

/// UPS client library panic category
_LIT(KUpsClientPanicCat, "UPS-Client");
/// UPS policy library panic codes
enum TUpsClientPanicCat 
	{
	/// RUpsSubsession has not been initialised
	EUpsClientNotInitialised = 0
	};


#endif // UPSERR_H

