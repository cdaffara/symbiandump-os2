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

#include <s32strm.h>
#include <s32mem.h>
#include "TXTFRMAT.H"
#include <txtfmstm.h>

#include "TXTSTD.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTFMSTMTraces.h"
#endif


const TInt KMaxFormatStreamLength=0x400;  // 1024 bytes

// Standard attributes
const TUint KFontProportional=0x01;
const TUint KFontSerif=0x02;
const TUint KFontSymbol=0x04;

const TUint KTypefaceFlags=sizeof(TUint8);
const TInt KTypeSize=sizeof(TUint8);
const TInt KColor=sizeof(TUint8)+sizeof(TUint8)+sizeof(TUint8);
const TInt KFontHeight=sizeof(TInt32);
const TInt KParaBorderThickness=sizeof(TInt32);
const TInt KTabPosition=sizeof(TUint32);
const TInt KTabType=sizeof(TUint8);

// Paragraph format attributes
const TInt KVariableLengthAttribute=0;
const TInt KParaLanguage=sizeof(TUint8);
const TInt KParaFillColor=KColor;
const TInt KParaLeftMargin=sizeof(TInt32);
const TInt KParaRightMargin=sizeof(TInt32);
const TInt KParaIndent=sizeof(TInt32);
const TInt KParaAlignment=sizeof(TUint8);
const TInt KParaVerticalAlignment=sizeof(TUint8);
const TInt KParaLineSpacing=sizeof(TInt32);
const TInt KParaLineSpacingControl=sizeof(TUint8);
const TInt KParaSpaceBefore=sizeof(TInt32);
const TInt KParaSpaceAfter=sizeof(TInt32);
const TInt KParaKeepTogether=sizeof(TUint8);
const TInt KParaKeepWithNext=sizeof(TUint8);
const TInt KParaStartNewPage=sizeof(TUint8);
const TInt KParaWidowOrphan=sizeof(TUint8);
const TInt KParaWrap=sizeof(TUint8);
const TInt KParaBorderMargin=sizeof(TInt32);
const TInt KParaTopBorder=sizeof(TUint8)+KParaBorderThickness+KColor+sizeof(TUint8);//linestyle/thickness/color/autocolor
const TInt KParaBottomBorder=KParaTopBorder;
const TInt KParaLeftBorder=KParaTopBorder;
const TInt KParaRightBorder=KParaTopBorder;
const TInt KParaBullet=KVariableLengthAttribute;
const TInt KParaDefaultTabWidth=sizeof(TUint32);
const TInt KParaTabStop=KTabPosition+KTabType;
const TInt KParaFillSystemColor=sizeof(TUint8);
const TInt KParaBulletSystemColor=sizeof(TUint8);
const TInt KParaTopBorderSystemColor=sizeof(TUint8);
const TInt KParaBottomBorderSystemColor=sizeof(TUint8);
const TInt KParaLeftBorderSystemColor=sizeof(TUint8);
const TInt KParaRightBorderSystemColor=sizeof(TUint8);
const TInt KParaLanguageX=sizeof(TInt32);
const TInt KParaBulletX=sizeof(TInt32) + 2;
const TInt KBitmapType=sizeof(TUint8);

// Character format attributes
const TInt KCharLanguage=sizeof(TUint8);
const TInt KCharColor=KColor;
const TInt KCharHighlightColor=KColor;
const TInt KCharHighlightStyle=sizeof(TUint8);
const TInt KCharStrikethrough=sizeof(TUint8);
const TInt KCharUnderline=sizeof(TUint8);
const TInt KCharStrokeWeight=sizeof(TUint8);
const TInt KCharPosture=sizeof(TUint8);
const TInt KCharPrintPos=sizeof(TUint8);
const TInt KCharFontHeight=KFontHeight;
const TInt KCharTypeface=KVariableLengthAttribute;
const TInt KCharHiddenText=sizeof(TUint8);
const TInt KCharPictureAlignment=sizeof(TUint8);
const TInt KCharTextSystemColor=sizeof(TUint8);
const TInt KCharFontHighlightSystemColor=sizeof(TUint8);
const TInt KCharLanguageX=sizeof(TInt32);
const TInt KCharParserTag=sizeof(TUint32);

/*
Lookup table indexed by TTextFormatAttribute enumerated constants.
Specifies the length in bytes of the format attributes.
*/
static const TInt8 TheAttributeLength[EAttributeCount] =
	{
	// Paragraph attribute value lengths.	
	KParaLanguage,
	KParaFillColor,
	KParaLeftMargin,
	KParaRightMargin,
	KParaIndent,
	KParaAlignment,
	KParaVerticalAlignment,
	KParaLineSpacing,
	KParaLineSpacingControl,
	KParaSpaceBefore,
	KParaSpaceAfter,
	KParaKeepTogether,
	KParaKeepWithNext,
	KParaStartNewPage,
	KParaWidowOrphan,
	KParaWrap,
	KParaBorderMargin,
	KParaTopBorder,
	KParaBottomBorder,
	KParaLeftBorder,
	KParaRightBorder,
	KParaBullet,
	KParaDefaultTabWidth,
	KParaTabStop,

	// Character attribute value lengths.
	KCharLanguage,
	KCharColor,
	KCharHighlightColor,
	KCharHighlightStyle,
	KCharFontHeight,
	KCharStrikethrough,
	KCharUnderline,
	KCharStrokeWeight,
	KCharPosture,
	KCharPrintPos,
	KCharTypeface,
	KCharHiddenText,
	KCharPictureAlignment,

	// Lengths of extended attributes.
	KParaFillSystemColor,
	KParaBulletSystemColor,
	KParaTopBorderSystemColor,
	KParaBottomBorderSystemColor,
	KParaLeftBorderSystemColor,
	KParaRightBorderSystemColor,
	KCharTextSystemColor,
	KCharFontHighlightSystemColor,
	KParaLanguageX,
	KCharLanguageX,
	KParaBulletX,
    KBitmapType,

	// Lengths of internal attributes
	KCharParserTag
	};


DLLEXPORT_C void RFormatStream::__DbgTestInvariant()const
// Provides class invariants.
// This class invariant checks the integrity of a completed format stream.
// As such, this invariant is only called by those methods that act upon a completed
// format stream; that is, not the set methods, since the format stream will not be complete until
// this call has completed.
//
	{
#ifdef _DEBUG
		__ASSERT_DEBUG(DoInvariantCheck(),User::Invariant());
#endif
	}

void RFormatStream::TestInvariantL()const
// Provides class invariants.
// This class invariant checks the integrity of a completed format stream.
// As such, this invariant is only called by those methods that act upon a completed
// format stream; that is, not the set methods, since the format stream will not be complete until
// this call has completed.
//
	{
	if (!DoInvariantCheck())
		User::Leave(KErrCorrupt);
	}

TBool RFormatStream::DoInvariantCheck() const
// Provides class invariants.
// This class invariant checks the integrity of a completed format stream.
// As such, this invariant is only called by those methods that act upon a completed
// format stream; that is, not the set methods, since the format stream will not be complete until
// this call has completed.
//
	{
	if (!iBase)
		return ETrue;
	if (iEnd>iBase)
		{// Assert: stream is self consistent
		// Walks through the stream (aBuffer), checking that it is in a consistent state.
		// (1) All entries in the buffer conform to a TYPE-VALUE structure, checking that
		// types do not occur in contiguous bytes.  ie there is a value.
		// (2) Checks that all encountered types are valid types.
		// (3) Checks that the buffer has not changed size as a result of this check
		// (4) Checks that all attributes in the stream occur only once.  Done by taking a register
		// as each attribute is read from the stream.  (The exception to this rule are Tab identifiers
		// which, if present, will typically occur several times in the stream).
		//
		enum {ENotPresent,EPresent};
		TInt8 attributeRegister[EAttributeCount];
		for (TInt offset=0;offset<EAttributeCount;offset++)
			{// Mark attributeRegister as all absent
			attributeRegister[offset]=(TInt8)ENotPresent;  // clear all items in the register
			}
		TUint8* tempStreamLoc=iBase;
		TUint8* streamLoc=iBase;
		TUint8* endOfStreamLoc=iEnd;
		TUint8 data=0;
		while (streamLoc<endOfStreamLoc)
			{
			data=*streamLoc;
			streamLoc++;
// Assert: data read is a valid format attribute type.
			if (!(data<((TUint8)EAttributeCount))) return EFalse;
			if (data!=EAttTabStop)
				{// Tab Identifiers can occur multiple times in a stream!!
// Assert: attribute type does not already exist in attributeRegister.
				if (!(attributeRegister[data]!=(TInt8)EPresent)) return EFalse;
				}
			attributeRegister[data]=(TInt8)EPresent;
			tempStreamLoc=streamLoc+Length(streamLoc,(TTextFormatAttribute)data);
			if (!(tempStreamLoc>=streamLoc)) return EFalse;
			streamLoc=tempStreamLoc;
			}
// Assert: everything is still the same size.
		if (!(streamLoc==endOfStreamLoc)) return EFalse;
		}
	return ETrue;
	}

static inline TUint32 Read32(const TUint8* aPtr)
	{
	TUint32 val = aPtr[0];
	val |= aPtr[1] << 8;
	val |= aPtr[2] << 16;
	val |= aPtr[3] << 24;
	return val;
	}


static inline void Write32(TUint8*& aPtr,TUint32 aVal)
	{
	*aPtr++ = TUint8(aVal);
	*aPtr++ = TUint8(aVal >> 8);
	*aPtr++ = TUint8(aVal >> 16);
	*aPtr++ = TUint8(aVal >> 24);
	}


RFormatStream::RFormatStream():
	iBase(NULL),
	iEnd(NULL)
	{
	}

		
// Allocate the buffer
void RFormatStream::AllocL(TInt aSize)
	{
    TUint8* pT = reinterpret_cast<TUint8*>(User::ReAllocL(iBase, aSize));
    
    iBase=pT;
    iEnd=pT+aSize;
	}


void RFormatStream::Reset()
// Free all storage
//
	{
	if (iBase)
		{
		User::Free(iBase);
		iBase = iEnd = NULL;
		}
	}


void RFormatStream::CopyL(const RFormatStream& aSource)
	{
	TInt size = aSource.iEnd - aSource.iBase;
	if (size < 0)
	    {
	    OstTrace0( TRACE_DUMP, RFORMATSTREAM_COPYL, "EDebug" );
	    }
	__ASSERT_DEBUG(size >= 0,Panic(EDebug));
	if (size == 0)
		Reset();
	else
		{
		AllocL(size);
		Mem::Copy(iBase,aSource.iBase,size);
		}
	}


/*
Write the bytecode to a stream. Don't write internal attributes; these are not part of the stored format,
but are used for transitory marking of text by URL parsers, etc.
*/
void RFormatStream::ExternalizeL(RWriteStream& aStream) const
	{
	__TEST_INVARIANT;
	
	const TUint8* base=iBase;
	if (base)
		{
		const TUint8* end = base;
		while (end < iEnd && *end < EExternalizedAttributeCount)
			{
			int bytes = TheAttributeLength[*end++];
			if (bytes == 0)
				bytes = *end++;
			end += bytes;
			}
		int length = end - base;
		aStream.WriteInt32L(length);
		aStream.WriteL(base,length);
		}
	else
		aStream.WriteInt32L(0);
	}


void RFormatStream::InternalizeL(RReadStream& aStream)
// Load the buffer from the specified stream
//
	{
	TestInvariantL();

	TInt length=aStream.ReadInt32L();
	//
	if (length<0 || length>KMaxFormatStreamLength)
		User::Leave(KErrCorrupt);
	//
	if (length==0)
		Reset();
	else
		{
		AllocL(length);
		aStream.ReadL(iBase,length);
		}

	TestInvariantL();
	}


// Return a pointer to the stored bytecode and put its length in bytes in aLength.
const TUint8* RFormatStream::Ptr(TInt& aLength) const
	{
	__TEST_INVARIANT;
	aLength=iEnd-iBase;
	if (!((iBase==NULL && aLength==0) || (iBase!=NULL && aLength>0)))
	    {
	    OstTrace0( TRACE_DUMP, RFORMATSTREAM_PTR, "ECorruptFormatLayer" );
	    }
	__ASSERT_DEBUG((iBase==NULL && aLength==0) || (iBase!=NULL && aLength>0),Panic(ECorruptFormatLayer));
	return iBase;
	}


// Save the attributes of aCharFormat specified by the corresponding mask aMask.
void RFormatStream::SetCharFormatL(const TCharFormatX& aCharFormatX,const TCharFormatXMask& aMask)
	{
	TInt size=DoCalcSizeCharFormat(aCharFormatX,aMask);
	if (size==0)
		Reset();
	else
		{
		AllocL(size);  // delete the current contents, after allocing a temporary
		DoStoreCharFormat(aCharFormatX,aMask);
		}

	__TEST_INVARIANT;
	}


void RFormatStream::SetParaFormatL(const CParaFormat& aDesiredFormat,const TParaFormatMask& aDesiredMask,
											const CParaFormat& aCurrentFormat)
// Sets the format layer with the specified paragraph format attributes.
// If a leave occurs at any stage, then revert back to original state, and
// propagate the leave.
//
	{
	TInt size=DoCalcSizeParaFormat(aDesiredFormat,aDesiredMask,aCurrentFormat);
	if (size==0)
		Reset();
	else
		{
		AllocL(size);  // delete the current contents, after allocing a temporary
		DoSetParaFormat(aDesiredFormat,aDesiredMask,aCurrentFormat);
		}

	__TEST_INVARIANT;
	}
	
// Write an auxiliary attribute for a system colour byte. Update aPtr.
static void WriteSystemColor(TUint8*& aPtr,TTextFormatNonMaskableAttribute aAttrib,const TLogicalRgb& aColor)
	{
	TUint8 index = (TUint8)aColor.SystemColorIndex();
	if (index)
		{
		*aPtr++ = TUint8(aAttrib);
		*aPtr++ = index;
		}
	}


// Read a system colour byte into a logical colour. Don't change aPtr.
static void ReadSystemColor(const TUint8* aPtr,TLogicalRgb& aColor)
	{
	TUint index = *aPtr;
	aColor.SetSystemColorIndex(index); 
	}


// Write paragraph attributes specified by aDesiredMask from aDesiredFormat to the stream.
void RFormatStream::DoSetParaFormat(const CParaFormat& aDesiredFormat,
									TParaFormatMask aMask,
									const CParaFormat& aCurrentFormat)
	{
	TUint8* ptr=iBase;
	if (aMask.AttribIsSet(EAttFillColor))
		{
		*ptr++=TUint8(EAttFillColor);
		ptr=Store(ptr,aDesiredFormat.iFillColor);
		}
	if (aMask.AttribIsSet(EAttLeftMargin))
		{
		*ptr++=TUint8(EAttLeftMargin);
		Write32(ptr,aDesiredFormat.iLeftMarginInTwips);
		}
	if (aMask.AttribIsSet(EAttRightMargin))
		{
		*ptr++=TUint8(EAttRightMargin);
		Write32(ptr,aDesiredFormat.iRightMarginInTwips);
		}
	if (aMask.AttribIsSet(EAttIndent))
		{
		*ptr++=TUint8(EAttIndent);
		Write32(ptr,aDesiredFormat.iIndentInTwips);
		}
	if (aMask.AttribIsSet(EAttAlignment))
		{
		*ptr++=TUint8(EAttAlignment);
		*ptr++=TUint8(aDesiredFormat.iHorizontalAlignment);
		}
	if (aMask.AttribIsSet(EAttVerticalAlignment))
		{
		*ptr++=TUint8(EAttVerticalAlignment);
		*ptr++=TUint8(aDesiredFormat.iVerticalAlignment);
		}
	if (aMask.AttribIsSet(EAttLineSpacing))
		{
		*ptr++=TUint8(EAttLineSpacing);
		Write32(ptr,aDesiredFormat.iLineSpacingInTwips);
		}
	if (aMask.AttribIsSet(EAttLineSpacingControl))
		{
		*ptr++=TUint8(EAttLineSpacingControl);
		*ptr++=TUint8(aDesiredFormat.iLineSpacingControl);
		}
	if (aMask.AttribIsSet(EAttSpaceBefore))
		{
		*ptr++=TUint8(EAttSpaceBefore);
		Write32(ptr,aDesiredFormat.iSpaceBeforeInTwips);
		}
	if (aMask.AttribIsSet(EAttSpaceAfter))
		{
		*ptr++=TUint8(EAttSpaceAfter);
		Write32(ptr,aDesiredFormat.iSpaceAfterInTwips);
		}
	if (aMask.AttribIsSet(EAttKeepTogether))
		{
		*ptr++=TUint8(EAttKeepTogether);
		*ptr++=TUint8(aDesiredFormat.iKeepTogether!=EFalse);
		}
	if (aMask.AttribIsSet(EAttKeepWithNext))
		{
		*ptr++=TUint8(EAttKeepWithNext);
		*ptr++=TUint8(aDesiredFormat.iKeepWithNext!=EFalse);
		}
	if (aMask.AttribIsSet(EAttStartNewPage))
		{
		*ptr++=TUint8(EAttStartNewPage);
		*ptr++=TUint8(aDesiredFormat.iStartNewPage!=EFalse);
		}
	if (aMask.AttribIsSet(EAttWidowOrphan))
		{
		*ptr++=TUint8(EAttWidowOrphan);
		*ptr++=TUint8(aDesiredFormat.iWidowOrphan!=EFalse);
		}
	if (aMask.AttribIsSet(EAttWrap))
		{
		*ptr++=TUint8(EAttWrap);
		*ptr++=TUint8(aDesiredFormat.iWrap!=EFalse);
		}
	if (aMask.AttribIsSet(EAttBorderMargin))
		{
		*ptr++=TUint8(EAttBorderMargin);
		Write32(ptr,aDesiredFormat.iBorderMarginInTwips);
		}
	if ( aDesiredFormat.BordersPresent() || aCurrentFormat.BordersPresent() )
		{
		if (aMask.AttribIsSet(EAttTopBorder))
			ptr=StoreBorder(ptr,EAttTopBorder,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderTop));
		if (aMask.AttribIsSet(EAttBottomBorder))
			ptr=StoreBorder(ptr,EAttBottomBorder,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderBottom));
		if (aMask.AttribIsSet(EAttLeftBorder))
			ptr=StoreBorder(ptr,EAttLeftBorder,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderLeft));
		if (aMask.AttribIsSet(EAttRightBorder))
			ptr=StoreBorder(ptr,EAttRightBorder,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderRight));
		}
	if (aMask.AttribIsSet(EAttDefaultTabWidth))
		{
		*ptr++=TUint8(EAttDefaultTabWidth);
		Write32(ptr,aDesiredFormat.iDefaultTabWidthInTwips);
		}
	if (aMask.AttribIsSet(EAttTabStop))
		{
		TUint8* tptr=ptr;	// prevent stacking of ptr;
		StoreTabs(tptr,aDesiredFormat,aCurrentFormat,ETrue);
		ptr=tptr;
		}
	if (aMask.AttribIsSet(EAttBullet))
		{
		if (aDesiredFormat.iBullet || aCurrentFormat.iBullet)
			{
			if (aDesiredFormat.iBullet)
				ptr = StoreBullet(ptr,*aDesiredFormat.iBullet);
			else if (aCurrentFormat.iBullet)
				ptr = StoreBullet(ptr,TBullet());
			}
		}
	if (!(aDesiredFormat.iLanguage & ~0xFF) && aMask.AttribIsSet(EAttParaLanguage))
		{
		*ptr++=TUint8(EAttParaLanguage);
		*ptr++=TUint8(aDesiredFormat.iLanguage);
		}

	/*
	Write the auxiliary attributes for system colours, language codes greater than 255, etc.
	These must go at the end of the stream because they will not be recognised by earlier versions
	of ETEXT and will prevent any further attributes from being read.
	*/
	if (aMask.AttribIsSet(EAttFillColor))
		WriteSystemColor(ptr,EAttFillSystemColor,aDesiredFormat.iFillColor);
	if (aMask.AttribIsSet(EAttBullet))
		{
		if (aDesiredFormat.iBullet)
			WriteSystemColor(ptr,EAttBulletSystemColor,aDesiredFormat.iBullet->iColor);
		else if (aCurrentFormat.iBullet)
			WriteSystemColor(ptr,EAttBulletSystemColor,TBullet().iColor);
		}
	if (aDesiredFormat.BordersPresent())
		{
		if (aMask.AttribIsSet(EAttTopBorder))
			WriteSystemColor(ptr,EAttTopBorderSystemColor,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderTop).iColor);
		if (aMask.AttribIsSet(EAttBottomBorder))
			WriteSystemColor(ptr,EAttBottomBorderSystemColor,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderBottom).iColor);
		if (aMask.AttribIsSet(EAttLeftBorder))
			WriteSystemColor(ptr,EAttLeftBorderSystemColor,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderLeft).iColor);
		if (aMask.AttribIsSet(EAttRightBorder))
			WriteSystemColor(ptr,EAttRightBorderSystemColor,aDesiredFormat.ParaBorder(CParaFormat::EParaBorderRight).iColor);
		}
	if ((aDesiredFormat.iLanguage & ~0xFF) && aMask.AttribIsSet(EAttParaLanguage))
		{
		*ptr++ = TUint8(EAttParaLanguageX);
		Write32(ptr,aDesiredFormat.iLanguage);
		}
	if (aMask.AttribIsSet(EAttBullet))
		{
		if (aDesiredFormat.iBullet || aCurrentFormat.iBullet)
			{
			TBullet bullet;
			const TBullet* b = aDesiredFormat.iBullet;
			if (!b)
				b = &bullet;
			*ptr++ = TUint8(EAttBulletX);
			*ptr++ = TUint8(b->iStyle);
			Write32(ptr,b->iStartNumber);
			*ptr++ = TUint8(b->iAlignment);
			}
		}
	}


TInt RFormatStream::DoCalcSizeParaFormat(const CParaFormat& aDesiredFormat,TParaFormatMask aMask,
										 const CParaFormat& aCurrentFormat)
// determine the amount of memory required to store the
// specified attriubutes from the specified paragraph format.
//
	{
	TInt size=0;
	if (aMask.AttribIsSet(EAttFillColor))
		{
		size+=(KTypeSize+KParaFillColor);
		if (aDesiredFormat.iFillColor.SystemColorIndex())
			size += KTypeSize + KParaFillSystemColor;
		}
	if (aMask.AttribIsSet(EAttLeftMargin))
		size+=(KTypeSize+KParaLeftMargin);
	if (aMask.AttribIsSet(EAttRightMargin))
		size+=(KTypeSize+KParaRightMargin);
	if (aMask.AttribIsSet(EAttIndent))
		size+=(KTypeSize+KParaIndent);
	if (aMask.AttribIsSet(EAttAlignment))
		size+=(KTypeSize+KParaAlignment);
	if (aMask.AttribIsSet(EAttVerticalAlignment))
		size+=(KTypeSize+KParaVerticalAlignment);
	if (aMask.AttribIsSet(EAttLineSpacing))
		size+=(KTypeSize+KParaLineSpacing);
	if (aMask.AttribIsSet(EAttLineSpacingControl))
		size+=(KTypeSize+KParaLineSpacingControl);
	if (aMask.AttribIsSet(EAttSpaceBefore))
		size+=(KTypeSize+KParaSpaceBefore);
	if (aMask.AttribIsSet(EAttSpaceAfter))
		size+=(KTypeSize+KParaSpaceAfter);
	if (aMask.AttribIsSet(EAttKeepTogether))
		size+=(KTypeSize+KParaKeepTogether);
	if (aMask.AttribIsSet(EAttKeepWithNext))
		size+=(KTypeSize+KParaKeepWithNext);
	if (aMask.AttribIsSet(EAttStartNewPage))
		size+=(KTypeSize+KParaStartNewPage);
	if (aMask.AttribIsSet(EAttWidowOrphan))
		size+=(KTypeSize+KParaWidowOrphan);
	if (aMask.AttribIsSet(EAttWrap))
		size+=(KTypeSize+KParaWrap);
	if (aMask.AttribIsSet(EAttBorderMargin))
		size+=(KTypeSize+KParaBorderMargin);
	if ( aDesiredFormat.BordersPresent() || aCurrentFormat.BordersPresent() )
		{
		if (aMask.AttribIsSet(EAttTopBorder))
			{
			size+=(KTypeSize+
			sizeof(TUint8)+  // line style
			sizeof(TInt32)+  // line thickness
			KColor+
			sizeof(TUint8));  // auto color flag
			if (aDesiredFormat.ParaBorder(CParaFormat::EParaBorderTop).iColor.SystemColorIndex())
				size += KTypeSize + KParaTopBorderSystemColor;
			}
		if (aMask.AttribIsSet(EAttBottomBorder))
			{
			size+=(KTypeSize+
			sizeof(TUint8)+  // line style
			sizeof(TInt32)+  // line thickness
			KColor+
			sizeof(TUint8));  // auto color flag
			if (aDesiredFormat.ParaBorder(CParaFormat::EParaBorderBottom).iColor.SystemColorIndex())
				size += KTypeSize + KParaBottomBorderSystemColor;
			}
		if (aMask.AttribIsSet(EAttLeftBorder))
			{
			size+=(KTypeSize+
			sizeof(TUint8)+  // line style
			sizeof(TInt32)+  // line thickness
			KColor+
			sizeof(TUint8));  // auto color flag
			if (aDesiredFormat.ParaBorder(CParaFormat::EParaBorderLeft).iColor.SystemColorIndex())
				size += KTypeSize + KParaLeftBorderSystemColor;
			}
		if (aMask.AttribIsSet(EAttRightBorder))
			{
			size+=(KTypeSize+
			sizeof(TUint8)+  // line style
			sizeof(TInt32)+  // line thickness
			KColor+
			sizeof(TUint8));  // auto color flag
			if (aDesiredFormat.ParaBorder(CParaFormat::EParaBorderRight).iColor.SystemColorIndex())
				size += KTypeSize + KParaBulletSystemColor;
			}
		}
	if (aMask.AttribIsSet(EAttDefaultTabWidth))
		size+=(KTypeSize+KParaDefaultTabWidth);
	if (aMask.AttribIsSet(EAttTabStop))
		{
		TUint8* ptr=NULL;
		size+=StoreTabs(ptr,aDesiredFormat,aCurrentFormat,EFalse);
		}
	if (aMask.AttribIsSet(EAttBullet))
		{
		if (aDesiredFormat.iBullet || aCurrentFormat.iBullet)
			{
			size += KTypeSize +
					sizeof(TUint8) +		// length of following data
					sizeof(TText) +			// iCharacterCode
					KFontHeight +			// iHeightInTwips
					sizeof(TUint8) +		// iHanging indent
					KCharColor +			// iColor
					sizeof(TUint8) +		// typeface name size
					KTypefaceFlags;			// typeface flags

			if (aDesiredFormat.iBullet)
				size += aDesiredFormat.iBullet->iTypeface.iName.Size();  // font name

			if ((aDesiredFormat.iBullet && aDesiredFormat.iBullet->iColor.SystemColorIndex()) ||
				(aCurrentFormat.iBullet))
				size += KTypeSize + KParaBulletSystemColor;

			size += KTypeSize + KParaBulletX;
			}
		}
	if (aMask.AttribIsSet(EAttParaLanguage))
		{
		if (aDesiredFormat.iLanguage & ~0xFF)
			size += KTypeSize + KParaLanguageX;
		else
			size += KTypeSize + KParaLanguage;
		}
	return size;
	}


// Store the specified values in this (allocated) format stream.
void RFormatStream::DoStoreCharFormat(const TCharFormatX& aCharFormat,TCharFormatXMask aMask)
	{
	TUint8* ptr = iBase;
	const TCharFormat format = aCharFormat.iCharFormat;

	if (aMask.AttribIsSet(EAttColor))
		{
		*ptr++=TUint8(EAttColor);
		ptr=Store(ptr,format.iFontPresentation.iTextColor);
		}
	if (aMask.AttribIsSet(EAttFontHighlightColor))
		{
		*ptr++=TUint8(EAttFontHighlightColor);
		ptr=Store(ptr,format.iFontPresentation.iHighlightColor);
		}
	if (aMask.AttribIsSet(EAttFontHighlightStyle))
		{
		*ptr++=TUint8(EAttFontHighlightStyle);
		*ptr++=(TUint8)format.iFontPresentation.iHighlightStyle;
		}
	if (aMask.AttribIsSet(EAttFontStrikethrough))
		{
		*ptr++=TUint8(EAttFontStrikethrough);
		*ptr++=(TUint8)format.iFontPresentation.iStrikethrough;
		}
	if (aMask.AttribIsSet(EAttFontUnderline))
		{
		*ptr++=TUint8(EAttFontUnderline);
		*ptr++=(TUint8)format.iFontPresentation.iUnderline;
		}
	if (aMask.AttribIsSet(EAttFontHeight))
		{
		*ptr++=TUint8(EAttFontHeight);
		Write32(ptr,format.iFontSpec.iHeight);
		}
	if (aMask.AttribIsSet(EAttFontPosture))
		{
		*ptr++=TUint8(EAttFontPosture);
		*ptr++=TUint8(format.iFontSpec.iFontStyle.Posture());
		}
	if (aMask.AttribIsSet(EAttFontStrokeWeight))
		{
		*ptr++=TUint8(EAttFontStrokeWeight);
		*ptr++=TUint8(format.iFontSpec.iFontStyle.StrokeWeight());
		}
	if (aMask.AttribIsSet(EAttFontPrintPos))
		{
		*ptr++=TUint8(EAttFontPrintPos);
		*ptr++=TUint8(format.iFontSpec.iFontStyle.PrintPosition());
		}
	if (aMask.AttribIsSet(EAttFontTypeface))
		{
		*ptr++=TUint8(EAttFontTypeface);
		ptr=Store(ptr,format.iFontSpec.iTypeface);
		*ptr++=TUint8(EAttBitmapType);
		*ptr++=TUint8(format.iFontSpec.iFontStyle.BitmapType());
		}
	if (!(format.iLanguage & ~0xFF) && aMask.AttribIsSet(EAttCharLanguage))
		{
		*ptr++=TUint8(EAttCharLanguage);
		*ptr++=TUint8(format.iLanguage);
		}
	if (aMask.AttribIsSet(EAttFontHiddenText))
		{
		*ptr++=TUint8(EAttFontHiddenText);
		*ptr++=TUint8(format.iFontPresentation.iHiddenText!=EFalse);
		}
	if (aMask.AttribIsSet(EAttFontPictureAlignment))
		{
		*ptr++=TUint8(EAttFontPictureAlignment);
		*ptr++=TUint8(format.iFontPresentation.iPictureAlignment);
		}

	// Auxiliary attributes for system colours, etc.
	if (aMask.AttribIsSet(EAttColor))
		WriteSystemColor(ptr,EAttTextSystemColor,format.iFontPresentation.iTextColor);
	if (aMask.AttribIsSet(EAttFontHighlightColor))
		WriteSystemColor(ptr,EAttFontHighlightSystemColor,format.iFontPresentation.iHighlightColor);
	if ((format.iLanguage & ~0xFF) && aMask.AttribIsSet(EAttCharLanguage))
		{
		*ptr++ = TUint8(EAttCharLanguageX);
		Write32(ptr,format.iLanguage);
		}

	// Internal character attributes:
	if (aMask.AttribIsSet(EAttParserTag))
		{
		*ptr++ = TUint8(EAttParserTag);
		Write32(ptr,aCharFormat.iParserTag);
		}
	}


TInt RFormatStream::DoCalcSizeCharFormat(const TCharFormatX& aCharFormatX,const TCharFormatXMask& aMask)
	{
	TInt size = 0;
	const TCharFormat format = aCharFormatX.iCharFormat;

	if (aMask.AttribIsSet(EAttColor))
		{
		size+=(KTypeSize+KCharColor);
		if (format.iFontPresentation.iTextColor.SystemColorIndex())
			size += KTypeSize + KCharTextSystemColor;
		}
	if (aMask.AttribIsSet(EAttFontHighlightColor))
		{
		size+=(KTypeSize+KCharHighlightColor);
		if (format.iFontPresentation.iHighlightColor.SystemColorIndex())
			size += KTypeSize + KCharFontHighlightSystemColor;
		}
	if (aMask.AttribIsSet(EAttFontHighlightStyle))
		size+=(KTypeSize+KCharHighlightStyle);
	if (aMask.AttribIsSet(EAttFontStrikethrough))
		size+=(KTypeSize+KCharStrikethrough);
	if (aMask.AttribIsSet(EAttFontUnderline))
		size+=(KTypeSize+KCharUnderline);
	if (aMask.AttribIsSet(EAttFontHeight))
		size+=(KTypeSize+KCharFontHeight);
	if (aMask.AttribIsSet(EAttFontPosture))
		size+=(KTypeSize+KCharPosture);
	if (aMask.AttribIsSet(EAttFontStrokeWeight))
		size+=(KTypeSize+KCharStrokeWeight);
	if (aMask.AttribIsSet(EAttFontPrintPos))
		size+=(KTypeSize+KCharPrintPos);
	if (aMask.AttribIsSet(EAttFontTypeface))
		{
		TUint8 length=(TUint8)format.iFontSpec.iTypeface.iName.Size();
		size+=(KTypeSize+sizeof(TUint8)+length+sizeof(TUint8));  // size in bytes
		size+=(KTypeSize+KBitmapType);
		}
	if (aMask.AttribIsSet(EAttCharLanguage))
		{
		if (format.iLanguage & ~0xFF)
			size += KTypeSize + KCharLanguageX;
		else
			size += KTypeSize + KCharLanguage;
		}
	if (aMask.AttribIsSet(EAttFontHiddenText))
		size+=(KTypeSize+KCharHiddenText);
	if (aMask.AttribIsSet(EAttFontPictureAlignment))
		size+=(KTypeSize+KCharPictureAlignment);

	// Internal character attributes
	if (aMask.AttribIsSet(EAttParserTag))
		size += KTypeSize + KCharParserTag;
	
	return size;
	}

void RFormatStream::RemoveRedundantCharFormat(TCharFormatMask& aMask,
											  const TCharFormatX& aFormat,const TCharFormatX& aEffectiveFormat)
	{
	TCharFormatXMask mask = aMask;
	const TCharFormat format = aFormat.iCharFormat;
	const TCharFormat effective_format = aEffectiveFormat.iCharFormat;

	if (mask.AttribIsSet(EAttColor))
		{
		if (format.iFontPresentation.iTextColor==effective_format.iFontPresentation.iTextColor)
			mask.ClearAttrib(EAttColor);
		}
	if (mask.AttribIsSet(EAttFontHighlightColor))
		{
		if (format.iFontPresentation.iHighlightColor==effective_format.iFontPresentation.iHighlightColor)
			mask.ClearAttrib(EAttFontHighlightColor);
		}
	if (mask.AttribIsSet(EAttFontHighlightStyle))
		{
		if (format.iFontPresentation.iHighlightStyle==effective_format.iFontPresentation.iHighlightStyle)
			mask.ClearAttrib(EAttFontHighlightStyle);
		}
	if (mask.AttribIsSet(EAttFontStrikethrough))
		{
		if (format.iFontPresentation.iStrikethrough==effective_format.iFontPresentation.iStrikethrough)
			mask.ClearAttrib(EAttFontStrikethrough);
		}
	if (mask.AttribIsSet(EAttFontUnderline))
		{
		if (format.iFontPresentation.iUnderline==effective_format.iFontPresentation.iUnderline)
			mask.ClearAttrib(EAttFontUnderline);
		}
	if (mask.AttribIsSet(EAttFontHeight))
		{
		if (format.iFontSpec.iHeight==effective_format.iFontSpec.iHeight)
			mask.ClearAttrib(EAttFontHeight);
		}
	if (mask.AttribIsSet(EAttFontPosture))
		{
		if (format.iFontSpec.iFontStyle.Posture()==effective_format.iFontSpec.iFontStyle.Posture())
			mask.ClearAttrib(EAttFontPosture);
		}
	if (mask.AttribIsSet(EAttFontStrokeWeight))
		{
		if (format.iFontSpec.iFontStyle.StrokeWeight()==effective_format.iFontSpec.iFontStyle.StrokeWeight())
			mask.ClearAttrib(EAttFontStrokeWeight);
		}
	if (mask.AttribIsSet(EAttFontPrintPos))
		{
		if (format.iFontSpec.iFontStyle.PrintPosition()==effective_format.iFontSpec.iFontStyle.PrintPosition())
			mask.ClearAttrib(EAttFontPrintPos);
		}
	if (mask.AttribIsSet(EAttFontTypeface))
		{
		if (format.iFontSpec.iTypeface==effective_format.iFontSpec.iTypeface)
			mask.ClearAttrib(EAttFontTypeface);
		}
	if (mask.AttribIsSet(EAttCharLanguage))
		{
		if (format.iLanguage==effective_format.iLanguage)
			mask.ClearAttrib(EAttCharLanguage);
		}
	if (mask.AttribIsSet(EAttFontHiddenText))
		{
		if (format.iFontPresentation.iHiddenText==effective_format.iFontPresentation.iHiddenText)
			mask.ClearAttrib(EAttFontHiddenText);
		}
	if (mask.AttribIsSet(EAttFontPictureAlignment))
		{
		if (format.iFontPresentation.iPictureAlignment==effective_format.iFontPresentation.iPictureAlignment)
			mask.ClearAttrib(EAttFontPictureAlignment);
		}
	if (mask.AttribIsSet(EAttParserTag))
		{
		if (aFormat.iParserTag == aEffectiveFormat.iParserTag)
			mask.ClearAttrib(EAttParserTag);
		}
	aMask = mask;
	}


void RFormatStream::SenseParaFormatL(CParaFormat& aParaFormat,TParaFormatMask& aMask,CParaFormat::TParaFormatGetMode aMode)const
// Dumps contents of the stream into aParaFormat.  The attributes written to aParaFormat
// are determined by the mode, aMode.
// If the mode is EAllAttributes, then all attributes are written,
// if the mode is EFixedAttributes, then only the fixed atttibutes are written,
// ie, not tabs,borders,bullet (which are variable).
// For each format attribute found in the stream:
// write it into a ParaFormat if the corresponding flag in aMask is not set,
// and set this flag showing the attribute has been written into aParaFormat.
//
// If aMode is EAllAttributes this function may leave through out-of-memory allocating paragraph borders,
// bullets or tabs.
// If aMode is EFixedAttributes, this function is guaranteed not to leave.
//
	{
	__TEST_INVARIANT;

	TUint8* ptr=iBase;
	if (ptr==NULL)
		return;
	TParaFormatMask old_mask = aMask;
	TParaFormatMask new_mask = aMask;
	TUint8* end=iEnd;
	TParaBorder* b = NULL;

	while (ptr < end)
		{
		TUint8 type = *ptr;
		ptr += KTypeSize;
		switch (type)
			{
			case EAttFillColor:
				if (!old_mask.AttribIsSet(EAttFillColor))
					{
					new_mask.SetAttrib(EAttFillColor);
					ptr=ReadValue(ptr,aParaFormat.iFillColor);
					}
				else
					ptr+=KParaFillColor;
				break;
			case EAttLeftMargin:
				if (!old_mask.AttribIsSet(EAttLeftMargin))
					{
					new_mask.SetAttrib(EAttLeftMargin);
					aParaFormat.iLeftMarginInTwips = Read32(ptr);
					}
				ptr+=KParaLeftMargin;
				break;
			case EAttRightMargin:
				if (!old_mask.AttribIsSet(EAttRightMargin))
					{
					new_mask.SetAttrib(EAttRightMargin);
					aParaFormat.iRightMarginInTwips = Read32(ptr);
					}
				ptr+=KParaRightMargin;
				break;
			case EAttIndent:
				if (!old_mask.AttribIsSet(EAttIndent))
					{
					new_mask.SetAttrib(EAttIndent);
					aParaFormat.iIndentInTwips = Read32(ptr);
					}
				ptr+=KParaIndent;
				break;
			case EAttAlignment:
				if (!old_mask.AttribIsSet(EAttAlignment))
					{
					new_mask.SetAttrib(EAttAlignment);
					aParaFormat.iHorizontalAlignment=CParaFormat::TAlignment(*ptr);
					}
				ptr+=KParaAlignment;
				break;
			case EAttVerticalAlignment:
				if (!old_mask.AttribIsSet(EAttVerticalAlignment))
					{
					new_mask.SetAttrib(EAttVerticalAlignment);
					aParaFormat.iVerticalAlignment=CParaFormat::TAlignment(*ptr);
					}
				ptr+=KParaVerticalAlignment;
				break;
			case EAttLineSpacing:
				if (!old_mask.AttribIsSet(EAttLineSpacing))
					{
					new_mask.SetAttrib(EAttLineSpacing);
					aParaFormat.iLineSpacingInTwips = Read32(ptr);
					}
				ptr+=KParaLineSpacing;
				break;
			case EAttLineSpacingControl:
				if (!old_mask.AttribIsSet(EAttLineSpacingControl))
					{
					new_mask.SetAttrib(EAttLineSpacingControl);
					aParaFormat.iLineSpacingControl=(CParaFormat::TLineSpacingControl)*ptr;
					}
				ptr+=KParaLineSpacingControl;
				break;
			case EAttSpaceBefore:
				if (!old_mask.AttribIsSet(EAttSpaceBefore))
					{
					new_mask.SetAttrib(EAttSpaceBefore);
					aParaFormat.iSpaceBeforeInTwips = Read32(ptr);
					}
				ptr+=KParaSpaceBefore;
				break;
			case EAttSpaceAfter:
				if (!old_mask.AttribIsSet(EAttSpaceAfter))
					{
					new_mask.SetAttrib(EAttSpaceAfter);
					aParaFormat.iSpaceAfterInTwips = Read32(ptr);
					}
				ptr+=KParaSpaceAfter;
				break;
			case EAttKeepTogether:
				if (!old_mask.AttribIsSet(EAttKeepTogether))
					{
					new_mask.SetAttrib(EAttKeepTogether);
					aParaFormat.iKeepTogether=TBool(*ptr);
					}
				ptr+=KParaKeepTogether;
				break;
			case EAttKeepWithNext:
				if (!old_mask.AttribIsSet(EAttKeepWithNext))
					{
					new_mask.SetAttrib(EAttKeepWithNext);
					aParaFormat.iKeepWithNext=(TBool)*ptr;
					}
				ptr+=KParaKeepWithNext;
				break;
			case EAttStartNewPage:
				if (!old_mask.AttribIsSet(EAttStartNewPage))
					{
					new_mask.SetAttrib(EAttStartNewPage);
					aParaFormat.iStartNewPage=TBool(*ptr);
					}
				ptr+=KParaStartNewPage;
				break;
			case EAttWidowOrphan:
				if (!old_mask.AttribIsSet(EAttWidowOrphan))
					{
					new_mask.SetAttrib(EAttWidowOrphan);
					aParaFormat.iWidowOrphan=TBool(*ptr);
					}
				ptr+=KParaWidowOrphan;
				break;
			case EAttWrap:
				if (!old_mask.AttribIsSet(EAttWrap))
					{
					new_mask.SetAttrib(EAttWrap);
					aParaFormat.iWrap=TBool(*ptr);
					}
				ptr+=KParaWrap;
				break;
			case EAttBorderMargin:
				if (!old_mask.AttribIsSet(EAttBorderMargin))
					{
					new_mask.SetAttrib(EAttBorderMargin);
					aParaFormat.iBorderMarginInTwips = Read32(ptr);
					}
				ptr+=KParaBorderMargin;
				break;
			case EAttTopBorder:
				if (aMode==CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttTopBorder) )
					{
					ptr+=KParaTopBorder;
					}
				else
					{
					TParaBorder border;
					ptr=ReadValue(ptr,border);
					aParaFormat.SetParaBorderL(CParaFormat::EParaBorderTop,border);
					new_mask.SetAttrib(EAttTopBorder);
					}
				break;
			case EAttBottomBorder:
				if (aMode==CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttBottomBorder))
					{
					ptr+=KParaBottomBorder;
					}
				else
					{
					TParaBorder border;
					ptr=ReadValue(ptr,border);
					aParaFormat.SetParaBorderL(CParaFormat::EParaBorderBottom,border);
					new_mask.SetAttrib(EAttBottomBorder);
					}
				break;
			case EAttLeftBorder:
				if (aMode==CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttLeftBorder))
					{
					ptr+=KParaLeftBorder;
					}
				else
					{
					TParaBorder border;
					ptr=ReadValue(ptr,border);
					aParaFormat.SetParaBorderL(CParaFormat::EParaBorderLeft,border);
					new_mask.SetAttrib(EAttLeftBorder);
					}
				break;
			case EAttRightBorder:
				if (aMode==CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttRightBorder))
					{
					ptr+=KParaRightBorder;
					}
				else
					{
					TParaBorder border;
					ptr=ReadValue(ptr,border);
					aParaFormat.SetParaBorderL(CParaFormat::EParaBorderRight,border);
					new_mask.SetAttrib(EAttRightBorder);
					}
				break;
			case EAttDefaultTabWidth:
				if (!old_mask.AttribIsSet(EAttDefaultTabWidth))
					{
					new_mask.SetAttrib(EAttDefaultTabWidth);
					aParaFormat.iDefaultTabWidthInTwips = Read32(ptr);
					}
				ptr+=KParaDefaultTabWidth;
				break;
			case EAttTabStop:
				{
				if (aMode==CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttTabStop))
					ptr += KParaTabStop;
				else
					{
					ptr=ReadTabL(ptr,aParaFormat);
					new_mask.SetAttrib(EAttTabStop);
					}
				break;
				}
			case EAttBullet:
				{
				if (aMode==CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttBullet))
					{
					ptr+=*ptr+1;  // variable length attribute
					}
				else
					{
					TBullet* bullet=aParaFormat.iBullet;
					if (!bullet)
						aParaFormat.iBullet=bullet=new(ELeave) TBullet;
					ptr=ReadValue(ptr,*bullet);
					new_mask.SetAttrib(EAttBullet);
					//coverity[memory_leak]
					}
				break;
				}
			case EAttParaLanguage:
				if (!old_mask.AttribIsSet(EAttParaLanguage))
					{
					new_mask.SetAttrib(EAttParaLanguage);
					aParaFormat.iLanguage=TInt32(*ptr);
					}
				ptr+=KParaLanguage;
				break;

			// Auxiliary attributes for system colours, etc.
			case EAttFillSystemColor:
				if (!old_mask.AttribIsSet(EAttFillColor))
					ReadSystemColor(ptr,aParaFormat.iFillColor);
				ptr++;
				break;
			case EAttBulletSystemColor:
				if (aMode != CParaFormat::EFixedAttributes && !old_mask.AttribIsSet(EAttBullet) &&
					aParaFormat.iBullet)
					ReadSystemColor(ptr,aParaFormat.iBullet->iColor);
				ptr++;					
				break;
			case EAttTopBorderSystemColor:
				if (aMode != CParaFormat::EFixedAttributes && !old_mask.AttribIsSet(EAttTopBorder))
					{
					b = aParaFormat.ParaBorderPtr(CParaFormat::EParaBorderTop);
					if (b)
						ReadSystemColor(ptr,b->iColor);
					}
				ptr++;
				break;
			case EAttBottomBorderSystemColor:
				if (aMode != CParaFormat::EFixedAttributes && !old_mask.AttribIsSet(EAttBottomBorder))
					{
					b = aParaFormat.ParaBorderPtr(CParaFormat::EParaBorderBottom);
					if (b)
						ReadSystemColor(ptr,b->iColor);
					}
				ptr++;
				break;
			case EAttLeftBorderSystemColor:
				if (aMode != CParaFormat::EFixedAttributes && !old_mask.AttribIsSet(EAttLeftBorder))
					{
					b = aParaFormat.ParaBorderPtr(CParaFormat::EParaBorderLeft);
					if (b)
						ReadSystemColor(ptr,b->iColor);
					}
				ptr++;
				break;
			case EAttRightBorderSystemColor:
				if (aMode != CParaFormat::EFixedAttributes && !old_mask.AttribIsSet(EAttRightBorder))
					{
					b = aParaFormat.ParaBorderPtr(CParaFormat::EParaBorderRight);
					if (b)
						ReadSystemColor(ptr,b->iColor);
					}
				ptr++;
				break;
			case EAttParaLanguageX:
				if (!old_mask.AttribIsSet(EAttParaLanguage))
					{
					new_mask.SetAttrib(EAttParaLanguage);
					aParaFormat.iLanguage = Read32(ptr);
					}
				ptr += KParaLanguageX;
				break;
			case EAttBulletX:
				if (aMode == CParaFormat::EFixedAttributes || old_mask.AttribIsSet(EAttBullet) ||
					!aParaFormat.iBullet)
					ptr += KParaBulletX;
				else
					{
					aParaFormat.iBullet->iStyle = (TBullet::TStyle)(*ptr++);
					aParaFormat.iBullet->iStartNumber = Read32(ptr);
					ptr += sizeof(TInt32);
					aParaFormat.iBullet->iAlignment = (TBullet::TAlignment)(*ptr++);
					}
				break;

			default:  // have maybe read an attribute defined by a later version; stop here
				aMask = new_mask;
				return;
			}
		}
	aMask = new_mask;
	}

/*
Extract format attributes selected by cleared bits in aMask.
Put them into aCharFormatX if the corresponding bit in aMask is NOT set, and set the bit.
*/
void RFormatStream::SenseCharFormat(TCharFormatX& aCharFormat,TCharFormatXMask& aMask) const
	{
	__TEST_INVARIANT;

	TUint8* ptr = iBase;
	if (ptr == NULL)
		return;
	TCharFormatXMask old_mask = aMask;
	TCharFormatXMask new_mask = aMask;
	TUint8* end = iEnd;
	TCharFormat& format = aCharFormat.iCharFormat;

	while (ptr<end)
		{
		TUint8 type = *ptr;
		ptr+=KTypeSize;
		switch (type)
			{
			case EAttColor:
				if (!old_mask.AttribIsSet(EAttColor))
					{
					new_mask.SetAttrib(EAttColor);
					ptr=ReadValue(ptr,format.iFontPresentation.iTextColor);
					}
				else
					ptr+=KCharColor;
				break;
			case EAttFontHighlightColor:
				if (!old_mask.AttribIsSet(EAttFontHighlightColor))
					{
					new_mask.SetAttrib(EAttFontHighlightColor);
					ptr=ReadValue(ptr,format.iFontPresentation.iHighlightColor);
					}
				else
					ptr+=KCharHighlightColor;
				break;
			case EAttFontHighlightStyle:
				if (!old_mask.AttribIsSet(EAttFontHighlightStyle))
					{
					new_mask.SetAttrib(EAttFontHighlightStyle);
					format.iFontPresentation.iHighlightStyle=(TFontPresentation::TFontHighlightStyle)*ptr;
					}
				ptr+=KCharHighlightStyle;
				break;
			case EAttFontStrikethrough:
				if (!old_mask.AttribIsSet(EAttFontStrikethrough))
					{
					new_mask.SetAttrib(EAttFontStrikethrough);
					format.iFontPresentation.iStrikethrough=(TFontStrikethrough)*ptr;
					}
				ptr+=KCharStrikethrough;
				break;
			case EAttFontUnderline:
				if (!old_mask.AttribIsSet(EAttFontUnderline))
					{
					new_mask.SetAttrib(EAttFontUnderline);
					format.iFontPresentation.iUnderline=(TFontUnderline)*ptr;
					}
				ptr+=KCharUnderline;
				break;
			case EAttFontHeight:
				if (!old_mask.AttribIsSet(EAttFontHeight))
					{
					new_mask.SetAttrib(EAttFontHeight);
					format.iFontSpec.iHeight = Read32(ptr);
					}
				ptr+=KCharFontHeight;
				break;
			case EAttFontPosture:
				if (!old_mask.AttribIsSet(EAttFontPosture))
					{
					new_mask.SetAttrib(EAttFontPosture);
					format.iFontSpec.iFontStyle.SetPosture((TFontPosture)*ptr);
					}
				ptr+=KCharPosture;
				break;
			case EAttFontStrokeWeight:
				if (!old_mask.AttribIsSet(EAttFontStrokeWeight))
					{
					new_mask.SetAttrib(EAttFontStrokeWeight);
					format.iFontSpec.iFontStyle.SetStrokeWeight((TFontStrokeWeight)*ptr);
					}
				ptr+=KCharStrokeWeight;
				break;
			case EAttFontPrintPos:
				if (!old_mask.AttribIsSet(EAttFontPrintPos))
					{
					new_mask.SetAttrib(EAttFontPrintPos);
					format.iFontSpec.iFontStyle.SetPrintPosition((TFontPrintPosition)*ptr);
					}
				ptr+=KCharPrintPos;
				break;
			case EAttFontTypeface:
				if (!old_mask.AttribIsSet(EAttFontTypeface))
					{
					new_mask.SetAttrib(EAttFontTypeface);
					ptr=ReadValue(ptr,format.iFontSpec.iTypeface);  // updates ptr
					}
				else
					ptr+=*ptr+1;	// variable length attribute
				break;
			case EAttBitmapType:
				if (!old_mask.AttribIsSet(EAttFontTypeface))
					{
					format.iFontSpec.iFontStyle.SetBitmapType(static_cast <TGlyphBitmapType> (*ptr));
					}
				ptr+=KBitmapType;
				break;
			case EAttCharLanguage:
				if (!old_mask.AttribIsSet(EAttCharLanguage))
					{
					new_mask.SetAttrib(EAttCharLanguage);
					format.iLanguage=*ptr;
					}
				ptr+=KCharLanguage;
				break;
			case EAttFontHiddenText:
				if (!old_mask.AttribIsSet(EAttFontHiddenText))
					{
					new_mask.SetAttrib(EAttFontHiddenText);
					format.iFontPresentation.iHiddenText=(TBool)*ptr;
					}
				ptr+=KCharHiddenText;
				break;
			case EAttFontPictureAlignment:
				if (!old_mask.AttribIsSet(EAttFontPictureAlignment))
					{
					new_mask.SetAttrib(EAttFontPictureAlignment);
					format.iFontPresentation.iPictureAlignment=(TFontPresentation::TAlignment)*ptr;
					}
				ptr+=KCharPictureAlignment;
				break;

			// Auxiliary attributes for system colours, etc.
			case EAttTextSystemColor:
				if (!old_mask.AttribIsSet(EAttColor))
					ReadSystemColor(ptr,format.iFontPresentation.iTextColor);
				ptr++;
				break;

			case EAttFontHighlightSystemColor:
				if (!old_mask.AttribIsSet(EAttFontHighlightColor))
					ReadSystemColor(ptr,format.iFontPresentation.iHighlightColor);
				ptr++;
				break;

			case EAttCharLanguageX:
				if (!old_mask.AttribIsSet(EAttCharLanguage))
					{
					new_mask.SetAttrib(EAttCharLanguage);
					format.iLanguage = Read32(ptr);
					}
				ptr += KCharLanguageX;
				break;

			// Internal character attributes:
			case EAttParserTag:
				if (!old_mask.AttribIsSet(EAttParserTag))
					{
					new_mask.SetAttrib(EAttParserTag);
					aCharFormat.iParserTag = Read32(ptr);
					}
				ptr += KCharParserTag;
				break;

			default:	// have maybe read an attribute defined by a later version; stop here
				aMask = new_mask;
				return;
			}
		}
	aMask = new_mask;
	}

/** Swaps the contents of this stream with aStream.
@param aStream Object to swap contents with. */
void RFormatStream::Swap(RFormatStream& aStream)
	{
	TUint8* t = iBase;
	iBase = aStream.iBase;
	aStream.iBase = t;
	t = iEnd;
	iEnd = aStream.iEnd;
	aStream.iEnd = t;
	}

TInt RFormatStream::StoreTabs(TUint8*& aPtr,const CParaFormat& aDesiredFormat,const CParaFormat& aCurrentFormat,TBool aStoreData)
// Merges the tabs from the desired paraFormat with those from the current effective paraFormat, resolving differences
// as described below, and store the resultant tablist in this layer.
// If aStoreData is false, then return the number of bytes necessary to store all required tabs
//
	{
	TInt requiredTabCount=0;
	TInt desiredTabCount=aDesiredFormat.TabCount();
	TInt currentTabCount=aCurrentFormat.TabCount();
	if (desiredTabCount==0 && currentTabCount==0)
		{// Nothing needs to be stored.
		return requiredTabCount;
		}
	if (desiredTabCount>0 && currentTabCount>0)
		{// The 2 tab lists need merging.
		requiredTabCount=MergeTabLists(aPtr,aDesiredFormat,desiredTabCount,aCurrentFormat,currentTabCount,aStoreData);
		}
	else if (desiredTabCount>0 && currentTabCount==0)
			{// There are no previous tabs to merge so just store all the desired ones.
			if (aStoreData)
				StoreAllTabs(aPtr,aDesiredFormat);
			else
				requiredTabCount+=desiredTabCount;
			}
		else if (desiredTabCount==0 && currentTabCount>0)
				{// We want to delete (NULL) all existing tabs.
				if (aStoreData)
					{
					for (TInt index=0;index<currentTabCount;index++)
						{
						TTabStop tab=aCurrentFormat.TabStop(index);
						tab.iType=TTabStop::ENullTab;
						StoreTab(aPtr,tab);
						}
					}
				else requiredTabCount+=currentTabCount;
				}
	return requiredTabCount*(KTypeSize+KParaTabStop);
	}


TInt RFormatStream::MergeTabLists(TUint8*& aPtr,const CParaFormat& aDesiredFormat,TInt aDesiredTabCount,
								  const CParaFormat& aCurrentFormat,TInt aCurrentTabCount,TBool aStoreData)
// Compares the desired tablist (from a dialog interaction) with that of the current tablist.
// The resulting tab stop is stored if aStoreData is TRUE.
// There may be 2 tabs for any for any given tabstops.
// The possible results of the comparison are:
// (1) Tab is in desired but not current --> Store the desired tab.
// (2) Tab is in current but not desired --> Null the current and store.
// (3) Tab is in both current and desired -->
//			(i) if tab type is the same then store nothing -- rely on the base one being inherited.
//			(ii)if tab type is not same then store the current -- overriding whatevers in the base.
//
	{
	TInt requiredTabCount=0;
	TInt offsetInDesired,offsetInCurrent;
	offsetInDesired=offsetInCurrent=0;
	TTabStop desiredTab,currentTab;
	while ((offsetInDesired!=aDesiredTabCount) && (offsetInCurrent!=aCurrentTabCount))
		{// Do this until we exhaust one of the tab lists.
		desiredTab=aDesiredFormat.TabStop(offsetInDesired);
		currentTab=aCurrentFormat.TabStop(offsetInCurrent);
		if (desiredTab.iTwipsPosition<currentTab.iTwipsPosition)
			{
			if (aStoreData)
				aPtr=StoreTab(aPtr,desiredTab);  // Store desiredTab.
			else 
				requiredTabCount++;
			offsetInDesired++;  // Move to the next tab in the desired tablist.
			continue;
			}
		if (desiredTab.iTwipsPosition>currentTab.iTwipsPosition)
			{
			if (aStoreData)
				{
				currentTab.iType=TTabStop::ENullTab;  // Null the current current tab.
				aPtr=StoreTab(aPtr,currentTab);  // Store NULL'd currentTab
				}
			else
				requiredTabCount++;
			offsetInCurrent++;  // Move to the next tab in the current tablist.
			continue;
			}
		if (desiredTab.iTwipsPosition==currentTab.iTwipsPosition)
			{
			if (desiredTab.iType!=currentTab.iType)
				{// Store desired type to override the one from the lower layer.
				if (aStoreData)
					aPtr=StoreTab(aPtr,desiredTab);
				else
					requiredTabCount++;
				}// Otherwise rely on the one in the base - store nothing but increment.
			offsetInDesired++;
			offsetInCurrent++;
			continue;
			}
		}
	TInt currentLeftToDo=aCurrentTabCount-offsetInCurrent;
	TInt desiredLeftToDo=aDesiredTabCount-offsetInDesired;
	// Spot which list is exhausted and process the rest of the remainder appropriately.
	if (currentLeftToDo)
		{// Store null'd remainder of current tab list.
		for (;offsetInCurrent<aCurrentTabCount;offsetInCurrent++)
			{
			if (aStoreData)
				{
				currentTab=aCurrentFormat.TabStop(offsetInCurrent);
				currentTab.iType=TTabStop::ENullTab;  // Null the current current tab.
				aPtr=StoreTab(aPtr,currentTab);  // Store NULL'd currentTab
				}
			else
				requiredTabCount++;
			}
		}
	if (desiredLeftToDo)
		{// Store remainder of desired tab list.
		for (;offsetInDesired<aDesiredTabCount;offsetInDesired++)
			{
			if (aStoreData)
				aPtr=StoreTab(aPtr,aDesiredFormat.TabStop(offsetInDesired));
			else
				requiredTabCount++;
			}
		}
// ASSERT: The tabList offsets are as we would expect in a normal (correct) completion.	
	if (offsetInDesired!=aDesiredTabCount)
	    {
	    OstTrace0( TRACE_FATAL, RFORMATSTREAM_MERGETABLISTS, "EStoreTabError" );
	    }
	__ASSERT_ALWAYS(offsetInDesired==aDesiredTabCount,Panic(EStoreTabError));
	if (offsetInCurrent!=aCurrentTabCount)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_RFORMATSTREAM_MERGETABLISTS, "EStoreTabError" );
	    }
	__ASSERT_ALWAYS(offsetInCurrent==aCurrentTabCount,Panic(EStoreTabError));
	return requiredTabCount;
	}


// Writes all tabs from aSource.
void RFormatStream::StoreAllTabs(TUint8*& aPtr,const CParaFormat& aSource)
	{
	int tabs = aSource.TabCount();
	for (TInt index = 0;index < tabs; index++)
		aPtr = StoreTab(aPtr,TTabStop(aSource.TabStop(index)));
	}


TUint8* RFormatStream::StoreBullet(TUint8* aPtr,const TBullet& aBullet)
	{
	// Write the attribute code and leave a gap for the length.
	*aPtr++ = TUint8(EAttBullet);
	TUint8* length_ptr = aPtr++;

	// Store the font height.
	Write32(aPtr,aBullet.iHeightInTwips);

	// Store the bullet character code.
	aPtr[0] = (TUint8)(aBullet.iCharacterCode);
	aPtr[1] = (TUint8)(aBullet.iCharacterCode >> 8);
	aPtr += sizeof(TText);

	// Store the hanging indent
	*aPtr++ = TUint8(aBullet.iHangingIndent != FALSE);

	// Store the colour.
	aPtr = Store(aPtr,aBullet.iColor);

	// Store the typeface
	aPtr = Store(aPtr,aBullet.iTypeface);

	// Store the number of data bytes stored.
	*length_ptr = (TUint8)(aPtr - length_ptr - 1);

	return aPtr;
	}


TUint8* RFormatStream::StoreBorder(TUint8* aPtr,TTextFormatAttribute aType,const TParaBorder& aSource)
// Stores paragraph border attributes.
// Writes the line style, thickness,autoColor flag
// and Color, from aSource to the end of the format stream.
// aType is converted from an enum to a TUint8 as it is stored in the format stream.
//
	{
	*aPtr++=TUint8(aType);
	// border line style
	*aPtr++=TUint8(aSource.iLineStyle);
	// border line thickness
	Write32(aPtr,aSource.iThickness);
	// border color
	aPtr=Store(aPtr,aSource.iColor);
	// border autocolor
	*aPtr++=TUint8(aSource.iAutoColor!=EFalse);
	return aPtr;
	}


TUint8* RFormatStream::StoreTab(TUint8* aPtr,const TTabStop& aSource)
// Stores a tabstop compound attribute at the end of the stream.
// Writes the tabstop twips position and the tab type, from aSource, to the end of the format stream.
// aType is converted from an enum to a TUint8 as it is stored in the format stream.
// The tab type is compressed from a TTabType enum to a TUint8.
// Uses InsertL, which may cause expansion of the stream storage, and thus may fail.
//
	{
	*aPtr++=TUint8(EAttTabStop);
	// Store tab poisition.
	Write32(aPtr,aSource.iTwipsPosition);
	// Compress the tab type.
	*aPtr++=TUint8(aSource.iType);
	return aPtr;
	}


TUint8* RFormatStream::Store(TUint8* aPtr,const TLogicalRgb& aRgb)
   // Store color value
   //
   	{
	*aPtr++=TUint8(aRgb.Red());
	*aPtr++=TUint8(aRgb.Green());
	*aPtr++=TUint8(aRgb.Blue());
	return aPtr;
   	}


TUint8* RFormatStream::Store(TUint8* aPtr,const TTypeface& aTypeface)
// Stores typeface name and flags.
//
	{
	//
	// Store typeface name size
	TUint8 length=TUint8(aTypeface.iName.Size());
	length+=KTypefaceFlags;
	*aPtr++=length;  // byte count.
	//
	// Store typeface name
	aPtr=Mem::Copy(aPtr,aTypeface.iName.Ptr(),length-KTypefaceFlags);
	//
	// Store typeface flags
	TUint8 flags=0;
	if (aTypeface.IsProportional())
		flags|=KFontProportional;
	if (aTypeface.IsSerif())
		flags|=KFontSerif;	
	if (aTypeface.IsSymbol())
		flags|=KFontSymbol;
	*aPtr=flags;
	aPtr+=KTypefaceFlags;
	//
	return aPtr;
	}

TUint8* RFormatStream::ReadValue(TUint8* aPtr,TLogicalRgb& aRgb)const
// Reads a color value from storage.
//
	{
	aRgb.SetRed(*aPtr++);
	aRgb.SetGreen(*aPtr++);
	aRgb.SetBlue(*aPtr++);
	aRgb.SetSystemColorIndex(0);
	return aPtr;
	}


TUint8* RFormatStream::ReadValue(TUint8* aPtr,TParaBorder& aBorder)const
// Reads a paragraph border from storage.
//
	{
	// Read line style
	aBorder.iLineStyle=TParaBorder::TLineStyle(*aPtr++);
	// Read thickness
	aBorder.iThickness = Read32(aPtr);
	aPtr+=sizeof(TInt32);
	// Read color
	aPtr=ReadValue(aPtr,aBorder.iColor);  // returns aPtr
	// Read autocolor
	aBorder.iAutoColor=TBool(*aPtr++);
	return aPtr;
	}


TUint8* RFormatStream::ReadValue(TUint8* aPtr,TBullet& aBullet)const
// Read the bullet compound attribute.
//
	{
	// Read the length of this bullet attribute
	aPtr++;  // length not used in this context, skip it
	// Read bullet twips height into target
	aBullet.iHeightInTwips = Read32(aPtr);
	aPtr+=sizeof(TInt32);
	// Read bullet character code into target.
	aBullet.iCharacterCode = (TText)aPtr[0] | ((TText)aPtr[1] << 8);
	aPtr+=sizeof(TText);
	// Read hanging indent.
	aBullet.iHangingIndent=TBool(*aPtr++);
	// Read Color.
	aPtr=ReadValue(aPtr,aBullet.iColor);  // returns aPtr
	// Read typeface
	return ReadValue(aPtr,aBullet.iTypeface);  // returns aPtr
	}


TUint8* RFormatStream::ReadValue(TUint8* aPtr,TTypeface& aTypeface)const
// Read a typeface name from storage and associated flags.
//
	{
	//
	// Read typeface name size
	TInt length=*aPtr++;
	//
	// Read typeface name
	TInt typefaceLength=length-KTypefaceFlags;
	
	if ((typefaceLength%2)!=0)
	    {
	    OstTrace0( TRACE_DUMP, RFORMATSTREAM_READVALUE, "ECorruptFormatLayer" );
	    }
	__ASSERT_DEBUG((typefaceLength%2)==0,Panic(ECorruptFormatLayer)); // must be an even number

	TPtr name=aTypeface.iName.Des();
	Mem::Copy(CONST_CAST(TText*,name.Ptr()),aPtr,typefaceLength);
	typefaceLength>>=1;
	name.SetLength(typefaceLength);

	aPtr+=length-KTypefaceFlags;
	//
	// Read typeface name flags
	TInt attrib=0;
	TUint flags=*aPtr;
	if (flags & KFontProportional)
		attrib|=TTypeface::EProportional;
	if (flags & KFontSerif)
		attrib|=TTypeface::ESerif;
	if (flags & KFontSymbol)
		attrib|=TTypeface::ESymbol;
	aTypeface.SetAttributes(attrib);  // reset the attributes
	return aPtr+KTypefaceFlags;
	}


TUint8* RFormatStream::ReadTabL(TUint8* aPtr,CParaFormat& aTarget)const
// Read the tab stop data from the stream, located at aPos, into 
// a temporary TTabStop, and use this to fill in aTarget.
// Does not read the tab if one already exists at the same twips position.
// (Implementation of tab inheritance policy).
// aPos is updated manually, rather than using Length(), since this is
// a compound attribute.
//
	{
	TTabStop tab;
	// Read tab position
	tab.iTwipsPosition = Read32(aPtr);
	aPtr+=sizeof(TInt32);
	// Read tab type
	tab.iType=TTabStop::TTabType(*aPtr++);
	// Set this tab in the paragraph format
	TInt ret=aTarget.LocateTab(tab.iTwipsPosition);		// is this necessary
	if (ret==KTabNotFound)
		aTarget.StoreTabL(tab);
	return aPtr;
	}


TInt RFormatStream::Length(TUint8*& aPtr,TTextFormatAttribute aType) const
// Returns the length of a Type's value
// A length of zero in the lookup table, indicates a variable
// length value.  In this case, the length is stored in the byte
// following the type, which is read and returned.
// aPtr is incremented by the appropriate amount following a read.
//
	{
	TInt length=TheAttributeLength[aType];

	if (length<0)
	    {
	    OstTrace0( TRACE_DUMP, RFORMATSTREAM_LENGTH, "EAttributeLengthLookupNegative" );
	    }
	__ASSERT_DEBUG(length>=0,Panic(EAttributeLengthLookupNegative));
	if (length>0)
		return length;
	else
		return *aPtr++;
	}
