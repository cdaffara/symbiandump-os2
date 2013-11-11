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

#include "TXTSTYLE.H"
#include "TXTFRMAT.H"
#include "TXTSTD.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTSTYLETraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTFMLYR_INTERNAL.H"
#include "TXTSTYLE_INTERNAL.H"
#endif

// Return the handle of a new paragraph style
 

EXPORT_C CParagraphStyle* CParagraphStyle::NewL(const CParaFormatLayer& aGlobalParaFormatLayer,
												const CCharFormatLayer& aGlobalCharFormatLayer)
/** Allocates and constructs a CParagraphStyle object whose formatting is based 
on a global paragraph and character format layer. The type UID is initialised 
to KUserDefinedParagraphStyleUid. The outline level is not initialised.

@param aGlobalParaFormatLayer The paragraph format layer on which the style's 
paragraph formatting is based. 
@param aGlobalCharFormatLayer The character format layer on which the style's 
character formatting is based. 
@return Pointer to the new CParagraphStyle object. */
	{
	CParagraphStyle* self=new(ELeave) CParagraphStyle();
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalParaFormatLayer,aGlobalCharFormatLayer);
	CleanupStack::Pop();
	return self;
	}


CParagraphStyle::CParagraphStyle():
	iType(KUserDefinedParagraphStyleUid)
	{
	}


// Create style and base it on the specified 'Normal' layers
void CParagraphStyle::ConstructL(const CParaFormatLayer& aGlobalParaFormatLayer,
										  const CCharFormatLayer& aGlobalCharFormatLayer)
	{
	SetBase(&aGlobalParaFormatLayer);
	//
	// Force para format layer storage allocation with empty layer
	TParaFormatMask paraMask;
	SetL((CParaFormat*)NULL,paraMask);
	//
	// Force char format layer storage allocation with empty layer;
/*	TCharFormat format;
	TCharFormatMask mask;
	iCharFormatLayer=CCharFormatLayer::NewL(format,mask);
*/
	iCharFormatLayer=CCharFormatLayer::NewL();
	iCharFormatLayer->SetBase(&aGlobalCharFormatLayer);
	}
 

EXPORT_C CParagraphStyle::~CParagraphStyle()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iCharFormatLayer;
	}


EXPORT_C CFormatLayer* CParagraphStyle::DoCloneL()const
	{
	const CCharFormatLayer* charLayer=STATIC_CAST(const CCharFormatLayer*,iCharFormatLayer->SenseBase());
	CParagraphStyle* clone=CParagraphStyle::NewL(STATIC_CAST(const CParaFormatLayer&,*SenseBase()),*charLayer);
	CleanupStack::PushL(clone);
	CloneLayerL(clone);  // clones the paragraph format layer
	delete clone->iCharFormatLayer;
	clone->iCharFormatLayer=NULL;  // necessary precaution if next instruction leaves
	clone->iCharFormatLayer=iCharFormatLayer->CloneL();  // clones the character format layer
	clone->iCharFormatLayer->SetBase(charLayer);  // restore the based on link
	CleanupStack::Pop();  // clone
	//
	clone->SetType(Type());
	clone->SetOutlineLevel(OutlineLevel());
	clone->iName=iName;
	//
	return clone;
	}


 

EXPORT_C TUid CParagraphStyle::Type()const
/** Gets the style's type UID.

@return The style's type UID. */
	{return iType;}


CParagraphStyle* CParagraphStyle::NewL(RReadStream& aStream,
										const CParaFormatLayer& aGlobalParaFormatLayer,
										const CCharFormatLayer& aGlobalCharFormatLayer)
	{
	CParagraphStyle* self=new(ELeave) CParagraphStyle();
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalParaFormatLayer,aGlobalCharFormatLayer);
	self->InternalizeL(aStream,&aGlobalParaFormatLayer,&aGlobalCharFormatLayer);
	CleanupStack::Pop();
	return self;
	}


// Load into this style from the specified readStream.  Base this style on the layer aBase - Default NULL
void CParagraphStyle::InternalizeL(RReadStream& aStream,const CFormatLayer* aParaLayerBase,const CFormatLayer* aCharLayerBase)
	{
	aStream>> iName;
	aStream>> iType;
	iOutlineLevel=aStream.ReadInt32L();
	iCharFormatLayer->InternalizeL(aStream,aCharLayerBase);
	//
	CParaFormatLayer::InternalizeL(aStream,aParaLayerBase);
	}


// Save the style in the specified WriteStream. The based on link is *NOT* stored.
void CParagraphStyle::ExternalizeL(RWriteStream& aStream)const
	{
	aStream<< iName;
	aStream<< iType;
	aStream.WriteInt32L(iOutlineLevel);
	aStream<< *iCharFormatLayer;
	//
	CParaFormatLayer::ExternalizeL(aStream);
	}


DLLEXPORT_C void CStyleList::__DbgTestInvariant()const
// Provides class invariants.  Explanations below:
//
	{
#ifdef _DEBUG
	TInt styleCount=Count();
	for (TInt nn=0;nn<styleCount;nn++)
		{
		RParagraphStyleInfo info=iList->At(nn);
		if (info.iStyleForNextPara)
			{
			TInt index=IndexByPtr(info.iStyleForNextPara);
			// ASSERT: The style-to-follow is present in the list.
			if (!((info.iStyleForNextPara==NULL) ||
	                ((info.iStyleForNextPara) && (index!=KErrNotFound && index>=0 && index<styleCount))))
			    {
			    OstTrace0( TRACE_DUMP, CSTYLELIST_DBGTESTINVARIANT, "Invariant" );
			    }
			__ASSERT_DEBUG(
				(info.iStyleForNextPara==NULL) ||
				((info.iStyleForNextPara) && (index!=KErrNotFound && index>=0 && index<styleCount)),User::Invariant());
			}
		}
#endif
	}


 

EXPORT_C CStyleList* CStyleList::NewL(TInt aCapacity /*=KMaxStyleListGranularity*/)
/** Allocates and constructs an empty CStyleList object with an array granularity.

@param aCapacity The number of entries by which the array of styles expands 
when its buffer is reallocated. By default, KMaxStyleListGranularity (= 4). 
Must be positive or a panic occurs. 
@return Pointer to the new style list. */
	{
	CStyleList* self=new(ELeave) CStyleList();
	CleanupStack::PushL(self);
	self->ConstructL(aCapacity);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CStyleList* CStyleList::NewL(RReadStream& aStream,
									  const CParaFormatLayer* aGlobalParaFormatLayer,
									  const CCharFormatLayer* aGlobalCharFormatLayer)
/** Allocates and constructs a CStyleList, restoring its contents from the specified 
stream store. Each style in the list is set to be based on the global format 
layers specified.

@param aStream Stream store from which the style list is restored. 
@param aGlobalParaFormatLayer Pointer to the global paragraph format layer 
on which all styles in the list are based. 
@param aGlobalCharFormatLayer Pointer to the global character format layer 
on which all styles in the list are based. 
@return Pointer to the new style list. */
	{
	CStyleList* self=new(ELeave) CStyleList();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream,aGlobalParaFormatLayer,aGlobalCharFormatLayer);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CStyleList::CStyleList()
	{
	}


EXPORT_C void CStyleList::ConstructL(TInt aCount)
// Allocate the style list.
//
	{
	iList=new(ELeave) CArrayFixFlat<RParagraphStyleInfo>(aCount);
	}


 

EXPORT_C CStyleList::~CStyleList()
/** Deletes all the entries in the list and the list itself. */
	{
	KillStyleList();
	}


 

EXPORT_C const RParagraphStyleInfo& CStyleList::At(TInt aIndex)const
/** Gets a style from the style list, from its index into the array. Two versions 
are supplied. The compiler chooses the appropriate version based on the use 
made of the returned reference. If it is used in an expression where that 
reference can be modified, then the non-const version is chosen.

@param aIndex The index of the style into the list. The first style is at 
position zero. Must be within the bounds of the array, or a panic occurs. 
@return A const reference to the style at position aIndex in the array. 
@return A non-const reference to the style at position aIndex in the array. */
	{
	if (!iList)
	    {
	    OstTrace0( TRACE_FATAL, CSTYLELIST_AT, "EStyleIntegrityError" );
	    }
	__ASSERT_ALWAYS(iList,Panic(EStyleIntegrityError));
	if (aIndex<0 || aIndex>=iList->Count())
	    {
	    OstTrace0( TRACE_DUMP, DUP3_CSTYLELIST_AT, "EStyleIndexOutOfRange" );
	    }
	__ASSERT_DEBUG(aIndex>=0 && aIndex<iList->Count(),Panic(EStyleIndexOutOfRange));
	
	return (*iList)[aIndex];
	}


EXPORT_C RParagraphStyleInfo& CStyleList::At(TInt aIndex)
	{
	if (!iList)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CSTYLELIST_AT, "EStyleIntegrityError" );
	    }
	__ASSERT_ALWAYS(iList,Panic(EStyleIntegrityError));
	if (aIndex<0 || aIndex>=iList->Count())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CSTYLELIST_AT, "EStyleIndexOutOfRange" );
	    }
	__ASSERT_DEBUG(aIndex>=0 && aIndex<iList->Count(),Panic(EStyleIndexOutOfRange));

	return (*iList)[aIndex];
	}


 
EXPORT_C void CStyleList::Reset()
 /** Deletes the contents of the list. */
	{
	__TEST_INVARIANT;
	
	TInt styleCount=Count();
	for (TInt nn=0;nn<styleCount;nn++)
		delete (iList->At(nn)).iStyle;
	if (iList)
		iList->Reset();

	__TEST_INVARIANT;
	}


void CStyleList::KillStyleList()
	{
	Reset();
	delete iList;
	}



EXPORT_C TStreamId CStyleList::StoreL(CStreamStore& aStore)const
/** Stores the style list to a stream store.

@param aStore Stream store to which the style list is written. 
@return The ID of the stream store. */
	{
	__TEST_INVARIANT;

	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	//
	stream<< *this;
	//
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	return id;
	}




EXPORT_C void CStyleList::ExternalizeL(RWriteStream& aStream)const
/** Externalises the style list to a write stream. The presence of this function 
means that the standard templated operator<<() (defined in s32strm.h) is available 
to externalise objects of this class. Does not externalise any styles' based-on 
links.

@param aStream Stream to which the object should be externalised. */
	{
	__TEST_INVARIANT;
	
	TInt count=Count();
	aStream.WriteUint8L(count);
	for (TInt mm=0;mm<count;mm++)
		{
		CParagraphStyle* style=(iList->At(mm)).iStyle;
		style->ExternalizeL(aStream);
		}
	for (TInt nn=0;nn<count;nn++)
		{
		RParagraphStyleInfo set=iList->At(nn);
		CParagraphStyle* style=set.iStyle;
		TInt offset=(set.iStyleForNextPara!=NULL)
			? IndexByPtr(style)
			: -1;
			if (!(offset==-1 || (offset>=0 && offset<Count())))
			    {
			    OstTrace0( TRACE_DUMP, CSTYLELIST_EXTERNALIZEL, "EStyleIntegrityError" );
			    }
		__ASSERT_DEBUG(offset==-1 || (offset>=0 && offset<Count()),Panic(EStyleIntegrityError));
		aStream.WriteInt8L(offset);
		}
	}



EXPORT_C void CStyleList::InternalizeL(RReadStream& aStream,
									   const CParaFormatLayer* aGlobalParaFormatLayer,
									   const CCharFormatLayer* aGlobalCharFormatLayer)
/** Internalises the style list from a read stream. The presence of this function 
means that the standard templated operator>>() (defined in s32strm.h) is available 
to internalise objects of this class. Any existing style list contents are 
replaced.

@param aStream Stream store from which the style list is internalised. 
@param aGlobalParaFormatLayer Pointer to the global paragraph format layer 
on which all styles in the list are based. 
@param aGlobalCharFormatLayer Pointer to the global character format layer 
on which all styles in the list are based. */
	{
	KillStyleList();
	TInt styleCount=aStream.ReadUint8L();
	ConstructL(Max(1,styleCount));  // panics if granularity==0
	RParagraphStyleInfo holdingSet;
	iList->AppendL(holdingSet,styleCount);
	for (TInt mm=0;mm<styleCount;mm++)
		{// restore the paragraph styles
		CParagraphStyle* style=CParagraphStyle::NewL(aStream,*aGlobalParaFormatLayer,*aGlobalCharFormatLayer);
		iList->At(mm).iStyle=style;
		}
	for (TInt nn=0;nn<styleCount;nn++)
		{// restore the "style for next paragraph" for each paragraph
		TInt offset=aStream.ReadInt8L();
		iList->At(nn).iStyleForNextPara=(offset==-1)
			? NULL
			: iList->At(offset).iStyle;
		}

	__TEST_INVARIANT;
	}



EXPORT_C TInt CStyleList::AppendL(RParagraphStyleInfo* aStyleSet)
/** Appends a style to the style list. The list takes ownership of the style.

@param aStyleSet The style (and optional style for the following paragraph) 
to append to the list. 
@return KErrNone if successful, or KErrAlreadyExists if the style is already 
present in the list. */
	{
	__TEST_INVARIANT;
	
	TInt count=Count();
	for (TInt ii=0;ii<count;ii++)
		{
		if (aStyleSet->iStyle==iList->At(ii).iStyle)
			return KErrAlreadyExists;  // we already own this style
		}
	CleanupStack::PushL(aStyleSet->iStyle);
	iList->AppendL(*aStyleSet);  // if the append fails, we must take responsibility for the style
	CleanupStack::Pop();  // aStyleSet.iStyle

	__TEST_INVARIANT;
	return KErrNone;
	}


EXPORT_C void CStyleList::Remove(CParagraphStyle* aStyle)
/** Removes a style from the style list. If the style is owned by the list, it 
is deleted. If the style is not owned by the list, but is referenced by a 
style owned by the list, (i.e. a style in the list is based on it, or references 
it as its iStyleForNextPara pointer) then the pointer to aStyle is set to 
NULL.

@param aStyle Pointer to the style to remove from the style list, or to set 
to NULL. */
	{
	__TEST_INVARIANT;

	TInt styles = Count();
	TInt index = -1;
	for (TInt i = 0; i < styles; i++)
		{
		RParagraphStyleInfo& cur_style = (*iList)[i];
		if (cur_style.iStyleForNextPara == aStyle)
			cur_style.iStyleForNextPara = NULL;
		if (cur_style.iStyle->iBasedOn == aStyle)
			cur_style.iStyle->iBasedOn = NULL;
		if (cur_style.iStyle == aStyle)
			{
			// Assert that the style must occur only once in the style list.
			if (index != -1)
			    {
			    OstTrace0( TRACE_DUMP, CSTYLELIST_REMOVE, "EStyleIntegrityError" );
			    }
			__ASSERT_DEBUG(index == -1,Panic(EStyleIntegrityError));
			index = i;
			}
		}

	if (index != -1)
		{
		delete aStyle;
		iList->Delete(index);
		}
	
	__TEST_INVARIANT;
	}



EXPORT_C TInt CStyleList::SetStyleToFollow(const RParagraphStyleInfo& aStyleSet)
/** Sets the style to use for the following paragraph for a style in the style 
list. Both the style (aStyleSet.iStyle) and the style to set for the following 
paragraph (aStyleSet.iStyleForNextPara) must exist in the style list.

The function first locates the style (aStyleSet.iStyle) in the list, then 
sets the style to use for the next paragraph to aStyleSet.iStyleForNextPara.

If aStyleSet.iStyle does not exist in the style list, the function returns 
with KErrNotFound. If aStyleSet.iStyleForNextPara does not exist in the style 
list, a panic occurs.

@param aStyleSet Identifies a style in the list, and a style to use for its 
following paragraph. 
@return KErrNone if successful, or KErrNotFound if the first style contained 
in the argument (aStyleSet.iStyle) is not in the style list. */
	{
	if (IndexByPtr(aStyleSet.iStyleForNextPara) == KErrNotFound)
	    {
	    OstTrace0( TRACE_FATAL, CSTYLELIST_SETSTYLETOFOLLOW, "EStyleIntegrityError" );
	    }
	__ASSERT_ALWAYS( IndexByPtr(aStyleSet.iStyleForNextPara) != KErrNotFound, Panic(EStyleIntegrityError) );
	__TEST_INVARIANT;
	
	TInt index = IndexByPtr(aStyleSet.iStyle);
	if (index == KErrNotFound)
		return index;
	
	(*iList)[index].iStyleForNextPara = aStyleSet.iStyleForNextPara;
	
	__TEST_INVARIANT;
	
	return KErrNone;

	}
	



EXPORT_C RParagraphStyleInfo* CStyleList::PtrByName(const TParagraphStyleName& aName) const
/** Gets the style with the specified name from the style list.

@param aName The name of the style to retrieve. 
@return Pointer to the style with the name specified. NULL if there is no style 
with this name in the list. */
	{
	// Return a pointer to the named style if it's in the list, or null if not.
	__TEST_INVARIANT;
	
	int count = Count();
	for (int i = 0; i < count; i++)
		{
		RParagraphStyleInfo& style_info = (*iList)[i];
		if (style_info.iStyle != NULL && style_info.iStyle->iName == aName)
			return &style_info;
		}
	return NULL;
	}




EXPORT_C RParagraphStyleInfo* CStyleList::PtrByType(const TUid aType) const
/** Gets the style with the specified type UID from the style list.

@param aType The UID of the style to retrieve. 
@return Pointer to the style with the type UID specified. NULL if there is 
no style with this type in the list. */
	{
	// Return a pointer to the first style with the specified type if any; or null if there are none with this type.
	__TEST_INVARIANT;

	int count = Count();
	for (int i = 0; i < count; i++)
		{
		RParagraphStyleInfo& style_info = (*iList)[i];
		if (style_info.iStyle != NULL && style_info.iStyle->Type() == aType)
			return &style_info;
		}
	return NULL;
	}




EXPORT_C TInt CStyleList::IndexByPtr(const CParaFormatLayer* aPtr)const
/** Gets the index into the style list of a specified paragraph style.

@param aPtr Pointer to the style. 
@return The style's index into the style list. KErrNotFound if the style is 
not found in the style list, or if aPtr is a paragraph format layer rather 
than a style. */
	{
	if (aPtr->Type()==KNormalParagraphStyleUid)
		return KErrNotFound;
	TInt count=Count();
	if (count==0)
		return KErrNotFound;  // ptr cannot be matched cos list is empty.
	TInt index=0;
	CParagraphStyle* style=NULL;
	while (index<count)
		{
		style=(*iList)[index].iStyle;
		if (style==aPtr)
			break;
		index++;
		}
	return (index<count)
		? index
		: KErrNotFound;
	}




EXPORT_C TInt CStyleList::IndexByName(const TDesC& aName)const
/** Gets the index into the style list of a specified paragraph style, identified 
by its name.

@param aName The name of the style. 
@return The style's index into the style list. KErrNotFound if the style name 
is not found in the style list. */
	{
	TInt count=Count();
	if (count==0)
		return KErrNotFound;  // name cannot be in list cos its empty.
	TInt index=0;
	CParagraphStyle* style=NULL;
	while (index<count)
		{
		style=(*iList)[index].iStyle;
		if (style != NULL && style->iName == aName)
			break;
		index++;
		}
	return (index<count)
		? index
		: KErrNotFound;
	}



EXPORT_C CStyleList* CStyleList::DeepCloneL() const
/** Creates and returns a style list which is a clone of the current style list.

@return Pointer to a clone of the current style list. */
	{
	__TEST_INVARIANT;
	
	CStyleList* newList = CStyleList::NewL();
	CleanupStack::PushL(newList);

	TInt styleCount=Count();
	for (TInt styleItem=0;styleItem<styleCount;styleItem++)
		{
		const RParagraphStyleInfo& source=iList->At(styleItem);
		CParagraphStyle* style=source.iStyle->CloneL();
		RParagraphStyleInfo info(style,source.iStyleForNextPara);
		newList->AppendL(&info);
		}

	CleanupStack::Pop();
	return newList;
	}
