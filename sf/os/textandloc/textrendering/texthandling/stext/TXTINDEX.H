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


#ifndef __TXTINDEX_H__
#define __TXTINDEX_H__

#include <e32std.h>
#include <e32base.h>
#include <txtmrtsr.h>
#include <txtrich.h>
#include "TXTSTD.H"

// forward declarations
class CParaAttribs;
class CPicturePhrase;
class RReadStream;
class RWriteStream;
class CStoreMap;
class CStreamStore;
class CPicture;
class CCharFormatLayer;
class CParaFormatLayer;
class CRichText;
class MPictureFactory;

/**
@internalComponent
*/
enum TRichTextStylePasteMode
	{
	EAddNewStyles,
	EConvertNewStyles,
	EIgnoreNewStyles
	};

class TTextFragment
/**
Describes a fragment of text that does not conform to the ERA text model.
@internalComponent
*/
	{
public:
	TTextFragment();
public:
	TInt iLength;  // Number of characters in text fragment
	TInt iPhraseCount;  // Number of distinct phrases in text fragment.
	};


class RPhraseAttribsEntry
/**
Provides a full description of a phrase.
A phrase consists of a specified number of contiguous characters [the phrase length]
of identical character format.
Where a picture is included as text content, it is described by its own phrase.
@internalComponent
*/
	{
public:
	enum {EPictureIndicator = -1};
	enum {EPicturePhraseLength = 1};
public:
	RPhraseAttribsEntry();
	RPhraseAttribsEntry(CCharFormatLayer* aCharFormat, TInt aLength = 0); // Sets iLen to zero - empty text phrase
	RPhraseAttribsEntry(CPicturePhrase* aPicturePhrase);  // Sets iLen to EPictureIndicator
	inline RPhraseAttribsEntry(const RPhraseAttribsEntry& aPhrase);
	void Discard();
	inline RPhraseAttribsEntry& operator=(const RPhraseAttribsEntry& aPhrase);
	//
	// Persist
	void ExternalizeL(RWriteStream& aStream) const;
	//
	// General enquiry
	inline TBool IsPicturePhrase() const;
	TBool IsIdentical(const RPhraseAttribsEntry& aPhrase) const;
	//
	// Getters
	inline TInt Length() const;
	CCharFormatLayer* CharFormat() const;
	CCharFormatLayer* ReleaseCharFormatLayerOwnership();
	TInt GetPictureSizeInTwips(TSize& aSize) const;
	TPictureHeader* PictureHeaderPtr() const;
	TPictureHeader PictureHeader() const;
	const CPicture* PictureHandleL(const MPictureFactory* aFactory,
								   const MRichTextStoreResolver* aResolver,
								   TInt aPos,
								   MLayDoc::TForcePictureLoad aForceLoad) const;
	//
	// Setters
	void AdjustLength(TInt aIncrement);
 	void SetLength(TInt aLength);
private:
	void AssignAndRelease(const RPhraseAttribsEntry& aPhrase);
private:
	TInt iLength;  // No. of characters covered by this phrase
	union
		{
		CCharFormatLayer* iCharFormat;  // *OWNED* format of this text phrase
		CPicturePhrase* iPicturePhrase;  // *OWNED* pointer to picture phrase information
		};
	__DECLARE_TEST;
	};


NONSHARABLE_CLASS(CPicturePhrase) : public CBase
/**
A low-level class providing a pointer to the character format layer for this picture phrase,
and a pointer to a picture itself.  (Which may be expressed in its persistent form).
@internalComponent
*/
	{
public:
	static CPicturePhrase* NewL(const TPictureHeader& aPicHdr,
	                            TCharFormatX& aFormat,
	                            TCharFormatXMask& aMask,
	                            CCharFormatLayer* aCharBase,
	                            TBool& aPictureOwnershipTaken);

	static CPicturePhrase* NewL(const TPictureHeader& aPicHdr,
	                            CCharFormatLayer* aCharLayer,
	                            TBool& aPictureOwnershipTaken);
	~CPicturePhrase();
protected:
	CPicturePhrase(const TPictureHeader& aPicHdr,TBool& aPictureOwnershipTaken);
	CPicturePhrase(const TPictureHeader& aPicHdr, CCharFormatLayer* aCharLayer,TBool& aPictureOwnershipTaken);
	void ConstructL(TCharFormatX& aFormat, TCharFormatXMask& aMask, CCharFormatLayer* aCharBase);
public:
	CCharFormatLayer* iCharFormat;
	TPictureHeader iPicHdr;
	};


class TParaAttribsEntry
/**
Records the length of a paragraph (no. of characters).
and references a CParaAttributes.
@internalComponent
*/
	{
public:
	TParaAttribsEntry();
	TParaAttribsEntry(TInt aLength, CParaAttribs* aParaAttribs);
public:
	TInt iLength;  // Number of characters in the paragraph (paragraph delimiter inclusive).
	CParaAttribs* iParaAttribs;  // Only references this, does not own it.
	};


class TCurrentIndexRecords
/**
Packages current phrase & para records.
@internalComponent
*/
	{
public:
	TCurrentIndexRecords();
public:
	TParaAttribsEntry* iParaEntry;
	CParaAttribs* iParaAttribs;
	RPhraseAttribsEntry* iPhrase;  // May be NULL.
	};



class TLogicalPosition
/**
Locates the current cursor position within all aspects of the index.
@internalComponent
*/
	{
public:
	TLogicalPosition();
	void Clear();
public:
	TInt iDocPos;					// Absolute document position.
	TInt iParaElement;				// paragraph index
	TInt iParaElementOffset;		// position from start of paragraph
	TInt iParaBasePhraseElement;	// index of the first phrase in the paragraph
	TInt iPhraseElement;
	TInt iPhraseElementOffset;
	};



NONSHARABLE_CLASS(CParaAttribs) : public CBase
/**
Describes the attributes that apply to the paragraph.
*owns* an iParaFormat(Layer), and *owns* an iPhraseAttribsIndex
or charFormatLayer.
@internalComponent
*/
	{
public:
	enum TParaRefCount {EPrimeNonSharedCount, EPrimeSharedCount};
	enum {EPhraseIxGranularity = 1};
public:
	// Create a CParaAttribs with constant character formatting.
	static CParaAttribs* NewL(const CParaFormatLayer* aParaLayer, const CCharFormatLayer* aCharLayer);
	// Create a CParaAttribs with specific character formatting.
	static CParaAttribs* NewL(const CParaFormatLayer* aParaLayer);
	static CParaAttribs* NewL(const CParaAttribs* aParaAttribs);
	void Release();
	void Release(TInt aCount);
	//
	// Utility functions
	inline TBool IsShared() const;
	TInt PhraseCount() const;
protected:
	CParaAttribs();
	~CParaAttribs();
	CParaAttribs* CopyParaFormatL();
private:
public:
	TDblQueLink link;
	TInt iRefCount;  // Indicates re-use of this.
	CParaFormatLayer* iParaFormat;
	union
		{
		CCharFormatLayer* iCharFormat;	// iRefCount>0 - a single shared phrase - constant char format.
		TInt iPhraseCount;	// iRefCount==0 - the number of phrases in this para.
		};
	};


//

class TRtPasteContext
/**
Provides the context for pasting of pictures from a clipboard.
@internalComponent
*/
	{
public:
	TRtPasteContext(const CStreamStore* aStore, const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer, const CStyleList* aStyleList);
public:
	const CStreamStore* iStore;  // the clipboard store
	const CParaFormatLayer* iGlobalParaLayer;  // the global para layer of the target text document
	const CCharFormatLayer* iGlobalCharLayer;  // the global char layer of the target text document
	const CStyleList* iStyleList;  // style list is present
	TBool iParagraphStylesFlag;  // ETrue indicates presence of inline style references in index data
	TBool iIncompleteParaFlag;  // ETrue indicates the pasted text fragment has no final para delimiter
	TBool iApplyFormatToLastFlag;  // ETrue indicates that para format should be applied to last text fragment
	TLogicalPosition iPastePos;  // the character position at which the text is to be pasted
	TInt iParasPasted;  // the number of paragraphs successfully pasted
	TInt iStylePasteMode;
	};



class TIndexDeleteInfo
/**
Contains information pertaining to an index delete operation.
Carries between a SetForDeleteL and a DeleteNow method call.
@internalComponent
*/
	{
public:
	enum TDeleteType
		{
		EDeleteFromParagraph,
		EDeleteParagraph,
		ENonspecificDelete
		};
public:
	TInt iStartPara;
	TInt iEndPara;
	TLogicalPosition iDeletePos;
	TInt iDeleteLength;
	TDeleteType iDeleteType;
	};


class TRichTextMapEntry
/**
A utility class for patching references to persisted shared objects
of the specified type.
Code at the bottom of the header
@internalComponent
*/
	{
public:
	TRichTextMapEntry(TInt aRefNumber, TAny* aItem);
	TRichTextMapEntry(TAny* aS, TAny* aT);
public:
	union
		{// The source item - a reference to a persistent one, or the in-memory original.
		TInt iRefNumber;
		TAny* iS;  // only used in appending one rich text to another
		};
	TAny* iT;  // the newly created/restored item - the target
	};


template <class T>
class CRichTextStoreMap : public CBase
/**
//
A utility class for patching references to persisted shared objects
of the specified type.
Code at the bottom of the header
Used for clipboarding to i) patch references to paragraph styles
ii) patch references to shared paragraph formats
and also used in the appending of one rich text to another.
@internalComponent
*/
	{
public:
	static CRichTextStoreMap* NewLC(TInt aMaximumCapacity);
	~CRichTextStoreMap();
	//
	void Bind(TInt aRefNumber, T* aItem);
	void Bind(T* aS, T* aT);
	//
	T* Item(TInt aRefNo)const;
	T* Item(const T* aS)const;
	inline const TRichTextMapEntry& At(TInt aIndex)const;
	//
	inline TInt Count()const;
	//
	void __DbgTestInvariant()const;
private:
	void ConstructL(TInt aMaximumCapacity);
private:
	CArrayFixFlat<TRichTextMapEntry>* iMap;
public:
	TInt iCapacity;
	};


class TGlobalLayerInfoAppend
/**
@internalComponent
*/
	{
public:
	TGlobalLayerInfoAppend();
	TGlobalLayerInfoAppend(const CParaFormatLayer* aAggParaFormatLayer, const CCharFormatLayer* aAggCharFormatLayer,
						   const CParaFormatLayer* aComParaFormatLayer, const CCharFormatLayer* aComCharFormatLayer);
public:
	const CParaFormatLayer* iAggParaFormatLayer;
	const CCharFormatLayer* iAggCharFormatLayer;
	const CParaFormatLayer* iComParaFormatLayer;
	const CCharFormatLayer* iComCharFormatLayer;
	};

const TInt KSingleParaGranularity = 1;
const TInt KMultiParaGranularity = 16;
const TInt KSmallPhraseGranularity = 5;
const TInt KLargePhraseGranularity = 16;
const TUint32 KPhraseSplit = 0x01;
const TUint32 KSpecificMarkupInternalized = 0x04;

/**
@internalComponent
*/
typedef CRichTextStoreMap<CParagraphStyle> CStyleMap;
typedef CRichTextStoreMap<CParaAttribs> CParaAttribsMap;

/** Stores the paragraph and character formatting information for CRichText.

iParaIx stores the paragraph formatting, one entry per paragraph. Paragraphs
are pieces of text delimited by the Unicode Paragraph Delimiter character
U+2029.

iPhraseIx stores the character formatting.

Paragraph formatting information can be shared or not shared between
paragraphs. Sharing is only permitted for paragraphs that do not have any
character formatting applied to their text.

A phrase is a run of consistently formatted characters. A phrase may only
contain a paragraph delimiter in its last character. In other words, phrases
are not permitted to span paragraph boundaries.

iPhraseIx only stores formatting for the non-shared paragraphs. Therefore
adding up the lengths of all the phrases in iPhraseIx will yield the sum of the
non-shared paragraphs stored in iParaIx.

Note that the "nominal" paragraph delimiter at the end of the text which is not
stored by CRichText is covered by both character and paragraph formatting. So
the length of the final paragraph and final phrase (if appropriate) includes
this character.

@internalComponent
*/
class CRichTextIndex : public CBase
	{
public:
	static CRichTextIndex* NewL(const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer,
								const CRichText& aText,
								TInt aParaGran,TInt aPhraseGran);
	static CRichTextIndex* NewL(const CStreamStore& aStore, TStreamId aId,
								const CParaFormatLayer* aGlobalParaLayer,
								const CCharFormatLayer* aGlobalCharLayer,
								const CRichText& aText);
	~CRichTextIndex();
	//
	// Store/Restore functions - Deferred loading of pictures
	TStreamId StoreMarkupL(CStreamStore& aStore, const CStyleList* aStyleList) const;
	//
	void RestoreL(const CStreamStore& aStore, TStreamId aId,
				  const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer,
				  const CStyleList* aStyleList);
	//
	void StorePicturesL(CStreamStore& aStore, CStoreMap& aMap) const;
	void StorePicturesL(CStreamStore& aStore, CStoreMap& aMap, TInt aPos, TInt aLength) const;
	void DetachFromStoreL(CPicture::TDetach aDegree, TInt aPos, TInt aLength);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream, const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer,
					  const CStyleList* aStyleList);
	//
	// 3n Persist functions
	TBool HasMarkupData(const CFormatLayer* aGlobalParaFormatLayer) const;
	TPictureHeader PictureHeader(TInt aPos) const;
	TPictureHeader* PictureHeaderPtr(TInt aPos);
	//
	// Undo/CutPaste
	void CopyToStreamL(RWriteStream& aStream, TInt aPos, TInt aLength, TBool aCopyStyles) const;
	void PasteFromStreamL(const CStreamStore& aStore, RReadStream& aStream, TInt aPos, CParagraphStyle::TStylePasteMode aStylePasteMode, const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer);
	//
	// Content modifier functions
	void Reset(const CParaFormatLayer& aNormalParagraphStyle);
	void InsertL(TInt aPos, const TDesC& aBuf, const CParaFormatLayer& aGlobalParaFormatLayer);
	void InsertL(TInt aPos, const TPictureHeader& aPicHdr, TBool& aPictureOwnershipTaken);
	void SetForDeleteL(TIndexDeleteInfo& aInfo, TInt aPos, TInt aLength);
	TBool DeleteNow(TIndexDeleteInfo& aInfo);
	void DeleteFromParagraph(TInt aPos, TInt aLength);
	TBool DeleteParagraph(TInt aPos, TInt aLength);
	void Normalize(TInt aPos);
	//
	// Insert a new paragraph at position aPos, based on the 'Normal' global defaults.
	// Any explicit paragraph/character formatting is carried forward into this paragraph.
	void InsertParagraphL(TInt aPos, const CParaFormatLayer& aGlobalParaFormatLayer);
	//
	// Special behaviour format modifier functions.
	TBool InsertCharFormatIsActive();
	void SetInsertCharFormatL(const TCharFormatX& aFormat, const TCharFormatXMask& aMask, TInt aPos);
	void NewInsertCharFormatL(const TCharFormatX& aFormat,
		const TCharFormatXMask& aMask, TInt aPos);
	void UpdateInsertCharFormatL(const TCharFormatX& aFormat,
		const TCharFormatXMask& aMask);
	CCharFormatLayer* GetCurrentInsertCharFormat();
	void CancelInsertCharFormat();
	TBool DelSetInsertCharFormatL(TInt aPos, TInt aLength);
	//
	// Laydoc interface support
	void GetParagraphFormatL(CParaFormat* aFormat,TInt aPos) const;
	TInt GetChars(TCharFormatX& aFormat,TInt aPos) const;
	TInt GetPictureSizeInTwips(TSize& aSize, TInt aPos) const;
	CPicture* PictureHandleL(TInt aPos, MLayDoc::TForcePictureLoad aForceLoad) const;
	//
	// FormatText interface support
	void GetParaFormatL(CParaFormat* aFormat, TParaFormatMask& aUndeterminedMask, TInt aPos, TInt aLength,
						CParaFormat::TParaFormatGetMode aMode) const;
	void ApplyParaFormatL(const CParaFormat* aFormat, const TParaFormatMask& aMask, TInt aPos, TInt aLength);
	void GetCharFormat(TCharFormatX& aFormat, TCharFormatXMask& aMask,TInt aPos, TInt aLength) const;
	void ApplyCharFormatL(const TCharFormatX& aFormat, const TCharFormatXMask& aMask, TInt aPos, TInt aLength, TBool aRemoveSpecific = EFalse);
	void RemoveSpecificCharFormatL(TInt aPos, TInt aLength);
	void RemoveSpecificParaFormatL(TInt aPos, TInt aLength);
	//
	//
	void GetSpecificCharFormat(TCharFormatX& aFormat, TCharFormatXMask& aMask, TInt aPos) const;
	void GetSpecificCharFormatDirection(TCharFormatX& aFormat, TCharFormatXMask& aMask,
		TInt aPos, TBool aGetLeft) const;
	//
	// Paragraph style implementation
	void ApplyParagraphStyleL(const CParagraphStyle& aStyle, TInt aPos, TInt aLength, const CCharFormatLayer* aCharStyleNormal, CParagraphStyle::TApplyParaStyleMode aMode);
	void NotifyStyleChangedL(const CParagraphStyle* aTo, const CParagraphStyle* aFrom, const CParaFormatLayer& aGlobalParaFormatLayer, const CCharFormatLayer& aGlobalCharFormatLayer);
	const CParaFormatLayer* ParagraphStyle(TBool& aStylesChangesOverRange, TInt aPos, TInt aLength) const;
	//
	// Utility functions
	inline TInt ParagraphCount() const;
	TInt CharPosOfParagraph(TInt& aLength, TInt aParaOffset) const;
	TInt ParagraphNumberForPos(TInt& aPos) const;
	inline void SetSpecificMarkupInternalized(TBool aBool);
	inline TBool SpecificMarkupInternalized() const;
	void DocumentChanged()const;
	//
	// Speciality functions
	void AppendTakingSolePictureOwnershipL(const CRichTextIndex* aSource, const TGlobalLayerInfoAppend& aGlobalLayerInfo);
	void AppendParagraphL(const CParaFormatLayer* aGlobalParaFormatLayer, const CCharFormatLayer* aGlobalCharFormatLayer,
						  TInt aReplicas);
	TInt SharedParaCount(const CRichTextIndex* aSource) const;
	void GetSpecificParagraphFormatL(CParaFormat* aFormat,
									 TParaFormatMask& aMask,
									 TInt aPos) const;
private:
	enum TScanToPositionMode {EScanToPositionMatchLeft, EScanToPositionAbsolute};
	enum {EInsertCharFormatReset = -1};
	enum TPositionOrPhrase 	{EPositionOnly, EFollowingPhrase};
private:
	CRichTextIndex(const CRichText& aText);
	void ConstructL(const CParaFormatLayer* aGlobalParaFormat, const CCharFormatLayer* aGlobalCharFormat, TInt aParaGran, TInt aPhraseGran);
	//
	// Persistence/Copy,Paste
	void InternalizeRtiHeaderL(RReadStream& aStream, TRtPasteContext& aContext);
	void ExternalizeRtiHeaderL(RWriteStream& aStream, const TLogicalPosition& aEnd, const CStyleList* aStyleList) const;
	void ExternalizeReferencedStylesL(RWriteStream& aStream, const TLogicalPosition& aStart, const TLogicalPosition& aEnd) const;
	void InternalizeSharedFormatsL(RReadStream& aStream, const TRtPasteContext& aContext);
	void ExternalizeSharedFormatsL(RWriteStream& aStream, const TLogicalPosition& aStart, const TLogicalPosition& aEnd, const CStyleList* aStyleList) const;
	void InternalizeParaIxL(RReadStream& aStream, const TRtPasteContext& aContext);
	void ExternalizeParagraphFormatL(RWriteStream& aStream, const CParaFormatLayer& aSpecificParaFormatLayer, const CStyleList* aStyleList) const;
	CParaFormatLayer* InternalizeParagraphFormatL(RReadStream& aStream, const TRtPasteContext& aContext);
	CParaFormatLayer* PasteParagraphFormatL(RReadStream& aStream, const TRtPasteContext& aContext, CStyleMap* styleMap);
	RPhraseAttribsEntry* ExternalizeParaIxL(RWriteStream& aStream, const TLogicalPosition& aStart,
					const TLogicalPosition& aEnd, const CStyleList* aStyleList) const;
	void InternalizePhraseIxL(RReadStream& aStream, const CCharFormatLayer* aGlobalCharFormat);
	void ExternalizePhraseIxL(RWriteStream& aStream) const;
	void ExternalizePhraseIxL(RWriteStream& aStream,
							  const TLogicalPosition& aStartPos,
							  TLogicalPosition& aEndPos,
							  const RPhraseAttribsEntry* aVirtualTrailingText) const;
	inline void ExternalizePhraseCountL(RWriteStream& aStream, TInt aPhraseCount) const;
	void ExternalizePhrasesL(RWriteStream& aStream, TInt aStart, TInt aPhraseCount, const RPhraseAttribsEntry* aVirtualPhrase = NULL) const;
	TInt MarkStyleRegister(TDes8& aBuf, TInt aStartPara, TInt aEndPara) const;
	TInt MarkRegister(TDes8& aBuf, TInt aStartPara, TInt aEndPara) const;
	void GenerateAllPhraseLinksL();
	void GeneratePhraseLink(CCharFormatLayer* aPhraseCharFormatLayer, const CParaFormatLayer* aBase);
	void ExternalizeItemsPresentInStyleRegisterL(RWriteStream& aStream, TInt aRefStyleCount, const TDes8& aBuf) const;
	void ExternalizeItemsPresentInRegisterL(RWriteStream& aStream, TInt aParaCount, const TDes8& aBuf, const CStyleList* aStyleList) const;
	CParaAttribs* SharedParaAttribs(TUint8 aOrdinal);
	TUint8 RefNum(const CParaAttribs* aParaAttribs) const;
	//
	// Copy,Paste
	void PasteStylesL(RReadStream& aStream, CStyleMap& aMap, const TRtPasteContext& aContext);
	void PasteSharedFormatsL(RReadStream& aStream, CParaAttribsMap& aMap, const TRtPasteContext& aContext, CStyleMap* aStyleMap);
	void ImposeCharacterStyleL(CCharFormatLayer** aCharLayer);
	void PasteIxL(RReadStream& aStream, TRtPasteContext& aContext, const CParaAttribsMap& aMap, CStyleMap* aStyleMap/*,CParaAttribs* aSecondReserved*/);
	TInt PasteParaIxL(RReadStream& aStream, TRtPasteContext& aContext, TInt aCompleteParaCount, const CParaAttribsMap& aMap, RPhraseAttribsEntry*& aFirstParaVirtualPhrase, CStyleMap* aStyleMap);
	TParaAttribsEntry DoPasteIntoParaL(RReadStream& aStream, const CParaAttribsMap& aMap, const TRtPasteContext& aContext, CStyleMap* aStyleMap);
	TParaAttribsEntry DoPasteFirstIntoParaL(RReadStream& aStream, const CParaAttribsMap& aMap, const TRtPasteContext& aContext, RPhraseAttribsEntry*& aFirstParaVirtualPhrase, CStyleMap* aStyleMap);
	void DoPasteCleanup(TLogicalPosition& aNormalizePos, CParaAttribs* aReclaimed);
	TTextFragment GetTextFragmentL(RReadStream& aStream);
	void PastePhraseIxL(RReadStream& aStream, TRtPasteContext& aContext, const RPhraseAttribsEntry* aFirstParaVirtualPhrase);
	void DoPastePhraseL(RReadStream& aStream, const TRtPasteContext& aContext, RPhraseAttribsEntry& aPhrase);
	void RbPasteSharedFormatsL(TInt aRet);
	void NormalizeSharedList();
	void RbRemoveInsertedParaAttribsEntries(TInt aFirstParaInsertPos, TInt aInsertedParaCount);
	void RbPasteParaIxL(const TLogicalPosition& aPastePos, TInt aParaCount, TInt aRet);
	void RbPastePhraseIxL(const TLogicalPosition& aPos,TInt aPhraseCount, TInt aRet);
	void RbRemoveInsertedPhraseAttribsEntries(TInt aFirstPhraseInsertPos, TInt aInsertedPhraseCount);
	TUint8 ReadTUint8CountL(RReadStream& aStream) const;
	//
	// General utility
	void DoSoloInsertL(TInt aPos, TInt aLength);
	void ScanToPosition(TInt aCharPos, TScanToPositionMode aMode, TLogicalPosition* aLastUsed=NULL);  // Reset the logical position to the passed document position.
	inline TInt PhraseCount() const;
	TBool FirstPhraseOfParagraph() const;  // Interrogates current iPos record.
	TInt CurrentPhraseLength() const;  // Encapsulates constant/specific character formatting.
	TInt CurrentPhraseOffset() const;  // Encapsulates constant/specific character formatting.
	void CheckForUndetermined(const TCharFormatX& aFormatA, const TCharFormatX& aFormatB, TCharFormatXMask& aVaries) const;
	void GetCurrentRecords(TCurrentIndexRecords& aRecord) const;
	void GetPhraseFormat(TCurrentIndexRecords& aCurrent, TCharFormatX& aFormat, TCharFormatXMask& aMask, CCharFormatLayer*& aCharBase) const;
	TInt OwningParagraph(TInt aPos, TLogicalPosition* aLastUsed = NULL) const;
	TBool MergePhrases(TInt aPos);
	TBool MergePhrases(const TLogicalPosition& aPos);
	TBool DeleteInsertCharFormat();
	void ConsolidateAt(TInt aPosition, TPositionOrPhrase aPositionOrPhrase);
	void RemoveFromPhraseIx(TInt aPos, TInt aCount = 1);
	void SplitPhraseL(TInt aSplitPos);
	void SplitPhraseL(TInt aPhraseElement, TInt aPhraseOffset, CParaAttribs* aParaAttribs);
	void DoSplitPhraseL(RPhraseAttribsEntry& aCurrentPhrase, TInt aPhraseOffset, CParaAttribs* aParaAttribs);
	TUint8 SharedParaAttribsEntryCountL() const;
	void ModifySpecificFormatting(CParaFormatLayer& aPl, CCharFormatLayer& aCl, CParagraphStyle::TApplyParaStyleMode aMode);
//
// support for applycharformat
	static void ApplyCharFormatCleanup(TAny* aPtr);
	void ApplyCharFormatRollback();
	TInt MergePhrases(TInt aPhrase, RPhraseAttribsEntry& aPhraseAttribs, CParaAttribs& aParaAttribs);
	void SplitPhraseL(TInt aPhrase, TInt anOffset, RPhraseAttribsEntry& aPhraseAttribs, CParaAttribs& aParaAttribs);
	void Share(TParaAttribsEntry& aParaEntry, TInt aPhrase);
	//
	// Specific rollback
	void RbInsertPicture(CParaAttribs* aGoodParaAttribs);
	//
	// General implementation
	void NormalizeNow(const TLogicalPosition& aNormalizePos);
	TBool DeleteParagraphText(TInt& aLength);
	TBool DoDeleteFromParagraph(const TIndexDeleteInfo& aInfo);
	void TidyAfterDelete(const TIndexDeleteInfo& aInfo);
	CParaAttribs* ReserveCellL();
	CParaAttribs* ReserveCellLC();
	void DoNewInsertCharFormatL(const TCharFormatX& aFormat,
		const TCharFormatXMask& aMask, CCharFormatLayer* aBasedOn,
		CParaAttribs* aParaAttribs);
	TInt ParaLengthFromBuffer(TDesC& aBuf) const;
	inline const CStreamStore* ResolvedStreamStore(TInt aPos) const;
	//
	// Agenda functions
	void AppendSharedFormatsL(CParaAttribsMap& aMap, const CRichTextIndex* aSource, const TGlobalLayerInfoAppend& aGlobalLayerInfo);
	void AppendParaIndexL(const CRichTextIndex* aSource, const TGlobalLayerInfoAppend& aGlobalLayerInfo);
	void AppendPhraseIndexL(const CRichTextIndex* aSource, const TGlobalLayerInfoAppend& aGlobalLayerInfo);
	//
	// Shared list management utilities
	CParaAttribs* RequestReclaimShareL(CParaAttribs* aParaAttribs, TParaAttribsEntry* aParaEntry);
	CParaAttribs* RequestShareL(CParaAttribs* aParaAttribs, CCharFormatLayer* aCharFormat = NULL, CParaAttribs* aReservedCell = NULL);
	CParaAttribs* GetParaAttribsL(const CParaFormatLayer* aParaFormat, const CCharFormatLayer* aCharFormat, CParaAttribs* aReservedCell = NULL);
	CParaAttribs* RequestShare(const TLogicalPosition& aLogicalPosition);
	CParaAttribs* GetParaAttribs(const TLogicalPosition& aLogicalPosition);
	CParaAttribs* GetParaAttribs(CParaAttribs* aParaAttribs, CCharFormatLayer& aCharFormatLayer);
	CParaAttribs* FindSharedParaAttribs(const CParaFormatLayer& aParaFormatLayer, const CCharFormatLayer& aCharFormatLayer);
	void RebalanceIndex();
	void SplitParagraphAtPastePosL(TLogicalPosition& aPastePos, TLogicalPosition& aNewPos,
								   const CParaFormatLayer& aGlobalParaFormatLayer);
	//
	inline void SetPhraseSplit(TBool aBool);
	inline TBool PhraseSplit() const;
private:
	const CRichText& iText;  // backward reference to owning rich text.
	TUint32 iFlags;
	TInt iPendingNewPhrasePos;  // When *INSERTSTATE* is active, this is used to place the zero-length phrase.
	CParaAttribs* iRollbackParaAttribsHandle;
	__MUTABLE TDblQue<CParaAttribs> iSharedParaQueHead;
	__MUTABLE TLogicalPosition iLastUsed;
	CCharFormatLayer* iLastCharacterStyle;
public:
	CArrayFixSeg<TParaAttribsEntry>* iParaIx;  // One entry per paragraph
	CArrayFixSeg<RPhraseAttribsEntry>* iPhraseIx;  // One entry per phrase in the rich text
	__MUTABLE TLogicalPosition iPos;
	TInt iPictureCount;  // a count of the number of pictures contained by this text

	__DECLARE_TEST;
	};

////////////////////////////////////////

template <class T>
void CRichTextStoreMap<T>::__DbgTestInvariant() const
// Provides class invariants.  Explanations below:
//
	{
#ifdef _DEBUG
	TInt count = Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		TInt ref = (*iMap)[ii].iRefNumber;
		__ASSERT_DEBUG(ref > 0, User::Invariant());
		}
#endif
	}


template <class T>
CRichTextStoreMap<T>* CRichTextStoreMap<T>::NewLC(TInt aMaximumCapacity)
// Return a handle to a new instance of this class.
//
	{
	CRichTextStoreMap* self = new(ELeave) CRichTextStoreMap();
	CleanupStack::PushL(self);
	self->ConstructL(aMaximumCapacity);
	return self;
	}



template <class T>
void CRichTextStoreMap<T>::ConstructL(TInt aMaximumCapacity)
//
//
	{
	if (aMaximumCapacity > 0)
		{
		iMap = new(ELeave) CArrayFixFlat<TRichTextMapEntry>(aMaximumCapacity);
		iMap->SetReserveL(aMaximumCapacity);	// never need to allocate memory
		}
	iCapacity = aMaximumCapacity;
	}


template <class T>
CRichTextStoreMap<T>::~CRichTextStoreMap()
//
//
	{delete iMap;}


template <class T>
void CRichTextStoreMap<T>::Bind(TInt aRef, T* aT)
//
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(Count() <= iCapacity, Panic(ESharedFormatsMapOverFlow));

	TRichTextMapEntry entry(aRef, aT);
	iMap->AppendL(entry);  // will not fail: we have reserved the space

	__TEST_INVARIANT;
	}



template <class T>
void CRichTextStoreMap<T>::Bind(T* aS, T* aT)
//
//
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(Count() <= iCapacity, Panic(ESharedFormatsMapOverFlow));

	TRichTextMapEntry entry(aS, aT);
	iMap->AppendL(entry);  // will not fail: we have reserved the space

	__TEST_INVARIANT;
	}


template <class T>
T* CRichTextStoreMap<T>::Item(TInt aRef) const
//
//
	{
	TInt count = Count();
	if (count > 0)
		{
		const TRichTextMapEntry* entry = &(*iMap)[0];
		for (const TRichTextMapEntry* end = entry + count; entry < end; entry++)
			{
			if (entry->iRefNumber == aRef)
				return (T*)entry->iT;
			}
		}
	return NULL;
	}


template <class T>
T* CRichTextStoreMap<T>::Item(const T* aS) const
//
//
	{
	TInt count = Count();
	if (count > 0)
		{
		const TRichTextMapEntry* entry = &(*iMap)[0];
		for (const TRichTextMapEntry* end = entry + count; entry < end; entry++)
			{
			if (entry->iS == (TAny*)aS)
				return (T*)entry->iT;
			}
		}
	return NULL;
	}


#include "TXTINDEX.INL"


#endif
