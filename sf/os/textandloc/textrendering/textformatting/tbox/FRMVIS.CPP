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
* Definitions of non-printing characters' visibility classes.
*
*/


#include <s32strm.h>
#include "FRMVIS.H"

EXPORT_C TNonPrintingCharVisibility::TNonPrintingCharVisibility()
	:iVisible(EFNothingVisible)
	{}


EXPORT_C TNonPrintingCharVisibility::TNonPrintingCharVisibility(const TNonPrintingCharVisibility& aVisibility)
	:iVisible(aVisibility.iVisible)
	{}

EXPORT_C TNonPrintingCharVisibility& TNonPrintingCharVisibility::operator=(const TNonPrintingCharVisibility& aVisibility)
	{
	iVisible=aVisibility.iVisible;
	return *this;
	}

/** Externalises a TNonPrintingCharVisibility object to a write stream. The
presence of this function means that the standard templated operator<<()
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream Stream to which the object should be externalised. */
EXPORT_C void TNonPrintingCharVisibility::ExternalizeL(RWriteStream& aStream)const
	{
	aStream.WriteUint32L(iVisible);
	}

/** Internalises a TNonPrintingCharVisibility object from a read stream. The
presence of this function means that the standard templated operator>>()
(defined in s32strm.h) is available to internalise objects of this class.

@param aStream Stream from which the object should be internalised. */
EXPORT_C void TNonPrintingCharVisibility::InternalizeL(RReadStream& aStream)
	{
	iVisible=aStream.ReadUint32L();
	}

/** Sets all non-printing characters to be drawn using symbols. */
EXPORT_C void TNonPrintingCharVisibility::SetAllVisible()
	{
	iVisible=(TUint32)EFEverythingVisible;
	}

/** Sets all non-printing characters to be hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetNoneVisible()
	{
	iVisible=EFNothingVisible;
	}

/** Sets the visibility of tab stops.

@param aVisible True for visible tab stops. False for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetTabsVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFTabsVisible : iVisible & ~EFTabsVisible;
	}

/** Sets the visibility of space characters.

@param aVisible True for visible space characters. False for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetSpacesVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFSpacesVisible : iVisible & ~EFSpacesVisible;
	}

/** Sets the visibility of paragraph delimiters.

@param aVisible True for visible paragraph delimiters, false for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetParagraphDelimitersVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFParagraphDelimitersVisible
		: iVisible & ~EFParagraphDelimitersVisible;
	}

/** Sets the visibility of line breaks (force a new line without beginning a
new paragraph).

@param aVisible True for visible line breaks. False for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetLineBreaksVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFLineBreaksVisible : iVisible & ~EFLineBreaksVisible;
	}

/** Sets the visibility of potential hyphens (inserted before a line break
within a word).

@param aVisible True for visible potential hyphens, false for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetPotentialHyphensVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFPotentialHyphensVisible
		: iVisible & ~EFPotentialHyphensVisible;
	}

/** Sets the visibility of non-breaking hyphens (enclosing word is always kept
on the same line).

@param aVisible True for visible non-breaking hyphens, false for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetNonBreakingHyphensVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFNonBreakingHyphensVisible
		: iVisible & ~EFNonBreakingHyphensVisible;
	}

/** Sets the visibility of non-breaking spaces.

@param aVisible True for visible non-breaking spaces, false for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetNonBreakingSpacesVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFNonBreakingSpacesVisible
		: iVisible & ~EFNonBreakingSpacesVisible;
	}

/** Sets the visibility of page breaks.

@param aVisible True for visible page breaks, false for hidden. */
EXPORT_C void TNonPrintingCharVisibility::SetPageBreaksVisible(TBool aVisible)
	{
	iVisible = aVisible?
		iVisible | EFPageBreaksVisible
		: iVisible & ~EFPageBreaksVisible;
	}
