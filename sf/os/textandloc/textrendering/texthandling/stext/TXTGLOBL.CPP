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
#include <gdi.h>

#include "TXTFMLYR.H"
#include "TXTGLOBL.H"

#include "TXTSTD.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTGLOBLTraces.h"
#endif


/**
@internalAll
*/
EXPORT_C void MLayDoc::MLayDoc_Reserved_1() {}
EXPORT_C void CGlobalText::__DbgTestInvariant()const
// Class Invariants.
//
	{
#ifdef _DEBUG
// ASSERT: The global format layers are never null.
	__ASSERT_DEBUG(iGlobalParaFormatLayer!=NULL,User::Invariant());
	__ASSERT_DEBUG(iGlobalCharFormatLayer!=NULL,User::Invariant());
#endif
	}


EXPORT_C CGlobalText* CGlobalText::NewL(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
										TDocumentStorage aStorage,TInt aDefaultTextGranularity)
/** Allocates and constructs an empty global text object with a paragraph and a 
character format layer. A single end-of-document delimiter is inserted.

@param aGlobalParaLayer Pointer to the paragraph format layer referenced by 
the text object. Must not be NULL, or a panic occurs. 
@param aGlobalCharLayer Pointer to the character format layer referenced by 
the text object. Must not be NULL, or a panic occurs. 
@param aStorage The type of in-memory buffer to use. Defaults to 
ESegmentedStorage. 
@param aDefaultTextGranularity Specifies the granularity of the in-memory buffer. 
Default is EDefaultTextGranularity bytes (=256). 
@return Pointer to the global text object. */
	{
	if (aGlobalParaLayer==NULL)
	    {
	    OstTrace0( TRACE_FATAL, CGLOBALTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalParaLayer!=NULL,Panic(ENullFormatLayerHandle));
	if (aGlobalCharLayer==NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CGLOBALTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalCharLayer!=NULL,Panic(ENullFormatLayerHandle));

	CGlobalText* self=new(ELeave) CGlobalText(aGlobalParaLayer,aGlobalCharLayer);
	CleanupStack::PushL(self);
	self->ConstructL(aStorage,aDefaultTextGranularity);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CGlobalText* CGlobalText::NewL(const CStreamStore& aStore,TStreamId aStreamId,
										const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
										MTextFieldFactory* aFactory,
										TDocumentStorage aStorage)
/** Returns a handle to a new instance of this class, whose textual content is 
restored from the specified read-stream.  The global text object *uses* (does 
not own) the supplied global format layers.*/
	{
	if (aGlobalParaLayer==NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CGLOBALTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalParaLayer!=NULL,Panic(ENullFormatLayerHandle));
	if (aGlobalCharLayer==NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP3_CGLOBALTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalCharLayer!=NULL,Panic(ENullFormatLayerHandle));

	CGlobalText* self=new(ELeave) CGlobalText(aGlobalParaLayer,aGlobalCharLayer);
	CleanupStack::PushL(self);
	self->ConstructL(aStore,aStreamId,aFactory,aStorage);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CGlobalText::CGlobalText()
	{}


EXPORT_C CGlobalText::CGlobalText(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer):
	iGlobalParaFormatLayer(aGlobalParaLayer),
	iGlobalCharFormatLayer(aGlobalCharLayer)
	{
	__TEST_INVARIANT;
	}

EXPORT_C CGlobalText::~CGlobalText()
/** The destructor is empty, and is present only to cause the virtual function 
table to be defined in a unique module. */
	{}

EXPORT_C void CGlobalText::SetGlobalParaFormat(const CParaFormatLayer* aParaFormatLayer)
/** Replaces the paragraph format layer referenced by the global text object.

@param aParaFormatLayer Pointer to the paragraph format layer to be referenced 
by the global text object. */
	{
	iGlobalParaFormatLayer=aParaFormatLayer;
	CEditableText::SetHasChanged(ETrue);
	
	__TEST_INVARIANT;
	}

EXPORT_C void CGlobalText::SetGlobalCharFormat(const CCharFormatLayer* aCharFormatLayer)
/** Replaces the character format layer referenced by the global text object.

@param aCharFormatLayer Pointer to the character format layer to be referenced 
by the global text object. */
	{
	iGlobalCharFormatLayer=aCharFormatLayer;
	CEditableText::SetHasChanged(ETrue);
	
	__TEST_INVARIANT;
	}

 
EXPORT_C void CGlobalText::ApplyParaFormatL(const CParaFormat* aFormat,const TParaFormatMask& aMask,
											TInt /*aPos*/,TInt /*aLength*/)
/** Changes the text object's paragraph formatting. The attributes which are 
set in the mask are read from aFormat into the text object's paragraph format 
layer. The attributes which are not set in the mask are not changed. Note 
that the position and length arguments are only used in the rich text 
implementation of this function.

@param aFormat Contains the paragraph format attribute values to apply. 
@param aMask Bitmask specifying the paragraph format attributes to change. 
@param aPos This argument is not used for global text. 
@param aLength This argument is not used for global text. */
	{
 
	__TEST_INVARIANT;

	TParaFormatMask applyMask=aMask;
	CParaFormat* pf=CParaFormat::NewL(*aFormat);
	CleanupStack::PushL(pf);
	iGlobalParaFormatLayer->SenseL(pf,applyMask);
	CONST_CAST(CParaFormatLayer*,iGlobalParaFormatLayer)->SetL(pf,applyMask);
	CleanupStack::PopAndDestroy();  // pf
	CEditableText::SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}

EXPORT_C void CGlobalText::ApplyCharFormatL(const TCharFormat& aFormat,const TCharFormatMask& aMask,
											TInt /*aPos*/,TInt /*aLength*/)
/** Changes the text object's character formatting. The attributes which are 
set in the mask are read from aFormat into the text object's character format 
layer. The attributes which are not set in the mask are not changed. Note 
that the position and length arguments are only used in the rich text 
implementation of this function.

@param aFormat Contains the character format attribute values to apply. 
@param aMask Bitmask specifying the character format attributes to change. 
@param aPos This argument is not used for global text. 
@param aLength This argument is not used for global text. */
	{

	__TEST_INVARIANT;

	TCharFormatMask applyMask=aMask;
	TCharFormat cf(aFormat);
	iGlobalCharFormatLayer->Sense(cf,applyMask);
	(CONST_CAST(CCharFormatLayer*,iGlobalCharFormatLayer))->SetL(cf,applyMask);
	CEditableText::SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}

EXPORT_C TInt CGlobalText::LdDocumentLength()const
/** Gets the the number of characters in the document.

Note: the count includes all non-printing characters but excludes the end 
of text paragraph delimiter, so that the smallest return value is always zero.

@return The number of characters contained in the document. */
   {
   __TEST_INVARIANT;
   
   return CPlainText::DocumentLength();
   }

EXPORT_C TInt CGlobalText::LdToParagraphStart(TInt& aCurrentPos)const
/** Gets the document position of the start of the paragraph containing a 
specified document position.

@param aCurrentPos Specifies a document position: must be valid or a panic 
occurs. On return, specifies the document position of the first character 
in the paragraph in which it is located. 
@return The number of characters skipped in scanning to the start of the 
paragraph. */
	{
	__TEST_INVARIANT;

	return CPlainText::ToParagraphStart(aCurrentPos);
	}

EXPORT_C TBool CGlobalText::EnquirePageBreak(TInt aPos,TInt aLength)const
/** Tests whether a page break occurs within a range of characters. Returns 
false if no page table has been set up: see CPlainText::SetPageTable(). The 
start and end of the range must be valid document positions, or a panic occurs.

@param aPos The document position from which to begin searching for a page 
break. 
@param aLength The number of characters to search for a page break, beginning 
at aPos. The default is zero. 
@return ETrue if a page break occurs within the specified range, otherwise 
EFalse. */
	{
	__TEST_INVARIANT;

	return PageContainingPos(aPos+aLength)>PageContainingPos(aPos);
	}

EXPORT_C void CGlobalText::GetChars(TPtrC& aView,TCharFormat& aFormat,TInt aStartPos)const
/** Gets a constant pointer descriptor to a portion of the text object. The 
portion starts at document position aStartPos, and ends at the end of the 
document, or the end of the segment, if segmented storage is being used. Also 
fills a character format object with the text object's effective character 
formatting. The start position must be valid, or a panic occurs.

@param aView On return, a constant pointer to a portion of the text. 
@param aFormat On return, contains the text object's effective character 
formatting. 
@param aStartPos The start position for the view. */
	{
	if (aStartPos<0 || aStartPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CGLOBALTEXT_GETCHARS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aStartPos>=0 && aStartPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	aView.Set(Read(aStartPos));
	iGlobalCharFormatLayer->SenseEffective(aFormat);
	OverrideFormatOfInlineTextIfApplicable(aView,aFormat,aStartPos);
	}

EXPORT_C void CGlobalText::GetParagraphFormatL(CParaFormat* aFormat,TInt /*aPos*/) const
/** Gets the text object's effective paragraph formatting. The aPos value is 
only used in the rich text implementation of this function.

@param aFormat On return, filled with the text object's effective paragraph 
formatting. 
@param aPos This argument is not used for global text. */
	{
	// Get the paragraph format. Global text has a single unvarying character format so aPos is ignored.
	iGlobalParaFormatLayer->SenseEffectiveL(aFormat);
	}

 
EXPORT_C void CGlobalText::GetParaFormatL(CParaFormat* aFormat,TParaFormatMask& aVaries,TInt /*aPos*/,TInt /*aLength*/,
										  CParaFormat::TParaFormatGetMode aMode) const
/** Gets the the global text object's effective paragraph formatting. Note that 
the position and length arguments are only used in the rich text implementation 
of this function.

@param aFormat Must not be NULL or a panic occurs. On return, contains the 
effective paragraph formatting for the global text object. 
@param aVaries On return, a bitmask indicating which paragraph format attributes 
vary over the range of characters selected. This is only relevant for rich 
text, so for global text returns a value of zero for all attributes. 
@param aPos This argument is not used for global text. 
@param aLength This argument is not used for global text. 
@param aMode The default, EAllAttributes indicates that values for all paragraph 
format attributes are written to aFormat. EFixedAttributes indicates that 
tabs, bullets and borders are not written to aFormat. */
	{
	iGlobalParaFormatLayer->SenseEffectiveL(aFormat,aMode);
	aVaries.ClearAll();
	}



EXPORT_C void CGlobalText::GetCharFormat(TCharFormat& aFormat,TCharFormatMask& aVaries,
										 TInt /*aPos*/,TInt /*aLength*/) const
/** Gets the global text object's effective character formatting. Note that the 
last three arguments are not relevant to the global text implementation of 
this function.

@param aFormat On return, contains the effective character formatting for 
the global text object. 
@param aVaries On return, a bitmask indicating which character format attributes 
vary over the range of characters selected. This is only relevant for rich 
text, so for global text returns a value of zero for all attributes. 
@param aPos This argument is not used for global text. 
@param aLength This argument is not used for global text. */
	{
	iGlobalCharFormatLayer->SenseEffective(aFormat);
	aVaries.ClearAll();
	}

EXPORT_C CPicture* CGlobalText::PictureHandleL(TInt /*aPos*/,MLayDoc::TForcePictureLoad /*aForceLoad*/) const
/**  Global text provides no support for pictures, so this implementation
of the function returns NULL. */
	{
	// Global text provides no support for pictures.
	return NULL;
	}

EXPORT_C TInt CGlobalText::GetPictureSizeInTwips(TSize& /*aSize*/, TInt /*aPos*/) const
/** Global text provides no support for pictures, so this implementation
of the function returns KErrNotFound. */	
	{
	// Global text provides no support for pictures.
	return KErrNotFound;
	}

EXPORT_C TBool CGlobalText::SelectParagraphLabel(TInt /*aPos*/)
/** Global text does not support paragraph labels, so this function
returns EFalse. */	
    {
	return EFalse;
	}

EXPORT_C void CGlobalText::CancelSelectLabel()
/** Not supported. */
	{
	}
