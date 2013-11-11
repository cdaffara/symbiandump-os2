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
 @internalComponent
 @released
*/

#ifndef POLICYREADER_H
#define POLICYREADER_H

#include <e32base.h>
#include <e32cmn.h>
#include <barsread2.h>
#include <barsc2.h>

#include <ups/policy.h>
#include "serviceconfig.h"
namespace UserPromptService
{

/**
Encapsulates the data in the policy file header. This is also used by the 
service config code.
*/
NONSHARABLE_CLASS(TPolicyHeader)
	{
 public:
	TPolicyHeader();
	
	TInt iFormatVersion;				///< Version no. of resource format.
	TUint16 iMajorVersion;				///< The major version no. of the UPS policy file.
	TUint16 iMinorVersion;				///< The minor version no. of the UPS policy file.
	TAuthorisationPolicy iAuthPolicy;	///< Authorisation policy for this service.
	TUid iDefaultPolicyEvaluator;		///< The ECOM implementation UID of the default dialog creator.
	TUid iDefaultDialogCreator;			///< The ECOM implementation UID of the default dialog creator.		
	};
	   
/**
 Class for parsing a single User Prompt Service policy resource files defined
 by ups\policies.rh.\n
 The policy file is opened when the reader object is constructed and 
 the set of policies may be enumerated by repeatedly calling the NextPolicy
 function. 
 NextPolicy returns NULL when the end of the file has been reached. After
 this the reader class should be destroyed.
 */
NONSHARABLE_CLASS(CPolicyReader) : public CBase
	{
public:	
	IMPORT_C static CPolicyReader* NewL(RFs& aRFs, const TDesC& aPolicyFileName);
	IMPORT_C static CPolicyReader* NewLC(RFs& aRFs, const TDesC& aPolicyFileName);
	IMPORT_C CPolicy* NextPolicyL();
	IMPORT_C CPolicy* DefaultPolicyL();
	IMPORT_C const TPolicyHeader& Header() const;
	
	~CPolicyReader();
private:
	CPolicyReader();
	void ConstructL(RFs& aRfs, const TDesC& aPolicyFileName);
	CPolicy* ReadPolicyL();
	void ReadPolicyHeaderL();
	
	CResourceFile* iResourceFile;	///< Handle to the resource file	
	RResourceReader iReader;		///< Parses the resource file	
	TPolicyHeader iHeader;			///< Header information
	TInt iPolicyCount;				///< The number of policies in the file		
	TInt iPolicyNum;				///< The current policy being parsed
	};

/// Policy files must only contain one top level resource.
static const int KPolicyResourceId = 1;
}

#endif /* POLICYREADER_H */
