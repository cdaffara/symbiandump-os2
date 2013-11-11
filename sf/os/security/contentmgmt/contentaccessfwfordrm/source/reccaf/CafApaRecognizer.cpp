/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32file.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "resolver.h"
#include "mimetypemapping.h"
#include "CafApaRecognizer.h"

using namespace ContentAccess;


CApaCafRecognizer* CApaCafRecognizer::NewL()
	{
	CApaCafRecognizer* self;
	self = new (ELeave) CApaCafRecognizer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CApaCafRecognizer::ConstructL()
	{
	iResolver = CAgentResolver::NewL(ETrue);
	SetupContentTypesL();
	}

CApaCafRecognizer::CApaCafRecognizer() : 
	CApaDataRecognizerType(KUidCafApaRecognizer, CApaDataRecognizerType::ELow)
	{
	// Assume initially we don't recognize any data types (ie. no agents)
	iCountDataTypes = 0;
	}

CApaCafRecognizer::~CApaCafRecognizer()
	{
	iContentTypes.ResetAndDestroy();
	delete iResolver;
	}

void CApaCafRecognizer::SetupContentTypesL()
	{
	// Construct the array used to map content types to our drm mime types
	CMimeTypeMapping* mimeType; 

	// The RecCaf configuration file is just a list of content mime types
	// CApaCafRecognizer expects each mime type to be on a separate line
	// The configuration file is stored in the Apparc server's private directory.
	// This should be ok since the recognizer is run in the apparc servers process
	
	// ROM file
	_LIT(KRomConfigurationFile, "Z:\\private\\10003A3F\\RecCaf\\RecCafMimeTypes.txt");

	// System drive file	
	_LIT(KConfigurationFile, "_:\\private\\10003A3F\\RecCaf\\RecCafMimeTypes.txt");
	TBuf<47> sysDriveConfigFile(KConfigurationFile);
	sysDriveConfigFile[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	TInt result = file.Open(fs, sysDriveConfigFile, EFileRead | EFileStream | EFileShareAny);
	if (result != KErrNone)
		{
		result = file.Open(fs, KRomConfigurationFile(), EFileRead | EFileStream | EFileShareAny);
		}

	if (result == KErrNone)
		{
		CleanupClosePushL(file);
		
		// create file stream
		RFileReadStream inputFileStream(file);
	    CleanupClosePushL(inputFileStream);

		TBuf8 <KMaxDataTypeLength> buf;

		do
			{
			// Read a content mime type from the file
			result = ReadLine(inputFileStream, buf);

			if (result == KErrNone || result == KErrEof)
				{
				// Add a new entry into the array of mime types
				if (buf.Length())
					{
					// force to lower case to ensure that chosen lower case scheme for mime
					// types is maintained
					buf.LowerCase();
					mimeType = CMimeTypeMapping::NewL(buf);
					CleanupStack::PushL(mimeType);
					User::LeaveIfError(iContentTypes.Append(mimeType));
					CleanupStack::Pop(mimeType); 
					}
				}
			else
				{
				// Encountered an error reading the file
				// don't know how to recover so leave
				iContentTypes.ResetAndDestroy();
				User::Leave(result);
				}
			} while (result != KErrEof);

		CleanupStack::PopAndDestroy(2, &file); // inputFileStream, file
		}

	CleanupStack::PopAndDestroy(&fs);

	// Add multiple content object file mime type
	_LIT8(KApplicationXCafStr, "application/x-caf");
	mimeType = CMimeTypeMapping::NewL(KApplicationXCafStr());
	CleanupStack::PushL(mimeType);
	User::LeaveIfError(iContentTypes.Append(mimeType));
	CleanupStack::Pop(mimeType); 


	// Add supplier Mime types 
	// otherwise nobody will recognize unprocessed files 
	// that could be churned through the CAF Supplier API
	// The resolver ensures that its supplier mime types are lower case
	for(TInt index = 0; index < iResolver->SupplierMimeTypes().Count(); index++)
		{
		mimeType = CMimeTypeMapping::NewL(iResolver->SupplierMimeTypes()[index],
										  iResolver->SupplierMimeTypes()[index]);
		CleanupStack::PushL(mimeType);
		User::LeaveIfError(iContentTypes.Append(mimeType));
		CleanupStack::Pop(mimeType); 
		}
	
	iCountDataTypes = iContentTypes.Count();
	}

TInt CApaCafRecognizer::ReadLine(RFileReadStream& aInStream, TDes8& aBuffer)
	{
	const TChar KCarriageReturn = '\r';
	const TChar KLineFeed = '\n';

	// Clear the buffer
	aBuffer.SetLength(0);
	
	// Read line upto linefeed delimiter
	TRAPD(err, aInStream.ReadL(aBuffer, KLineFeed));
	TInt length = aBuffer.Length();
	
	// assume all lines must end in CRLF and do not 
	// allow blank lines. So length must be at least 2 for
	// the CRLF characters
	if(length < 3)
		{
		err = KErrEof;
		}
	if (err == KErrNone)
		{
		// remove line feed and any carriage return 
		if (aBuffer[length - 1] == KLineFeed)
			{
			--length;
			}
		if (aBuffer[length - 1] == KCarriageReturn)
			{
			--length;
			}
		aBuffer.SetLength(length);
		}
	return err;
	}


TUint CApaCafRecognizer::PreferredBufSize()
	{
	// Ask all of the agents if they have a preferred buffer size for calls to DoRecognizeL()
	return iResolver->PreferredBufferSize();
	}

TDataType CApaCafRecognizer::SupportedDataTypeL(TInt aIndex) const
	{
	// leave if an out of range aIndex is supplied
	if (aIndex < 0 || aIndex >= iContentTypes.Count())
		{
		User::Leave(KErrArgument);
		}

	return TDataType(iContentTypes[aIndex]->CafMimeType());
	}
	
void CApaCafRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	TBuf8 <KMaxDataTypeLength> FileMimeType;
	TBuf8 <KMaxDataTypeLength> ContentMimeType;
	
	// Need to start by assuming we don't recognize the file
	iConfidence = ENotRecognized;
	
	TBool isRecognized = EFalse;
	
	// If aName is prepended with 2 colons (::), then it is not valid, and we need to
	// derive it from the file handle. Otherwise, we assume that aName is valid
	if (aName.Match(_L("::*")) == KErrNotFound)
		{
		isRecognized = iResolver->DoRecognizeL(aName, aBuffer, FileMimeType, ContentMimeType);
		}
	// aName is not a valid URI/filename and a file handle must be used. This is marked
	// by two colons at the start of aName.
	// We need to get the actual file name from the file handle.
	else 
		{
		RFile* fileHandle = CApaCafRecognizer::FilePassedByHandleL();
		
		if( fileHandle == NULL )
		{
			return;
		}

		TBuf<KMaxDataTypeLength> fileName;
		User::LeaveIfError(fileHandle->Name(fileName));
		isRecognized = iResolver->DoRecognizeL(fileName, aBuffer, FileMimeType, ContentMimeType);
		}
	
	if (isRecognized == EFalse)
	{
		return;
	}


	// If there is no content type but the file is recognized it is assumed
	// to be a file that can be processed through the supplier interface
		
	// dummy object for comparison in match function
	CMimeTypeMapping* mimeTypeMapping = (ContentMimeType.Length() == 0) ?  
										 CMimeTypeMapping::NewL(FileMimeType) :
										 CMimeTypeMapping::NewL(ContentMimeType);


	// check that the content mime type is one of the ones we
	// told Apparc we can recognize in the beginning
	// The mime type should always be found as the file has been recognised.
	// If not it is because an agent, when recognising the file, has set a mime type not in its supplier list.
	// Mime types are always forced to lower case therefore this match can be case sensitive.
	TInt index = iContentTypes.Find(mimeTypeMapping, CMimeTypeMapping::ContentMimeTypeMatch);
	if (index != KErrNotFound)
	{
	    iDataType = TDataType(iContentTypes[index]->CafMimeType());
		iConfidence=ECertain;
	}

	delete mimeTypeMapping;
	}

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x102031E9, CApaCafRecognizer::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	


