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


#include <caf/caf.h>
#include "TestAgentDrmContent.h"

using namespace ContentAccess;

CTestAgentDrmContent* CTestAgentDrmContent::NewL(RFile& aFile)
	{
	CTestAgentDrmContent *self = new (ELeave) CTestAgentDrmContent(aFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentDrmContent::CTestAgentDrmContent(RFile& aFile) : iFile(aFile)
	{
	}

CTestAgentDrmContent::~CTestAgentDrmContent()
	{	
	delete iContentMimeType;
	}	

void CTestAgentDrmContent::ConstructL()
	{
	// buffer for reading field sizes from the file
	TBuf8 <4> buf;

	// Get size of mime type field
	User::LeaveIfError(iFile.Read(buf,1));
	TUint8 contentTypeLength = buf[0];

	// Read mime type field
	iContentMimeType = HBufC8::NewL((TInt) contentTypeLength);
	TPtr8 mimePtr = iContentMimeType->Des();
	User::LeaveIfError(iFile.Read(mimePtr, (TInt) contentTypeLength));
	
	// Read whether or not file has been imported yet
	User::LeaveIfError(iFile.Read(buf,1));
	
	// Get size of data 
	User::LeaveIfError(iFile.Read(buf,4));
	iDataSize = buf[0];
	iDataSize <<= 8;
	iDataSize += buf[1];
	iDataSize <<= 8;
	iDataSize += buf[2];
	iDataSize <<= 8;
	iDataSize += buf[3];
	
	// Calculate offset of data
	iDataOffset = 1 + 1 + 4 + contentTypeLength;
	
	// Go to the start of the DRM file
	TInt pos = 0;
	Seek(ESeekStart, pos);
	}

TInt CTestAgentDrmContent::ReadTUintvarL(TDes8& aBuffer, TInt Offset, TInt& aNumBytes) const
	{
	TUint8 byte;
	TInt Value=0;
	TBool Continue = ETrue;

	for(aNumBytes = 0; Continue ; aNumBytes++)
		{
		Value <<= 7;
		if(Offset + aNumBytes == aBuffer.Length())
			User::Leave(KErrOverflow);

		byte = aBuffer[Offset + aNumBytes];
		if((byte & 0x80) == 0)
			{
			Continue=EFalse;
			}
		else 
			{
			byte &= 0x7F;
			}

		Value+=byte;
		}
	return Value;
	}


TInt CTestAgentDrmContent::Read(TDes8& aDes,TInt aLength)
	{
	return iFile.Read(aDes, aLength);
	}

void CTestAgentDrmContent::Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	iFile.Read(aDes, aLength, aStatus);
	}
	
void CTestAgentDrmContent::ReadCancel(TRequestStatus& aStatus)
{
	iFile.ReadCancel(aStatus);
}	

TInt CTestAgentDrmContent::Seek(TSeek aMode, TInt& aPos)
	{
	TInt rval;
	// Don't allow the user to seek into the OMA header, only around the data itself
	TInt Offset;

	switch(aMode)
		{
	case ESeekAddress:
		// Unlikely for this case to be used
		if(aPos > iDataSize || aPos < 0)
			return KErrArgument;

		Offset = iDataOffset + aPos;
		rval = iFile.Seek(aMode, Offset);
		//  Offset is populated with the ROM address, fill in aPos for the caller
		aPos=Offset;
		break;
	
	case ESeekStart:
		if(aPos > iDataSize)
			aPos = iDataSize;
		if(aPos < 0)
			return KErrArgument;
		Offset = iDataOffset + aPos;
		rval = iFile.Seek(aMode, Offset);
		break;
	case ESeekCurrent:
		Offset = aPos;
		rval = iFile.Seek(aMode, Offset);
		// if we have done a seek before the start of the data
		// do another seek to get back to the start of the data
		if(Offset < iDataOffset)
			{
			Offset = iDataOffset;
			rval = iFile.Seek(ESeekStart, Offset);
			aPos = 0;
			}
		else
			{
			aPos = Offset - iDataOffset;
			}
		break;
	case ESeekEnd:
		// offsets are negative for ESeekEnd
		Offset = aPos;
		if(Offset < - iDataSize)
			Offset = - iDataSize;
		rval = iFile.Seek(aMode, Offset);
		aPos = Offset - iDataOffset;
		break;
	default:
		return KErrNotSupported;
		}
	return rval;
	}

TPtrC8 CTestAgentDrmContent::ContentMimeType()
	{
	return *iContentMimeType;
	}

TInt CTestAgentDrmContent::GetAttribute(TInt aAttribute, TInt& aValue)
	{
	TInt err = KErrNone;
	
	switch(aAttribute)
		{
	case EIsProtected:
		aValue = ETrue;
		break;
	case EIsForwardable:
		aValue = EFalse;
		break;
	case EIsModifyable:
		aValue = EFalse;
		break;
	case EIsCopyable:
		aValue = EFalse;
		break;
	case ECanPlay:
		aValue = ETrue;
		break;
	case ECanPrint:
		aValue = ETrue;
		break;
	case ECanExecute:
		aValue = ETrue;
		break;
	case ECanView:
		aValue = ETrue;
		break;
	case EPreviewAvailable:
		aValue = EFalse;
		break;
	case EContentCDataInUse:
		aValue = KErrCANotSupported;
		break;
	case ECanRewind:
		aValue = ETrue;
		break;
	case ECopyPaste:
		aValue = ETrue;	
		break;
	case ECanMove:
		aValue = ETrue;
		break;
	case ECanRename:
		aValue = ETrue;
		break;
	case ECanAutomaticConsume:
		aValue = ETrue;
		break;
	case EContentVersion:
	default:
		err = KErrCANotSupported;
		break;
		};
	return err;
	}
	
TInt CTestAgentDrmContent::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
	TInt err = KErrNone;
	switch(aAttribute)
		{
		// the content object
		case EMimeType:
		aValue.Copy(*iContentMimeType);
		break;
	default:
		err = KErrCANotSupported;
		break;
		};
	return err;
	}

void CTestAgentDrmContent::DataSizeL(TInt &aSize)
	{
	aSize = iDataSize;
	}
	
void CTestAgentDrmContent::Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	TInt offset = aPos + iDataOffset;
	iFile.Read(offset, aDes, aLength, aStatus);
	}
