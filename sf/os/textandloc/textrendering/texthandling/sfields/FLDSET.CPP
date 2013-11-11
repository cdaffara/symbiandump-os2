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

#include "FLDSET.H"
#include "FLDDEF.H"
#include "FLDARRAY.H"
#include "FLDSTD.H"


EXPORT_C void CTextFieldSet::__DbgTestInvariant()const
// Provides class invariants.  Explanations below:
//
	{
#ifdef _DEBUG
	if (!iFieldArray)
		return;
// check that every entry in the array has length >=0 and that all but the last have valid header handles
	TBool valid = ETrue;
	TInt index = iFieldArray->Count()-1;
	if (index<0)
		valid = EFalse;
	else 
		{// check last entry: should have null fieldHeader, zero field length and non-negative PreFieldLen
		if ( (*iFieldArray)[index].iFieldHeader.iField
		||(*iFieldArray)[index].iFieldValueLen!=0
		||(*iFieldArray)[index].iPreFieldLen<0 )
			{
			valid = EFalse;
			}
		index--;
		// check the other entries: should have non-null fieldHeader and non-negative field length and PreFieldLen
		for (; (index>=0)&&(valid) ; index--)
			{
			if ((*iFieldArray)[index].iFieldValueLen<0
				||(*iFieldArray)[index].iPreFieldLen<0 )
					{
					valid = EFalse;
					}
			}
		}
	__ASSERT_ALWAYS(valid,User::Invariant());
#endif
	}


EXPORT_C CTextFieldSet* CTextFieldSet::NewL(TInt aDocumentLength)
	{
	CTextFieldSet* self=new(ELeave) CTextFieldSet();
	CleanupStack::PushL(self);
	self->ConstructL(aDocumentLength);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CTextFieldSet* CTextFieldSet::NewL(const MTextFieldFactory* aFactory,const CStreamStore& aFieldStore,TStreamId aStreamId)
	{
	CTextFieldSet* self=new(ELeave) CTextFieldSet();
	CleanupStack::PushL(self);
	self->SetFieldFactory(CONST_CAST(MTextFieldFactory*,aFactory));
	self->ConstructL(0);
	self->DoRestoreL(aFieldStore,aStreamId);
	CleanupStack::Pop();
	return self;
	}


CTextFieldSet::CTextFieldSet()
	{}


void CTextFieldSet::ConstructL(TInt aDocumentLength)
// Creates an array in which to store all fields
// Inserts an initial entry into the array to cover any text that lies after the last field
//
	{
	iFieldArray = new(ELeave) CArrayFixSeg<TTextFieldEntry>(KFieldArrayGranularity);
	AddInitialFieldEntryL(iFieldArray,aDocumentLength);

	__TEST_INVARIANT;
	}


EXPORT_C CTextFieldSet::~CTextFieldSet()
	{
	delete iRollbackInfo;
	if (iFieldArray)
		{
		TInt fieldCount=iFieldArray->Count();
		for (TInt index=fieldCount-1;index>=0;index--)
			delete (*iFieldArray)[index].iFieldHeader.iField.AsPtr(); // Delete the textField objects
		delete iFieldArray;
		}
	}


EXPORT_C void CTextFieldSet::SetFieldFactory(MTextFieldFactory* aFactory)
	{
	iFieldFactory = aFactory;
	}


EXPORT_C MTextFieldFactory* CTextFieldSet::FieldFactory()const
	{
	return iFieldFactory;
	}


void CTextFieldSet::InsertEntryL(TInt aIndex,TTextFieldEntry& aEntry)
	{
	InsertEntryL(aIndex,aEntry,iFieldArray);
	}


void CTextFieldSet::InsertEntryL(TInt aIndex,TTextFieldEntry& aEntry,CArrayFixSeg<TTextFieldEntry>* aArray)
// if this function leaves it will be as if it had never been called...
//
	{
	if (aEntry.iFieldHeader.iField.IsPtr())
		CleanupStack::PushL(aEntry.iFieldHeader.iField.AsPtr());
	aArray->InsertL(aIndex,aEntry); // insert new entry
	if (aEntry.iFieldHeader.iField.IsPtr())
		CleanupStack::Pop();
	}


void CTextFieldSet::AppendEntryL(TTextFieldEntry& aEntry)
	{
	AppendEntryL(aEntry,iFieldArray);
	}


void CTextFieldSet::AppendEntryL(TTextFieldEntry& aEntry,CArrayFixSeg<TTextFieldEntry>* aArray)
	{
	if (aEntry.iFieldHeader.iField.IsPtr())
		CleanupStack::PushL(aEntry.iFieldHeader.iField.AsPtr());
	aArray->AppendL(aEntry); // insert new entry
	if (aEntry.iFieldHeader.iField.IsPtr())
		CleanupStack::Pop();
	}


TInt CTextFieldSet::EntryLen(TInt aIndex)const
	{
	return EntryLen((*iFieldArray)[aIndex]);
	}


TInt CTextFieldSet::EntryLen(const TTextFieldEntry& aEntry)const
	{
	return aEntry.iPreFieldLen+aEntry.iFieldValueLen;
	}


TTextFieldEntry CTextFieldSet::SplitEntry(TInt aIndex,TInt aOffset, TInt aRange)const
// Splits the entry aIndex, returning the part demarked by the offset (from the start of the entry) and the range
//
	{
	__TEST_INVARIANT;

	__ASSERT_DEBUG((aIndex>=0)&&(aIndex<iFieldArray->Count()),Panic(EIndexOutOfRange));
	TInt entryLength = EntryLen(aIndex);
	__ASSERT_DEBUG((aOffset>=0)&&(aOffset<=entryLength),Panic(EPosOutOfRange));
	__ASSERT_DEBUG((aRange>=0),Panic(ENegativeRange));

	if ((aOffset+aRange)>entryLength)
		aRange = entryLength-aOffset; // scale range down to entry size if neccessary

	if ((aOffset==0)&&(aRange==entryLength))
		return (*iFieldArray)[aIndex]; //entry does not need to be split

	TInt charsCopied=0;
	TTextFieldEntry entry;
	entry.iPreFieldLen = 0;
	entry.iFieldValueLen = 0;
	entry.iFieldHeader.iField = NULL;
	if (aOffset<(*iFieldArray)[aIndex].iPreFieldLen)
		{// At least some of the pre needs to be copied
		entry.iPreFieldLen = (*iFieldArray)[aIndex].iPreFieldLen-aOffset;
		if ((entry.iPreFieldLen)>aRange)
			entry.iPreFieldLen = aRange;
		charsCopied = entry.iPreFieldLen;
		}

	if (charsCopied<aRange)
		{// more to do, so at least some of the field needs to be copied
		if ((aOffset+aRange)==entryLength)
			{// whole field in range so copy it
			entry.iFieldValueLen = (*iFieldArray)[aIndex].iFieldValueLen;
			entry.iFieldHeader = (*iFieldArray)[aIndex].iFieldHeader;
			}
		else
			// only part of field in range so convert it to text
			entry.iPreFieldLen += aRange-charsCopied;
		}
	return entry;
	}


void CTextFieldSet::AddInitialFieldEntryL(CArrayFixSeg<TTextFieldEntry>* aArray,TInt aDocumentLength)
// Add initial entry
	{
	TTextFieldEntry initialEntry;
	initialEntry.iPreFieldLen = aDocumentLength;
	initialEntry.iFieldValueLen = 0;
	initialEntry.iFieldHeader.iFieldType = KNullUid;
	initialEntry.iFieldHeader.iField = NULL;
	aArray->AppendL(initialEntry);
	}


EXPORT_C void CTextFieldSet::Reset()
// deletes all fields (but not corresponding text) and reinitialises the field array
//
	{
	__TEST_INVARIANT;

	for (TInt index=FieldCount()-1 ; index>=0 ; index--)
		delete (*iFieldArray)[index].iFieldHeader.iField.AsPtr(); // Delete the textField objects
	iFieldArray->Reset();
	AddInitialFieldEntryL(iFieldArray,0); // cannot leave in this context
	iFieldArray->Compress(); // compress array

	__TEST_INVARIANT;
	}


EXPORT_C CTextField* CTextFieldSet::NewFieldL(TUid aFieldType)
	{
	if (iFieldFactory)
		return iFieldFactory->NewFieldL(aFieldType);
	else
		return NULL;
	}


EXPORT_C TInt CTextFieldSet::InsertFieldL(TInt aPos,CTextField* aField,TUid aFieldType)
// Inserts a field header aField at aPos (aField should be declared on the heap)
// The field initially has zero length: Update must be called afterward
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS((aPos>=0),Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));
	__ASSERT_ALWAYS(aField,Panic(ENoTextField));

	TInt errLevel=KErrAlreadyExists;
	TTextFieldEntry entry;
	entry.iFieldHeader.iField = aField;
	entry.iFieldHeader.iFieldType = aFieldType;
	entry.iFieldValueLen = 0;
	TInt index; TInt offset;
	TBool inField = InField(aPos,index,offset);
	if (!inField)
		{// not inserting into a field
		entry.iPreFieldLen = offset;
		iFieldArray->InsertL(index,entry); // insert new entry
		(*iFieldArray)[index+1].iPreFieldLen -= offset; // update old entry
		errLevel = KErrNone;
		}
	else if (offset==0)
		{// at start of field
		entry.iPreFieldLen = (*iFieldArray)[index].iPreFieldLen;
		iFieldArray->InsertL(index,entry); // insert new entry
		(*iFieldArray)[index+1].iPreFieldLen = 0; // update old entry
		errLevel = KErrNone;
		}

	__TEST_INVARIANT;
	return errLevel;
	}


EXPORT_C const CTextField* CTextFieldSet::TextField(TInt aPos)const
// Return a handle to the concrete field at document position aPos.
// Returns NULL if there is no field at position aPos.
//
	{
	__TEST_INVARIANT;

	TInt index=-1;
	TInt offset=0;
	TBool inField=InField(aPos,index,offset);
	if (!inField)
		return NULL;
	TSwizzle<CTextField> field=(*iFieldArray)[index].iFieldHeader.iField;
	__ASSERT_DEBUG(field.IsPtr(),User::Invariant());
	return field.AsPtr();
	}


EXPORT_C TInt CTextFieldSet::RemoveField(TInt aPos)
// Removes the field from the array, adding it's content to the "before" of the next field.
// After this function is called the text the field contained should be deleted. If this does
//   not happen this function acts as a "ConvertFieldToText()"
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS((aPos>=0),Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));

	TInt errLevel=KErrNone;
	TInt index; TInt offset;
	if (!InField(aPos,index,offset))
		errLevel = KErrNotFound;
	else
		// delete the field entry tidily
		DeleteFieldEntry(index);

	__TEST_INVARIANT;
	return errLevel;
	}


EXPORT_C TInt CTextFieldSet::FieldCount() const
	{
	__TEST_INVARIANT;

	// return count-1 because there is always an entry in the array for the text at the
	//  end of the document, after the last field (maybe just the end of doc char)
	return (iFieldArray->Count()-1);
	}


EXPORT_C TInt CTextFieldSet::CharCount() const
// returns the number of characters in the document according to the field array
	{
	__TEST_INVARIANT;

	TInt charCount = 0;
	for (TInt index=FieldCount() ; index>=0 ; index--)
		charCount += (*iFieldArray)[index].iPreFieldLen+(*iFieldArray)[index].iFieldValueLen;
	return charCount;
	}


EXPORT_C TBool CTextFieldSet::FindFields(TInt aPos) const
// Return ETrue if aPos is in a field
//
	{
	TFindFieldInfo dummy;
	return (FindFields(dummy,aPos,0));
	}


EXPORT_C TBool CTextFieldSet::FindFields(TFindFieldInfo& aInfo,TInt aPos, TInt aRange) const
// Check whether aPos is in a field, then check whether any fields start in aRange
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aRange>=0,Panic(ENegativeRange));
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutsideDoc));
	__ASSERT_ALWAYS(aPos+aRange<=CharCount(),Panic(EPosOutsideDoc));
	
	aInfo.iFieldCountInRange = 0;
	aInfo.iFirstFieldLen = 0;
	aInfo.iFirstFieldPos = 0;
	TInt pos=aPos; // position in doc
	// are we in a field to begin with?
	TInt index; TInt offset;
	if (InField(aPos,index,offset)) 
		{
		aInfo.iFieldCountInRange++;
		aInfo.iFirstFieldLen = (*iFieldArray)[index].iFieldValueLen;
		aInfo.iFirstFieldPos = aPos-offset;
		pos += (*iFieldArray)[index].iFieldValueLen-offset+(*iFieldArray)[index+1].iPreFieldLen;
		index++;
		}
	else
		pos += (*iFieldArray)[index].iPreFieldLen-offset;
	// step through array until aRange
	while (pos<aPos+aRange)
		// When entering this loop we're always at the beginning of a field value
		// set aFieldEntry to first field found (if fieldcount==0)
		// for each start of field encountered, fieldCount++
		{
		if (aInfo.iFieldCountInRange==0)
			{
			aInfo.iFirstFieldLen = (*iFieldArray)[index].iFieldValueLen;
			aInfo.iFirstFieldPos = pos;
			}
		aInfo.iFieldCountInRange++;
		pos += (*iFieldArray)[index].iFieldValueLen+(*iFieldArray)[index+1].iPreFieldLen;
		index++;
		}
	// (if sensing right) check that we haven't ended adjacent to one or more zero-length fields 
	if ( (aRange==0) && (pos==aPos) && (index<(iFieldArray->Count()-1)) && ((*iFieldArray)[index].iFieldValueLen==0) )
		{
		aInfo.iFieldCountInRange++;
		index++;
		while ( ((*iFieldArray)[index].iPreFieldLen==0) && ((*iFieldArray)[index].iFieldValueLen==0) && (index<(iFieldArray->Count()-1)) )
			{
			aInfo.iFieldCountInRange++;
			index++;
			}
		}

	__ASSERT_DEBUG(aInfo.iFieldCountInRange<=FieldCount(),Panic(EDebug));
	return aInfo.iFieldCountInRange;
	}


EXPORT_C TInt CTextFieldSet::NewFieldValueL(HBufC*& aBuf, TInt aPos)
// Returns the new value of the field at aPos (if applicable).
// The method might reallocate aBuf parameter, so don't forget to:
// 1) Push aBuf parameter in the CleanupStack before the call. The call
//      may fail, then aBuf gets lost.
// 2) Pop aBuf parameter from the CleanupStack after the call - aBuf may get
//      reallocated, so the pushed pointer becomes invalid.
// 3) Push aBuf in the CleanupStack again.
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aBuf,Panic(ENoBuffer));
	__ASSERT_ALWAYS((aPos>=0),Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));
	
	TInt errLevel = KErrNotFound;
	TInt index; TInt offset;
	if (InField(aPos,index,offset))
		{// There's a field at aPos
		TPtr bufPtr = aBuf->Des();
		TInt reqLen = (*iFieldArray)[index].iFieldHeader.iField->Value(bufPtr);
		while (reqLen>0)
			{// If more storage is required for the value then reallocate the buffer
			aBuf = aBuf->ReAllocL(reqLen); // for unicode compatability, rounds up
			TPtr pointer = aBuf->Des();
			reqLen = (*iFieldArray)[index].iFieldHeader.iField->Value(pointer);
			}
		// dont set new field length - this will be done in a subsequent NotifyInsert()
		errLevel = KErrNone;
		}

	__TEST_INVARIANT;
	return errLevel;
	}


EXPORT_C void CTextFieldSet::NotifyInsertion(TInt aPos, TInt aNumberAdded)
// Informs the array that aNumberAdded characters have been inserted in the document
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));
	__ASSERT_ALWAYS(aNumberAdded>=0,Panic(EIllegalNegativeValue));

	TInt index; TInt offset;
	// are the extra characters in a field (matching away from fields)?
	if (!InField(aPos,index,offset))
		(*iFieldArray)[index].iPreFieldLen += aNumberAdded;
	else
		if (offset>0)
			(*iFieldArray)[index].iFieldValueLen += aNumberAdded;
		else
			(*iFieldArray)[index].iPreFieldLen += aNumberAdded;

	__TEST_INVARIANT;
	}                             


EXPORT_C void CTextFieldSet::NotifyFieldUpdate(TInt aPos, TInt aNewFieldValueLength)
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS((aPos>=0),Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));
	__ASSERT_ALWAYS(aNewFieldValueLength>=0,Panic(EIllegalNegativeValue));

	// Is the insert pos in a field? If so which?
	TInt index; TInt offset;
	__ASSERT_ALWAYS(InField(aPos,index,offset),Panic(EPosNotInField));
	// Update the length of the relevant field
	(*iFieldArray)[index].iFieldValueLen = aNewFieldValueLength;
	
	__TEST_INVARIANT;
	}


EXPORT_C void CTextFieldSet::NotifyDeletion(TInt aPos,TInt aTotalRemoved)
// Informs the array that aTotalRemoved characters have been removed from the document
// Any fields wholely contained will be removed, those partially intersecting will just be shortened
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aPos>=0,Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));
	__ASSERT_ALWAYS(aTotalRemoved>=0,Panic(EIllegalNegativeValue));
	
	TInt charCount = CharCount();
	
	//There is a possibility that there exists hidden end-of-document character in rich text objects.
	//This is checked by the if statement & accordingly aTotalRemoved is decremented.
	//i.e. aPos + aTotalRemoved could be greater by 1 than the CharCount(),  
	//this is because the aTotalRemoved might also include 1 hidden character. - DEF095911
	if(aPos+aTotalRemoved > charCount)
		aTotalRemoved--;

	__ASSERT_DEBUG(aPos+aTotalRemoved<=charCount, Panic(EPosOutsideDoc));

	int index = 0;
	int offset = 0;
	int cur_pos = aPos;
	int end_pos = aPos + aTotalRemoved;
	TBool in_field = InField(cur_pos,index,offset);
	TTextFieldEntry* field = NULL;
	int field_start = 0;
	int field_end = 0;
	if (index >= 0 && index < iFieldArray->Count())
		{
		field = &(*iFieldArray)[index];
		field_start = cur_pos - offset;
		if (!in_field)
			field_start += field->iPreFieldLen;
		field_end = field_start + field->iFieldValueLen;
		}

	while (field)
		{
		// Reduce the size of the gap before the field if any.
		int gap = Min(end_pos,field_start) - cur_pos;
		if (gap > 0)
			{
			cur_pos += gap;
			field->iPreFieldLen -= gap;
			}
		if (cur_pos >= end_pos)
			break;

		// Reduce the field length.
		int remove_start = cur_pos;
		int remove_end = Min(field_end,end_pos);
		cur_pos = field_end;
		field->iFieldValueLen -= (remove_end - remove_start);

		// Delete the field if it is now of zero length.
		int added_to_next = 0;
		if (field->iFieldValueLen == 0)
			{
			added_to_next = field->iPreFieldLen;
			DeleteFieldEntry(index);
			}
		else
			index++;

		// Move to the next field.
		if (index < iFieldArray->Count())
			{
			field = &(*iFieldArray)[index];
			field_start = cur_pos + field->iPreFieldLen - added_to_next;
			field_end = field_start + field->iFieldValueLen;
			}
		else
			field = NULL;
		}

	__TEST_INVARIANT;
	}


TBool CTextFieldSet::InField(const TInt aPos, TInt& anIndex, TInt& anOffset) const
// works out whether or not aPos is in a field (matching right),
//  sets anIndex to the index number of the field entry,
//  and sets anOffset to the distance aPos is into the field or its preceeding gap
//
	{
	__ASSERT_ALWAYS((aPos>=0),Panic(EPosOutsideDoc));
	__ASSERT_DEBUG(aPos<=CharCount(),Panic(EPosOutsideDoc));

	TBool inField;
	TInt currentPos = 0;
	anIndex = 0;
	TInt lengthOfDoc = CharCount();
	TInt lastFieldNum = iFieldArray->Count()-1;
	// Find the index of the entry containing aPos
	if (aPos == lengthOfDoc)
		{
		anIndex = lastFieldNum;
		currentPos = aPos;
		}
	else 
		{
		anIndex = -1;
		while (aPos >= currentPos)
			{
			anIndex++;
			currentPos += (*iFieldArray)[anIndex].iPreFieldLen+(*iFieldArray)[anIndex].iFieldValueLen;
			}
		}
	// Check that we have not skipped over any zero-length fields
	if ( ((anIndex-1)>=0) && ((*iFieldArray)[anIndex-1].iFieldValueLen==0) )
		{
		TInt temp = currentPos - ((*iFieldArray)[anIndex].iPreFieldLen+(*iFieldArray)[anIndex].iFieldValueLen);
		if (temp==aPos)
			{// aPos is on a field boundary
			currentPos = temp;
			anIndex--;
			while ( ((anIndex-1)>=0)
				&&((*iFieldArray)[anIndex].iPreFieldLen==0)
				&&((*iFieldArray)[anIndex-1].iFieldValueLen==0) )
				{
				anIndex--;
				}
			}
		}
	// Move to the start of the field (end of prefield) in entry [anIndex]
	currentPos -= (*iFieldArray)[anIndex].iFieldValueLen;
	// Determine whether or not aPos is in the field of entry[anIndex]
	if (anIndex == lastFieldNum)
		inField = EFalse;
	else if (aPos >= currentPos)
			inField = ETrue;
		else
			inField = EFalse;
	// Calculate the offset
	if (inField)
		anOffset = aPos-currentPos;
	else
		anOffset = aPos+(*iFieldArray)[anIndex].iPreFieldLen-currentPos;
	return inField;
	}


void CTextFieldSet::DeleteFieldEntry(TInt anIndex)
	{
	DeleteFieldEntry(iFieldArray,anIndex);
	}


void CTextFieldSet::DeleteFieldEntry(CArrayFixSeg<TTextFieldEntry>* aArray,TInt anIndex)
// remove the entry anIndex from the array but don't delete the text from the doc.
//
	{
	__ASSERT_ALWAYS(anIndex<(aArray->Count()-1),Panic(EIndexOutOfRange));

	// add the entry's "before" to the "before" of the next entry.
	(*aArray)[anIndex+1].iPreFieldLen += (*aArray)[anIndex].iPreFieldLen;
	// add the field's length to the "before" of the next entry.	
	(*aArray)[anIndex+1].iPreFieldLen += (*aArray)[anIndex].iFieldValueLen;
	if ((*aArray)[anIndex].iFieldHeader.iField.IsPtr())
		delete (*aArray)[anIndex].iFieldHeader.iField.AsPtr(); // delete the field
	aArray->Delete(anIndex); // remove the entry from the array
	}


///////////////////////////////////////////////
// TTextFieldEntry
///////////////////////////////////////////////


EXPORT_C void TTextFieldEntry::InternalizeL(RReadStream& aStream)
// entry must have a header (ie cant be last entry in the array)
	{
	iPreFieldLen = aStream.ReadInt32L();
	iFieldValueLen = aStream.ReadInt32L();
	aStream>> iFieldHeader;
	}


EXPORT_C void TTextFieldEntry::ExternalizeL(RWriteStream& aStream)const
// entry must have a header (ie cant be last entry in the array)
	{
	aStream.WriteInt32L(iPreFieldLen);
	aStream.WriteInt32L(iFieldValueLen);
	aStream<< iFieldHeader;
	}


