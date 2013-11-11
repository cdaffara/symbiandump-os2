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


#ifndef __FRMVIS_H__
#define __FRMVIS_H__

#include <e32std.h>
#include <e32base.h>

class RReadStream;
class RWriteStream;

/** 
A set of flags to indicate which non-printing characters (e.g. space, tab,
paragraph break, etc.) should be drawn using symbols. By default, all
non-printing characters are hidden.

An instance of this class is used in
CTextLayout::SetNonPrintingCharsVisibility().
@publishedAll
@released
*/
class TNonPrintingCharVisibility

	{
private:
	enum TShowFormatChars
		{
		EFNothingVisible=0x000,
		EFTabsVisible=0x001,
		EFSpacesVisible=0x002,
		EFParagraphDelimitersVisible=0x004,
		EFLineBreaksVisible=0x008,
		EFPotentialHyphensVisible=0x010,
		EFNonBreakingHyphensVisible=0x020,
		EFNonBreakingSpacesVisible=0x040,
		EFPageBreaksVisible=0x080,
		EFEverythingVisible=0xffffffff
		};
public:
	IMPORT_C TNonPrintingCharVisibility();
	IMPORT_C TNonPrintingCharVisibility(const TNonPrintingCharVisibility& aVisibility);
	IMPORT_C TNonPrintingCharVisibility& operator=(const TNonPrintingCharVisibility& aVisibility);
	IMPORT_C void ExternalizeL(RWriteStream& aStream)const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void SetAllVisible();
	IMPORT_C void SetNoneVisible();
	IMPORT_C void SetTabsVisible(TBool aVisible);
	IMPORT_C void SetSpacesVisible(TBool aVisible);
	IMPORT_C void SetParagraphDelimitersVisible(TBool aVisible);
	IMPORT_C void SetLineBreaksVisible(TBool aVisible);
	IMPORT_C void SetPotentialHyphensVisible(TBool aVisible);
	IMPORT_C void SetNonBreakingHyphensVisible(TBool aVisible);
	IMPORT_C void SetNonBreakingSpacesVisible(TBool aVisible);
	IMPORT_C void SetPageBreaksVisible(TBool aVisible);

	/** Tests whether all non-printing characters are visible.
	@return ETrue if all non-printing characters are visible. EFalse if any or
	all hidden. */
	inline TBool AllVisible()const{return iVisible==EFEverythingVisible;};

	/** Tests whether all non-printing characters are hidden.
	@return ETrue if all non-printing characters are hidden. EFalse if any are
	visible. */
	inline TBool NoneVisible()const{return iVisible==EFNothingVisible;};

	/** Tests whether tab stop characters are visible.
	@return ETrue if tab stop characters are visible. EFalse if hidden. */
	inline TBool TabsVisible()const{return iVisible&EFTabsVisible;};

	/** Tests whether space characters are visible.

	Note: To get the the visibility of non-breaking space characters, use
	NonBreakingSpacesVisible() instead.
	@return ETrue if space characters are visible. EFalse if hidden. */
	inline TBool SpacesVisible()const{return iVisible&EFSpacesVisible;};

	/** Tests whether paragraph delimiters are visible.
	@return ETrue if paragraph delimiters are visible. EFalse if hidden. */
	inline TBool ParagraphDelimitersVisible()const{return iVisible&EFParagraphDelimitersVisible;};
	
	/** Tests whether forced line break characters are visible.
	@return ETrue if forced line break characters are visible. EFalse if hidden.
	*/
	inline TBool LineBreaksVisible()const{return iVisible&EFLineBreaksVisible;};
	
	/** Tests whether potential hyphen characters (inserted before a line break
	within a word) are visible.

	@return ETrue if potential hyphen characters are visible. EFalse if hidden.
	*/
	inline TBool PotentialHyphensVisible()const{return iVisible&EFPotentialHyphensVisible;};
	
	/** Tests whether non-breaking hyphens (enclosing word is always kept on
	the same line) are visible.
	@return ETrue if non-breaking hyphens are visible. EFalse if hidden. */
	inline TBool NonBreakingHyphensVisible()const{return iVisible&EFNonBreakingHyphensVisible;};
	
	/** Tests whether non-breaking spaces are visible.
	@return ETrue if non-breaking spaces are visible. EFalse if hidden. */
	inline TBool NonBreakingSpacesVisible()const{return iVisible&EFNonBreakingSpacesVisible;};

	/** Tests whether page break characters are visible.
	@return ETrue if page break characters are visible. EFalse if hidden. */
	inline TBool PageBreaksVisible()const{return iVisible&EFPageBreaksVisible;};

private:
	TUint32 iVisible;
	};

#endif


