/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TXTFRMAT.H"
#include "TXTFMLYR.H"
#include <txtfmstm.h>
#include "TXTSTD.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTFMLYRTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTFMLYR_INTERNAL.H"
#endif

CFormatLayer::CFormatLayer()
	{
	}


CFormatLayer::~CFormatLayer()
	{
	iStore.Reset();
	}


DLLEXPORT_C void CFormatLayer::__DbgTestInvariant() const
	{
	}


 
EXPORT_C void CFormatLayer::SetBase(const CFormatLayer* aBaseFormatLayer)
/** Sets the format layer which this layer's based-on link points to.

@param aBaseFormatLayer The format layer which this layer's based-on link 
points to. Specify NULL if this is the final layer in the chain (the layer 
on which all other layers are based). */
	{
	iBasedOn = aBaseFormatLayer;
	}


 
EXPORT_C const CFormatLayer* CFormatLayer::SenseBase() const
/** Gets the format layer which this layer's based-on link points to. If NULL, 
this layer is the final layer in the chain.

@return The format layer on which this layer is based. */
	{
	return iBasedOn;
	}


 
EXPORT_C TInt CFormatLayer::ChainCount() const
/** Gets the number of format layers in the chain, inclusive of itself. Assumes 
that the format layer chain has been correctly set up to terminate with a 
NULL based-on link.

@return The number of format layers in the chain, counting from the current 
layer. */
	{
	TInt chainCount=1;
	const CFormatLayer* next=iBasedOn;
	while (next)
		{
		++chainCount;
		next=next->iBasedOn;
		}
	return chainCount;
	}



const TUint8* CFormatLayer::Ptr(TInt& aSize)const
/** Return a pointer to the stored bytecode.*/	
    {
	return iStore.Ptr(aSize);
	}



EXPORT_C void CFormatLayer::Reset()
/** Deletes the contents of the format layer. The based-on link is not affected. */
	{
	iStore.Reset();
	}



EXPORT_C void CFormatLayer::InternalizeChainL(RReadStream& aStream,const CFormatLayer* aBase)
/** Restores a chain of format layers from a read stream. The layer at the end 
of the chain (the one furthest from this layer) is set to be based on the 
specified layer aBase, which may be NULL. This layer is set to be at the head 
of the restored chain (i.e. no other layers are based on it).

@param aStream Stream from which the format layer chain should be internalised. 

@param aBase The format layer at the end of the chain (furthest from this layer). 
May be NULL. */
	{
	TInt chainLength=aStream.ReadInt8L();
	if (chainLength<1)
		User::Leave(KErrCorrupt);  // Must restore at least one layer (this), else corrupt stream.
	TInt descendentCount=chainLength-1;
	for (TInt loop=0;loop<descendentCount;loop++)
		{// Restore each descendent of [this] layer.
		CFormatLayer* layer=RestoreNewL(aStream);
		layer->SetBase(aBase);
		CleanupStack::PushL(layer);
		aBase=layer;
		}
	aStream>> *this;
	SetBase(aBase);
	CleanupStack::Pop(descendentCount);
	}




EXPORT_C void CFormatLayer::ExternalizeChainL(RWriteStream& aStream,TInt aExcludeCount)const
/** Stores a chain of format layers. By default an entire chain is stored unless 
an exclude count is provided. In this case, the length of the chain stored 
is the ChainCount() minus the exclude count. The excluded layers are the ones 
starting with the layer with the NULL based-on link.

Note

The exclude count must be zero or greater but must be less than the total 
number of layers in the chain, otherwise a panic occurs.

@param aStream Stream to which the format layer chain should be externalised. 
@param aExcludeCount The number of layers to be excluded. By default, zero. */
	{
// ASSERT: aExcludeCount is positive.
	if (aExcludeCount<0)
	    {
	    OstTrace0( TRACE_FATAL, CFORMATLAYER_EXTERNALIZECHAINL, "ECannotStoreFormatLayerChain" );
	    }
	__ASSERT_ALWAYS(aExcludeCount>=0,Panic(ECannotStoreFormatLayerChain));
// ASSERT: The number of layers to be excluded is less than the total no. of layers.
	if (aExcludeCount>=ChainCount())
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CFORMATLAYER_EXTERNALIZECHAINL, "ECannotStoreFormatLayerChain" );
	    }
	__ASSERT_ALWAYS(aExcludeCount<ChainCount(),Panic(ECannotStoreFormatLayerChain));
	TInt aCount=ChainCount()-aExcludeCount;
	aStream.WriteInt8L(aCount);  // Store the chain length.
	ExternalizeLayersRecurseL(aStream,--aCount);
	}	
	

void CFormatLayer::ExternalizeLayersRecurseL(RWriteStream& aStream,TInt aDescendantCount)const
/** Stores a format layer chain with *aLength* number of layers in a stack-like 
fashion. This necessitates navigating to the end of the chain and storing the 
layers as we unwind back to the top of the chain.*/
//
	{
	if (aDescendantCount)
		{
// ASSERT: The format layer chain is consistent.
		if (iBasedOn==NULL)
		    {
		    OstTrace0( TRACE_DUMP, CFORMATLAYER_EXTERNALIZELAYERSRECURSEL, "ECorruptFormatLayerChain" );
		    }
		__ASSERT_ALWAYS(iBasedOn!=NULL,Panic(ECorruptFormatLayerChain));
		iBasedOn->ExternalizeLayersRecurseL(aStream,--aDescendantCount);
		}
	aStream<< *this;
	}
	
/** Implementations of this function compare another format layer with the
current object. For the two layers to be equal, they must have the same 
contents and (if the second parameter is ETrue),their based-on links must 
point to the same format layer.

@param aLayer The layer to compare to this format layer.
@param aCheckBasedOnLink If ETrue, both layers' based-on links must point to 
the same format layer. If EFalse, the based-on links are not used in the 
comparison. By default, ETrue.
@return  ETrue if the two layers are identical, otherwise EFalse. */
TBool CFormatLayer::IsIdentical(const TUint8* aPtr,TInt aSize) const
	{
	TInt size=0;
	const TUint8* ptr=iStore.Ptr(size);
	if ((ptr==NULL && aPtr!=NULL) || (ptr!=NULL && aPtr==NULL))
		return EFalse;
	if (ptr==NULL && aPtr==NULL)
		return ETrue;
	return (!(TBool)(Mem::Compare(ptr,size,aPtr,aSize)));
	}

/** Tests whether any formatting is stored in the format layer.

@return ETrue if no formatting is stored in the format layer, otherwise returns 
EFalse. */
EXPORT_C TBool CFormatLayer::IsEmpty() const
	{
	TInt size=0;
	return iStore.Ptr(size)==NULL;
	}

/** Swaps the contents of this with aLayer.
@param aLayer The layer to swap contents with.
@internalComponent */
void CFormatLayer::Swap(CFormatLayer& aLayer)
	{
	iStore.Swap(aLayer.iStore);
	const CFormatLayer* t = iBasedOn;
	iBasedOn = aLayer.iBasedOn;
	aLayer.iBasedOn = t;
	}

/** Allocates and constructs an empty CParaFormatLayer. Its based-on link is 
NULL.

Note: Use SetL() to set format attributes in the layer. Use SetBase(), defined 
in the base class CFormatLayer, to set the layer's based on link.

@return Pointer to the new paragraph format layer. */
EXPORT_C CParaFormatLayer* CParaFormatLayer::NewL()
	{
	return new(ELeave) CParaFormatLayer;
	}


EXPORT_C CParaFormatLayer* CParaFormatLayer::NewL(RReadStream& aStream)
/** Allocates and constructs a CParaFormatLayer, restoring its format attributes 
from a stream. The layer's based-on link is set to NULL.

@param aStream Stream from which the layer is restored. 
@return Pointer to the new paragraph format layer. */
	{
	CParaFormatLayer* self=NewL();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CParaFormatLayer* CParaFormatLayer::NewL(const CParaFormat* aFormat,const TParaFormatMask& aMask)
/** Allocates and constructs a CParaFormatLayer. The attributes which are set 
in the mask are initialised to the values specified in the format container 
aParaFormat. The attributes which are not set in the mask are initialised to 
the default values for class CParaFormat. The new layer's based-on link is set 
to NULL.

@param aParaFormat Contains the attribute values to assign to the format layer. 
@param aMask Mask specifying which attributes should be initialized from 
aParaFormat. 
@return Pointer to ParaFormatLayer the new paragraph format layer. */
	{
	CParaFormatLayer* self=NewL();
	CleanupStack::PushL(self);
	self->SetL(aFormat,aMask);
	CleanupStack::Pop();
	return self;
	}


CParaFormatLayer* CParaFormatLayer::NewL(const CParaFormatLayer* aLayer)
/** Copying construction
 does not copy the based on link*/
	{
	CParaFormatLayer* self=NewL();  // based-on is NULL
	CleanupStack::PushL(self);
	aLayer->CloneLayerL(self);
	CleanupStack::Pop();			// self
	return self;
	}

CParaFormatLayer* CParaFormatLayer::NewCopyBaseL(const CParaFormatLayer* aLayer)
/** Copying construction
 copies based-on link*/
	{
	CParaFormatLayer* self=NewL(aLayer);  // based-on is NULL
	self->iBasedOn=aLayer->iBasedOn;
	return self;
	}

CParaFormatLayer::CParaFormatLayer()
/** Constructor.*/
// No ConstructL method since allocation is postponed until first Set.
//
	{
	}

EXPORT_C void CParaFormatLayer::ExternalizeL(RWriteStream& aStream)const
/** Externalises the paragraph format layer but not its based-on link to a 
write stream. The presence of this function means that the standard templated 
operator<<() (defined in s32strm.h) is available to externalise objects of 
this class.

@param aStream Stream to which the format layer should be externalised. */
	{aStream<< iStore;}

EXPORT_C void CParaFormatLayer::InternalizeL(RReadStream& aStream,const CFormatLayer* aBase)
/** Internalises the paragraph format layer but not its based-on link from a 
read stream. The presence of this function means that the standard templated 
operator>>() (defined in s32strm.h) is available to internalise objects of 
this class. The internalised layer is set to be based on the layer specified.

@param aStream Stream from which the format layer should be internalised. 
@param aBase The based-on link to assign to the layer. By default NULL. */
	{
	aStream>> iStore;
	SetBase(aBase);
	}

EXPORT_C void CParaFormatLayer::SetL(const CParaFormat* aDesiredEffectiveFormat,const TParaFormatMask& aMask)
/** Sets the layer's format attributes. The attributes which are set in the 
mask are set in the layer to the values specified in the format container 
aDesiredEffectiveFormat. The attributes which are not set in the mask are not 
changed.

Note: Any tab stops in aDesiredEffectiveFormat are merged with the tab stops in 
the current layer.

@param aDesiredEffectiveFormat Contains the attribute values to assign to 
the format layer. 
@param aMask Mask specifying which attributes should be set from 
aDesiredEffectiveFormat. */
	{
	if ( !aDesiredEffectiveFormat )
		{
		return;
		}
	
	const CParaFormat& desiredFormat = *aDesiredEffectiveFormat;
	CParaFormat currentEffectiveFormat;
	ResetOnCleanupL( &currentEffectiveFormat);
	if (iBasedOn)
		((CParaFormatLayer*)iBasedOn)->SenseEffectiveL(&currentEffectiveFormat);
	iStore.SetParaFormatL(desiredFormat,aMask,currentEffectiveFormat);
	CleanupStack::Pop();
	}

EXPORT_C void CParaFormatLayer::SenseEffectiveL(CParaFormat* aParaFormat,CParaFormat::TParaFormatGetMode aMode)const
/** Senses the layer's effective format, searching all based-on links. The 
resulting aParaFormat is fully populated, except that if aMode is 
EFixedAttributes, then only the fixed attributes (not tabs, paragraph borders 
or bullets) are written to it.

Notes:

The function also "tidies up" the layer's effective paragraph formatting, 
so that any zero height bullets, paragraph borders with a NULL line style 
or NULL tab stops are removed. 

The function can only leave if aMode has a value of EAllAttributes.

@param aParaFormat On return, contains the layer's effective formatting. 
Depending on the value of aMode, tabs, borders and bullets may be excluded. 
Must not be NULL or a panic occurs. 
@param aMode Controls which attributes are written to aParaFormat. If 
EAllAttributes, all attributes are written; if EFixedAttributes, tabs, 
bullets and borders are not written. */
	{
	if (aParaFormat==NULL)
	    {
	    OstTrace0( TRACE_FATAL, CPARAFORMATLAYER_SENSEEFFECTIVEL, "ENullFormatPointer" );
	    }
	__ASSERT_ALWAYS(aParaFormat!=NULL,Panic(ENullFormatPointer));
	aParaFormat->Reset();
	TParaFormatMask mask;
	FillParaFormatL(aParaFormat,mask,aMode);
	CleanupEffectiveFormat(aParaFormat,mask);
	}

EXPORT_C void CParaFormatLayer::SenseL(CParaFormat* aParaFormat,TParaFormatMask& aMask,CParaFormat::TParaFormatGetMode aMode)const
/** Senses the formatting which has been applied to the current layer only. No 
based-on links are searched. This function does not get the effective formatting, 
but the resulting aParaFormat is useable even if not all attributes are flagged 
for sensing in aMask because any attribute values not sensed from the current 
layer, are set to default values.

The function can only leave if aMode has a value of EAllAttributes.

@param aParaFormat On return, contains the formatting which has been applied 
to the current layer only. Any attributes not explicitly set in the current 
layer are initialised to the default values for a CParaFormat. Attributes 
specified in aMask are not sensed from this layer. The values for these 
attributes are also initialised to the default settings. Must not be NULL or 
a panic occurs. 
@param aMask A bitmask. Any attributes which are set in the mask as passed 
into the function are not sensed from the current layer. On return, indicates 
the attributes which were sensed from this layer. So, normally, when passed 
to the function, all attributes in the mask should be unset. 
@param aMode Controls which attributes are written to aParaFormat. If 
EAllAttributes, all attributes are written; if EFixedAttributes, tabs, bullets 
and borders are not written. */
	{
	if (aParaFormat==NULL)
	    {
	    OstTrace0( TRACE_FATAL, CPARAFORMATLAYER_SENSEL, "ENullFormatPointer" );
	    }
	__ASSERT_ALWAYS(aParaFormat!=NULL,Panic(ENullFormatPointer));

	iStore.SenseParaFormatL(*aParaFormat,aMask,aMode);
	}


void CFormatLayer::CloneLayerL(CFormatLayer* aClone)const
	{
	aClone->iStore.CopyL(iStore);
	}


EXPORT_C CFormatLayer* CParaFormatLayer::DoCloneL()const
//
//
	{
	return NewL(this);  // use copy construction
	}



EXPORT_C TBool CParaFormatLayer::IsIdenticalL(const CParaFormat* aParaFormat,const TParaFormatMask& aMask)const
/** Compares a format attribute container with the current layer. For the two 
objects to be identical, the current layer must contain only the attributes 
specified in the argument aMask, and these attributes must have the same values 
as those in aParaFormat. None of the current layer's based-on links are searched.

@param aParaFormat Contains the attribute values used in the comparison. 
@param aMask A bitmask specifying which attributes are relevant to the function. 

@return ETrue if the formatting of the current layer exactly matches that 
contained in aParaFormat. Otherwise EFalse. */
	{
	CParaFormat* thisParaFormat=CParaFormat::NewLC();
	TParaFormatMask thisParaFormatMask;
	SenseL(thisParaFormat,thisParaFormatMask);
	TBool result=EFalse;
	if (thisParaFormatMask!=aMask)
		result=EFalse;
	else if (thisParaFormat->IsEqual(*aParaFormat,aMask))
		result=ETrue;
	CleanupStack::PopAndDestroy();
	return result;
	}

EXPORT_C TBool CParaFormatLayer::IsIdentical(CFormatLayer* aLayer,TBool aCheckBasedOnLink)const
/** Compares another paragraph format layer with the current layer. For the two 
layers to be equal, they must have the same contents and (if the second 
parameter is ETrue), their based-on links must point to the same format layer.

@param aLayer The paragraph format layer to compare to this format layer. 
@param aCheckBasedOnLink If ETrue, both layers' based-on links must point to 
the same format layer. If EFalse, the based-on links are not used in the 
comparison. By default, ETrue. 
@return ETrue if the two layers are identical, otherwise EFalse. */
	{
	if (aCheckBasedOnLink)
		{
		if (iBasedOn!=aLayer->SenseBase())
			return EFalse;
		}
	TInt size;
	const TUint8* ptr=((CParaFormatLayer*)aLayer)->Ptr(size);		// some design went wrong here!
	return CFormatLayer::IsIdentical(ptr,size);
	}

EXPORT_C const TUint8* CParaFormatLayer::Ptr(TInt& aSize)const
/** Gets a pointer to the start of the buffer containing the layer's format 
attribute values.

@param aSize On return, set to the size of the buffer. 
@return Pointer to the buffer which contains the layer's format attribute 
values. */
	{return CFormatLayer::Ptr(aSize);}

EXPORT_C TUid CParaFormatLayer::Type()const
/** Returns the paragraph format layer UID. This can be used to distinguish 
between an ordinary paragraph format layer and paragraph styles, which have a 
different UID.

@return The UID of a paragraph format layer (KNormalParagraphStyleUid). */
	{return KNormalParagraphStyleUid;}

CFormatLayer* CParaFormatLayer::RestoreNewL(RReadStream& aStream)
/** Return a new CParaFormatLayer, having restored it from aStream.
 Overrides the base class method, to provide a new format layer of the correct 
 type.*/
	{return NewL(aStream);}

void CParaFormatLayer::FillParaFormatL(CParaFormat* aParaFormat,TParaFormatMask& aMask,CParaFormat::TParaFormatGetMode aMode)const
/** Fills aParaFormat by dumping the current format layer into it.  
Next traverse the 'basedOn' link if it is not NULL, and repeat.*/
	{
	if ( !aParaFormat )
		{
		return;
		}
	
	CParaFormat& senseFormat = *aParaFormat;

	iStore.SenseParaFormatL(senseFormat,aMask,aMode);
	if (iBasedOn)
		((CParaFormatLayer*)iBasedOn)->FillParaFormatL(aParaFormat,aMask,aMode);
	}

void CParaFormatLayer::CleanupEffectiveFormat(CParaFormat* aParaFormat,TParaFormatMask aMask)const
/** Removes anti-tabs, zero height and null bullets, and paragraph borders with
 null linestyles, from aParaFormat.  An effective format does not support the
 notion of anti-tabs etc.*/
	{
	if (aMask.AttribIsSet(EAttBullet))
		{
		if (aParaFormat->iBullet->iStyle == TBullet::ENullStyle || aParaFormat->iBullet->iHeightInTwips <= 0)
			{
			delete aParaFormat->iBullet;
			aParaFormat->iBullet = NULL;
			}
		}

	if (aMask.AttribIsSet(EAttTopBorder) ||
		aMask.AttribIsSet(EAttBottomBorder) ||
		aMask.AttribIsSet(EAttLeftBorder) ||
		aMask.AttribIsSet(EAttRightBorder))
		CleanupBorders(aParaFormat);

	if (aMask.AttribIsSet(EAttTabStop))
		{
		int index = 0;
		while (index < aParaFormat->TabCount())
			{
			TTabStop tab = aParaFormat->TabStop(index);
			if (tab.iType == TTabStop::ENullTab)
				aParaFormat->RemoveTab(tab.iTwipsPosition);
			else
				index++;
			}
		}
	}


void CParaFormatLayer::CleanupBorders(CParaFormat* aParaFormat)const
/** Destroys the paragraph border if it is of NULL linestyle,
 and nulls the pointer to it (aBorder).*/
	{
	if (aParaFormat->ParaBorder(CParaFormat::EParaBorderTop).iLineStyle==TParaBorder::ENullLineStyle &&
		aParaFormat->ParaBorder(CParaFormat::EParaBorderBottom).iLineStyle==TParaBorder::ENullLineStyle &&
		aParaFormat->ParaBorder(CParaFormat::EParaBorderLeft).iLineStyle==TParaBorder::ENullLineStyle &&
		aParaFormat->ParaBorder(CParaFormat::EParaBorderRight).iLineStyle==TParaBorder::ENullLineStyle)
		{
		aParaFormat->RemoveAllBorders();
		}
	}


EXPORT_C CCharFormatLayer* CCharFormatLayer::NewL()
/** Allocates and constructs an empty CCharFormatLayer. Its based-on link is 
NULL.

Note: Use SetL() to set format attributes in the layer. Use SetBase(), defined 
in the base class CFormatLayer, to set the layers based on link.

@return Pointer to the new character format layer. */
	{return new(ELeave)CCharFormatLayer;}



EXPORT_C CCharFormatLayer* CCharFormatLayer::NewL(RReadStream& aStream)
/** Allocates and constructs a CCharFormatLayer, restoring its format attributes 
from a stream. The layer's based-on link is set to NULL.

@param aStream Stream from which the layer is restored. 
@return Pointer to the new character format layer. */
	{
	CCharFormatLayer* self=NewL();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CCharFormatLayer* CCharFormatLayer::NewL(const TCharFormat& aFormat,const TCharFormatMask& aMask)
/** Returns a handle to a new charFormatLayer, after constructing
 it and setting it with the specified format and format mask.*/
	{
	CCharFormatLayer* self=NewL();
	CleanupStack::PushL(self);
	self->SetL(aFormat,aMask);
	CleanupStack::Pop();
	return self;
	}


CCharFormatLayer* CCharFormatLayer::NewL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask)
	{
	CCharFormatLayer* self = NewL();
	CleanupStack::PushL(self);
	self->SetL(aFormat,aMask);
	CleanupStack::Pop();
	return self;
	}


CCharFormatLayer* CCharFormatLayer::NewL(const CCharFormatLayer* aLayer)
/** Copying construction
 does not copy based-on link.*/
	{
	CCharFormatLayer* self=NewL();  // based-on is NULL
	CleanupStack::PushL(self);
	aLayer->CloneLayerL(self);
	CleanupStack::Pop();			// self
	return self;
	}

CCharFormatLayer* CCharFormatLayer::NewCopyBaseL(const CCharFormatLayer* aLayer)
/** Copying construction
 copies based-on link*/
	{
	CCharFormatLayer* self=NewL(aLayer);  // based-on is NULL
	self->iBasedOn=aLayer->iBasedOn;
	return self;
	}


CCharFormatLayer::CCharFormatLayer()
	{
	}

EXPORT_C void CCharFormatLayer::ExternalizeL(RWriteStream& aStream)const
/** Externalises the character format layer but not its based-on link to a 
write stream. The presence of this function means that the standard templated 
operator<<() (defined in s32strm.h) is available to externalise objects of 
this class.

@param aStream Stream to which the format layer should be externalised. */
	{
	aStream << iStore;
	}

EXPORT_C void CCharFormatLayer::InternalizeL(RReadStream& aStream,const CFormatLayer* aBase)
/** Internalises the character format layer but not its based-on link from a 
read stream. The presence of this function means that the standard templated 
operator>>() (defined in s32strm.h) is available to internalise objects of 
this class. The internalised layer is set to be based on the layer specified.

@param aStream Stream from which the format layer should be internalised. 
@param aBase The based-on link to assign to the layer. By default NULL. */
	{
	aStream >> iStore;
	SetBase(aBase);
	}


EXPORT_C const TUint8* CCharFormatLayer::Ptr(TInt& aSize)const
/** Gets a pointer to the start of the buffer containing the layer's format 
attribute values.

@param aSize On return, set to the size of the buffer. 
@return Pointer to the buffer which contains the layer's format attribute 
values. */
	{
	return CFormatLayer::Ptr(aSize);
	}

EXPORT_C void CCharFormatLayer::SetL(const TCharFormat& aCharFormat,const TCharFormatMask& aMask)
/** Sets the layer's format attributes. The attributes which are set in the 
mask are set in the layer to the values specified in the format container 
aCharFormat. The attributes which are not set in the mask are not changed.

@param aCharFormat Contains the attribute values to assign to the format layer. 
@param aMask Mask specifying which attributes should be set from aCharFormat. */
	{
	TCharFormatX format(aCharFormat);
	iStore.SetCharFormatL(format,aMask);
	}

void CCharFormatLayer::SetL(const TCharFormatX& aCharFormat,const TCharFormatXMask& aMask)
/** Sets the layer's format attributes. The attributes which are set in the 
mask are set in the layer to the values specified in the format container 
aCharFormat. The attributes which are not set in the mask are not changed.

@param aCharFormat Contains the attribute values to assign to the format layer. 
@param aMask Mask specifying which attributes should be set from aCharFormat. */
	{
	iStore.SetCharFormatL(aCharFormat,aMask);
	}

EXPORT_C void CCharFormatLayer::SenseEffective(TCharFormat& aCharFormat)const
/** Senses the layer's effective format, searching all based-on links. The 
resulting aCharFormat is fully populated.

@param aCharFormat On return, contains the layer's effective formatting. */
	{
	TCharFormatX format;
	TCharFormatXMask mask;
	FillCharFormat(format,mask);
	aCharFormat = format.iCharFormat;
	}

void CCharFormatLayer::SenseEffective(TCharFormatX& aCharFormat) const
/** Senses the layer's effective format, searching all based-on links. The 
resulting aCharFormat is fully populated.

@param aCharFormat On return, contains the layer's effective formatting. */
	{
	TCharFormatXMask mask;
	aCharFormat = TCharFormatX(); // initialise character format; FillCharFormat doesn't do this
	FillCharFormat(aCharFormat,mask);
	}

EXPORT_C void CCharFormatLayer::Sense(TCharFormat& aCharFormat,TCharFormatMask& aMask)const
/** Senses the formatting which has been applied to the current layer only. No 
based-on links are searched. This function does not get the layer's effective 
formatting, but the resulting aCharFormat is fully populated, even if not 
all attributes are flagged for sensing in aMask because any attribute values 
not sensed from the current layer are set to default values.

@param aCharFormat On return, contains the formatting which has been applied 
to the current layer only. Any attributes not explicitly set in the current 
layer are set to the default values for a TCharFormat. Any attributes specified 
in aMask are not sensed from this layer. The values for these attributes are 
also initialised to the default settings. 
@param aMask A bitmask. Any attributes which are set in the mask as passed 
into the function are not sensed from the current layer. On return, indicates 
the attributes which were sensed from this layer. So, normally, when passed 
to the function, all attributes in the mask should be unset. */
	{
	TCharFormatX format(aCharFormat);
	TCharFormatXMask mask = aMask;
	iStore.SenseCharFormat(format,mask);
	aCharFormat = format.iCharFormat;
	mask.ClearExtendedAttribs();
	aMask = mask;
	}

void CCharFormatLayer::Sense(TCharFormatX& aCharFormat,TCharFormatXMask& aMask) const
/** Senses the formatting which has been applied to the current layer only. No 
based-on links are searched. This function does not get the layer's effective 
formatting, but the resulting aCharFormat is fully populated, even if not 
all attributes are flagged for sensing in aMask because any attribute values 
not sensed from the current layer are set to default values.

@param aCharFormat On return, contains the formatting which has been applied 
to the current layer only. Any attributes not explicitly set in the current 
layer are set to the default values for a TCharFormat. Any attributes specified 
in aMask are not sensed from this layer. The values for these attributes are 
also initialised to the default settings. 
@param aMask A bitmask. Any attributes which are set in the mask as passed 
into the function are not sensed from the current layer. On return, indicates 
the attributes which were sensed from this layer. So, normally, when passed 
to the function, all attributes in the mask should be unset. */
	{
	iStore.SenseCharFormat(aCharFormat,aMask);
	}


EXPORT_C CFormatLayer* CCharFormatLayer::DoCloneL()const
	{
	return NewL(this);  // use copy construction
	}


EXPORT_C TBool CCharFormatLayer::IsIdentical(CFormatLayer* aLayer,TBool aCheckBasedOnLink)const
/** Compares another character format layer with the current layer. For the two 
layers to be equal, they must have the same contents and (if the second 
parameter is ETrue), their based-on links must point to the same format layer.

@param aLayer The character format layer to compare to this format layer. 
@param aCheckBasedOnLink If ETrue, both layers' based-on links must point to 
the same format layer. If EFalse, the based-on links are not used in the 
comparison. By default, ETrue. 
@return ETrue if the two layers are identical, otherwise EFalse. */
	{
	if (aCheckBasedOnLink)
		{
		if (iBasedOn!=aLayer->SenseBase())
			return EFalse;
		}
	TInt size;
	const TUint8* ptr=((CCharFormatLayer*)aLayer)->Ptr(size);		// some naff design at work here!
	return CFormatLayer::IsIdentical(ptr,size);
	}

EXPORT_C TBool CCharFormatLayer::IsIdentical(const TCharFormat& aCharFormat,const TCharFormatMask& aMask)const
/** Compares a format attribute container with the current layer. For the two 
objects to be identical, the current layer must contain only the attributes 
specified in the argument aMask, and these attributes must have the same 
values as those in aCharFormat. None of the current layer's based-on links are 
searched.

@param aCharFormat Contains the attribute values used in the comparison. 
@param aMask A bitmask specifying which attributes are relevant to the function. 

@return ETrue if the formatting of the current layer exactly matches that 
contained in aCharFormat, otherwise EFalse. */
	{
	TCharFormat thisCharFormat;
	TCharFormatMask thisCharFormatMask;
	Sense(thisCharFormat,thisCharFormatMask);
	if (thisCharFormatMask!=aMask)
		return EFalse;
	return thisCharFormat.IsEqual(aCharFormat,aMask);
	}


CFormatLayer* CCharFormatLayer::RestoreNewL(RReadStream& aStream)
/** Return a new CCharFormatLayer, having restored it from aStream.
Overrides the base class method, to provide a new format layer of 
the correct type.*/
	{return CCharFormatLayer::NewL(aStream);}


void CCharFormatLayer::FillCharFormat(TCharFormatX& aCharFormat,TCharFormatXMask& aMask)const
/** Fills aCharFormat by dumping the current format layer into it, then follows 
each 'BasedOn' link in turn if it is not null.  It is assumed that all based on 
links eventually terminate with nulls.*/
	{
	iStore.SenseCharFormat(aCharFormat,aMask);
	if (iBasedOn)
		((CCharFormatLayer*)iBasedOn)->FillCharFormat(aCharFormat,aMask);
	}
