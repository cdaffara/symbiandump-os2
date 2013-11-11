/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "FLDBASE.H"
#include "FLDSTD.H"


///////////////////////////////////////
// CTextField
///////////////////////////////////////

 
EXPORT_C TStreamId CTextField::StoreL(CStreamStore& aStore) const
// Assumes everything goes into the head stream.
// (Must be replaced by concrete pictures that have components)
/** Stores the field data to a stream store. Concrete field types with no persistent 
data should override this function to return KNullStreamId.

@param aStore Stream store to which the field data is written. 
@return The ID of the stream store. */
	{
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
    stream<< *this;  // provided by the concrete field
    stream.CommitL();
	CleanupStack::PopAndDestroy();
	return id;
	}

 
EXPORT_C void CTextField::RestoreL(const CStreamStore& aStore,TStreamId aStreamId)
// Creates a read-stream over aStore, and opens it over the specified stream ID.
//
	/** Restores the field data from a stream store. Concrete field types with no persistent 
	data should override this function to do nothing.
	
	@param aStore Stream store containing the field data to restore. 
	@param aId The ID of the stream store in which the field data was previously 
	stored. */
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	stream>> *this;
	CleanupStack::PopAndDestroy();
	}



EXPORT_C void CTextField::ExternalizeL(RWriteStream& /*aStream*/)const
//
	/** Externalises the field data. Called by StoreL().
	
	Calling this default implementation raises a panic. Concrete field classes 
	with persistent data must provide their own implementation of this function. 
	Concrete field classes with no persistent data must provide a StoreL() implementation 
	that just returns KNullStreamId.
	
	@param aStream Not used. */
	{Panic(EDefaultFieldExternalizeCalled);}

