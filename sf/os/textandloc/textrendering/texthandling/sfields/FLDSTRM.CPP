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


#include <e32std.h>
#include <e32base.h>

#include <s32strm.h>
#include <s32stor.h>

#include "FLDDEF.H"
#include "FLDSET.H"
#include "FLDARRAY.H"

#include "FLDSTD.H"



EXPORT_C TFieldMapExternalizer::TFieldMapExternalizer(const CStoreMap& aMap)
	: iMap(&aMap)
	{}

EXPORT_C void TFieldMapExternalizer::ExternalizeL(const TStreamRef& aRef,RWriteStream& aStream) const
// Write the stream id bound to aRef to aStream. If not bound, write KNullStreamId
//
	{
	TSwizzleC<TAny> swizzle=aRef;
	aStream<<iMap->At(swizzle);
	}


EXPORT_C TStreamId CTextFieldSet::StoreL(CStreamStore& aStore)const
// Save the fields and the fieldSet in their own streams
// Encapsulates the storing of its components.
//
	{
	CStoreMap* map=CStoreMap::NewLC(aStore);
	StoreFieldsL(aStore,*map);  // binds id's to swizzles
//
	// create custom externalizer over the map
	TFieldMapExternalizer fMap(*map);
	RStoreWriteStream stream(fMap);
	TStreamId id=stream.CreateLC(aStore);
	stream<< *this;
	stream.CommitL();
	CleanupStack::PopAndDestroy();  // stream
//
	map->Reset();
	CleanupStack::PopAndDestroy();  // map
	return id;
	}


EXPORT_C void CTextFieldSet::StoreFieldsL(CStreamStore& aStore,CStoreMap& aMap)const
// Stores all fields in the set
//
	{StoreFieldsL(aStore,aMap,iFieldArray);}


void CTextFieldSet::StoreFieldsL(CStreamStore& aStore,CStoreMap& aMap,CArrayFixSeg<TTextFieldEntry>* aArray)const
// Stores all fields contained in the set provided
//
	{
	__TEST_INVARIANT;

	for (TInt i=0 ; i<(aArray->Count()-1) ; i++)
		{
		TStreamId id=(*aArray)[i].iFieldHeader.iField->StoreL(aStore);
		if (id!=KNullStreamId)
			aMap.BindL((*aArray)[i].iFieldHeader.iField,id);
		}
	}


EXPORT_C void CTextFieldSet::ExternalizeL(RWriteStream& aStream)const
	{
	__TEST_INVARIANT;

	ExternalizeL(aStream,iFieldArray);
	}


void CTextFieldSet::ExternalizeL(RWriteStream& aStream,CArrayFixSeg<TTextFieldEntry>* aArray)const
	{
	TInt numFieldEntries = aArray->Count();
	aStream.WriteInt32L(numFieldEntries);
	// write out fields
	for (TInt i=0 ; i<numFieldEntries-1 ; i++)
		aStream<< (*aArray)[i];
	// write out last entry in array: the bit after the last field
	aStream.WriteInt32L((*aArray)[numFieldEntries-1].iPreFieldLen);
	}


EXPORT_C void CTextFieldSet::RestoreL(const CStreamStore& aFieldStore,TStreamId aStreamId)
	{
	// reset the array and stream into it
	Reset();
	DoRestoreL(aFieldStore,aStreamId);
	}


EXPORT_C void CTextFieldSet::RestoreFieldsL(const CStreamStore& aFieldStore)
	{
	DoRestoreFieldsL(iFieldArray,aFieldStore); // restore the fields individually from their own streams
	}


void CTextFieldSet::DoRestoreL(const CStreamStore& aFieldStore,TStreamId aStreamId)
// Restores a field set and its associated fields from the store provded.
// 
	{
	__ASSERT_ALWAYS(iFieldArray->Count()==1,Panic(EArrayNotEmptyOnRestore)); // array must be empty
	__ASSERT_ALWAYS((*iFieldArray)[0].iPreFieldLen==0,Panic(EArrayNotEmptyOnRestore));

	// retrieve the headstream from the store
	RStoreReadStream stream;
	stream.OpenLC(aFieldStore,aStreamId);
	// restore the set, then the individual fields
	stream>> *this;  // internalize the field set (the headers)
	CleanupStack::PopAndDestroy(); // stream
	DoRestoreFieldsL(iFieldArray,aFieldStore); // restore the fields individually from their own streams
	}


EXPORT_C void CTextFieldSet::InternalizeL(RReadStream& aStream)
	{
	InternalizeL(iFieldArray,aStream);

	__TEST_INVARIANT;
	}


void CTextFieldSet::InternalizeL(CArrayFixSeg<TTextFieldEntry>* aArray,RReadStream& aStream)
	{// assume the array is empty
	TInt numFieldEntries = aStream.ReadInt32L();
	// read in the fields
	TTextFieldEntry entry;
	for (TInt i=0 ; i<numFieldEntries-1 ; i++)
		{
		aStream>> entry;
		InsertEntryL(i,entry,aArray); // insert new entry
		}
	// read in the last entry: the bit after the last field. This will not contain a field
	(*aArray)[numFieldEntries-1].iPreFieldLen = aStream.ReadInt32L();
	}


void CTextFieldSet::DoRestoreFieldsL(CArrayFixSeg<TTextFieldEntry>* aArray,const CStreamStore& aFieldStore,TInt aStartIndex)
// This fn is called after all FieldHeaders have been internalized - the Swizzles hold the stream id's.
// One by one the fields are created (with the factory) and then have their settings streamed in from the store.
// If no factory exists all fields are converted to text.
//
	{
	TInt ii=aArray->Count()-2;  // -2 because we skip the last (empty) entry
	while (ii>=aStartIndex)
		{
		if ((*aArray)[ii].iFieldHeader.iField.IsId()) 
			{// restore the field only if it isn't the very last (dummy) entry
			if (iFieldFactory==NULL)
				// no factory - convert the field to text
				DeleteFieldEntry(aArray,ii);
			else
				{
				TStreamId id = (*aArray)[ii].iFieldHeader.iField.AsId();
				(*aArray)[ii].iFieldHeader.iField = iFieldFactory->NewFieldL((*aArray)[ii].iFieldHeader.iFieldType);
				if ((*aArray)[ii].iFieldHeader.iField!=NULL)
					(*aArray)[ii].iFieldHeader.iField->RestoreL(aFieldStore,id);
				else
					DeleteFieldEntry(aArray,ii); // handle "field type not recognised" by converting field to text
				}
			}
		ii--;
		}
	}


/***************************************** cut & paste *******************************************/


EXPORT_C TStreamId CTextFieldSet::CopyToStoreL(CStreamStore& aStore,TInt aPos,TInt aLength)const
// Copy any fields in the selected region to the specified store, returning the id of the head-stream.
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutOfRange));
	__ASSERT_ALWAYS(aLength>=0,Panic(ENegativeRange));
	__ASSERT_DEBUG((aPos+aLength)<=CharCount(),Panic(EPosOutOfRange));

	// Create a store map and store the fields themselves
	CStoreMap* map=CStoreMap::NewLC(aStore);
	CopyComponentsL(aStore,*map,aPos,aLength);

	// Create a head-stream in which to store the field entries and do so
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
	CopyToStreamL(stream,aPos,aLength);

	// tidy up
	stream.CommitL();
	map->Reset();
	CleanupStack::PopAndDestroy(2); // map, stream
	return id;
	}


EXPORT_C void CTextFieldSet::CopyComponentsL(CStreamStore& aStore,CStoreMap& aMap,TInt aPos,TInt aLength)const
// Stores all fields in the set
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutOfRange));
	__ASSERT_ALWAYS(aLength>=0,Panic(ENegativeRange));
	__ASSERT_DEBUG((aPos+aLength)<=CharCount(),Panic(EPosOutOfRange));

	// Create an array of the fields to be cut/copied
	CArrayFixSeg<TTextFieldEntry>* tempArray = new(ELeave) CArrayFixSeg<TTextFieldEntry>(KFieldArrayGranularity);
	CleanupStack::PushL(tempArray);
	CopyToArrayL(tempArray,aPos,aLength);

	// stream the required fields in their own streams
	StoreFieldsL(aStore,aMap,tempArray);
	CleanupStack::PopAndDestroy(); // tempArray
	}


EXPORT_C void CTextFieldSet::CopyToStreamL(RWriteStream& aStream,TInt aPos,TInt aLength)const
// Stores all fields in the set
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutOfRange));
	__ASSERT_ALWAYS(aLength>=0,Panic(ENegativeRange));
	__ASSERT_DEBUG((aPos+aLength)<=CharCount(),Panic(EPosOutOfRange));

	// Create an array of the fields to be cut/copied
	CArrayFixSeg<TTextFieldEntry>* tempArray = new(ELeave) CArrayFixSeg<TTextFieldEntry>(KFieldArrayGranularity);
	CleanupStack::PushL(tempArray);
	CopyToArrayL(tempArray,aPos,aLength);

	// stream the field entries in the temp array
	ExternalizeL(aStream,tempArray);
	CleanupStack::PopAndDestroy(); // tempArray
	}


void CTextFieldSet::CopyToArrayL(CArrayFixSeg<TTextFieldEntry>* aArray,TInt aPos,TInt aLength)const
	{
	TInt index; TInt offset;
	if (InField(aPos,index,offset))
		offset += (*iFieldArray)[index].iPreFieldLen; // make offset relative to start of entry
	// split first entry in range
	TTextFieldEntry entry = SplitEntry(index,offset,aLength);
	index++;
	TInt charsCopied=EntryLen(entry);
	// split second if neccessary
	if ((!entry.iFieldHeader.iField)&&(charsCopied<aLength))
		{
		TInt preFieldLen = entry.iPreFieldLen;
		entry = SplitEntry(index,0,aLength-preFieldLen);
		entry.iPreFieldLen += preFieldLen;
		charsCopied = EntryLen(entry);
		index++;
		}
	((CTextFieldSet*)this)->AppendEntryL(entry,aArray); // append the first entry to the storage array
	// write out all whole entries
	while (charsCopied<aLength)
		{
		if ((charsCopied+EntryLen(index))<=aLength)
			((CTextFieldSet*)this)->AppendEntryL((*iFieldArray)[index],aArray); 
		charsCopied += EntryLen(index);
		index++;
		}
	// split last entry if neccessary
	if (charsCopied>aLength)
		{// The last entry needs to be split
		// first get back to the beginning of the entry
		index--;
		charsCopied -= EntryLen(index);
		entry = SplitEntry(index,0,aLength-charsCopied); // split up the last entry as required
		((CTextFieldSet*)this)->AppendEntryL(entry,aArray); // append the last entry to the storage array
		}
	// add an empty last entry if neccessary
	TInt numFieldEntries = aArray->Count();
	if (((*aArray)[numFieldEntries-1].iFieldHeader.iField) || ((*aArray)[numFieldEntries-1].iFieldValueLen!=0))
		{
		entry.iPreFieldLen = 0;
		entry.iFieldValueLen = 0;
		entry.iFieldHeader.iFieldType = KNullUid;
		entry.iFieldHeader.iField = NULL;
		((CTextFieldSet*)this)->AppendEntryL(entry,aArray);
		numFieldEntries++;
		}
	}


EXPORT_C void CTextFieldSet::PasteFromStoreL(const CStreamStore& aFieldStore,TStreamId aStreamId,TInt aPos,TInt aMaxLen)
// Paste from aStore into the document at insert position aPos.
// Optionally the pasted text can be clipped to a maximum length aMaxLen.
//
	{
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutOfRange));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutOfRange));

	// retrieve the headstream from the store
	RStoreReadStream stream;
	stream.OpenLC(aFieldStore,aStreamId);

	// restore the set...
	PasteFromStreamL(stream,aPos,aMaxLen); // internalize the field set (the headers)
	CleanupStack::PopAndDestroy(); // stream

	// ...then the individual fields
	PasteComponentsL(aFieldStore,aPos); // restore the fields individually from their own streams
	}


EXPORT_C void CTextFieldSet::PasteFromStreamL(RReadStream& aStream,TInt aPos,TInt aMaxLen)
// streams the field entries into a temporary array, which is returned. 
// PasteComponents() must be called after this to actually carry out the paste...
	{
	// create a temporary array to stream in to, inserting the first entry
	CArrayFixSeg<TTextFieldEntry>* tempFieldArray = new(ELeave) CArrayFixSeg<TTextFieldEntry>(KFieldArrayGranularity);
	CleanupStack::PushL(tempFieldArray);
	AddInitialFieldEntryL(tempFieldArray,0);

	// internalize the field entries
	InternalizeL(tempFieldArray,aStream);

	// trim off any entries that lie beyond aMaxLength
	if (aMaxLen!=ENoPasteLimit)
		{// if aMaxLen is not ENoPasteLimit discard the excess fields
		__ASSERT_ALWAYS(aMaxLen>=0,Panic(ELengthOutOfRange));
		//
		TInt length=0;
		TInt i=0;
		for (i=0 ; (length<aMaxLen)&&(i<tempFieldArray->Count()) ; i++)
			length += EntryLen((*tempFieldArray)[i]);
		if (aMaxLen==0)
			{// make first entry zero len, delete all others
			i++;
			(*tempFieldArray)[i-1].iPreFieldLen = 0;
			}
		else if (length>aMaxLen)
			// truncate the last field in range
			(*tempFieldArray)[i-1].iPreFieldLen += (*tempFieldArray)[i-1].iFieldValueLen-(length-aMaxLen);
		else if ((length==aMaxLen) && ((*tempFieldArray)[i-1].iFieldHeader.iField!=NULL))
			{// if the terminating entry has a field add a zero length entry, the mandatory last entry
			i++;
			(*tempFieldArray)[i-1].iPreFieldLen = 0;
			}
		// ensure the last entry is of the correct format
		(*tempFieldArray)[i-1].iFieldValueLen = 0;
		(*tempFieldArray)[i-1].iFieldHeader.iFieldType = KNullUid;
		(*tempFieldArray)[i-1].iFieldHeader.iField = NULL;
		// delete all the fields wholely out of range
		for (TInt index=i ; index<tempFieldArray->Count() ; index++)
			(*tempFieldArray)[index].iFieldHeader.iField = NULL;
		tempFieldArray->Delete(i,tempFieldArray->Count()-i); // pos,count
		}

	DoPasteL(tempFieldArray,aPos);
	CleanupStack::PopAndDestroy(); // tempFieldArray
	}


EXPORT_C void CTextFieldSet::PasteComponentsL(const CStreamStore& aFieldStore,TInt aPos)
	{
	// Restore the fields individually from their own streams
	TInt index; TInt offset;
	// We don't need to make any difference between in and not in field situation here
	// all we need is the index
	TBool isInField = InField(aPos,index,offset);
	DoRestoreFieldsL(iFieldArray,aFieldStore,index); 
	}


void CTextFieldSet::DoPasteL(CArrayFixSeg<TTextFieldEntry>* aSourceArray,TInt aPos)
// Insert into this instance, at character position aPos, the entire (field entry) contents of the field array aSourceArray.
// All iField objects in aSourceArray are ID's at this time.
//
	{
	// are we inserting into a field?
	TInt numFieldEntries = aSourceArray->Count();
	TInt index; TInt offset;
	
	TBool inField = InField(aPos,index,offset);
	// record the rollback info
	RecordRollbackInfoL(index);
	if ((inField)&&(offset!=0))
		{// everything we insert will become text - no chance of leaving
		// insert all but last entry
		TInt i=0;
		for (; i<numFieldEntries-1 ; i++)
			{// copy text (no need to delete field)
			(*iFieldArray)[index].iFieldValueLen += EntryLen((*aSourceArray)[i]);
			}
		// read in the last entry (has no field attached)
		(*iFieldArray)[index].iFieldValueLen += (*aSourceArray)[i].iPreFieldLen;
		}
	else
		{// else split the entry we are going to bisect - this may leave
		if (inField)
			offset = (*iFieldArray)[index].iPreFieldLen; // must be at start of field
		if (numFieldEntries>1)
			{// read 1st field & carry out split.
			InsertEntryL(index,(*aSourceArray)[0]); // if this leaves the model will be intact
			(*iFieldArray)[index].iPreFieldLen += offset;
			(*iFieldArray)[index+1].iPreFieldLen -= offset;
			index++;
			}
		// insert all other fields except last.
		for (TInt i=1 ; i<numFieldEntries-1 ; i++)
			{
			TRAPD(ret,\
				InsertEntryL(index,(*aSourceArray)[i]));
			if (ret!=KErrNone)
				{// do rollback, then propagate leave
				RollbackPaste();
				User::Leave(ret);
				}
			index++;
			}
		// join last field up to successor
		(*iFieldArray)[index].iPreFieldLen += (*aSourceArray)[numFieldEntries-1].iPreFieldLen;
		}

	__TEST_INVARIANT;
	}


void CTextFieldSet::RecordRollbackInfoL(TInt aIndex)
	{
	delete iRollbackInfo;
	iRollbackInfo = new(ELeave) TRollbackInfo();
	iRollbackInfo->iEntryNum = aIndex;
	iRollbackInfo->iPreFieldLen = (*iFieldArray)[aIndex].iPreFieldLen;
	iRollbackInfo->iFieldValueLen = (*iFieldArray)[aIndex].iFieldValueLen;
	iRollbackInfo->iTotalEntries = iFieldArray->Count();
	}


EXPORT_C void CTextFieldSet::RollbackPaste()
// Carries out rollback from a paste function
// This will only have an effect after a PasteFromStream() has been called
// nb it would be distasterous if this were called at random some time after a paste!
//
	{
	if (!iRollbackInfo)
		return; // nothing to do
	// remove added entries from array
	TInt entriesToRemove=iFieldArray->Count()-iRollbackInfo->iTotalEntries;
	TInt i=0;
	for (i=iRollbackInfo->iEntryNum ; i<iRollbackInfo->iEntryNum+entriesToRemove ; i++)
		{
		if ((*iFieldArray)[i].iFieldHeader.iField.IsPtr())
			delete (*iFieldArray)[i].iFieldHeader.iField.AsPtr(); // Delete the textField object
		iFieldArray->Delete(i);
		}
	// now right num entries, but wrong length - use backup info to correct length
	(*iFieldArray)[i].iPreFieldLen = iRollbackInfo->iPreFieldLen;
	(*iFieldArray)[i].iFieldValueLen = iRollbackInfo->iFieldValueLen;

	__ASSERT_DEBUG(iFieldArray->Count()==iRollbackInfo->iTotalEntries,Panic(EDebug));
	delete iRollbackInfo;
	}

