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


#ifndef __TXTLAYDC_H__
#define __TXTLAYDC_H__

#include <e32std.h>

class CParaFormat;
class TCharFormat;
class CPicture;


class MLayDoc
/** 
Specifies the interface for providing the information needed by the text 
layout engine to lay out a text object. 

Examples of derived classes are CRichText and CGlobalText.

At the heart of the interface are two interrogation functions; one gives the 
paragraph formatting for a document position, and the other gives a pointer 
descriptor to a series of contiguous characters with identical character 
formatting, and their character formatting. This information is sufficient 
for the layout engine to find the formatting and content of an entire document.

The constructor for the layout engine (CTextLayout) is prototyped as:
static CTextLayout *NewL(MLayDoc *aDoc,TInt aWrapWidth);
By viewing the text object purely in terms of the MLayDoc interface, the layout 
engine is insulated from any editable text implementation issues.
@see static CTextLayout *NewL(MLayDoc *aDoc,TInt aWrapWidth)
@publishedAll
@released
*/
	{
	// Defines interface for extracting content & format information from text source.
	// Designed to be 'mixed-in' with other classes.
	//
public:
	
	/** Used as a parameter to the PictureHandleL() function to control whether 
	picture data should be loaded into memory or not. */
	enum TForcePictureLoad 
		{
		/** Do not load the picture data into memory. */
		EForceLoadFalse,
		/** Load the picture data into memory. */
		EForceLoadTrue
		};
public:
		
	/** Gets the the number of characters in the document.
	
	@return The number of characters contained in the document. */
	virtual TInt LdDocumentLength()const=0;  // Does not include final document terminator

	/** Gets the document position of the start of the paragraph containing a 
	specified position.
	
	@param aCurrentPos A valid document position. On return contains the 
	document position of the first character in the paragraph. 
	@return The number of characters skipped in scanning to the start of the 
	paragraph. */
	virtual TInt LdToParagraphStart(TInt& aCurrentPos)const=0;
		
	/** Gets the effective paragraph formatting which applies to the paragraph 
	which contains a specified document position. On return, aFormat is filled 
	with values for all paragraph format attributes.
	
	@param aFormat On return, filled with the paragraph's effective paragraph 
	formatting. 
	@param aPos Any document position within the paragraph of interest. */
	virtual void GetParagraphFormatL(CParaFormat* aFormat,TInt aPos)const=0;
	
	/** Gets a constant pointer descriptor to a portion of the text object with 
	constant character formatting. The view starts at the document position 
	specified, and ends at: 
	
	the last character which shares the same character formatting (rich text 
	only), or
	
	the end of the document, or
	
	the end of the segment, if segmented storage is being used
	
	whichever occurs first. 
	
	Also fills a character format object with the character formatting of the 
	range of characters.
	
	@param aView On return, a constant pointer to a portion of the text. 
	@param aFormat On return, contains the character formatting of the text. 
	@param aStartPos The start position for the view. Must be a valid document 
	position, or a panic occurs. */
	virtual void GetChars(TPtrC& aView,TCharFormat& aFormat,TInt aStartPos)const=0;	

	/** Gets the size of a picture located at a specified document position.
	
	Note: The global text implementation of this function always returns 
	KErrNotFound because global text does not support pictures.
	
	@param aSize On return, contains the size of the picture located at aPos.
	@param aPos Document position of the picture. 
	@return KErrNotFound if there is no picture at the specified document 
	position, KErrNone if there is. */
	virtual TInt GetPictureSizeInTwips(TSize& aSize, TInt aPos)const=0;

	/** Gets a pointer to the picture located at a specified document position, 
	if one exists. If the picture is not in memory, the function loads it (if 
	the second argument has a value of EForceLoadTrue).
	
	Note: The global text implementation of this function always returns NULL 
	because global text does not support pictures.
	
	@param aPos Document position of the picture character. 
	@param aForceLoad If the picture is not loaded into memory, EForceLoadTrue 
	loads it using the picture factory; EForceLoadFalse does not, and in this 
	case, the function returns NULL. 
	@return A pointer to the picture located at aPos. NULL if aPos does not 
	specify a picture character, or if there is a picture at aPos which is not 
	in memory, and the second argument is EForceLoadFalse. */
	virtual CPicture* PictureHandleL(TInt aPos,TForcePictureLoad aForceLoad=EForceLoadTrue)const=0;

	/** Tests whether a page break occurs within a range of characters. A page 
	table should have been set up first: see CPlainText::SetPageTable().
	
	@param aPos The document position from which to begin searching for a page 
	break.
	@param aLength The number of characters to search for a page break, 
	beginning at aPos. The default is zero. 
	@return ETrue if a page break occurs within the specified range, otherwise 
	EFalse. */
	virtual TBool EnquirePageBreak(TInt aPos,TInt aLength=0)const=0;
	
	/** This function should be implemented by text objects which support 
	paragraph labels. Each paragraph may have at most one label. By default, 
	the functions provided by MLayDoc apply to the main body of text. This 
	function is provided to change this so that the MLayDoc object is an 
	individual paragraph label, (each of which is treated as a separate 
	document). Paragraph labelling has not been implemented by global or rich 
	text.
	
	This function should select the paragraph label associated with the 
	paragraph containing document position aPos (aPos is a document position 
	in the main document).
	
	@param aPos A document position within a paragraph. This paragraph's label 
	should be set as the main MLayDoc document. 
	@return Should return EFalse if the paragraph has no label, in which case 
	the function has no effect, ETrue otherwise. */
	virtual TBool SelectParagraphLabel(TInt aPos)=0;

	/** Cancels a paragraph label selection made by a call to 
	SelectParagraphLabel() so that the main document reverts back to the main 
	body of text. */
	virtual void CancelSelectLabel()=0;
	
	/** This function is reserved for future purpose.
	It should not be used externally for now.
	@internalAll */
	IMPORT_C virtual void MLayDoc_Reserved_1();
	};

#endif

