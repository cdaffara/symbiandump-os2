/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <test/testexecutelog.h>
#include <s32mem.h>
#include "cafserver.h"
#include "oomstep.h"
#include "content.h"
#include "data.h"
#include "supplier.h"
#include "cafmimeheader.h"
#include "importfile.h"
#include "supplieroutputfile.h"
#include "attribute.h"
#include "bitset.h"

using namespace ContentAccess;

const TInt KOomReadContent = 1;
const TInt KOomSupply = 2;


/* 
 * This step imports a DCF file into CAF 
 *
 */
CCafOomStep::~CCafOomStep()
	{
	}

CCafOomStep::CCafOomStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFOomStep);
	}


TVerdict CCafOomStep::doTestStepL()
	{
	SetTestStepResult(EInconclusive);

	TInt failCount=1;
	TInt ret=-1;
	const TInt MaxAllocationFailures=450;


	TInt OomTest = KErrNotFound;
	GetIntFromConfig(ConfigSection(),_L("OomTest"),OomTest);


	if(OomTest == 2)
		{
		INFO_PRINTF2(_L("Starting Apparc..."),failCount);	
		StartApparcServerL();
		User::After(500000); // Allow async libraries to unload
		}


	while(failCount < MaxAllocationFailures && OomTest > 0)
		{
		// Mark heap and set allocation to fail
		__UHEAP_MARK;	

		switch(OomTest)
			{
		case KOomReadContent:
			INFO_PRINTF2(_L("ReadContent() __UHEAP_FAILNEXT: %d"),failCount);	
			__UHEAP_FAILNEXT(failCount);
			TRAP(ret,OomReadContentL());
			break;
		case KOomSupply:
			INFO_PRINTF2(_L("Supply() __UHEAP_FAILNEXT: %d"),failCount);	
			__UHEAP_FAILNEXT(failCount);
			TRAP(ret,OomSupplyL());
			break;
		default:
			INFO_PRINTF2(_L("Out of Memory test %d does not exist"),OomTest);
			SetTestStepResult(EFail);
			ret = KErrNone;
			break;
			};

		User::After(500000); // Allow async libraries to unload


		// Check all heap is free'd
		__UHEAP_MARKEND;
		
		// cancel heap failure
		__UHEAP_RESET;
		if(ret == KErrNone)
			{
			// Allocation failure has been moved all the way through the test
			// Succeeded after failCount allocations
			return TestStepResult();
			}
		else 
			{
			// Try failing a little further into the process
			failCount++;
			}
		}

	// We must have reached our maximum number of allocation failures
	// There must be some other problem 
	SetTestStepResult(EFail);

	return TestStepResult();
	}	

TVerdict CCafOomStep::OomReadContentL()
	{
	
	TBuf8<2> buffer;
	TRequestStatus status;

	TPtrC uri;
	GetStringFromConfig(ConfigSection(),_L("URI"),uri);

	CContent* content = CContent::NewLC(uri);
	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	TInt size = 0;
	data->DataSizeL(size);
	
	// Seek and read from start using both CAF and RFile
	TInt pos = size/4;
	data->Seek(ESeekStart, pos);
	data->Read(buffer);

	pos=0;
	data->Seek(ESeekCurrent, pos);
	
	data->Read(buffer,1,status);
	User::WaitForRequest(status);

	CleanupStack::PopAndDestroy(2, content);

	SetTestStepResult(EPass);

	return TestStepResult();
	}


TVerdict CCafOomStep::OomSupplyL()
	{
	SetTestStepResult(EInconclusive);

	RFs fs;
	RFile file;
	TBuf8 <128> readBuffer;
	
	TPtrC outputDirectory, sourceFileName, suggestedFileName, mimeType;
	TInt expectedLeave;

	// Get parameters from INI file
	GetStringFromConfig(ConfigSection(),_L("OutputPath"),outputDirectory);
	GetStringFromConfig(ConfigSection(),_L("SourceFileName"),sourceFileName);
	GetStringFromConfig(ConfigSection(),_L("SuggestedFileName"),suggestedFileName);
	GetStringFromConfig(ConfigSection(),_L("MimeType"),mimeType);
	GetIntFromConfig(ConfigSection(),_L("LeaveResult"),expectedLeave);
	

	// delete any file previously supplied, don't care if this 
	// has errors so trap.
	Delete(_L("C:\\supplied.drm"));
	Delete(_L("C:\\receipt.txt"));


	HBufC8 *mime = ConvertDes16toHBufC8LC(mimeType);

	CCafMimeHeader *header = CCafMimeHeader::NewL(mime->Des());
	CleanupStack::PushL(header);

	CSupplier *mySupplier = CSupplier::NewLC();
	mySupplier->SetOutputDirectoryL(outputDirectory);
	
	// ignore return value, just exercises code for CCover
	mySupplier->IsImportSupported(header->StandardMimeData(EContentType));

	CImportFile *import = NULL;
	import = mySupplier->ImportFileL(*header, suggestedFileName);
	CleanupStack::PushL(import);

	// read the input file and pass it to the CAF
	fs.Connect();
	CleanupClosePushL(fs);
	
	TInt result = file.Open(fs, sourceFileName, EFileRead | EFileStream | EFileShareAny);
	CleanupClosePushL(file);
	while(result == KErrNone)
		{
		result = file.Read(readBuffer);
		if(readBuffer.Length() == 0)
			break;
		User::LeaveIfError(import->WriteData(readBuffer));
		}
	CleanupStack::PopAndDestroy(&file); 
	User::LeaveIfError(import->WriteDataComplete());

	TInt n = import->OutputFileCountL();
	if(n > 0)
		{
		// get output file type (content or receipt)
		TOutputType outputType = import->OutputFileL(0).OutputType();

		// get output file name, but ignore it because it would cause a warning
		import->OutputFileL(0).FileName();
			
		// get output file mime type
		TPtrC8 OutputMimeType = import->OutputFileL(0).MimeTypeL();
		HBufC *outmime = ConvertDes8toHBufC16LC(OutputMimeType );
		CleanupStack::PopAndDestroy(outmime); 

		if(outputType != EContent)
			{
			SetTestStepResult(EFail);
			}
		else
			{
			// check an attribute, it's only a reference so no need to delete it
			import->OutputFileL(0).AttributesL(ETrue);
			}
		}

	CleanupStack::PopAndDestroy(&fs); 
	CleanupStack::PopAndDestroy(import); 
	CleanupStack::PopAndDestroy(mySupplier); 
	CleanupStack::PopAndDestroy(header); 
	CleanupStack::PopAndDestroy(mime); 

	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

