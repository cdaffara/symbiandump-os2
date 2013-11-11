/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <fntstore.h>
#include <gdi.h>
#include "FNTSTD.H"
#include <graphics/shapeimpl.h>
#include "ShaperCache.H"

//class COpenFontShaperCacheEntry;
COpenFontShaperCacheEntry* COpenFontShaperCacheEntry::New(RHeap* aHeap)
	{
	COpenFontShaperCacheEntry* entry = (COpenFontShaperCacheEntry*)aHeap->Alloc(sizeof(COpenFontShaperCacheEntry));
	if (!entry)
		return NULL;
	new(entry) COpenFontShaperCacheEntry();	
	return entry;
	}
	
COpenFontShaperCacheEntry* COpenFontShaperCacheEntry::New(RHeap* aHeap, CShaper::TInput aInput, TShapeHeader* aShapeHeader)
	{
	COpenFontShaperCacheEntry* entry = (COpenFontShaperCacheEntry*)aHeap->Alloc(sizeof(COpenFontShaperCacheEntry));
	if (!entry)
		return NULL;
	new(entry) COpenFontShaperCacheEntry(aHeap,aInput);
	TInt ret=entry->Construct(aInput,aShapeHeader);
	if (ret!=KErrNone)
		{
		aHeap->Free(entry);
		return NULL;
		}
	return entry;
	}

TInt COpenFontShaperCacheEntry::Construct(CShaper::TInput aInput,TShapeHeader* aShapeHeader)
	{
	// Get some memory for the shape header
	iShapeHeader = (TShapeHeader*)( iHeap->Alloc(
	sizeof(TShapeHeader) + (aShapeHeader->iGlyphCount * 10) + 4));
	if (iShapeHeader==NULL)
		{
		return KErrNoMemory;
		}
	Mem::Copy(iShapeHeader, aShapeHeader,sizeof(TShapeHeader) + (aShapeHeader->iGlyphCount * 10) + 4);
	
	// Get some memory for the input text
	iText = (TUint16*)(iHeap->Alloc(sizeof(TUint16)*aInput.iText->Length()));
	if (iText==NULL)
		{
		iHeap->Free(iShapeHeader);
		return KErrNoMemory;
		}
	TUint16* inputText = (TUint16*)aInput.iText->Ptr();
	Mem::Copy(iText, inputText, sizeof(TUint16)*aInput.iText->Length());
	return KErrNone;
	}
	
/**
Utility function used to free memory taken up by an entry to the cache
@internalTechnology
**/
void COpenFontShaperCacheEntry::Delete(RHeap* aHeap,COpenFontShaperCacheEntry* aCacheEntry)
	{
	if (!aCacheEntry->IsSentinel())
		{
		aHeap->Free(aCacheEntry->iShapeHeader);
		aHeap->Free(aCacheEntry->iText);
		//now free the THandleCount list
		if (aCacheEntry->iHandleRefList)
			{
			THandleCount* ptr=aCacheEntry->iHandleRefList;
			while (ptr)
				{
				THandleCount* next=ptr->iNext;
				aHeap->Free(ptr);
				ptr=next;
				}
			aCacheEntry->iHandleRefList=NULL;
			}
		aHeap->Free(aCacheEntry);
		}
	}

//function to update the ref count for a particular session handle	
TInt COpenFontShaperCacheEntry::IncRefCount(TInt aSessionHandle)
	{
	//get pointer to start of the linked list
	THandleCount* ptr=iHandleRefList;
	THandleCount* last=NULL;
	//loop through existing entries first
	while (ptr)
		{
		if (ptr->iSessionHandle==aSessionHandle)
			{
			ptr->iRefCount++;
			return KErrNone;			
			}
		last=ptr;
		ptr=ptr->iNext;
		}
	// now we have reached the end of the list or we start with empty list
	THandleCount* handle=(THandleCount*)iHeap->Alloc(sizeof(THandleCount));
	if (!handle)
		return KErrNoMemory;
	new (handle)THandleCount(aSessionHandle);
	//if the list is initially empty
	if (!last)
		iHandleRefList=handle;
	else
		last->iNext=handle;
	iHandleRefCount++;
	return KErrNone;	
	}

//function to decrement the ref count for a particular session handle
//aReset here is used to immediately reset the handleCount reference to zero
//and remove from the entry(this will be called in the case when a client session dies
//Note that once the ref count reaches zero the THandleCount entry is automatically
//removed from the list
TInt COpenFontShaperCacheEntry::DecRefCount(TInt aSessionHandle,TBool aReset)
	{
	//get pointer to start of the linked list
	THandleCount* ptr=iHandleRefList;
	THandleCount* last=NULL;
	//loop through existing entries first
	while (ptr)
		{
		if (ptr->iSessionHandle==aSessionHandle)
			{
			if (aReset || ptr->iRefCount==1)
				{
				THandleCount* next=ptr->iNext;
				//delete from first item
				if (last==NULL)
					iHandleRefList=next;
				else
					last->iNext=next;
				iHeap->Free(ptr);
				iHandleRefCount--;			
				return KErrNone;
				}
			ptr->iRefCount--;
			return KErrNone;			
			}
		last=ptr;	
		ptr=ptr->iNext;
		}
	return KErrNotFound;	
	}	
	
