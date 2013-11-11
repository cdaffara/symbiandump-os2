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
#include "SupplierStep.h"
#include "supplier.h"
#include "cafmimeheader.h"
#include "caferr.h"
#include "metadataarray.h"
#include "supplieroutputfile.h"
#include "attribute.h"
#include "bitset.h"

#include <uri8.h>
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
#include <http.h>
#include <http/rhttpsession.h>
#include <http/rhttptransaction.h>
#include <http/rhttpheaders.h>
#endif

using namespace ContentAccess;


/* 
 * This step imports a DCF file into CAF 
 *
 */
CCafSupplierStep::~CCafSupplierStep()
	{
	}

CCafSupplierStep::CCafSupplierStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFSupplierStep);
	}


TVerdict CCafSupplierStep::doTestStepL()
	{
	SetTestStepResult(EInconclusive);

	RFs fs;
	RFile file;
	TBuf8 <128> readBuffer;
	
	INFO_PRINTF1(_L("Supplier Test - Agent provides output files"));
	
	TPtrC outputDirectory, sourceFileName, suggestedFileName, mimeType;
	TInt expectedLeave;

	// Get parameters from INI file
	GetStringFromConfig(ConfigSection(),_L("OutputPath"),outputDirectory);
	GetStringFromConfig(ConfigSection(),_L("SourceFileName"),sourceFileName);
	GetStringFromConfig(ConfigSection(),_L("SuggestedFileName"),suggestedFileName);
	GetStringFromConfig(ConfigSection(),_L("MimeType"),mimeType);
	GetIntFromConfig(ConfigSection(),_L("LeaveResult"),expectedLeave);
	
	StartApparcServerL();

	__UHEAP_MARK;
	
	HBufC8 *mime = ConvertDes16toHBufC8LC(mimeType);

	CCafMimeHeader *header = CCafMimeHeader::NewL(mime->Des());
	CleanupStack::PushL(header);

	CSupplier *mySupplier = CSupplier::NewLC();
	mySupplier->SetOutputDirectoryL(outputDirectory);
	
	// ignore return value, just exercises code for CCover
	mySupplier->IsImportSupported(header->StandardMimeData(EContentType));

	CImportFile *import = NULL;
	TRAPD(leaveResult, import = mySupplier->ImportFileL(*header, suggestedFileName));
	if(leaveResult != expectedLeave)
		{
		SetTestStepResult(EFail);
		}
	if(leaveResult == KErrNone)
		{
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
		for(TInt i = 0; i < n; i++)
			{
			// get output file name
			TPtrC OutputFileName = import->OutputFileL(i).FileName();
			INFO_PRINTF2(_L("Output File Created: %S"),&OutputFileName);

			// get output file type (content or receipt)
			TOutputType outputType = import->OutputFileL(i).OutputType();

			if(outputType == EReceipt)
				{
				INFO_PRINTF1(_L("Output File is a receipt"));
				}
			else
				{
				INFO_PRINTF1(_L("Output File is content"));
				}

			// get output file mime type
			TPtrC8 OutputMimeType = import->OutputFileL(i).MimeTypeL();
			HBufC *mime = ConvertDes8toHBufC16LC(OutputMimeType );
			INFO_PRINTF2(_L("Output File Mime Type: %S"),mime);
			CleanupStack::PopAndDestroy(mime); 
			}
		CleanupStack::PopAndDestroy(&fs); 
		CleanupStack::PopAndDestroy(import); 
		}
	CleanupStack::PopAndDestroy(mySupplier); 
	CleanupStack::PopAndDestroy(header); 
	CleanupStack::PopAndDestroy(mime); 
	__UHEAP_MARKEND;

	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}



/* 
 * This step imports a DCF file into CAF 
 *
 */
CCafSupplierAsyncStep::~CCafSupplierAsyncStep()
	{
	}

CCafSupplierAsyncStep::CCafSupplierAsyncStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFSupplierAsyncStep);
	}


TVerdict CCafSupplierAsyncStep::doTestStepL()
	{
	SetTestStepResult(EInconclusive);

	RFs fs;
	RFile file;
	TBuf8 <128> readBuffer;
	TRequestStatus status;

	INFO_PRINTF1(_L("Asynchronous Supplier Test - Agent provides output files"));
	
	TPtrC outputDirectory, sourceFileName, suggestedFileName, mimeType;
	TInt expectedLeave;
	
	// Get parameters from INI file
	GetStringFromConfig(ConfigSection(),_L("OutputPath"),outputDirectory);
	GetStringFromConfig(ConfigSection(),_L("SourceFileName"),sourceFileName);
	GetStringFromConfig(ConfigSection(),_L("SuggestedFileName"),suggestedFileName);
	GetStringFromConfig(ConfigSection(),_L("MimeType"),mimeType);
	GetIntFromConfig(ConfigSection(),_L("LeaveResult"),expectedLeave);

	StartApparcServerL();


	__UHEAP_MARK;
	
	HBufC8 *mime = ConvertDes16toHBufC8LC(mimeType);

	CCafMimeHeader *header = CCafMimeHeader::NewL(mime->Des());
	CleanupStack::PushL(header);

	CSupplier *mySupplier = CSupplier::NewLC();
	mySupplier->SetOutputDirectoryL(outputDirectory);

	// ignore return value, just exercises code for CCover
	mySupplier->IsImportSupported(header->StandardMimeData(EContentType));

	CImportFile *import = NULL;
	TRAPD(leaveResult, import = mySupplier->ImportFileL(*header, suggestedFileName));
	if(leaveResult != expectedLeave)
		{
		SetTestStepResult(EFail);
		}
	if(leaveResult == KErrNone)
		{
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
			status = KRequestPending;
			import->WriteData(readBuffer,status);
			User::WaitForRequest(status);
			}
		CleanupStack::PopAndDestroy(&file); 
		status = KRequestPending;
		import->WriteDataComplete(status);
		User::WaitForRequest(status);

		TInt n = import->OutputFileCountL();
		for(TInt i = 0; i < n; i++)
			{
			// get output file name
			TPtrC OutputFileName = import->OutputFileL(i).FileName();
			INFO_PRINTF2(_L("Output File Created: %S"),&OutputFileName);

			// get output file type (content or receipt)
			TOutputType outputType = import->OutputFileL(i).OutputType();

			if(outputType == EReceipt)
				{
				INFO_PRINTF1(_L("Output File is a receipt"));
				}
			else
				{
				INFO_PRINTF1(_L("Output File is content"));
				}

			// get output file mime type
			TPtrC8 OutputMimeType = import->OutputFileL(i).MimeTypeL();
			HBufC *mime = ConvertDes8toHBufC16LC(OutputMimeType );
			INFO_PRINTF2(_L("Output File Mime Type: %S"),mime);
			CleanupStack::PopAndDestroy(mime); 
			}

		CleanupStack::PopAndDestroy(&fs); 
		CleanupStack::PopAndDestroy(import); 
		}
	CleanupStack::PopAndDestroy(mySupplier); 
	CleanupStack::PopAndDestroy(header); 
	CleanupStack::PopAndDestroy(mime); 
	__UHEAP_MARKEND;
		
	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}



/* 
 * This step imports a DCF file into CAF 
 *
 */
CCafClientOutputSupplierStep::~CCafClientOutputSupplierStep()
	{
	}

CCafClientOutputSupplierStep::CCafClientOutputSupplierStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFClientOutputSupplierStep);
	}

void CCafClientOutputSupplierStep::CheckContentMimeL(CImportFile* aImport, TDes8& aContentMime, TDesC8& aExpectedContentMime)
	{
	if(!aImport->ContentMimeTypeL(aContentMime))
		{
		//Agent can't determine the content MIME type with available data at the moment
						
		INFO_PRINTF1(_L("Content MIME Type can't be determined"));
		SetTestStepResult(EFail);
		return;
		}
	
									    
	//If we reach here, we are expecting a correct Content MIME type 
	HBufC *mime = ConvertDes8toHBufC16LC(aContentMime);
	INFO_PRINTF2(_L("Content MIME Type: %S"), mime);
	CleanupStack::PopAndDestroy(mime);
										    					    				    					    	
	if(aContentMime.CompareF(aExpectedContentMime) == 0)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		ERR_PRINTF1(_L("Content MIME Type doesn't match expected"));
		SetTestStepResult(EFail);
		}
			
	}


TVerdict CCafClientOutputSupplierStep::doTestStepL()
	{
	SetTestStepResult(EInconclusive);

	RFs fs;
	RFile file;
	TBuf8 <128> readBuffer;
	TBuf8 <255> mimetype8;
	TFileName outputFileName;
	RFile outputFile;
	CImportFile *import = NULL;
	
	// first output file will be a.txt
	_LIT(KOutputFileName, "C:\\something.drm");
	outputFileName.Copy(KOutputFileName);
	
	INFO_PRINTF1(_L("Supplier Test - Client provides output files"));
	
	TPtrC outputDirectory, sourceFileName, suggestedFileName, mimeType, expectedContentMime;
	TInt expectedLeave;
	TBool checkContentMime = EFalse;
	TBuf8<KMaxDataTypeLength> contentMimeType;	
	
	// Get parameters from INI file
	GetStringFromConfig(ConfigSection(),_L("OutputPath"),outputDirectory);
	GetStringFromConfig(ConfigSection(),_L("SourceFileName"),sourceFileName);
	GetStringFromConfig(ConfigSection(),_L("SuggestedFileName"),suggestedFileName);
	GetStringFromConfig(ConfigSection(),_L("MimeType"),mimeType);
	GetIntFromConfig(ConfigSection(),_L("LeaveResult"),expectedLeave);
	GetStringFromConfig(ConfigSection(),_L("ExpectedContentMime"),expectedContentMime);
	GetBoolFromConfig(ConfigSection(),_L("CheckContentMime"),checkContentMime);
	
	
	StartApparcServerL();

	__UHEAP_MARK;
	
	mimetype8.Copy(mimeType);

	// fill in meta data - just the mime type really
	CMetaDataArray *array = CMetaDataArray::NewL();
	CleanupStack::PushL(array);
	_LIT8(KContentType,"content-type");
	array->AddL(KContentType(), mimetype8);

	// create a supplier session
	CSupplier *mySupplier = CSupplier::NewL();
	CleanupStack::PushL(mySupplier);
	
	// check import is supported, ignore return value, just exercises code for CCover
	mySupplier->IsImportSupported(array->SearchL(KContentType()));

	// create import session
	TRAPD(leaveResult, import = mySupplier->ImportFileL(mimetype8, *array));
	if(leaveResult != expectedLeave)
		{
		SetTestStepResult(EFail);
		}
	if(leaveResult == KErrNone)
		{
		CleanupStack::PushL(import);

		// read the input file and pass it to the CAF
		fs.Connect();
		CleanupClosePushL(fs);
		TInt result = file.Open(fs, sourceFileName, EFileRead | EFileStream | EFileShareAny);
		CleanupClosePushL(file);
		
		TParsePtrC parse(suggestedFileName);
		TPtrC desiredOutFileName( parse.Name() );
		
		while(result == KErrNone)
			{
			result = file.Read(readBuffer);
			if(readBuffer.Length() == 0)
				break;
			result = import->WriteData(readBuffer);
			while(result == KErrCANewFileHandleRequired)
				{
				
				if(checkContentMime)
					{
					
					//If CheckContentMimeL failed, Client code should assume content MIME type wouldn't
					//be available until after the supply session has finished. The imported data could be
					//output to default location
					//In this test step, data are always output to default location regardless of content MIME
					//type
					TBuf8<KMaxDataTypeLength> expectedContentMime8bit;
					expectedContentMime8bit.Copy(expectedContentMime);
					CheckContentMimeL(import, contentMimeType, expectedContentMime8bit);
					//no more check needed in following iterations
					checkContentMime = EFalse;
						
					//At this point client should decide where to save the file based on content mime
					
					}
				
				const TInt KMaxExtLen = 28;
				TBuf<KMaxExtLen> suggestedExt;
				User::LeaveIfError(import->GetSuggestedOutputFileExtension(suggestedExt));
				outputFileName.Copy(outputDirectory);
				outputFileName.Append(desiredOutFileName);
				outputFileName.Append(suggestedExt);
				// create a new outputfile
				fs.Delete(outputFileName);
				User::LeaveIfError(outputFile.Create(fs,outputFileName, EFileShareAny  | EFileStream | EFileWrite));
				result = import->ContinueWithNewOutputFile(outputFile,outputFileName);
				outputFile.Close();
				}
			User::LeaveIfError(result);
			}
		CleanupStack::PopAndDestroy(&file); 
		result = import->WriteDataComplete();
		while(result == KErrCANewFileHandleRequired)
				{
				TFileName suggestedName;
				User::LeaveIfError(import->GetSuggestedOutputFileName(suggestedName));
				outputFileName.Copy(outputDirectory);
				outputFileName.Append(suggestedName);

				// create a new outputfile
				fs.Delete(outputFileName);
				User::LeaveIfError(outputFile.Create(fs,outputFileName, EFileShareAny  | EFileStream | EFileWrite));
				result = import->ContinueWithNewOutputFile(outputFile,outputFileName);
				outputFile.Close();
				}
	

		TInt n = import->OutputFileCountL();
		for(TInt i = 0; i < n; i++)
			{
			// get output file name
			TPtrC OutputFileName = import->OutputFileL(i).FileName();
			INFO_PRINTF2(_L("Output File Created: %S"),&OutputFileName);

			// get output file type (content or receipt)
			TOutputType outputType = import->OutputFileL(i).OutputType();

			if(outputType == EReceipt)
				{
				INFO_PRINTF1(_L("Output File is a receipt"));
				}
			else
				{
				INFO_PRINTF1(_L("Output File is content"));
				}

			// get output file mime type
			TPtrC8 OutputMimeType = import->OutputFileL(i).MimeTypeL();
			HBufC *mime = ConvertDes8toHBufC16LC(OutputMimeType );
			INFO_PRINTF2(_L("Output File Mime Type: %S"),mime);
			CleanupStack::PopAndDestroy(mime); 
			}
		CleanupStack::PopAndDestroy(&fs); 
		CleanupStack::PopAndDestroy(import); 
		}
	CleanupStack::PopAndDestroy(mySupplier); 
	CleanupStack::PopAndDestroy(array); 
	__UHEAP_MARKEND;

	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}


/* 
 * This step tests the ExternalizeL() and InternalizeL() functions for
 * CCafMimeHeader
 */
CCAFSupplierSerializeStep::~CCAFSupplierSerializeStep()
	{
	}

CCAFSupplierSerializeStep::CCAFSupplierSerializeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFSupplierSerializeStep);
	}


TVerdict CCAFSupplierSerializeStep::doTestStepL()
	{	
	_LIT8(KDrmMime,"application/testagent.drm");
	_LIT8(KBinaryEncoding,"Binary");
	_LIT8(KOmaRightsPending, "X-Oma-Drm-Separate-Delivery");
	_LIT8(KOmaRightsPendingValue, "12");
	
	SetTestStepResult(EInconclusive); // Default result to EInconclusive

	__UHEAP_MARK;
	
	// create a CafMimeHeader with "content type:" application/testagent.drm
	CCafMimeHeader *header = CCafMimeHeader::NewL(KDrmMime());
	CleanupStack::PushL(header);
	
	// Add some information to the header
	header->SetStandardMimeDataL(EContentTransferEncoding, KBinaryEncoding());
	header->AddNonStandardMimeL(KOmaRightsPending(), KOmaRightsPendingValue());

	// Create a buffer stream
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	// call the stream function
	stream << *header;
	CleanupStack::PopAndDestroy(&stream);

	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* des = HBufC8::NewL(buf->Size());
	TPtr8 ptr(des->Des());
	buf->Read(0, ptr, buf->Size());

	// destroy the buffer
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(des);

	// Now, stream a new CCafMimeHeader from the descriptor
	CCafMimeHeader* newHeader = CCafMimeHeader::NewL(KNullDesC8());
	CleanupStack::PushL(newHeader);
	RDesReadStream readstream(*des);
	CleanupClosePushL(readstream);
	readstream >> *newHeader;
	CleanupStack::PopAndDestroy(&readstream);

	// Now check that the new bitset equals the old one
	TInt result = newHeader->StandardMimeData(EContentType).Compare(KDrmMime());
	if (result != 0)
		{
		INFO_PRINTF1(_L("Content type was not copied properly during seriaization"));
		SetTestStepResult(EFail);
		}
	result = newHeader->StandardMimeData(EContentTransferEncoding).Compare(KBinaryEncoding());
	if (result != 0)
		{
		INFO_PRINTF1(_L("Content Length was not copied properly during seriaization"));
		SetTestStepResult(EFail);
		}
	
	if (newHeader->NonStandardMimeCount() != 1)
		{
		INFO_PRINTF1(_L("Non standard field array not copied properly during seriaization"));
		SetTestStepResult(EFail);
		}

	result = newHeader->NonStandardMimeField(0).Compare(KOmaRightsPending());
	if (result != 0)
		{
		INFO_PRINTF1(_L("Non standard field not copied properly during seriaization"));
		SetTestStepResult(EFail);
		}

	result = newHeader->NonStandardMimeData(0).Compare(KOmaRightsPendingValue());
	if (result != 0)
		{
		INFO_PRINTF1(_L("Non standard field data was not copied properly during seriaization"));
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(3, header);

	__UHEAP_MARKEND;
	
	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY

/* 
 * Check that applications can retrieve the HTTP request headers
 */
CCAFHTTPRequestHeadersStep::~CCAFHTTPRequestHeadersStep()
	{
	}

CCAFHTTPRequestHeadersStep::CCAFHTTPRequestHeadersStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFHTTPRequestHeadersStep);
	}


void CCAFHTTPRequestHeadersStep::MHFRunL(RHTTPTransaction, const THTTPEvent&)
	{
	
	}
	
TInt CCAFHTTPRequestHeadersStep::MHFRunError(TInt, RHTTPTransaction, const THTTPEvent&)
	{
	return 0;
	}


TVerdict CCAFHTTPRequestHeadersStep::doTestStepL()
	{	
	_LIT8(Kuri, "http://www.symbian.com/");
	TPtrC8 acceptHeader;
	TInt i = 0;
	TInt result = 0;
	
	SetTestStepResult(EInconclusive); // Default result to EInconclusive
	THTTPHdrVal aValue;
	
	__UHEAP_MARK;

	CSupplier* supplier = CSupplier::NewLC();
	
	// Create an HTTP session 
	RHTTPSession session;
	session.OpenL();
	RStringPool pool = session.StringPool();
	
	
	// Convert the URI string into a TUri8
	TUriParser8 parser;
	parser.Parse(Kuri());
	
	// create an HTTP transaction
	RHTTPTransaction transaction = session.OpenTransactionL(parser, *this, pool.StringF(HTTP::EGET,RHTTPSession::GetTable()));
	RHTTPHeaders hdr = transaction.Request().GetHeaderCollection();

	// Get the request headers from the agent
	supplier->PrepareHTTPRequestHeaders(pool, hdr);
	
	TInt fieldParts = hdr.FieldPartsL(pool.StringF(HTTP::EAccept, RHTTPSession::GetTable()));
	if(fieldParts == 3)
		{
		for(i = 0; i < fieldParts; i++)
			{
			// loop over all accept headers make sure we find the three we expect
			User::LeaveIfError(hdr.GetField(pool.StringF(HTTP::EAccept, RHTTPSession::GetTable()), i, aValue));
	
			RStringF header = (RStringF) aValue;
			acceptHeader.Set(header.DesC()); 
		
			// Reference Test Agent - content MIME type
			if(acceptHeader.Compare(_L8("application/x-rta.drm.content")) == 0)
				{
				result |= 1;
				}
			// Reference Test Agent - content + rights MIME type
			else if(acceptHeader.Compare(_L8("application/x-rta.drm.contentrights")) == 0)
				{
				result |= 2;
				}
			// Test Agent - MIME type
			else if(acceptHeader.Compare(_L8("APPLICATION/TESTAGENT.DRM")) == 0)
				{
				result |= 4;
				}
			}
		if(result != 7)
			{
			// not all three headers were found
			INFO_PRINTF1(_L("Not all expected HTTP Accept headers were present"));
			SetTestStepResult(EFail);		
			}
		}
	else
		{
		// We were expecting three accept headers to be set
		// two from the Reference Test Agent and one from Test Agent
		INFO_PRINTF2(_L("ERROR: Only %d HTTP Accept headers were returned, expected 3"), fieldParts);
		SetTestStepResult(EFail);	
		}
		
	transaction.Close();
	session.Close();
	CleanupStack::PopAndDestroy(supplier);
	__UHEAP_MARKEND;
	
	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

#endif


