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


#include <f32file.h>
#include <caf/caferr.h>
#include "rtaImport.h"
#include "drmrights.h"
#include "drmrightsparser.h"
#include "rtarights.h"
#include "rightsobject.h"
#include "reftestagentconstants.h"
#include "rtapanic.h"
#include <iniparser.h>

using namespace ReferenceTestAgent;

EXPORT_C CRefTestAgentImport* CRefTestAgentImport::NewL(const TDesC& aClientSuggestedFileName, RFs& aFs, const TImportFileMimeType& aMimeType)
	{
	CRefTestAgentImport* self = new (ELeave) CRefTestAgentImport(aFs, aMimeType);
	CleanupStack::PushL(self);
	self->ConstructL(aClientSuggestedFileName);
	CleanupStack::Pop(self);
	return self;
	}

CRefTestAgentImport::CRefTestAgentImport(RFs& aFs, const TImportFileMimeType& aMimeType) 
	: iFs(aFs), iOutputFileOpened(EFalse),
	iRightsOffsetPckg(iRightsOffset),
	iDrmFilesOffsetPckg(iDrmFilesOffset),
    iRightsSizePckg(iRightsSize),
	iDrmFilesSizePckg(iDrmFilesSize),
	iMimeType(aMimeType),
	iOffsetSize(sizeof(TInt))
	{
	// Set the length of the buffers to receive integers to be zero initially
	iRightsOffsetPckg.Zero();
	iRightsSizePckg.Zero();
	iDrmFilesOffsetPckg.Zero();
	iDrmFilesSizePckg.Zero();
	}

void CRefTestAgentImport::ConstructL(const TDesC& aClientSuggestedFileName)
	{
	iDrmRightsParser = CDrmRightsParser::NewL();
	
	/*
	 * Read KRtaTempOutputDir from the ini file. Set it to c:\\rta\\ if
	 * no value is available
	 */
	_LIT(KIniFile, "z:\\rta\\settings.ini");
	_LIT(KRtaTempOutDirInScript, "RtaTempOutputDir");
	CIniData* iniData = NULL;
	TPtrC tempOutputDir;
	
	iniData = CIniData::NewL(KIniFile());
	CleanupStack::PushL(iniData);
	if(iniData->FindVar(KRtaTempOutDirInScript(), tempOutputDir))
		{
		iRtaTempOutDir = tempOutputDir.AllocL();
		}
	else
		{
		_LIT(KDefaultRtaDir, ":\\rta\\");
		iRtaTempOutDir = HBufC::NewL(KDefaultRtaDir().Length()+1);
		TPtr ptr(iRtaTempOutDir->Des());
		ptr[0] = RFs::GetSystemDriveChar();
		ptr.Append(KDefaultRtaDir);
		}
	CleanupStack::PopAndDestroy(iniData);
	
	TInt nameLen = aClientSuggestedFileName.Length();
	if (nameLen)
		{
		iClientSuggestedFileName = HBufC8::NewL(nameLen);
		iClientSuggestedFileName->Des().Copy(aClientSuggestedFileName);
		}

	TInt err = iFs.MkDirAll(*iRtaTempOutDir);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	}

CRefTestAgentImport::~CRefTestAgentImport()
	{
	if (iTempOutFileName)
		{
	// This means importing failed half way thru
		iFile.Close();
		iFs.Delete(*iTempOutFileName);
		delete iTempOutFileName;
		}

	delete iDrmRightsParser;
	delete iClientSuggestedFileName;
	delete iRightObjectCid;
	delete iRtaTempOutDir;
	}


EXPORT_C void CRefTestAgentImport::WriteL(const TDesC8& aData)
	 {
	 if(iMimeType == EMimeContentOrRights)
		 {//If a file content and its rights object are imported
		 WriteFileDataL(aData);
		 }
	 else
		 {//If a streamed content and its rights object are imported
		 WriteStreamDataL(aData);
		 }
	 }

void CRefTestAgentImport::WriteStreamDataL(const TDesC8& aData)
	{
	TPtrC8 inputData(aData);
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	if(EReceivingHeaderMark == iImportStage)
		{
		iImportStage = EReceivingRightsOffset;
		}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	
	if(aData.Length() == 0)
		{
		if(iImportStage == EReceivingRightsOffset)
			{// zero length data is not expected
			User::Leave(KErrCorrupt);
			}
		//If the length of the sent data is zero and the import stage is not EReceivingRightsOffset, 
		//it implies that the streaming data import has finished.
		if(iImportStage != EFinished)
			{//Change the state to EFinished and leave
			iImportStage = EFinished;
			
			if(iMimeType == EMimePostAcquisitionContent)
				{//if the content being imported is post-acquisition, add the constant data part to complete import operation.
				WriteDescriptorL(KContentXml());
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
				TInt64 offset = KRTAArchiveHeaderSize + sizeof(TInt64);
				User::LeaveIfError(iFile.Seek(ESeekStart, offset));
				TInt64 drmFileOffset = offset + sizeof(TInt) + 
										KMimeTypeRecordedContent().Length()+ sizeof(TInt) + iRightObjectCid->Length() + 
										sizeof(TInt64) + iBytesReceived + sizeof(TInt64); 
				WriteInt64L(drmFileOffset);
				User::LeaveIfError(iFile.Seek(ESeekStart, iDataOffset));
				WriteInt64L(iBytesReceived);
#else
				TInt intLen = 4;
				User::LeaveIfError(iFile.Seek(ESeekStart, intLen));
				WriteIntL(intLen + intLen + KMimeTypeRecordedContent().Length()+ intLen + iRightObjectCid->Length() + intLen + iBytesReceived + intLen);
				User::LeaveIfError(iFile.Seek(ESeekStart, iDataOffset));
				WriteIntL(iBytesReceived);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
				}
			User::Leave(KErrCANewFileHandleRequired);
			}
		return;
		}
			
	switch(iImportStage)
		{
	case EReceivingRightsOffset:	
		
		OpenTempOutFileL();
		WriteHeaderWithNoRightsL();
		
		if(iMimeType == EMimePostAcquisitionRights)
			{
			iImportStage = EReceivingRights;
			iRightsOffset = 1; //Set this variable to be able to get rights count when it required.
			}
		else if(iMimeType == EMimePostAcquisitionContent)
			{
			WriteDescriptorL(KMimeTypeRecordedContent());
			WriteDescriptorL(*iRightObjectCid);
			User::LeaveIfError(iFile.Seek(ESeekCurrent, iDataOffset));
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
			WriteInt64L(0);
#else
			WriteIntL(0);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
			iImportStage = EReceivingDrmFiles;	
			}
		
		WriteStreamDataL(inputData);
		break;
	
	case EReceivingRights:
		// Parse the Xml data as it's received
		iDrmRightsParser->ParseL(inputData);
		iBytesReceived += inputData.Length();
		break;
	
	case EReceivingDrmFiles:
		// Write the streaming content as it's received
		User::LeaveIfError(iFile.Write(inputData));
		iBytesReceived += inputData.Length();
		break;
		
	default:
		User::Panic(KRtaPanicString(), ERtaPanicImportInvalidState);
		break;
		}
	}


 
 void CRefTestAgentImport::WriteFileDataL(const TDesC8& aData)
	 {
	 TPtrC8 rightsOffsetFragment;
	 TPtrC8 drmFilesOffsetFragment;
	 TPtrC8 remainingContentFragment;
	 TPtrC8 drmFilesSizeFragment;
	 TPtrC8 remainingDrmFilesFragment;
	 TPtrC8 rightsSizeFragment;
	 TPtrC8 inputData(aData);
	 
	 if(aData.Length() > 0)
		 {
		 switch(iImportStage)
		 	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		 case EReceivingHeaderMark:
			 {
			 TUint size = iHeaderBuff.MaxLength() - iHeaderBuff.Length();
			 if(inputData.Length() < size)
				 {
				 size = inputData.Length();				 
				 }
			 iHeaderBuff.Append(inputData.Left(size));
			 inputData.Set(aData.Right(aData.Length() - size));
			 iBytesReceived += size;
			 if(iHeaderBuff.Length() == KRTAArchiveHeaderSize)
				 {
				 iImportStage = EReceivingRightsOffset;
				 if(iHeaderBuff.Compare(KRTAArchiveHeader) == 0)
					 {
					 iOffsetSize = sizeof(TInt64);
					 }
				 else
					 {
					 iOffsetSize = sizeof(TInt);
					 // Bytes received in case of header's absence in the archive.
					 iBytesReceived -= KRTAArchiveHeaderSize;
					 WriteFileDataL(iHeaderBuff);
					 }
				 WriteFileDataL(inputData);
				 }
			 
			 break;
			 }
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		 case EReceivingRightsOffset:
		 	//ASSERT_ALWAYS(iBytesReceived < iRightsOffsetPckg.MaxLength());
			
		 	// Grab the remaining bytes of the rights offset
		 	rightsOffsetFragment.Set(inputData.Left(iOffsetSize - iRightsOffsetPckg.Length()));
		 	iRightsOffsetPckg.Append(rightsOffsetFragment);
		 	iBytesReceived += rightsOffsetFragment.Length();
				
			// See if we have received the entire iRightsOffset
			if(iRightsOffsetPckg.Length() == iOffsetSize)
				{
				// Continue processing any remaining data in the next stage
				// by recursively calling WriteData()
				inputData.Set(aData.Right(aData.Length() - rightsOffsetFragment.Length()));
				iImportStage = EReceivingDrmFilesOffset;
				WriteFileDataL(inputData);
				}
			break;

		 case EReceivingDrmFilesOffset:
		 	//ASSERT_ALWAYS(iBytesReceived >= iRightsOffsetPckg.MaxLength());
		 	//ASSERT_ALWAYS(iBytesReceived < iRightsOffsetPckg.MaxLength() + iDrmFilesOffsetPckg.MaxLength());
			
		 	// Grab the remaining bytes of the rights offset
		 	drmFilesOffsetFragment.Set(inputData.Left(iOffsetSize - iDrmFilesOffsetPckg.Length()));
		 	iDrmFilesOffsetPckg.Append(drmFilesOffsetFragment);
		 	iBytesReceived += drmFilesOffsetFragment.Length();

		 	// See if we have received the entire iDrmFilesOffset
		 	if(iDrmFilesOffsetPckg.Length() == iOffsetSize)
		 		{
		 		// Continue processing any remaining data in the next stage
		 		// by recursively calling WriteData()
		 		inputData.Set(aData.Right(aData.Length() - drmFilesOffsetFragment.Length()));
		 		
		 		OpenTempOutFileL();
		 		WriteHeaderWithNoRightsL();

		 		// Next part of the archive to process is the content. But
		 		// if no DRM section then next item is rights size.
		 		iImportStage = (iDrmFilesOffset == 0) ? EReceivingRightsSize : EReceivingContent;
		 		
		 		WriteFileDataL(inputData);
		 		}
		 	break;

		 case EReceivingContent:
		 	// ASSERT_ALWAYS(iBytesReceived < iDrmFilesOffset);
		 	if(iBytesReceived + inputData.Length() <= iDrmFilesOffset)
		 		{
		 		// all the data to receive is just content stuff
		 		// write it straight to the output file
		 		User::LeaveIfError(iFile.Write(inputData));
		 		iBytesReceived += inputData.Length();
		 		}
			else
				{
				// The data written includes the boundary indicating the
				// end of the content and the start of the content XML file
				// ie. the offset given by iDrmFilesOffset
				
				// Grab the remaining bytes of the content and write them to the file
				remainingContentFragment.Set(inputData.Left(iDrmFilesOffset - iBytesReceived));
				User::LeaveIfError(iFile.Write(remainingContentFragment));
				iBytesReceived += remainingContentFragment.Length();
				
				// the remaining inputData includes the content Xml File size
				inputData.Set(aData.Right(aData.Length() - remainingContentFragment.Length()));
				iImportStage = EReceivingDrmFilesSize;
				WriteFileDataL(inputData);
				}
			break;
		case EReceivingDrmFilesSize:
			// Grab the remaining bytes of the rights offset
			drmFilesSizeFragment.Set(inputData.Left(iDrmFilesSizePckg.MaxLength() - iDrmFilesSizePckg.Length()));
			iDrmFilesSizePckg.Append(drmFilesSizeFragment);
			iBytesReceived += drmFilesSizeFragment.Length();

			// See if we have received the entire iDrmFilesOffset
			if(iDrmFilesSizePckg.Length() == iDrmFilesSizePckg.MaxLength())
				{
				// Write the DrmFiles size to the output file
				User::LeaveIfError(iFile.Write(iDrmFilesSizePckg));
				
				// Continue processing any remaining data in the next stage
				// by recursively calling WriteData()
				inputData.Set(aData.Right(aData.Length() - drmFilesSizeFragment.Length()));
				
				iImportStage = EReceivingDrmFiles;
				WriteFileDataL(inputData);
				}
			break;
		case EReceivingDrmFiles:
			// ASSERT_ALWAYS(iBytesReceived < iDrmFilesOffset);
			if(iBytesReceived + inputData.Length() < iDrmFilesOffset + iDrmFilesSize + iDrmFilesSizePckg.MaxLength())
				{
				// all the data to receive is just drm files xml stuff
				// write it straight to the output file
				User::LeaveIfError(iFile.Write(inputData));
				iBytesReceived += inputData.Length();
				}
			else
				{
				// The data written includes the boundary indicating the
				// end of the drmFiles and the start of the rights file
				// ie. the offset given by iDrmRightsOffset

				// Grab the remaining bytes of the drmFiles XML and write it to the file
				remainingDrmFilesFragment.Set(inputData.Left(iDrmFilesOffset + iDrmFilesSize + iDrmFilesSizePckg.MaxLength() - iBytesReceived));
				User::LeaveIfError(iFile.Write(remainingDrmFilesFragment));
				iBytesReceived += remainingDrmFilesFragment.Length();
				
				// any remaining inputData is the rights Xml File
				inputData.Set(aData.Right(aData.Length() - remainingDrmFilesFragment.Length()));
				if(iRightsOffset == 0)
					{
					iImportStage = EFinished;	
					if(inputData.Length() > 0)
						{
						User::Leave(KErrCorrupt);
						}
					User::Leave(KErrCANewFileHandleRequired);
					}
				else
					{
					iImportStage = EReceivingRightsSize;		
					}
				WriteFileDataL(inputData);
				}
			break;
		case EReceivingRightsSize:
			// Grab the remaining bytes of the rights offset
			rightsSizeFragment.Set(inputData.Left(iRightsSizePckg.MaxLength() - iRightsSizePckg.Length()));
			iRightsSizePckg.Append(rightsSizeFragment);
			iBytesReceived += rightsSizeFragment.Length();

			// See if we have received the entire iDrmFilesOffset
			if(iRightsSizePckg.Length() == iRightsSizePckg.MaxLength())
				{
				// Don't write the rights size to the output file
				// Rights are parsed not written to the file
					
				// Continue processing any remaining data in the next stage
				// by recursively calling WriteFileDataL()
				inputData.Set(aData.Right(aData.Length() - rightsSizeFragment.Length()));
				
				iImportStage = EReceivingRights;
				WriteFileDataL(inputData);
				}
			break;
		case EReceivingRights:
			if(iBytesReceived + inputData.Length() <= iRightsOffset + iRightsSize + iRightsSizePckg.MaxLength())
				{
				// Parse the Xml data as it's received
				iDrmRightsParser->ParseL(inputData);
				iBytesReceived += inputData.Length();
				}
			else
				{
				// unexpected data after the rights
				User::Leave(KErrCorrupt);		
				}
				
			// See if all the rights have been received
			if(iBytesReceived == iRightsOffset + iRightsSize + iRightsSizePckg.MaxLength())
				{
				iImportStage = EFinished;
				User::Leave(KErrCANewFileHandleRequired);
				}
			break;
			
		case EFinished:
			// not expecting any more data
			User::Leave(KErrCorrupt);		
			break;
		default:
			User::Panic(KRtaPanicString(), ERtaPanicImportInvalidState);
			break;
		 	}
		 }
	 }


EXPORT_C void CRefTestAgentImport::ContinueWithNewOutputFileL(RFile& aFile, const TDesC& aFileName)
	{
	if(iImportStage != EFinished)
		{
		User::Panic(KRtaPanicString(), ERtaPanicImportInvalidState);
		}

	if (! iOutputFileOpened)
		{
		__ASSERT_DEBUG(iTempOutFileName, User::Invariant() );

		// If OutputContentL leaves, do not want to do it again.
		iOutputFileOpened = ETrue;
		OutputContentL(aFile);

		if (NULL == iClientSuggestedFileName)
			{
			iClientSuggestedFileName = HBufC8::NewL(aFileName.Length());
			iClientSuggestedFileName->Des().Copy(aFileName);
			}
		}
	else
		{
		ProcessRightsL(aFile);
		}
	}


void CRefTestAgentImport::WriteHeaderWithNoRightsL()
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	iFile.Write(KRTAArchiveHeader);
	TInt64 zero = 0;
	TPckg <TInt64> zeroPckg(zero);
#else
	TInt zero = 0;
	TPckg <TInt> zeroPckg(zero);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	// Write a zero to the file in place of the rights offset
	// since we are removing rights as the file is processed
	User::LeaveIfError(iFile.Write(zeroPckg));
	
	if(iMimeType == EMimeContentOrRights)
		{// Write the DrmFiles Offset to the output file
		User::LeaveIfError(iFile.Write(iDrmFilesOffsetPckg));
		}
	else
		{// Write a zero to the file in place of the drm files offset
		User::LeaveIfError(iFile.Write(zeroPckg));
		}
	}
	
_LIT(KReceiptFileExtension, ".TXT");

EXPORT_C TInt CRefTestAgentImport::GetSuggestedOutputFileExtension(TDes& aFileExtension)
	{
	if(! iOutputFileOpened)
		{
		aFileExtension.Copy(KRtaExtensionContent());
		}
	else
		{
		// Use ".txt", not .receipt because there are testcases
		// out there which use CAF's DeleteFile API to delete receipts.
		// When using RTA's manager's DeleteFile on a "cid.receipt",
		// it means delete rights with the given cid.
		aFileExtension.Copy(KReceiptFileExtension());
		}
	return KErrNone;
	}

EXPORT_C TInt CRefTestAgentImport::GetSuggestedOutputFileName(TDes& aFileName)
	{
	TRAPD( err, GetSuggestedOutputFileNameL(aFileName) );
	return err;
	}

void CRefTestAgentImport::GetSuggestedOutputFileNameL(TDes& aFileName)
	{
	if(iImportStage != EFinished)
		{
		User::Leave(KErrNotReady);
		}

	TInt rightsCount = (iRightsOffset == 0) ? 0 : iDrmRightsParser->DrmRightsL().RightsCount();

	if (! iOutputFileOpened)
		{
		if (iClientSuggestedFileName)
			{
			TChar slash(KPathDelimiter);
			// If LocateReverse returns a negative number, TDesC8::Right
			// will return the entire des anyway.
			TInt n = iClientSuggestedFileName->Length() - iClientSuggestedFileName->LocateReverse(slash) - 1;
			aFileName.Copy(iClientSuggestedFileName->Right(n));
			}
		else if(rightsCount)
			{
			// Use first rights object's cid as output filename.
			aFileName.Copy(iDrmRightsParser->DrmRightsL().Rights(0).Cid());
			if(iMimeType == EMimePostAcquisitionRights)
				{
				aFileName.Append(KRtaExtensionPostAcquisitionRights);
				}
			else
				{
				aFileName.Append(KRtaExtensionContent);
				}
			}
		else
			{
			aFileName.Copy(KNullDesC);
			}
		}
	else if(rightsCount)
		{
		aFileName.Copy(iDrmRightsParser->DrmRightsL().Rights(0).Cid());
		// Use ".txt", not .receipt because there are testcases
		// out there which use CAF's DeleteFile API to delete receipts.
		// When using RTA's manager's DeleteFile on a "cid.receipt",
		// it means delete rights with the given cid.
		aFileName.Append(KReceiptFileExtension());
		}
	else
		{
		// All rights have been processed.
		User::Leave(KErrCompletion);
		}
	}

EXPORT_C TInt CRefTestAgentImport::WriteComplete()
	{
	TRAPD( err, WriteCompleteL() );
	return err;
	}

void CRefTestAgentImport::WriteCompleteL()
	{
	if(iImportStage != EFinished)
		{
		User::Leave( KErrCorrupt );
		}

	TInt rightsCount = (iRightsOffset) ? iDrmRightsParser->DrmRightsL().RightsCount() : 0;
	if(rightsCount > 0)
		{
		// Erase the client suggested filename from db before adding
		// the imported rights. This way the client who uses the
		// feature of DeleteFile(suggestedFileName) will not
		// touch other's rights. And clients who do not know
		// about this feature are not affected. They can just
		// keep using the default suggested filename and tests
		// continue to pass.
		RRtaRights rightsServer;
		User::LeaveIfError(rightsServer.Connect());
		CleanupClosePushL(rightsServer);
		if (iClientSuggestedFileName)
			{
			rightsServer.EraseDeleteKeyL(*iClientSuggestedFileName);
			}
		CleanupStack::PopAndDestroy(&rightsServer);

		// This will start a loop where all the rights are processed
		// Through Continue with new output fileL
		User::Leave(KErrCANewFileHandleRequired);
		}
	}

void CRefTestAgentImport::ProcessRightsL(RFile& aFile)
	{
	RRtaRights rightsServer;

	// Just process one rights object at a time
	if(iDrmRightsParser->DrmRightsL().RightsCount() > 0)
		{
		User::LeaveIfError(rightsServer.Connect());
		CleanupClosePushL(rightsServer);

		CRightsObject& ro = iDrmRightsParser->DrmRightsL().Rights(0);
		// A client (JMI) requested that the DeleteFile API doubles
		// as API to delete rights objects. Clients will pass the 
		// suggested output filename at rights import time to specify
		// the rights to delete.
		if (iClientSuggestedFileName == NULL)
			{
			ro.SetDeleteKeyL(KNullDesC8);
			}
		else
			{
			ro.SetDeleteKeyL(*iClientSuggestedFileName);
			}
		rightsServer.AddRightsL(ro);

		// write the receipt file		
		iDrmRightsParser->DrmRightsL().Rights(0).WriteDescription(aFile);
		
		// Remove the rights object from our array
		iDrmRightsParser->DrmRightsL().Remove(0);
		CleanupStack::PopAndDestroy(&rightsServer);
		
		if(iDrmRightsParser->DrmRightsL().RightsCount() > 0)
			{
			User::Leave(KErrCANewFileHandleRequired);
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CRefTestAgentImport::OutputContentL(RFile& aFile)
	{
	const TInt KBufSize = 512;
	HBufC8* hbuf = HBufC8::NewLC(KBufSize);
	TPtr8 bufptr = hbuf->Des();

	// The content to output has been buffered in iFile.
	// Thus rewind iFile and then copy the contents of
	// iFile to aFile.
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 pos = 0;
#else
	TInt pos = 0;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	User::LeaveIfError( iFile.Seek(ESeekStart,pos) );

	do
		{
		User::LeaveIfError(iFile.Read(bufptr));
		User::LeaveIfError(aFile.Write(bufptr));
		}
	while (bufptr.Length() == bufptr.MaxLength());

	CleanupStack::PopAndDestroy(hbuf);
	aFile.Close();
	iFile.Close();

	iFs.Delete(*iTempOutFileName);
	delete iTempOutFileName;
	iTempOutFileName = NULL;
	}

void CRefTestAgentImport::OpenTempOutFileL()
	{
	const TInt KMaxTry = 2048;
	_LIT(KRtaTempFileNameFmt, "%S%S%x%S");

	TInt namelength = (*iRtaTempOutDir).Length() +
		              KRtaTempOutFileName().Length() + 
					  4 +
		              KRtaExtensionContent().Length();

	delete iTempOutFileName;
	User::LeaveIfNull( iTempOutFileName = HBufC::New(namelength) );

	TPtr des = iTempOutFileName->Des();
	TInt ret = KErrAlreadyExists;

	for (TInt i = 1; (i <= KMaxTry) && (KErrAlreadyExists == ret); i++)
		{
		des.Format(KRtaTempFileNameFmt, iRtaTempOutDir, &KRtaTempOutFileName, i, &KRtaExtensionContent);
		ret = iFile.Create(iFs, des, EFileStream | EFileWrite);
		if (ret == KErrNone)
			{
			return;
			}
		}

	// C:\rta\tmpout1.content to C:\rta\tmpout800.content all exist!
	delete iTempOutFileName;
	iTempOutFileName = NULL;
	User::Leave(ret);
	}

void CRefTestAgentImport::WriteIntL(TInt aLen)
	{
	TPckg<TInt> lengthPckg(aLen);
	User::LeaveIfError(iFile.Write(lengthPckg,lengthPckg.Length()));
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
void CRefTestAgentImport::WriteInt64L(TInt64 aLen)
	{
	TPckg<TInt64> lengthPckg(aLen);
	User::LeaveIfError(iFile.Write(lengthPckg,lengthPckg.Length()));
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

void CRefTestAgentImport::WriteDescriptorL(const TDesC8& aBuffer)
	{
	TInt length = aBuffer.Length();
	WriteIntL(length);	
	User::LeaveIfError(iFile.Write(aBuffer, length));
	}


EXPORT_C void CRefTestAgentImport::SetMetaDataL(const ContentAccess::CMetaDataArray& aMetaDataArray)
	{
	iRightObjectCid = aMetaDataArray.SearchL(_L8("Cid")).AllocL();
	}

EXPORT_C void CRefTestAgentImport::GetCurrentFileNameAndBuffer(TDes& aFileName, TDes8& aBuffer)
	{
	iFile.FullName(aFileName);
	ReadBuffer(aBuffer);
	}

void CRefTestAgentImport::ReadBuffer(TDes8& aBuffer)
     {
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
     TInt64 originalPos = 0, startOffset = 0;
#else
     TInt originalPos = 0, startOffset = 0;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
     iFile.Seek(ESeekCurrent, originalPos);
     iFile.Seek(ESeekStart, startOffset);
     iFile.Read(aBuffer);
     iFile.Seek(ESeekStart, originalPos);
     }


