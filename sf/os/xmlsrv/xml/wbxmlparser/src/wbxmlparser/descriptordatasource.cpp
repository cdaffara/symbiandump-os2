// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <xml/xmlframeworkerrors.h>

#include "descriptordatasource.h"

const TInt KNoTransaction = -1;
const TInt KGranularity = 0x100; 


CDescriptorDataSource* CDescriptorDataSource::NewL()
/**
This method creates an instance of this class.

@return				The new'ed object.

*/
	{
	CDescriptorDataSource* self = new(ELeave) CDescriptorDataSource();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}



CDescriptorDataSource* CDescriptorDataSource::NewLC()
/**
This method creates an instance of this class and leaves it on the cleanup stack

@return				The new'ed object.

*/
	{
	CDescriptorDataSource* self = new(ELeave) CDescriptorDataSource();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}



CDescriptorDataSource::CDescriptorDataSource() 
/**
Constructor.

@post				The object is ready to accept it's document buffer.

*/
:	iStartTransaction(KNoTransaction),
	iEof(EFalse)
	{
	// do nothing;
	}



void CDescriptorDataSource::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KGranularity);
	
	// We need to get the position of the next insert.
	// CBufFlat only has Size() that does not return this.
	iBufferPtr.Set(iBuffer->Ptr(0));
	}



CDescriptorDataSource::~CDescriptorDataSource()
/**
Destructor.

@post				This object is properly destroyed.

*/
	{
	if (iBuffer)
		{
		delete iBuffer;
		}
	}



TUint8 CDescriptorDataSource::GetByteL() 
/**
This method gets a byte of data from the next position within the buffer.
The position is incremented by one.

@return				The next byte of data.

@pre				The buffer has been set.
@post				The position in the buffer has moved along by one byte.
@leave				KErrEof if there is no more data in the buffer.

@panic				The internal buffer positional mechanism has failed.

*/	
	{
	if (iBytesInBuffer-iPosition == 0)
		{
		iEof = ETrue;
		// Reached end of buffer
		User::Leave(KErrEof);
		}

	TUint8 value = iBufferPtr[iPosition++];

	return value;		
	}
	

TPtrC8 CDescriptorDataSource::GetBytesL(TInt aNumberOfBytes)
/**
This method gets the consecutive, requested number of bytes from the next position 
within the buffer.

@return				an object encapsulating those bytes requested.

@pre				The buffer has been set.
@post				The position in the buffer has moved along by the number of bytes requested.
@leave				KErrEof if there is no more data in the buffer.

@panic				The internal buffer positional mechanism has failed.

@param				aNumberOfBytes The number of bytes requested.

*/
	{
		
		if (iPosition+aNumberOfBytes >=0x10000)
			{
			User::Leave(Xml::KErrXmlStringTooBig);
		  }	
	// iPosition is the next byte to read
	// so aNumberOfBytes includes the byte under iPosition
	if (iPosition+aNumberOfBytes > iBufferPtr.Length())
		{
		iEof = ETrue;
		User::Leave(KErrEof); // Need all the data before we can read
		}

	// Get pointer to data, could be large
	TPtrC8 buf(iBufferPtr.Mid(iPosition, aNumberOfBytes));
	iPosition+=aNumberOfBytes;

	return buf;
	}


void CDescriptorDataSource::GetBytesL(TDes8& aDes8)
/**
This method reads bytes up to the maximum length of the supplied descriptor.  
End of file is not treated as an error.

@param				aDes8 On return holds data upto the maximum length of the 
					descriptor passed.
*/
	{
	aDes8.Zero();

	TInt bytesNeeded = aDes8.MaxLength();

	while(bytesNeeded>0)
		{
		TInt bytesLeftInBuffer = iBytesInBuffer-iPosition;
		TInt bytesToCopy = bytesNeeded>bytesLeftInBuffer ? bytesLeftInBuffer : bytesNeeded;

		aDes8.Append(iBufferPtr.Mid(iPosition, bytesToCopy));

		iPosition += bytesToCopy;
		bytesNeeded -= bytesToCopy;
		}
	}



void CDescriptorDataSource::StartTransaction()
/**
This method begins a transaction and sets the internal transaction position to reflect the 
transaction start.

A transaction reflects a subset of a document, or the whole document.

Starting a transaction allows the user to rollback to the start for whatever reason.

An EndTransaction should match every StartTransaction.

Ideally a transaction should be started for every block access on the document.

@pre				The buffer has been set and no other transaction has started.
@post				The internal transaction position has been updated.

*/
	{
	iStartTransaction=iPosition;
	}



void CDescriptorDataSource::EndTransaction()
/**
This method ends a transaction and sets the internal transaction position to reflect the 
transaction end.
A transaction reflects a subset of a document, or the whole document.
An EndTransaction should match every StartTransaction.

@pre				A buffer transaction has been started.
@post				The internal transaction position has been updated.

*/
	{
	iStartTransaction=KNoTransaction;
	}



void CDescriptorDataSource::Rollback()
/**
This method allows a transaction to be rolled back to the position it was
started from
.
A transaction reflects a subset of a document, or the whole document.

@pre				A transaction has been started.
@post				The transaction has been rolled back to its start.

*/
	{
	if (iStartTransaction!=KNoTransaction)
		{
		iPosition=iStartTransaction;
		}
	}




TBool CDescriptorDataSource::Eof() const
/**
This method determines the EOF of the current chunk of data being parsed.
EOF can occur in two cases. The first is when the data is not complete but there
is more to come.
The second EOF, is when all data has been received and read.

@return				Indicates the EOF of the current chunk.

*/
	{
	return iEof;
	}
	
	
	
TInt CDescriptorDataSource::BytesLeft() const
/**
Returns the bytes remaining to be parsed in the current chunk.

@return				Bytes in chunk left to parse.

*/
	{
	return iBytesInBuffer-iPosition;
	}



TInt CDescriptorDataSource::ReadBytesL(const TDesC8& aDescriptor)
	{
	TInt left = BytesLeft();
	
	if (left)
		{
		// We have some excess to carry across into the next parse.
		iBuffer->Delete(0, iPosition);
		}
	else
		{
		// Otherwise remove all old data for efficiency
		// but don't compress the buffer
		iBuffer->Delete(0, iBuffer->Size());
		}

	iPosition = 0;
	
	// Need to reflect any changes from the above Delete
	iBufferPtr.Set(iBuffer->Ptr(0));
	iBuffer->InsertL(iBufferPtr.Length(), aDescriptor);
	// Need to reflect any changes from the above Insert
	iBufferPtr.Set(iBuffer->Ptr(0));
	iBytesInBuffer = iBufferPtr.Length();
	
	return aDescriptor.Length();
	}

