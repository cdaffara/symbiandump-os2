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


#ifndef __TXTSTD_H__
#define __TXTSTD_H__

#include <e32std.h>
#include <txtfrmat.h>

class TCharFormatX
/**
The extended character format used internally.
As well as the public TCharFormat attributes it stores
any attributes that are useful only inside ETEXT.
@internalComponent
*/
	{
	public:
	TCharFormatX(): iParserTag(0) { }
	TCharFormatX(const TCharFormat& aCharFormat): iCharFormat(aCharFormat), iParserTag(0) { }

	TCharFormat iCharFormat;
	TUint iParserTag;
	};

/**
Attributes not used in the format masks, but used to store system colour indices, etc.
This method maintains data compatibility. For example, a paragraph fill colour, including its
fourth byte that is used as the system colour index, is selected by a single mask but corresponding
to EAttFillColor, but is stored using two attribute flags: one for the three bytes of the RGB colour,
and one extra optional one for the system colour index.
@internalComponent
*/
enum TTextFormatNonMaskableAttribute
	{
	EAttFillSystemColor = ETextFormatAttributeCount,
	EAttBulletSystemColor,
	EAttTopBorderSystemColor,
	EAttBottomBorderSystemColor,
	EAttLeftBorderSystemColor,
	EAttRightBorderSystemColor,
	EAttTextSystemColor,
	EAttFontHighlightSystemColor,
	EAttParaLanguageX,
	EAttCharLanguageX,
	EAttBulletX,
	EAttBitmapType,

	EExternalizedAttributeCount
	};


/**
Attributes used to mask internal character attributes.
@internalComponent
*/
enum TCharFormatXAttribute
	{
	EAttParserTag = EExternalizedAttributeCount,

	EAttributeCount
	};

class TCharFormatXMask: public TCharFormatMask
/**
A mask that can deal with TCharFormatXAttributes as well as TTextFormatAttributes
@internalComponent
*/
	{
	public:
	TCharFormatXMask(): TCharFormatMask() { }
	TCharFormatXMask(TCharFormatMask aMask): TCharFormatMask(aMask) { }

	/*
	An offset to make extended attribute flags contiguous with ordinary character attributes
	so that we don't run out of bits if more non-maskable attributes are invented.
	*/
	enum
		{
		EAttribOffset = EExternalizedAttributeCount - ETextFormatAttributeCount
		};

	void SetAttrib(TTextFormatAttribute aAttribute)
		{ TCharFormatMask::SetAttrib(aAttribute); }
	void ClearAttrib(TTextFormatAttribute aAttribute)
		{ TCharFormatMask::ClearAttrib(aAttribute); }
	TBool AttribIsSet(TTextFormatAttribute aAttribute) const
		{ return TCharFormatMask::AttribIsSet(aAttribute); }

	void SetAttrib(TCharFormatXAttribute aAttribute)
		{ TCharFormatMask::SetAttrib((TTextFormatAttribute)(aAttribute - EAttribOffset)); }
	void ClearAttrib(TCharFormatXAttribute aAttribute)
		{ TCharFormatMask::ClearAttrib((TTextFormatAttribute)(aAttribute - EAttribOffset)); }
	TBool AttribIsSet(TCharFormatXAttribute aAttribute) const
		{ return TCharFormatMask::AttribIsSet((TTextFormatAttribute)(aAttribute - EAttribOffset)); }
	void ClearExtendedAttribs() { ClearAttrib(EAttParserTag); }
	};


/**
@internalComponent
*/
enum TETextPanic
	{
	// General
	EDebug,
	EDebugDeleteZeroLength,

	// Format attributes
	ETabNotFound,

	// Format layer storage
	EAttributeLengthLookupNegative,
	EStoreTabError,
	ECannotStoreFormatLayerChain,
	ECorruptFormatLayerChain,
	ECorruptFormatLayer,
	ENullFormatPointer,

	// Picture Restoration
	ENullStore,
	EInvalidPictureFactorySettings,
	ENullStoreResolver,

	// Plain text
	ECharPosBeyondDocument,
	EConstructCalledTwice,
	ECopyToStreamNegativeLength,
	EPasteNegativeLength,
	EDeleteNegativeLength,
	ECorruptTextStore,
	EExtractBufferTooSmall,
	ENoTextField,
	EExportLineWrapInvalid,

	// Global text
	ENullFormatLayerHandle,
	EApplyParaFormatNegativeLength,
	EApplyCharFormatNegativeLength,
	EGetParaFormatNegativeLength,
	EGetCharFormatNegativeLength,

	// Rich text
	ENullRichTextInsert,
	EMarkupAlreadyPresent,

	// Rich text index
	ERichTextIndexIntegrityErr,
	EPhraseIxPresentWithNoParaIx,
	ERichTextIndexReset,
	ESetInsertCharFormatIntegrityErr,
	EInsertEmbeddedParaErr,
	EInsertNullPicHdrData,
	EDeleteFromParagraphInvalidRange,
	EDeleteParagraphInvalidStartValue,
	EDeleteParagraphInvalidEndValue,
	EReclaimShareError,
	EParaAttribsNotInSharedList,
	ERichTextNotSetForUsingStyles,
	EPictureHeaderIndexOutOfRange,
	ELostPictureHeader,
	ESplitPhraseCalledOnSharedPara,
	ESplitPhraseCalledOnPicturePhrase,
	EGetPictureSizeCalledOnNonPicturePhrase,
	EModifiedPicturePhraseLength,
	EReleasCharFormatLayerOwnershipCalledOnPicturePhrase,
	ERichTextStorePictureIntegrityError,
	ERtIndexInternalizeCalledOnNonEmptySource,
	ERtExternalizeParaIx,
	ESharedParaCountStreamOverflow,
	EEndOfSharedParaListEncountered,
	EPasteParaIxError,
	EPastePhraseIxErr,
	ESharedFormatsMapIntegrityError,
	ESharedFormatsMapOverFlow,
	EParagraphStyleNegativeLength,
	EApplyParaStyleNegativeLength,
	ERemoveSpecificParaFormatNegativeLength,
	ECopyToClipboardNegativeLength,
	EClipboardIntegrity,
	ENonOverloadedInsertCalledWithPictureCharacter,
	EAppendLFoundInMemoryPicture,

	// style clipboarding
	EStyleClipboardIntegrityError,

	// Paragraph styles
	EStyleIntegrityError,
	EStyleIndexOutOfRange,

	// Front end processor inline editing
	ENotCurrentlyFepInlineEditing,
	EAlreadyFepInlineEditing,

	// Parser system
	EParserListNotInitialized,
	EParserListAlreadyExists,
	EParserListNotActive,
	EParserCallbackAlreadySet,
	EParserTagNotFound,
	EParserListTextIndexNotInitialized,
	};

/**
@internalComponent
*/
GLREF_C void Panic(TETextPanic aPanic);
GLREF_C void ReleaseOnCleanup(TAny* aParaAttribs);
GLREF_C void ResetOnCleanup(TAny* aParaFormat);

inline void ResetOnCleanupL( CParaFormat* aParaFormat)
	{
	CleanupStack::PushL(TCleanupItem(ResetOnCleanup,aParaFormat));
	}

#if defined(__DLL__)
#define DLLEXPORT_C EXPORT_C 
#else
#define DLLEXPORT_C
#endif

#endif
