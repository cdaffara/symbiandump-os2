/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <f32file.h>
#include <s32file.h>
#include <caf/caferr.h>
#include <caf/supplieroutputfile.h>
#include "testagentimportfile.h"



using namespace ContentAccess;

CTestAgentImportFile* CTestAgentImportFile::NewL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
	{
	CTestAgentImportFile* self=new(ELeave) CTestAgentImportFile(ETrue);
	CleanupStack::PushL(self);
	self->ConstructL(aMimeType, aMetaDataArray, aOutputDirectory, aSuggestedFileName);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentImportFile* CTestAgentImportFile::NewL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray)
	{
	CTestAgentImportFile* self=new(ELeave) CTestAgentImportFile(EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMimeType, aMetaDataArray, KNullDesC(), KNullDesC());
	CleanupStack::Pop(self);
	return self;
	}
	
CTestAgentImportFile::CTestAgentImportFile(TBool aAgentCreatesOutputFiles) : iAgentCreatesOutputFiles(aAgentCreatesOutputFiles)
	{
	iImportStatus = EInProgress;
	}

CTestAgentImportFile::~CTestAgentImportFile()
	{
	if(iFileOpen)
		{
		iFile.Close();
		iFileOpen = EFalse;
		}
	if(iOutputFileName)
		{
		iFs.Delete(iOutputFileName->Des());
		delete iOutputFileName;
		}
	iFs.Close();
	delete iOutputDirectory;
	delete iSuggestedFileName;
	delete iLastWriteData;
	iOutputFiles.ResetAndDestroy();
	}
  
void CTestAgentImportFile::ConstructL(const TDesC8& aMimeType, const CMetaDataArray& /*aMetaDataArray*/, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
	{
	_LIT8(KDrmMimeType, "application/testagent.drm");
	TBuf8 <KMaxDataTypeLength> mimeType;
	mimeType.Copy(aMimeType);
	mimeType.LowerCase();
	if(mimeType != KDrmMimeType())
		{
		User::Leave(KErrCANotSupported);
		}

	// create buffer to store any output path and filename supplied by the client
	iOutputDirectory = aOutputDirectory.AllocL();
	iSuggestedFileName = aSuggestedFileName.AllocL();

	User::LeaveIfError(iFs.Connect());
	}

TInt CTestAgentImportFile::OpenOutputFile()
	{
	TInt err = KErrNone;
	if(iOutputFileName)
		{
		return KErrInUse;
		}

	if(iAgentCreatesOutputFiles)
		{
		// Figure out a name including full path for the output file
		TRAP(err, iOutputFileName = HBufC::NewL(iOutputDirectory->Des().Length() + iSuggestedFileName->Des().Length()));
		if(err == KErrNone)
			{
			TPtr fileNamePtr = iOutputFileName->Des();
			fileNamePtr.Copy(*iOutputDirectory);
			fileNamePtr.Append(*iSuggestedFileName);

			// create output file
			err = iFile.Create(iFs, fileNamePtr, EFileShareAny  | EFileStream | EFileWrite);
			if(err == KErrNone)
				{
				iFileOpen = ETrue;
				}
			}
		return err;
		}
	else
		{
		// The application must supply a new file handle for us to adopt
		return KErrCANewFileHandleRequired;
		}
	}

TInt CTestAgentImportFile::WriteData(const TDesC8& aData)
	{
	TInt err = KErrNone;

	if(!iFileOpen)
		{
		err = OpenOutputFile();
		}
	if(err == KErrNone)
		{
		err = iFile.Write(aData);
		}

	// check if the output file creation or file writing needs a new file handle
	if(err == KErrCANewFileHandleRequired)
		{
		// save write parameters

		delete iLastWriteData;
		iLastWriteData = aData.Alloc();
		if (NULL == iLastWriteData)
			{
			err = KErrNoMemory;
			}
		}
	return err;
	}

TInt CTestAgentImportFile::WriteDataComplete()
	{
	TRAPD(err, WriteDataCompleteL());
	return err;
	}

void CTestAgentImportFile::WriteDataCompleteL()
	{
	CSupplierOutputFile *temp = NULL; 
	if(iFileOpen)
		{	
		iFile.Close();
		iFileOpen = EFalse;
		}

	// add a new entry to the array of output files
	if(iOutputFileName)
		{
		temp = CSupplierOutputFile::NewL(iOutputFileName->Des(), EContent, _L8("application/testagent.drm"));

		CleanupStack::PushL(temp);
		iOutputFiles.AppendL(temp);
		CleanupStack::Pop(temp);

		delete iOutputFileName;
		iOutputFileName = NULL;
		}

	if(!iAgentCreatesOutputFiles)
		{
		iWaitingForReceiptHandle = ETrue;
		User::Leave(KErrCANewFileHandleRequired);
		}
	else
		{
		// Figure out receipt name including full path for the output file
		iOutputFileName = HBufC::NewL(KMaxFileName);
		TPtr fileNamePtr = iOutputFileName->Des();
		fileNamePtr.Copy(*iOutputDirectory);
		fileNamePtr.Append(_L("receipt.txt"));

		// create output file
		User::LeaveIfError(iFile.Create(iFs, fileNamePtr, EFileShareAny  | EFileStream | EFileWrite));
		CleanupClosePushL(iFile);

		RFileWriteStream stream(iFile);
		CleanupClosePushL(stream);

		stream.WriteL(_L("This is a receipt"));
		stream.CommitL();

		CleanupStack::PopAndDestroy(2, &iFile);  // stream, file
		
		// Add to list of output files
		temp = CSupplierOutputFile::NewL(iOutputFileName->Des(), EReceipt, _L8("text/plain"));

		CleanupStack::PushL(temp);
		iOutputFiles.AppendL(temp);
		CleanupStack::Pop(temp);

		delete iOutputFileName;
		iOutputFileName = NULL;	
		}
	}
		
void CTestAgentImportFile::WriteData(const TDesC8& aData, TRequestStatus &aStatus)
	{
	TRequestStatus *ptr = &aStatus;
	
	// call synchronous WriteData
	TInt rVal = WriteData(aData);
	
	// complete the request
	User::RequestComplete(ptr,rVal);
	}

void CTestAgentImportFile::WriteDataComplete(TRequestStatus &aStatus)
	{
	TRequestStatus *ptr = &aStatus;
	// call synchronous WriteDataComplete
	TInt rVal = WriteDataComplete();
	
	// complete the request
	User::RequestComplete(ptr,rVal);
	}
	
TInt CTestAgentImportFile::OutputFileCountL() const
	{
	return iOutputFiles.Count();
	}

CSupplierOutputFile& CTestAgentImportFile::OutputFileL(TInt aIndex)
	{
	return *iOutputFiles[aIndex];
	}

TImportStatus CTestAgentImportFile::GetImportStatus() const
	{
	return iImportStatus;
	}	

TInt CTestAgentImportFile::GetSuggestedOutputFileExtension(TDes& aFileExtension)
	{
	if(iWaitingForReceiptHandle)
		{
		aFileExtension.Copy(_L(".TXT"));		
		}
	else
		{
		aFileExtension.Copy(_L(".DRM"));		
		}
	return KErrNone;
	}

TInt CTestAgentImportFile::GetSuggestedOutputFileName(TDes& aFileName)
	{
	if(iWaitingForReceiptHandle)
		{
		aFileName.Copy(_L("RECEIPT.TXT"));
		}
	else
		{
		aFileName.Copy(_L("OUTPUTFILE.DRM"));
		}
	return KErrNone;
	}

TInt CTestAgentImportFile::ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName)
	{
	TRAPD(err, ContinueWithNewOutputFileL(aFile, aFileName));
	return err;
	}

void CTestAgentImportFile::ContinueWithNewOutputFileL(RFile& aFile, const TDesC& aFileName)
	{
	User::LeaveIfError( iFile.Duplicate(aFile) );

	if(iWaitingForReceiptHandle)
		{
		RFileWriteStream stream(iFile);
		stream.WriteL(_L("This is a receipt"));
		stream.CommitL();
		stream.Close();
		iFile.Close();
		
		CSupplierOutputFile *temp =
			CSupplierOutputFile::NewL(aFileName, EReceipt, _L8("text/plain"));
		CleanupStack::PushL(temp);
		iOutputFiles.AppendL(temp);
		CleanupStack::Pop(temp);
		}
	else
		{
		iFileOpen = ETrue;
		iOutputFileName = aFileName.AllocL();
		User::LeaveIfError(WriteData(*iLastWriteData));
		}
	}

void CTestAgentImportFile::ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName, TRequestStatus& aStatus)
	{
	TRequestStatus *ptr = &aStatus;
	// call synchronous WriteDataComplete
	TInt rVal = ContinueWithNewOutputFile(aFile, aFileName);
	
	// complete the request
	User::RequestComplete(ptr,rVal);
	}


TBool CTestAgentImportFile::ContentMimeTypeL(TDes8& aContentMime)
	{
	//Just set aContentMime to text/plain
	//Real agent should implement this function for discovering content type based on available buffer
	//Please refer to reftestagent as an example.
	_LIT8(KPlainText, "text/plain");
	aContentMime.Copy(KPlainText);
	
	return ETrue;
	}

