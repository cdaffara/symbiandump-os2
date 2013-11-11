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


#include <apmstd.h>
#include "drmfilecontent.h"
#include "EntireFile.h"
#include "PartOfFile.h"
#include "parsehandlerbase.h"
#include <caf/caferr.h>

using namespace ReferenceTestAgent;
using namespace ContentAccess;

CDrmFileContent* CDrmFileContent::NewL(const TDesC8& aCid, const TDesC& aName, const TDesC& aUniqueId, const TDesC& aFileName, RAttributeSet& aAttributeSet, RStringAttributeSet& aStringAttributeSet)
	{
	CDrmFileContent* self = new (ELeave) CDrmFileContent;
	CleanupStack::PushL(self);
	self->ConstructL(aCid, aName, aUniqueId, aFileName, aAttributeSet, aStringAttributeSet);
	CleanupStack::Pop(self);
	return self;
	}


CDrmFileContent::CDrmFileContent()
	{
	}

void CDrmFileContent::ConstructL(const TDesC8& aCid, const TDesC& aName, const TDesC& aUniqueId, const TDesC& aFileName, RAttributeSet& aAttributeSet, RStringAttributeSet& aStringAttributeSet)
	{
	TInt err = KErrNone;
	TInt value = 0;
	iCid = aCid.AllocL();	
	iName = aName.AllocL();
	iUniqueId = aUniqueId.AllocL();
	iFileName = aFileName.AllocL();
	
	// Initialise default attributes
	SetDefaultAttributeL(EIsProtected, ETrue, KErrNone);
	SetDefaultAttributeL(EIsForwardable, EFalse, KErrNone);
	SetDefaultAttributeL(EIsModifyable, EFalse, KErrNone);
	SetDefaultAttributeL(EIsCopyable, EFalse, KErrNone);
	SetDefaultAttributeL(ECanPlay, EFalse, KErrNone);
	SetDefaultAttributeL(ECanPrint, EFalse, KErrNone);
	SetDefaultAttributeL(ECanExecute, EFalse, KErrNone);
	SetDefaultAttributeL(ECanView, EFalse, KErrNone);
	SetDefaultAttributeL(ERightsNone, ETrue, KErrNone);
	SetDefaultAttributeL(ERightsPending, EFalse, KErrNone);
	SetDefaultAttributeL(ERightsHaveExpired, EFalse, KErrNone);
	SetDefaultAttributeL(EPreviewAvailable, EFalse, KErrNone);
	SetDefaultAttributeL(EContentCDataInUse, 0, KErrCANotSupported);
	SetDefaultAttributeL(ECanRewind, EFalse, KErrNone);
	SetDefaultAttributeL(ECopyPaste, EFalse, KErrNone);
	SetDefaultAttributeL(ERightsConsumable, EFalse, KErrNone);
	SetDefaultAttributeL(ERightsStateless, EFalse, KErrNone);
	SetDefaultAttributeL(ECanMove, ETrue, KErrNone);
	SetDefaultAttributeL(ECanRename, ETrue, KErrNone);
	SetDefaultAttributeL(ECanAutomaticConsume, EFalse, KErrNone);
	SetDefaultAttributeL(EContentVersion, 0, KErrCANotSupported);
	
	// Set attributes from construction arguments
	TInt i = 0;
	for(i = 0; i < aAttributeSet.Count(); i++)
		{
		// Don't leave if there is an error, it is valid to have errors
		// such as KErrCANotSupported
		TInt attribute = aAttributeSet[i];
		err = aAttributeSet.GetValue(attribute, value);
		iAttributeSet.AddL(attribute);
		User::LeaveIfError(iAttributeSet.SetValue(attribute, value, err));
		}

	// Set string attributes from construction parameters
	for(i = 0; i < aStringAttributeSet.Count(); i++)
		{
		// Don't leave if there is an error, it is valid to have errors
		// such as KErrCANotSupported
		TInt attribute = aStringAttributeSet[i];
		HBufC* valueBuf = HBufC::NewLC(aStringAttributeSet.GetValueLength(attribute));
		TPtr valuePtr = valueBuf->Des();
		err = aStringAttributeSet.GetValue(attribute, valuePtr);
		iStringAttributeSet.AddL(attribute);
		User::LeaveIfError(iStringAttributeSet.SetValue(attribute, valuePtr, err));
		CleanupStack::PopAndDestroy(valueBuf);
		}
		
	// It is essential that a mime type attribute was set
	TBuf <KMaxDataTypeLength> mimeType;		
	User::LeaveIfError(iStringAttributeSet.GetValue(EMimeType, mimeType));
	iMimeType = HBufC8::NewL(mimeType.Length());
	TPtr8 mimeTypePtr = iMimeType->Des();
	mimeTypePtr.Copy(mimeType);
	}

void CDrmFileContent::SetDefaultAttributeL(TInt aAttribute, TInt aValue, TInt aErrorCode)	
	{
	iDefaultAttributeSet.AddL(aAttribute);
	User::LeaveIfError(iDefaultAttributeSet.SetValue(aAttribute, aValue, aErrorCode));
	}
	


CDrmFileContent::~CDrmFileContent()
	{
	delete iCid;
	delete iName;
	delete iUniqueId;
	delete iFileName;
	delete iVirtualFile;
	delete iMimeType;
	
	iDefaultAttributeSet.Close();
	iAttributeSet.Close();
	iStringAttributeSet.Close();
	}

TInt CDrmFileContent::VirtualFilePresent()
	{
	_LIT(KCantOpenContentFile, "ERROR: Cannot open content file: ");
	TBuf <KMaxPath + sizeof(KCantOpenContentFile)> errorMsg;
	
	TInt err = KErrNone;
	if(!iVirtualFile)
		{
		TRAP(err, iVirtualFile = CEntireFile::NewL(*iFileName));
		if(err != KErrNone)
			{
			errorMsg.Copy(KCantOpenContentFile);
			errorMsg.Append(*iFileName);

			#ifdef _DEBUG
			TRAP_IGNORE( CParseHandlerBase::LogL(errorMsg) );
			#endif

			}
		}
	return err;
	}

EXPORT_C TInt CDrmFileContent::Read(TDes8& aBuffer, TInt aLength)
	{
	TInt err = VirtualFilePresent();
	if(err != KErrNone)
		{
		return err;
		}
	return iVirtualFile->Read(aBuffer, aLength);
	}
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt CDrmFileContent::Read(TInt64 aPos, TDes8& aBuffer, TInt aLength)
#else
EXPORT_C TInt CDrmFileContent::Read(TInt aPos, TDes8& aBuffer, TInt aLength)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	TInt err = VirtualFilePresent();
	if(err != KErrNone)
		{
		return err;
		}
	return iVirtualFile->Read(aPos, aBuffer, aLength);
	}
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt CDrmFileContent::Seek(TSeek aMode, TInt64 &aPos) 
#else
EXPORT_C TInt CDrmFileContent::Seek(TSeek aMode, TInt &aPos) 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	TInt err = VirtualFilePresent();
	if(err != KErrNone)
		{
		return err;
		}

	return iVirtualFile->Seek(aMode, aPos);
	}
		
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt CDrmFileContent::Size(TInt64& aSize)
#else
EXPORT_C TInt CDrmFileContent::Size(TInt& aSize)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	TInt err = VirtualFilePresent();
	if(err != KErrNone)
		{
		return err;
		}

	return iVirtualFile->Size(aSize);	
	}

EXPORT_C TInt CDrmFileContent::GetAttribute(TInt aAttribute, TInt& aValue)
	{
	// Get one of the attribute values preset in the XML file
	TInt err = iAttributeSet.GetValue(aAttribute, aValue);
	if(err == KErrNotFound)
		{
		// If no value was preset use the default
		err = iDefaultAttributeSet.GetValue(aAttribute, aValue);
		}
	return err;
	}
	
EXPORT_C TInt CDrmFileContent::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
	return iStringAttributeSet.GetValue(aAttribute, aValue);
	}
	
EXPORT_C TInt CDrmFileContent::GetAttributeSet(RAttributeSet &aAttributeSet)
	{
	TInt i = 0;
	TInt value = 0;
	for(i = 0; i < aAttributeSet.Count(); i++)
		{
		TInt attribute = aAttributeSet[i];
		TInt err = GetAttribute(attribute, value);
		aAttributeSet.SetValue(attribute, value, err);
		}
	return KErrNone;
	}

EXPORT_C TInt CDrmFileContent::GetStringAttributeSet(RStringAttributeSet &aStringAttributeSet)
	{
	TInt i = 0;
	TInt err = KErrNone;
	for(i = 0; i < aStringAttributeSet.Count(); i++)
		{
		TInt attribute = aStringAttributeSet[i];
		HBufC* value = HBufC::New(iStringAttributeSet.GetValueLength(attribute));
		if(value == NULL)
			{
			err = KErrNoMemory;
			break;
			}
		TPtr valuePtr = value->Des();
		err = GetStringAttribute(attribute, valuePtr);
		aStringAttributeSet.SetValue(attribute, valuePtr, err);
		delete value;
		}
	return err;
	}

EXPORT_C const TDesC8& CDrmFileContent::Cid()
	{
	return *iCid;
	}
	
const TDesC& CDrmFileContent::UniqueId()
	{
	return *iUniqueId;	
	}
	
const TDesC& CDrmFileContent::Name()
	{
	return *iName;
	}

EXPORT_C const TDesC8& CDrmFileContent::MimeType()
	{
	return *iMimeType;	
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C void CDrmFileContent::SetCommonSourceFileL(RFile64& aFile, RSemaphore& aFileLock, TInt64 aOffset, TInt64 aSize)
#else
EXPORT_C void CDrmFileContent::SetCommonSourceFileL(RFile& aFile, RSemaphore& aFileLock, TInt aOffset, TInt aSize)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	if (iVirtualFile)
		{
			delete iVirtualFile;
			iVirtualFile = NULL;
		}
	iVirtualFile = CPartOfFile::NewL(aFile, aFileLock, aOffset, aSize);
	}

