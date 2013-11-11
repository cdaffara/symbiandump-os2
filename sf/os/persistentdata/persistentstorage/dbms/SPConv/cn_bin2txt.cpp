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
// DBMS - security policy file tool
// 
//

#include <s32file.h>
#include "cn_main.h"
#include "cn_bin2txt.h"
#include "cn_util.h"
#include "SC_StrmIn.h"
#include "SC_TextOut.h"

using namespace DBSC;

/**
CSPBin2Txt::RunL() implements pure virtual CCmdProcessor::RunL().
It opens the input binary policy file and creates in-memory presentation of the 
policies found there. Then it stores the policies in a text file (in human readable format).
CSPBin2Txt::RunL() uses TPDStreamLoader class to load policies for the binary file
and CPDTextPersister class to store them in a text file.
@leave System-wide error codes in case of failure
@see CCmdProcessor::RunL()
@see TPDStreamLoader
@see CPDTextPersister
*/
void CSPBin2Txt::RunL()
	{
	RFileReadStream streamIn;
	CleanupClosePushL(streamIn);
	__LEAVE_IF_ERROR(streamIn.Open(iFs, iCmdLinePrm.iBinFile, EFileRead));

	TPDStreamLoader streamLoader(streamIn);

	TUid domainUid = TSPConvUtil::UidFromFileNameL(iCmdLinePrm.iBinFile);

	CPolicyDomain* policyDomain = CPolicyDomain::NewLC(domainUid, streamLoader);

	CPDTextPersister* textPersister = CPDTextPersister::NewLC(iFs, iCmdLinePrm.iTxtFile);
	policyDomain->ExternalizeL(*textPersister);
	CleanupStack::PopAndDestroy(textPersister);

	CleanupStack::PopAndDestroy(policyDomain);
	CleanupStack::PopAndDestroy(&streamIn);
	}

/**
*/
CSPBin2Txt::~CSPBin2Txt()
	{
	}

/**
*/
inline CSPBin2Txt::CSPBin2Txt(RFs& aFs, const TCmdLinePrm& aCmdLinePrm) :
	CCmdProcessor(aFs, aCmdLinePrm)
	{
	}

/**
Standard phase-one CSPBin2Txt factory method.
@param aFs File server session
@param aCmdLinePrm Parsed command line arguments: requested action, input and output file paths
@return A pointer to successfully created CSPBin2Txt instance.
@leave KErrNoMemory - not enough memory
@leave KErrNotFound - input file not found
@leave KErrAlreadyExists - output file already exists
*/
CSPBin2Txt* CSPBin2Txt::NewLC(RFs& aFs, const TCmdLinePrm& aCmdLinePrm)
	{
	__ASSERT(aCmdLinePrm.iAction == TCmdLinePrm::EBin2Txt);
	CSPBin2Txt* self = new (ELeave) CSPBin2Txt(aFs, aCmdLinePrm);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Standard phase-two CSPBin2Txt construction method.
*/
void CSPBin2Txt::ConstructL()
	{
	__ASSERT(iCmdLinePrm.iTxtFile.Length() > 0);
	__ASSERT(iCmdLinePrm.iBinFile.Length() > 0);
	if(!TSPConvUtil::FileExists(iFs, iCmdLinePrm.iBinFile))
		{
		_LIT(KText, "\"UID\" file \"%S\" not found\n");
		TSPConvUtil::Print(KText, iCmdLinePrm.iBinFile);
		__LEAVE(KErrNotFound);
		}
	if(TSPConvUtil::FileExists(iFs, iCmdLinePrm.iTxtFile))
		{
		_LIT(KText, "File \"%S\" already exists!\n");
		TSPConvUtil::Print(KText, iCmdLinePrm.iTxtFile);
		__LEAVE(KErrAlreadyExists);
		}
	}
