/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "policyreader.h"
#include <ups/ups.hrh>
#include <ups/upserr.h>
#include "upslog.h"

using namespace UserPromptService;

/// The policy resource format is versioned.
static const TInt KPolicyFormatVersion = 1;

TPolicyHeader::TPolicyHeader()
/**
Constructor
*/
	: iFormatVersion(0), iMajorVersion(0), iMinorVersion(0), iAuthPolicy(EAlwaysCheck),
	iDefaultPolicyEvaluator(TUid::Null()), iDefaultDialogCreator(TUid::Null())
	{	
	}

void CPolicyReader::ReadPolicyHeaderL() 
/**
Reads the header information from the policy file.
*/
	{
	TUint16 formatVersion = iReader.ReadUint16L(); 
	if (formatVersion != KPolicyFormatVersion)
		{
		User::Leave(KErrUpsBadPolicyFile);
		}
	
	(void) iReader.ReadUint32L();	// Skip reserved LLINK	
	
	iHeader.iFormatVersion = formatVersion; 
	iHeader.iMajorVersion = iReader.ReadUint16L();
	iHeader.iMinorVersion = iReader.ReadUint16L();	
	iHeader.iAuthPolicy = static_cast<TAuthorisationPolicy>(iReader.ReadInt8L());
	iHeader.iDefaultPolicyEvaluator = TUid::Uid(iReader.ReadInt32L());
	iHeader.iDefaultDialogCreator = TUid::Uid(iReader.ReadInt32L());	
	if (iHeader.iDefaultDialogCreator.iUid == 0)
		{
		User::Leave(KErrUpsBadPolicyFile);
		}
	}

EXPORT_C CPolicyReader* CPolicyReader::NewL(RFs& aRFs, const TDesC& aPolicyFileName)
/**
Creates a policy reader object.
@param aRFs the file server session used by the resource parser.
@param aPolicyFileName the name of the User Prompt Service policy file to read.

@return The new policy reader.
 */
	{
	CPolicyReader* self = CPolicyReader::NewLC(aRFs, aPolicyFileName);
	CleanupStack::Pop(self);
	return self;
	}		
	
EXPORT_C CPolicyReader* CPolicyReader::NewLC(RFs& aRFs, const TDesC& aPolicyFileName)
/**
Creates a policy reader object and places the pointer on the cleanup stack.
@param aRFs the file server session used by the resource parser.
@param aPolicyFileName the name of the User Prompt Service policy file to read.

@return The new policy reader.
*/	
	{
	CPolicyReader* self = new(ELeave) CPolicyReader();
	CleanupStack::PushL(self);
	self->ConstructL(aRFs, aPolicyFileName);
	return self;
	}

void CPolicyReader::ConstructL(RFs& aFs, const TDesC& aResourceFileName)
/**
Second phase constructor.
 - Creates the resource reader.
 - Reads the default policy evaluator and dialog creator UIDs.
 - Reads the number of policies in the resource file.
 
@param aRFs				The file server session used by the resource parser.
@param aPolicyFileName	The name of the User Prompt Service policy file to read.
*/
	{
	DEBUG_PRINTF2(_L("Loading UPS policy file %S"), &aResourceFileName);
	
	RFile r;
	User::LeaveIfError(r.Open(aFs, aResourceFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(r);
	TInt size;
	User::LeaveIfError(r.Size(size));
	CleanupStack::PopAndDestroy(&r);
	
	iResourceFile = CResourceFile::NewL(aFs, aResourceFileName, 0, size);
	iReader.OpenL(iResourceFile, KPolicyResourceId);

	TUidType u(iResourceFile->UidType());
	if (u[1] != TUid::Uid(KUidUpsPolicyResourceFile))	
		{
		User::Leave(KErrUpsBadPolicyFile);
		}	
		
	ReadPolicyHeaderL();

	iPolicyCount = iReader.ReadUint16L(); 
	if (iPolicyCount < 0)
		{
		User::Leave(KErrUpsBadPolicyFile);
		}
	}

EXPORT_C const TPolicyHeader& CPolicyReader::Header() const
/**
Gets the header information for this policy file.
@return A const reference to the policy header information.
*/
	{
	return iHeader;
	}

CPolicyReader::CPolicyReader()
/**
Constructor
*/
	{
	}

CPolicyReader::~CPolicyReader() 
/**
Destructor
*/
	{
	iReader.Close();
	delete iResourceFile;
	}

EXPORT_C CPolicy* CPolicyReader::DefaultPolicyL()
/**
Factory method for creating a default policy object from the 
information in the UPS policy file header.

@return A pointer to the default policy object.
*/
	{
	_LIT(KAllDestinations, "*");
	TSidClasses allSidClasses(0xFFFF);
	RArray<TSecureId> sidList;
	CleanupClosePushL(sidList);
	
	CPolicy* p = CPolicy::NewL(
		allSidClasses, sidList, KAllDestinations,
		CPolicy::EYes | CPolicy::ENo, iHeader.iDefaultPolicyEvaluator, iHeader.iDefaultDialogCreator, 
		0, CPolicy::ESystemServerSecurityPassedOrFailed,
		iHeader.iMajorVersion, iHeader.iMinorVersion, ETrue);
		
	CleanupStack::PopAndDestroy(&sidList);
	return p;
	}

EXPORT_C CPolicy* CPolicyReader::NextPolicyL()
/**
Gets the next policy from the resource file.

@return A pointer to the new policy object or null if the end of
		the file has been reached.
*/
	{
	CPolicy* p(0);
	if (iPolicyNum < iPolicyCount)
		{
		p = ReadPolicyL();
		++iPolicyNum;
		}
	return p;
	}

CPolicy* CPolicyReader::ReadPolicyL()
/**
Reads the next policy from the resource file.

@return A pointer to the new policy object.
 */
	{
	TUint32 classes = iReader.ReadUint32L();
	TSidClasses sidClasses(classes);		

	TInt numSids = iReader.ReadInt16L();
	if (numSids < 0)
		{
		User::Leave(KErrUpsBadPolicyFile);
		}		
	RArray<TSecureId> sidList(8);
	CleanupClosePushL(sidList);		
	for (TInt i = 0; i < numSids; ++i)
		{
		sidList.AppendL(TSecureId(iReader.ReadUint32L()));
		}
	
	TUint8 ssSecurity = iReader.ReadUint8L();
	CPolicy::TSystemServerSecurity systemServerSecurity = 
		CPolicy::ESystemServerSecurityPassedOrFailed;
	switch (ssSecurity)
		{
		case CPolicy::ESystemServerSecurityPassedOrFailed:
			systemServerSecurity = CPolicy::ESystemServerSecurityPassedOrFailed;
			break;
		case CPolicy::ESystemServerSecurityPassed:
			systemServerSecurity = CPolicy::ESystemServerSecurityPassed;
			break;
		case CPolicy::ESystemServerSecurityFailed:
			systemServerSecurity = CPolicy::ESystemServerSecurityFailed;
			break;
		default:
			User::Leave(KErrUpsBadPolicyFile);
			break;
		}
	
	HBufC* destination(iReader.ReadHBufCL());
	if (! destination)
		{
		destination = HBufC::NewL(0);
		}
	CleanupStack::PushL(destination);

	TUint options(iReader.ReadUint32L());
	if ((options & 
		~(CPolicy::EYes|CPolicy::ENo|CPolicy::ESessionYes|CPolicy::ESessionNo|CPolicy::EAlways|CPolicy::ENever)) != 0)
		{
		User::Leave(KErrUpsBadPolicyFile);
		}
	
	TUid policyEvaluator = TUid::Uid(iReader.ReadInt32L());
	if (policyEvaluator == TUid::Null())
		{
		policyEvaluator = iHeader.iDefaultPolicyEvaluator;
		}
	
	TUid dialogCreator = TUid::Uid(iReader.ReadInt32L());
	if (dialogCreator == TUid::Null())
		{
		dialogCreator = iHeader.iDefaultDialogCreator;
		}
	
	TUint16 flags(iReader.ReadUint16L());
	iReader.ReadUint16L(); // skip over reserved flags
	iReader.ReadUint32L(); // skip over reserved link

	CPolicy* pol = CPolicy::NewL(sidClasses, sidList, *destination,
		options, policyEvaluator, dialogCreator, flags, systemServerSecurity,
		iHeader.iMajorVersion, iHeader.iMinorVersion);
	
	CleanupStack::PopAndDestroy(2, &sidList); // destination, sidList
	return pol;
	}	
