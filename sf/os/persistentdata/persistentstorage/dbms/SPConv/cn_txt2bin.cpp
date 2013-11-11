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
// CSPTxt2Bin class
// 
//

#include "cn_main.h"
#include "cn_util.h"
#include "cn_txt2bin.h"
#include "SC_TextIn.h"
#include "SC_StrmOut.h"

using namespace DBSC;

/**
CSPTxt2Bin::RunL() implements pure virtual CCmdProcessor::RunL().
It opens the input text policy file and creates in-memory presentation of the 
policies found there. Then it stores the policies in a binary policy file,
which may be used then by the DBMS server.
CSPTxt2Bin::RunL() uses CPDTextLoader class to load policies for the text policy file
and TPDStreamPersister class to store them in a binary policy file.
@leave System-wide error codes in case of failure
@see CCmdProcessor::RunL()
@see CPDTextLoader
@see TPDStreamPersister
*/
void CSPTxt2Bin::RunL()
	{
	CPDTextLoader* textLoader = CPDTextLoader::NewLC(iFs, iCmdLinePrm.iTxtFile);

#ifdef __DBDUMP__
	TBuf<KMaxFileName> dumpFileName;
	dumpFileName = iCmdLinePrm.iTxtFile;
	_LIT(KExt, "D");
	dumpFileName += KExt;
	(void)iFs.Delete(dumpFileName);
#endif//__DBDUMP__

	TUid domainUid = TSPConvUtil::UidFromFileNameL(iCmdLinePrm.iBinFile);
	CPolicyDomain* policyDomain = CPolicyDomain::NewLC(domainUid, *textLoader);

#ifdef __DBDUMP__
	RFile dumpFile;
	CleanupClosePushL(dumpFile);
	__LEAVE_IF_ERROR(dumpFile.Replace(iFs, dumpFileName, EFileWrite));
	policyDomain->Dump(dumpFile);
	CleanupStack::PopAndDestroy(&dumpFile);
#endif//__DBDUMP__

	RFileWriteStream streamOut;
	CleanupClosePushL(streamOut);
	__LEAVE_IF_ERROR(streamOut.Replace(iFs, iCmdLinePrm.iBinFile, EFileWrite));
	TPDStreamPersister streamPersister(streamOut);
	policyDomain->ExternalizeL(streamPersister);
	CleanupStack::PopAndDestroy(&streamOut);

	CleanupStack::PopAndDestroy(policyDomain);
	CleanupStack::PopAndDestroy(textLoader);
	}

/**
*/
inline CSPTxt2Bin::CSPTxt2Bin(RFs& aFs, const TCmdLinePrm& aCmdLinePrm) :
	CCmdProcessor(aFs, aCmdLinePrm)
	{
	}

/**
*/
CSPTxt2Bin::~CSPTxt2Bin()
	{
	}

/**
Standard phase-one CSPTxt2Bin factory method.
@param aFs File server session
@param aCmdLinePrm Parsed command line arguments: requested action, input and output file paths
@return A pointer to successfully created CSPTxt2Bin instance.
@leave KErrNoMemory - not enough memory
@leave KErrNotFound - input file not found
*/
CSPTxt2Bin* CSPTxt2Bin::NewLC(RFs& aFs, const TCmdLinePrm& aCmdLinePrm)
	{
	__ASSERT(aCmdLinePrm.iAction == TCmdLinePrm::ETxt2Bin);
	CSPTxt2Bin* self = new (ELeave) CSPTxt2Bin(aFs, aCmdLinePrm);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Standard phase-two CSPTxt2Bin construction method.
*/
void CSPTxt2Bin::ConstructL()
	{
	if(!TSPConvUtil::FileExists(iFs, iCmdLinePrm.iTxtFile))
		{
		_LIT(KText, "Text file \"%S\" not found\n");
		TSPConvUtil::Print(KText, iCmdLinePrm.iTxtFile);
		__LEAVE(KErrNotFound);
		}
	}
