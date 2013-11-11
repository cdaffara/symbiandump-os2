// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CPolicyDomainLoader class
// 
//

#include <s32file.h>
#include "D32Strings.h"
#include "SC_StrmIn.h"
#include "SC_DomainLoader.h"

//This function returns ETrue if the drives in aPath1 and aPath2 are equal.
TBool CheckDrivesL(const TDesC& aPath1, const TDesC& aPath2)
	{
	__ASSERT(aPath1.Length() > 0);
	__ASSERT(aPath2.Length() > 0);
	
	TChar drvc1 = aPath1[0];
	TInt drv1;
	__LEAVE_IF_ERROR(RFs::CharToDrive(drvc1, drv1));
	
	TChar drvc2 = aPath2[0];
	TInt drv2;
	__LEAVE_IF_ERROR(RFs::CharToDrive(drvc2, drv2));
	
	return drv1 == drv2;
	}

namespace DBSC
{

/**
@param aFs A reference to a file server session object
@param aPolicyDir A reference to policy file sdirectory
@param aPDCollection The created policy domain objects will be stored in this output parameter.
*/
inline CPolicyDomainLoader::CPolicyDomainLoader(RFs& aFs, const TDesC& aPolicyDir, 
												RMap<TInt, CPolicyDomain*>& aPDCollection) :
	iFs(aFs),
	iPolicyDir(aPolicyDir),
	iPDCollection(aPDCollection)
	{
	__ASSERT(iPolicyDir.Length() > 0);
	__ASSERT(iPDCollection.Count() == 0);
	}

/**
Standard phase-one CPolicyDomainLoader factory method
@param aFs A reference to a file server session object
@param aPolicyDir A reference to policy file sdirectory
@param aPDCollection The created policy domain objects will be stored in this output parameter.
@return A pointer to just created CPolicyDomainLoader instance.
@leave System-wide error codes, including KErrNoMemory
*/
CPolicyDomainLoader* CPolicyDomainLoader::NewLC(RFs& aFs, const TDesC& aPolicyDir, 
												RMap<TInt,CPolicyDomain*>& aPDCollection)
	{
	CPolicyDomainLoader* self = new (ELeave) CPolicyDomainLoader(aFs, aPolicyDir, aPDCollection);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
*/
CPolicyDomainLoader::~CPolicyDomainLoader()
	{
	delete iFoundPolicyFiles;
	}

/**
Standard phase-two CPolicyDomainLoader construction method.
It does a search for binary policy files in the system and creates and stores
a list of found ones in iFoundPolicyFiles data member.
@leave System-wide error codes, including KErrNoMemory
*/
void CPolicyDomainLoader::ConstructL()
	{
	TFindFile findFile(iFs);
	TInt err = findFile.FindWildByDir(KPolicyFileExtMask, iPolicyDir, iFoundPolicyFiles);
	if(err == KErrNone)
		{//Check if the found drive is the same as the drive in iPolicyDir.
		if(!::CheckDrivesL(findFile.File(), iPolicyDir))
			{
			err = KErrNotFound;
			}
		}
	__LEAVE_IF_ERROR(err);
	}

/**
The method creates the policy file path related to aPolicyFileIndex entry in
iFoundPolicyFiles list of binary policy file names. The path will be stored
in iPolicyFilePath data member.
@param aPolicyFileIndex Index of binary policy file name in iFoundPolicyFiles list.
@return The UID, extracted from binary policy file name.
@leave System-wide error codes, including KErrNoMemory
*/
TUid CPolicyDomainLoader::CreatePolicyFilePathL(TInt aPolicyFileIndex)
	{
	__ASSERT(aPolicyFileIndex < iFoundPolicyFiles->Count());
	const ::TEntry& entry = (*iFoundPolicyFiles)[aPolicyFileIndex];
	__LEAVE_IF_ERROR(iParser.Set(entry.iName, NULL, NULL));
	TPtrC fileName = iParser.Name();
	TUid domainUid = KNullUid;
	TLex lex(fileName);
	__LEAVE_IF_ERROR(lex.Val(*(TUint32*)&domainUid, EHex));
	if(domainUid == KNullUid)
		{
		__LEAVE(KErrCorrupt);
		}
	iPolicyFilePath.Copy(iPolicyDir);
	iPolicyFilePath.Append(entry.iName);
	return domainUid;
	}

/**
The method creates the policy domain object related to aPolicyFileIndex entry in
iFoundPolicyFiles list of binary policy file names. The created policy domain object
will be added to iPDCollection collection of policy domain objects.
@param aPolicyFileIndex Index of binary policy file name in iFoundPolicyFiles list.
@leave System-wide error codes, including KErrNoMemory
*/
void CPolicyDomainLoader::DoCreatePolicyDomainL(TInt aPolicyFileIndex)
	{
	TUid domainUid = CreatePolicyFilePathL(aPolicyFileIndex);
	RFileReadStream strm;
	CleanupClosePushL(strm);
	__LEAVE_IF_ERROR(strm.Open(iFs, iPolicyFilePath, EFileRead));
	TPDStreamLoader in(strm);
	CPolicyDomain* policyDomain = CPolicyDomain::NewLC(domainUid, in);
	__ASSERT(policyDomain);
	__LEAVE_IF_ERROR(iPDCollection.Insert(domainUid.iUid, policyDomain));
	CleanupStack::Pop(policyDomain);
	CleanupStack::PopAndDestroy(&strm);
	}

/**
The method creates the policy domain object related to aPolicyFileIndex entry in
iFoundPolicyFiles list of binary policy file names. The created policy domain object
will be added to iPDCollection collection of policy domain objects.
@param aPolicyFileIndex Index of binary policy file name in iFoundPolicyFiles list.
@leave KErrNoMemory is the only possible leaving error code. The  rest of errors
                    will be suppressed.
*/
void CPolicyDomainLoader::CreatePolicyDomainL(TInt aPolicyFileIndex)
	{
	TRAPD(err, DoCreatePolicyDomainL(aPolicyFileIndex));
#ifdef _DEBUG
	if(err != KErrNone)
		{
		_LIT(KText, "Failed to load policy file \"%S\"!\n");
		TBuf<KMaxPath> buf;
		buf.Format(KText, &iPolicyFilePath);
		RDebug::Print(buf);
		}
#endif//_DEBUG
	if(err == KErrNoMemory)
		{
		__LEAVE(err);
		}
	}

/**
The method creates a policy domain object for each binary policy file found in the system.
The created objects will be added to iPDCollection collection of policy domain objects.
@leave KErrNoMemory is the only possible leaving error code. The  rest of errors
                    will be suppressed.
*/
void CPolicyDomainLoader::RunL()
	{
	__ASSERT(iFoundPolicyFiles);
	for(TInt i=(iFoundPolicyFiles->Count()-1);i>-1;--i)
		{
		CreatePolicyDomainL(i);
		}
	}

} //end of - namespace DBSC
