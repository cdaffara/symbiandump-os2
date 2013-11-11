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


#ifndef __TXTFMLYR_H__
#define __TXTFMLYR_H__

#include <e32std.h>
#include <e32base.h>
#include <txtfmstm.h>
#include <txtfrmat.h>

// forward declarations
class CFormatStream;  
class RReadStream;
class RWriteStream;
class TCharFormatX;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtfmlyr_internal.h>
#endif

/** 
Abstract base class for the paragraph and character format layers.

A format layer is a set of character or paragraph format attributes which 
may own a pointer to another format layer. This pointer is called a based-on 
link. The effective formatting of a text object may be built up from a chain 
of format layers - the final layer in the chain has a NULL based-on link. 
In case of conflict, attribute values set in upper layers (layers furthest 
from the layer with the NULL based-on link) override those set in lower layers. 
In rich text, additional formatting may be applied on top of these format 
layers.

The system of based-on links is implemented by the CFormatLayer class. It 
also implements persistence for chains of format layers.

When setting or sensing the attributes of a CParaFormatLayer or CCharFormatLayer, 
a format mask and container are specified as parameters. The container has 
data members for every format attribute, which may be set independently. When 
setting the layer, the mask indicates the attributes which will be taken from 
the container. Any attributes not specified in the mask will be taken from 
the system-provided default values.

When sensing a layer, on return, the mask indicates which attributes have 
been explicitly set in the layer, (i.e. not taken from the default values). 
In addition, a layer's effective format may be sensed. In this case, no 
mask is used because the format container will, on return, contain a value 
for every attribute. 
@publishedAll
@released
*/
class CFormatLayer : public CBase
	{
public:
	//
	// Based-on link utilities
	IMPORT_C void Reset();  // Remove all contents of this layer in a leave safe manner.
	IMPORT_C void SetBase(const CFormatLayer* aBaseFormatLayer);  // Set this layer to be based on the specified layer.
	IMPORT_C const CFormatLayer* SenseBase()const;
	IMPORT_C TInt ChainCount()const;  // Returns the number of format layers in the chain, inclusive of itself.
	//
	// Persistence
	

	/** Implementations of this function internalise the format layer but not its based-on 
	link from a read stream. The presence of this function means that the standard 
	templated operator>>() (defined in s32strm.h) is available to internalise 
	objects of the derived class. The internalised layer is set to be based on 
	the layer specified.
	
	@param aStream Stream from which the format layer should be internalised. 
	@param aBase The based-on link to assign to the layer. By default, NULL. */
	virtual void InternalizeL(RReadStream& aStream,const CFormatLayer* aBase=NULL)=0;
	
 
	/** Implementations of this function externalise the format layer but not its based-on 
	link to a write stream. The presence of this function means that the standard 
	templatedoperator<<() (defined in s32strm.h) is available to externalise objects 
	of the derived class.
	
	@param aStream Stream to which the format layer should be externalised. */
	virtual void ExternalizeL(RWriteStream& aStream)const=0;
	//
	//	Restore a format chain where the end of the chain is based on aBase (where aBase may be null).
	IMPORT_C void InternalizeChainL(RReadStream& aStream,const CFormatLayer* aBase=NULL);
	//	Stores a format layer chain of length length-aExcludeCount (or by default the whole chain).
	IMPORT_C void ExternalizeChainL(RWriteStream& aStream,TInt aExcludeCount=0)const;
	/** Implementations of this function compare another format layer with the current 
	object. For the two layers to be equal, they must have the same contents and 
	(if the second parameter is ETrue), their based-on links must point to the 
	same format layer.
	
	@param aLayer The layer to compare to this format layer. 
	@param aCheckBasedOnLink If ETrue, both layers' based-on links must point to 
	the same format layer. If EFalse, the based-on links are not used in the comparison. 
	By default, ETrue. 
	@return ETrue if the two layers are identical, otherwise EFalse. */
	virtual TBool IsIdentical(CFormatLayer* aLayer,TBool aCheckBasedOnLink=ETrue)const=0;
	IMPORT_C TBool IsEmpty()const;
	void Swap(CFormatLayer& aLayer);
private:
	CFormatLayer(const CFormatLayer& aFormatLayer);
	CFormatLayer& operator=(const CFormatLayer& aFormatLayer);
	virtual CFormatLayer* DoCloneL()const=0;
protected:
	CFormatLayer();
	~CFormatLayer();
	virtual CFormatLayer* RestoreNewL(RReadStream& aStream)=0;
	void ExternalizeLayersRecurseL(RWriteStream& aStream,TInt aDescendantCount)const;
	TBool IsIdentical(const TUint8* aPtr,TInt aSize)const;
	const TUint8* Ptr(TInt& aSize)const;
	void CloneLayerL(CFormatLayer* aClone)const;
protected:
	RFormatStream iStore;
	const CFormatLayer* iBasedOn;  // If non-null used to inherit format attributes from the lower layer.
	__DECLARE_TEST;
	};


/** 
A paragraph format layer. 

Has a pointer (stored in its base class CFormatLayer) to another paragraph 
format layer which may be NULL. This pointer is referred to as the based-on 
link. A paragraph format layer is owned by an instance of the CGlobalText 
class and stores the object's global paragraph formatting. Implements persistence 
and allows attributes to be set and sensed. 
@publishedAll
@released
*/
class CParaFormatLayer : public CFormatLayer
	{
public:
	IMPORT_C static CParaFormatLayer* NewL();
	IMPORT_C static CParaFormatLayer* NewL(const CParaFormat* aParaFormat,const TParaFormatMask& aMask);
	IMPORT_C static CParaFormatLayer* NewL(RReadStream& aStream);
	static CParaFormatLayer* NewL(const CParaFormatLayer* aLayer);
	static CParaFormatLayer* NewCopyBaseL(const CParaFormatLayer* aLayer);
	// Create a new instance, restoring it from the specified stream.
	// The based on link is NULL. Restores only one layer.
	//
	// Persistence
	IMPORT_C virtual void InternalizeL(RReadStream& aStream,const CFormatLayer* aBase=NULL);	
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream)const;
	//
	// Core methods
	IMPORT_C void SetL(const CParaFormat* aDesiredEffectiveFormat,const TParaFormatMask& aMask);
	IMPORT_C void SenseEffectiveL(CParaFormat* aParaFormat,CParaFormat::TParaFormatGetMode aMode=CParaFormat::EAllAttributes)const;
	IMPORT_C void SenseL(CParaFormat* aParaFormat,TParaFormatMask& aMask,CParaFormat::TParaFormatGetMode aMode=CParaFormat::EAllAttributes)const;
	//
	// Utilities
	inline CParaFormatLayer* CloneL()const;
	IMPORT_C TBool IsIdenticalL(const CParaFormat* aParaFormat,const TParaFormatMask& aMask)const;
	IMPORT_C virtual TBool IsIdentical(CFormatLayer* aLayer,TBool aCheckBasedOnLink=ETrue)const;
	IMPORT_C virtual TUid Type()const;
	IMPORT_C const TUint8* Ptr(TInt& aSize)const;
private:
	//
	// No implementation provided
	CParaFormatLayer(const CParaFormatLayer& aParaFormatLayer);
	CParaFormatLayer& operator=(const CParaFormatLayer& aParaFormatLayer);
	IMPORT_C virtual CFormatLayer* DoCloneL()const;
	void FillParaFormatL(CParaFormat* aParaFormat,TParaFormatMask& aMask,CParaFormat::TParaFormatGetMode aMode)const;
	void CleanupEffectiveFormat(CParaFormat* aParaFormat,TParaFormatMask aMask)const;
	void CleanupBorders(CParaFormat* aParaFormat)const;
protected:
	CParaFormatLayer();
	virtual CFormatLayer* RestoreNewL(RReadStream& aStream);
	};

/** 
Character format layer.

Uses a pointer (stored in its base class CFormatLayer) to another character 
format layer which may be NULL. This pointer is referred to as the based-on 
link. A character format layer is owned by an instance of the CGlobalText 
class and stores the object's global character formatting. Implements persistence 
and allows attributes to be set and sensed. 
@publishedAll
@released
*/
class CCharFormatLayer : public CFormatLayer
	{
public:
	IMPORT_C static CCharFormatLayer* NewL();
	IMPORT_C static CCharFormatLayer* NewL(const TCharFormat& aFormat,const TCharFormatMask& aMask);
	IMPORT_C static CCharFormatLayer* NewL(RReadStream& aStream);
	IMPORT_C virtual void InternalizeL(RReadStream& aStream,const CFormatLayer* aBase=NULL);	
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream)const;
	IMPORT_C void SetL(const TCharFormat& aCharFormat,const TCharFormatMask& aMask);
	IMPORT_C void SenseEffective(TCharFormat& aCharFormat)const;
	IMPORT_C void Sense(TCharFormat& aCharFormat,TCharFormatMask& aMask)const;
	inline CCharFormatLayer* CloneL()const;
	IMPORT_C virtual TBool IsIdentical(CFormatLayer* aLayer,TBool aCheckBasedOnLink=ETrue)const;
	IMPORT_C TBool IsIdentical(const TCharFormat& aCharFormat,const TCharFormatMask& aMask)const;
	IMPORT_C const TUint8* Ptr(TInt& aSize)const;

	// non-exported public functions
	static CCharFormatLayer* NewL(const CCharFormatLayer* aLayer);
	static CCharFormatLayer* NewCopyBaseL(const CCharFormatLayer* aLayer);
	static CCharFormatLayer* NewL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask);
	void SetL(const TCharFormatX& aCharFormat,const TCharFormatXMask& aMask);
	void SenseEffective(TCharFormatX& aCharFormat)const;
	void Sense(TCharFormatX& aCharFormat,TCharFormatXMask& aMask) const;
	
private:
	CCharFormatLayer();
	virtual CFormatLayer* RestoreNewL(RReadStream& aStream);
	virtual void FillCharFormat(TCharFormatX& aCharFormat,TCharFormatXMask& aMask)const;
	IMPORT_C virtual CFormatLayer* DoCloneL()const;
	};


#include <txtfmlyr.inl>


#endif
