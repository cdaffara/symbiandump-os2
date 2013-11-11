// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UT_STD.H"
#include <s32mem.h>

#define UNUSED_VAR(a) a = a

const TInt KStreamDictGranularity=16;

NONSHARABLE_CLASS(HDictionaryStoreBuf) : public MStreamBuf
	{
public:
	HDictionaryStoreBuf(MStreamBuf& aBuf,CDictionaryStore& aDic,TStreamId aStreamId,TUid aUid);
private:
	// from MStreamBuf
	void DoSynchL(); // only update the stream dic during synch
	// just delegate the rest straight through...
	void DoRelease();
	TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	void DoWriteL(const TAny* aPtr,TInt aLength);
	TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
	TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	MStreamBuf* iRealBuf; // delegate everything to this
	CDictionaryStore* iDictionary;
	TStreamId iStreamId; // id of the actual stream in the store
	TUid iStreamUid; // Uid to be paired with the stream id in the dic
	};

NONSHARABLE_CLASS(HNullBuf) : public TMemBuf
	{
public:
	HNullBuf();
private:
	void DoRelease();
	};

////////////////////////////////////////
// CStreamDictionary
////////////////////////////////////////

EXPORT_C CStreamDictionary* CStreamDictionary::NewL()
/** Allocates, constructs and returns a pointer to a new stream dictionary.

The function leaves if it cannot complete successfully.

@return A pointer to the new stream dictionary object. */
	{
	return new(ELeave) CStreamDictionary;
	}

EXPORT_C CStreamDictionary* CStreamDictionary::NewLC()
/** Allocates, constructs and returns a pointer to a new stream dictionary, putting 
the pointer onto the cleanup stack.

Putting the pointer onto the cleanup stack allows the object and allocated 
resources to be cleaned up if a subsequent leave occurs.

The function leaves if it cannot complete successfully.

@return A pointer to the new stream dictionary object. */
	{
	CStreamDictionary* dict=NewL();
	CleanupStack::PushL(dict);
	return dict;
	}

EXPORT_C CStreamDictionary::CStreamDictionary()
	: iArray(KStreamDictGranularity)
	{}

EXPORT_C CStreamDictionary::~CStreamDictionary()
/** Frees resources owned by the object, prior to its destruction. */
	{}

EXPORT_C void CStreamDictionary::AssignL(TUid aUid,TStreamId anId)
/** Creates or changes an association between a UID and a stream id.

If the stream dictionary already contains an entry with the same UID as aUid, 
then the associated stream id in that entry is replaced by anId.

If anId has the value KNullStreamId, then the function attempts to remove 
the entry whose unique identifier matches aUid.

@param aUid The UID
@param anId The stream id.
@see CStreamDictionary::Remove() */
	{
	if (anId==KNullStreamId)
		{
		Remove(aUid); // default associated stream is null
		return;
		}
//
	TEntry entry;
	entry.iUid=aUid;
	TKeyArrayFix key(_FOFF(TEntry,iUid),ECmpTUint32);
	TInt i;
	if (iArray.Find(entry,key,i)==0)
		{
		iArray[i].iId=anId;
		return;
		}
//
	__ASSERT_DEBUG(i==iArray.Count(),User::Invariant());
	entry.iId=anId;
	iArray.InsertL(i,entry);
	}

EXPORT_C void CStreamDictionary::Remove(TUid aUid)
/** Removes an association from the stream dictionary.

The function searches the stream dictionary for an entry whose UID matches 
aUid. If a match is found, the entry is removed. If no match is found, the 
stream dictionary remains unchanged.

@param aUid The UID whose matching entry is to be removed from the stream 
dictionary. */
	{
	TEntry entry;
	entry.iUid=aUid;
	TKeyArrayFix key(_FOFF(TEntry,iUid),ECmpTUint32);
	TInt i;
	if (iArray.Find(entry,key,i)==0)
		iArray.Delete(i);
	}

EXPORT_C TStreamId CStreamDictionary::At(TUid aUid) const
/** Returns the stream id associated with the specified UID.

@param aUid The UID whose associated stream id is to be returned.
@return The stream id associated with the specified UID.KNullStreamId, if no 
entry in the stream dictionary contains the specified UID. */
	{
	TEntry entry;
	entry.iUid=aUid;
	TKeyArrayFix key(_FOFF(TEntry,iUid),ECmpTUint32);
	TInt i;
	if (iArray.Find(entry,key,i)!=0)
		return KNullStreamId;
//
	return iArray[i].iId;
	}

EXPORT_C TBool CStreamDictionary::IsNull() const
/** Tests whether the stream dictionary is empty.

@return True, if the stream dictionary is empty. False if the stream dictionary 
contains at least one entry. */
	{
	return iArray.Count()==0;
	}

EXPORT_C void CStreamDictionary::ExternalizeL(RWriteStream& aStream) const
/** Externalises an object of this class to a write stream.

The presence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class.

@param aStream Stream to which the object should be externalised */
	{
	aStream<<iArray;
	}

EXPORT_C void CStreamDictionary::InternalizeL(RReadStream& aStream)
/** Internalises an object of this class from a read stream.

The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

Note that this function has assignment semantics. It replaces the old value 
of the object with a new value read from the read stream.

@param aStream Stream from which the object should be internalised. */
	{
	aStream>>iArray;
	}

void CStreamDictionary::TEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream<<iUid;
	aStream<<iId;
	}

void CStreamDictionary::TEntry::InternalizeL(RReadStream& aStream)
	{
	aStream>>iUid;
	aStream>>iId;
	}


////////////////////////////////////////
// CDictionaryStore
////////////////////////////////////////

EXPORT_C CDictionaryStore::~CDictionaryStore()
/** Frees resources owned by the object, prior to its destruction. */
	{
	delete iDictionary;
	delete iStore;
	}


EXPORT_C void CDictionaryStore::ConstructL()
	{
	iDictionary = CStreamDictionary::NewL();
	RStoreWriteStream stream;
	iStore->SetRootL(stream.CreateLC(*iStore));
	stream<<*iDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // dictionary stream
	iStore->CommitL();
	}


EXPORT_C void CDictionaryStore::Remove(TUid aUid)
/** Removes the stream associated with the specified UID from the dictionary store.

If there is no stream associated with the specified UID, then the dictionary 
store remains unchanged.

The function cannot leave; it returns whether or not it is succesful.

@param aUid The UID whose associated stream is to be removed from the dictionary 
store. */
	{
	TRAPD(ignore,RemoveL(aUid));
    UNUSED_VAR(ignore);
	}


EXPORT_C void CDictionaryStore::RemoveL(TUid aUid)
/** Removes the stream associated with the specified UID from the dictionary store 
and leaves if unsuccessful.

If there is no stream associated with the specified UID, then the dictionary 
store remains unchanged.

@param aUid The UID whose associated stream is to be removed from the dictionary 
store. */
	{
	TStreamId id = DictionaryL()->At(aUid);
	if (id!=KNullStreamId)
		{
		DictionaryL()->Remove(aUid);
		iStore->DeleteL(id);
		iDictionaryHasChanged=ETrue;
		}
	}


EXPORT_C TInt CDictionaryStore::Commit()
/** Commits changes.

It establishes a new commit point and then compacts the dictionary store. 
Typically, this is done after changes to new or existing streams are complete 
and the streams themselves have been committed.

Establishing a new commit point makes changes to the store permanent. Until 
such changes are committed, they can be rolled back, effectively causing the 
store to revert back to its state before the changes were made.

This ensures that persistent data moves from one consistent state to another 
and guarantees the integrity of persistent store data in the event of failures. 
In particular, if a process terminates or a media failure occurs, the store 
reverts automatically to its state at the last successful commit point.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TRAPD(ret,CommitL());
	return ret;
	}


EXPORT_C void CDictionaryStore::CommitL()
/** Commits changes and leaves if unsuccessful.

@see CDictionaryStore::Commit() */
	{
	if (iDictionaryHasChanged)
		{
		// rewrite the root stream
		RStoreWriteStream stream;
		stream.ReplaceLC(*iStore,iStore->Root());
		stream<< *DictionaryL();
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // dictionary stream
		}
	// commit the store
	iStore->CommitL();
	// compact the store, ignoring any failure (commit was good)
	TRAPD(
		ignore,
			{
			if (iStore->ReclaimL() >= KDictionaryCommitThreshold)
				{
				iStore->CompactL();
				}
			iStore->CommitL();
			} );
    UNUSED_VAR(ignore);
	}


EXPORT_C void CDictionaryStore::Revert()
/** Rolls the dictionary store back to its state at the last commit point.

A commit point is set using the Commit() or CommitL() functions.

The function returns, whether or not it completes successfully.

@see CDictionaryStore::Commit()
@see CDictionaryStore::CommitL() */
	{
	TRAPD(ignore,RevertL());
    UNUSED_VAR(ignore);
	}


EXPORT_C void CDictionaryStore::RevertL()
/** Rolls the dictionary store back to its state at the last commit point and leaves 
if unsuccessful.

A commit point is set using the Commit() or CommitL() functions.

@see CDictionaryStore::Commit()
@see CDictionaryStore::CommitL() */
	{
	delete iDictionary;
	iDictionary = NULL;
	iDictionaryHasChanged = EFalse;
	iStore->RevertL();
	}


CStreamDictionary* CDictionaryStore::DictionaryL() const
// Returns the handle of the stream dictionary, internalizing it first if necessary
	{
	if (!iDictionary)
		{
		// create a temporary dictionary
		CStreamDictionary* dictionary = CStreamDictionary::NewLC();
		// stream into the dictionary
		RStoreReadStream root;
		root.OpenLC(*iStore,iStore->Root());
		root>> *dictionary;
		root.Close();
		CleanupStack::PopAndDestroy(); // root
		// set iDictionary
		MUTABLE_CAST(CStreamDictionary*&,iDictionary) = dictionary;
		CleanupStack::Pop(); // dictionary
		}
	return iDictionary;
	}


EXPORT_C TBool CDictionaryStore::IsNullL() const
/** Tests whether the dictionary stores stream dictionary is empty.

@return True, if the stream dictionary is empty. False, if the stream dictionary 
contains at least one entry.
@see CStreamDictionary */
	{
	return DictionaryL()->IsNull();
	}


EXPORT_C TBool CDictionaryStore::IsPresentL(TUid aUid) const
/** Tests whether the specified UID has an associated stream within this dictionary 
store.

@param aUid The UID.
@return True, if there is a stream associated with the specified UID in the 
dictionary store; false otherwise.
@see CStreamDictionary */
	{
	return DictionaryL()->At(aUid)!=KNullStreamId;
	}


MStreamBuf* CDictionaryStore::GetSourceL(TUid aUid) const
// Opens the stream matching aUid and returns its source.
// If no stream matches aUid an empty stream buf is returned
// Ownership of the source is transfered to the caller.
	{
	// get the id of the stream
	TStreamId id = DictionaryL()->At(aUid);
	// open the stream if it exists, otherwise open a dummy empty stream
	if (id==KNullStreamId)
		return (new(ELeave) HNullBuf);
	else
		{
		// open the stream and steal its source
		RStoreReadStream stream;
		stream.OpenL(*iStore,id);
		return stream.Source();
		}
	}


MStreamBuf* CDictionaryStore::GetSinkL(TUid aUid)
// If a stream matches aUid it is replaced ad its sink returned 
// If no stream matches aUid a new stream is created and its sink returned
// Ownership of the sink is transfered to the caller.
	{
	// get the id of the stream
	TStreamId id = DictionaryL()->At(aUid);
	RStoreWriteStream stream;
	if (id!=KNullStreamId)
		//replace the stream
		stream.ReplaceLC(*iStore,id);
	else
		// create a new stream
		id = stream.CreateLC(*iStore);
	// create a dictionary store buffer to hold the incoming data
	MStreamBuf* buf = new(ELeave) HDictionaryStoreBuf(*stream.Sink(),*this,id,aUid);
	CleanupStack::Pop(); // stream;
	return buf;
	}


//////////////////////////////////////////////
// RDictionaryReadStream
//////////////////////////////////////////////

EXPORT_C void RDictionaryReadStream::OpenL(const CDictionaryStore& aDictStore,TUid aUid)
// Open the stream if it exists, otherwise open a dummy empty stream
/** Opens the stream associated with the specified UID in the specified dictionary 
store, and prepares it for reading.

@param aDictStore The dictionary store containing the stream.
@param aUid The unique identifier associated with the stream to be read. */
	{
	Attach(aDictStore.GetSourceL(aUid));
	}


EXPORT_C void RDictionaryReadStream::OpenLC(const CDictionaryStore& aDictStore,TUid aUid)
/** Opens the stream associated with the specified UID in the specified dictionary 
store, prepares it for reading, and puts a a cleanup item onto the cleanup 
stack..

Placing a cleanup item on the cleanup stack allows allocated resources to 
be cleaned up if a subsequent leave occurs.

@param aDictStore The dictionary store containing the stream.
@param aUid The unique identifier associated with the stream to be read. */
	{
	OpenL(aDictStore,aUid);
	PushL();
	}


//////////////////////////////////////////////
// RDictionaryWriteStream
//////////////////////////////////////////////

EXPORT_C void RDictionaryWriteStream::AssignL(CDictionaryStore& aDictStore,TUid aUid)
// Replace the stream if it exists, otherwise create a new one
/** Prepares a stream in the specified dictionary store for writing.

If no stream is associated with the specified UID, then a new stream is created 
and an association is made between the resulting stream id and the specified 
UID. If a stream is currently associated with the specified UID, then this 
existing stream is prepared for replacement.

@param aDictStore The dictionary store which contains the new or replacement 
stream.
@param aUid The UID associated with the stream.
@see RWriteStream::Release() */
	{
	Attach(aDictStore.GetSinkL(aUid));
	}


EXPORT_C void RDictionaryWriteStream::AssignLC(CDictionaryStore& aDictStore,TUid aUid)
/** Prepares a stream in the specified dictionary store for writing, and places 
a cleanup item for this RDictionaryWriteStream object onto the cleanup stack. 

If no stream is associated with the specified UID, then a new stream is created 
and an association is made between the resulting stream id and the specified 
UID. If a stream is currently associated with the specified UID, then this 
existing stream is prepared for replacement.

Placing a cleanup item onto the cleanup stack allows allocated resources to 
be cleaned up if a subsequent leave occurs. 

@param aDictStore The dictionary store which contains the new or replacement 
stream.
@param aUid The UID associated with the stream. */
	{
	AssignL(aDictStore,aUid);
	PushL();
	}


//////////////////////////////////////////////
// HDictionaryStoreBuf
//////////////////////////////////////////////

HDictionaryStoreBuf::HDictionaryStoreBuf(MStreamBuf& aBuf,CDictionaryStore& aDic,TStreamId aStreamId,TUid aUid)
	:iRealBuf(&aBuf),
	iDictionary(&aDic),
	iStreamId(aStreamId),
	iStreamUid(aUid)
	{}


void HDictionaryStoreBuf::DoSynchL()
// only update the stream dic during synch - this is when the stream gets added to the store
	{
	CStreamDictionary& dic = *iDictionary->DictionaryL();
	TBool newEntry = (dic.At(iStreamUid)==KNullStreamId);
	if (iRealBuf->SizeL()>0)
		{
		dic.AssignL(iStreamUid,iStreamId);
		TInt ret = iRealBuf->Synch();
		if (ret!=KErrNone)
			{// attempt rollback
			if (newEntry)
				dic.Remove(iStreamUid);
			__LEAVE(ret);
			}
		if (newEntry)
			iDictionary->iDictionaryHasChanged=ETrue;
		}
	else
		{
		iRealBuf->SynchL();
		iDictionary->iStore->Delete(iStreamId);
		iDictionary->Remove(iStreamUid);
		}
	}


void HDictionaryStoreBuf::DoRelease()
	{
	iRealBuf->Release();
	delete this;
	}


TInt HDictionaryStoreBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
	{
	return iRealBuf->ReadL(aPtr,aMaxLength);
	}


TInt HDictionaryStoreBuf::DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
	{
	return iRealBuf->ReadL(aDes,aMaxLength,aStatus);
	}


TStreamTransfer HDictionaryStoreBuf::DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer)
	{
	return iRealBuf->ReadL(anInput,aTransfer);
	}


void HDictionaryStoreBuf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	iRealBuf->WriteL(aPtr,aLength);
	}


TInt HDictionaryStoreBuf::DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
	{
	return iRealBuf->WriteL(aDes,aMaxLength,aStatus);
	}


TStreamTransfer HDictionaryStoreBuf::DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
	{
	return iRealBuf->WriteL(anOutput,aTransfer);
	}


TStreamPos HDictionaryStoreBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
	{
	return iRealBuf->SeekL(aMark,aLocation,anOffset);
	}

// class HNullBuf

HNullBuf::HNullBuf()
	{
	Set(0,0,ERead);
	}

void HNullBuf::DoRelease()
	{
	delete this;
	}

