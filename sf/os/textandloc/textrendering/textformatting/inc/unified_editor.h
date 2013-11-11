/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __UNIFIED_EDITOR_H__
#define __UNIFIED_EDITOR_H__ 1

#include <tagma.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma_internal.h>
#endif

/*
The header file for MUnifiedEditor, an interface that unifies editing operations on backing text and layout,
which are traditionally separated between ETEXT (backing text) and FORM (layout). It is not dependent
on FORM or ETEXT, although there is an implementation (TFormAndTextEditor) using those components.
*/

/**
Base class for optional interfaces
@internalComponent
*/
class MTmOptionalInterface

	{
	};

/**
UIDs for standard optional interfaces
@internalComponent
*/
const TUint KUidMUnifiedEditorStyleSupport = 0x100092B6;
const TUint KUidMUnifiedEditorPictureSupport = 0x100092B7;
const TUint KUidMUnifiedEditorFieldSupport = 0x100092B8;
const TUint KUidMUnifiedEditorClipboardSupport = 0x100095EF;

/**
An interface class for text editing.

An object that supports text editing has some backing text - zero or more Unicode characters - and
character and paragraph formats applicable to ranges of these characters. A paragraph is a sequence of characters
terminating either at the end of the document or after the next paragraph separator, whichever comes first.

The format for a given position is derived from up to three formatting layers. The base layer is the lowest and
provides default attributes. These may be optionally overridden by attributes from a named style, and these in
turn may be overridden by specific formatting. Text editing objects may support any or none of these layers; they
may in fact ignore all attempts to set format attributes.
@internalComponent
*/
class MUnifiedEditor

	{
public:
	/**
	 * constants used when getting formats
	 */
	enum TFormatLevel
		{
		ESpecific,			// format applied by the user
		EEffective			// the actual format: a combination of base and specific formats
		};
	class MStyleSupport : public MTmOptionalInterface
	/**
	An optional interface to support styles
	@internalComponent
	*/
		{
	public:
		/**
		 * Creates a style.
		 */
		virtual TInt CreateStyleL(const RTmStyle& /*aStyle*/) { return KErrNotSupported; }
		/**
		 * Sets the attributes of a style.
		 */
		virtual TInt ChangeStyleL(const RTmStyle& /*aStyle*/) { return KErrNotSupported; }
		/**
		 * Applies the named style to the specified run of text.
		 */
		virtual TInt SetStyleL(TInt aPos, TInt aLength, const TDesC& aName) = 0;
		/**
		 * Renames a style.
		 */
		virtual TInt RenameStyleL(const TDesC& /*aOldName*/,const TDesC& /*aNewName*/) { return KErrNotSupported; }
		/**
		 * Deletes the named style.
		 */
		virtual TInt DeleteStyleL(const TDesC& /*aName*/) { return KErrNotSupported; }
		/**
		 * Get the number of named styles.
		 */
		virtual TInt StyleCount() const = 0;
		/**
		 * Gets the style set at the document position aPos. If no style is set
		 * there return an empty string and the run length over which no style
		 * applies.
		 */
		virtual void GetStyle(TInt aPos, TPtrC& aName, TInt& aRunLength) const = 0;
		/**
		 * Gets the attributes of a named style by completing aStyle from its
		 * name.
		 */
		virtual TInt GetStyleByNameL(const TDesC& aName, RTmStyle& aStyle) const = 0;
		/**
		 * Gets the name and attributes of a style by index; use this in
		 * conjunction with StyleCount to enumerate existing styles.
		 */
		virtual TInt GetStyleByIndexL(TInt aIndex, RTmStyle& aStyle) const = 0;
		};

	/**
	An optional interface to support embedded pictures
	@internalComponent
	*/
	class MPictureSupport : public MTmOptionalInterface

		{
	public:
		/**
		 * Inserts a picture, passing ownership in.
		 */
		virtual void InsertPictureL(TInt aPos, const TPictureHeader& aPictureIn) = 0;
		/**
		 * Deletes picture character in the text at aPos, if any. Ownership of
		 * any picture is passed to the caller, therefore Picture(aPos,
		 * aPictureOut) must have been called previously, and
		 * aPictureOut.DeletePicture() must be called in the future to avoid a
		 * memory leak.
		 * If the character at aPos is a picture character, it will be deleted,
		 * regardless of whether or not a picture was actually attatched.
		 */
		virtual void DropPictureL(TInt aPos) = 0;
		/**
		 * Gets the picture if any at aPos. No picture is indicated by
		 * aPictureOut.iPictureType containing KUidNull.
		 * Ownership of the picture is retained. Note that aPictureOut is
		 * merely overwritten by this function: aPictureOut.DeletePicture() is
		 * NOT called!
		 */
		virtual void Picture(TInt aPos, TPictureHeader& aPictureOut) const = 0;
		};

	/**
	An optional interface to support clipboard operations
	@internalComponent 
	*/
	class MClipboardSupport : public MTmOptionalInterface
	
		{
	public:
		/**
		 * Copy the text and formatting specified to the stream, updating
		 * the dictionary as appropriate.
		 */
		virtual void CopyToStoreL(CStreamStore& aStore, CStreamDictionary& aDictionary,
			TInt aPos, TInt aLength) const = 0;
		/**
		 * Insert text and formatting from the stream at aPos.
		 */
		virtual void PasteFromStoreL(const CStreamStore& aStore,
			const CStreamDictionary& aDictionary, TInt aPos) = 0;
		};

	// VIRTUAL FUNCTIONS

	// getters

	/**
	 * Returns the optional interface with the specified UID, or 0 if it is not
	 * supported.
	 */
	virtual MTmOptionalInterface* Interface(TUint /*aId*/) { return NULL; }
	/**
	 * Returns the length of the document in characters, not including any
	 * notional final paragraph separator.
	 */
	virtual TInt DocumentLength() const = 0;
	/**
	 * Gets some text starting at aPos. The amount of text returned may be as
	 * much or as little as is convenient to the implementation, but must be
	 * at least one character. The text is raw Unicode text including any
	 * paragraph separators.
	 */
	virtual void GetText(TInt aPos,TPtrC& aText) const = 0;
	/**
	 * Gets the base character and paragraph formats
	 */
	virtual void GetBaseFormatL(TTmCharFormat& aCharFormat,RTmParFormat& aParFormat) const = 0;
	/**
	 * Gets the specific or effective character format and the run over which
	 * that format applies.
	 */
	virtual void GetCharFormat(TInt aPos,TFormatLevel aLevel,
							   TTmCharFormatLayer& aFormat,TInt& aRunLength) const = 0;
	/**
	 * Gets the specific or effective paragraph format and the run over which
	 * that format applies.
	 */
	virtual void GetParFormatL(TInt aPos,TFormatLevel aLevel,
							   RTmParFormatLayer& aFormat,TInt& aRunLength) const = 0;

	// setters

	/**
	 * Inserts text at aPos, optionally applying the specified character and
	 * paragraph formats. Unspecified attributes take the format at the
	 * insertion point; what this means in detail is implementation-dependent.
	 */
	virtual void InsertTextL(TInt aPos,const TDesC& aText,
							 const TDesC* aStyle = NULL,
							 const TTmCharFormatLayer* aCharFormat = NULL,
							 const RTmParFormatLayer* aParFormat = NULL) = 0;
	/**
	 * Deletes aLength characters starting at aPos.
	 */
	virtual void DeleteTextL(TInt aPos,TInt aLength) = 0;
	/**
	 * Sets the base character and paragraph formats
	 */
	virtual void SetBaseFormatL(const TTmCharFormat& aCharFormat,const RTmParFormat& aParFormat) = 0;
	/**
	 * Sets specific character attributes beginning at aPos for aLength characters.
	 */
	virtual void SetCharFormatL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat) = 0;
	/**
	 * Sets specific paragraph attributes beginning at aPos for aLength characters.
	 */
	virtual void SetParFormatL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat) = 0;
	/**
	 * Deletes specific character attributes beginning at aPos for aLength characters.
	 */
	virtual void DeleteCharFormatL(TInt aPos,TInt aLength) = 0;
	/**
	 * Deletes specific paragraph attributes beginning at aPos for aLength characters.
	 */
	virtual void DeleteParFormatL(TInt aPos,TInt aLength) = 0;

	// NON-VIRTUAL FUNCTIONS
	/**
	 * Reads text into a writable descriptor.
	 *
	 * @see GetText(TInt, TPtrC) const
	 */
	IMPORT_C void GetText(TInt aPos, TDes& aText) const;
	/**
	 * Returns the interface for manipulating styles, if applicable.
	 */
	inline MStyleSupport* StyleSupport();
	inline const MStyleSupport* StyleSupport() const;
	/**
	 * Returns the interface for manipulating pictures, if applicable.
	 */
	inline MPictureSupport* PictureSupport();
	inline const MPictureSupport* PictureSupport() const;
	/**
	 * Returns the interface for clipboard operations, if applicable.
	 */
	inline MClipboardSupport* ClipboardSupport();
	inline const MClipboardSupport* ClipboardSupport() const;
	};

// inline functions

MUnifiedEditor::MStyleSupport* MUnifiedEditor::StyleSupport()
	{
	return static_cast<MStyleSupport*>(Interface(KUidMUnifiedEditorStyleSupport));
	}

const MUnifiedEditor::MStyleSupport* MUnifiedEditor::StyleSupport() const
	{
	return static_cast<MStyleSupport*>(
		const_cast<MUnifiedEditor*>(this)->Interface(KUidMUnifiedEditorStyleSupport));
	}

MUnifiedEditor::MPictureSupport* MUnifiedEditor::PictureSupport()
	{
	return static_cast<MPictureSupport*>(Interface(KUidMUnifiedEditorPictureSupport));
	}

const MUnifiedEditor::MPictureSupport* MUnifiedEditor::PictureSupport() const
	{
	return static_cast<MPictureSupport*>(
		const_cast<MUnifiedEditor*>(this)->Interface(KUidMUnifiedEditorPictureSupport));
	}

MUnifiedEditor::MClipboardSupport* MUnifiedEditor::ClipboardSupport()
	{
	return static_cast<MClipboardSupport*>(Interface(KUidMUnifiedEditorClipboardSupport));
	}

const MUnifiedEditor::MClipboardSupport* MUnifiedEditor::ClipboardSupport() const
	{
	return static_cast<MClipboardSupport*>(
		const_cast<MUnifiedEditor*>(this)->Interface(KUidMUnifiedEditorClipboardSupport));
	}

#endif // __UNIFIED_EDITOR_H__
