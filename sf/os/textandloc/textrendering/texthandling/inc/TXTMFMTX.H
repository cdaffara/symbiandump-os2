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


#ifndef __TXTMFMTX_H__
#define __TXTMFMTX_H__

#include <e32std.h>
#include <txtfrmat.h>

// Defines interface for getting/setting formatting attributes.



class MFormatText
/** 
Specifies the mixin protocol for getting and setting character and paragraph 
format attributes. 

It is inherited by classes which support character and paragraph formatting, 
e.g. CRichText and CGlobalText. Its purpose is to allow developers to apply 
and retrieve formatting without needing to know which type of editable text 
is involved.

All functions defined in this class use a document position and a length. 
These values are only relevant to rich text. In general, they must be valid 
(i.e. the range must not extend beyond the bounds of the document), or a panic 
occurs. In global text, formatting is applied globally, so that the length 
and position values are ignored. 
@publishedAll
@released
*/
	{
public:
	//
	// Paragraph format attributes
	
	
	/** Gets the effective paragraph formatting which applies to a range of paragraphs. 
	Also gets a bit mask which indicates which values change over the region, 
	and whose value is therefore indeterminate.
	
	@param aFormat On return, contains the effective paragraph formatting for 
	the range of paragraphs. For global text, this is the paragraph formatting 
	of the entire text object. 
	@param aVaries On return, a bitmask indicating which paragraph format attributes 
	vary over the range of characters selected. For global text, this mask contains 
	a value of zero for every attribute because formatting is applied globally. 
	@param aPos The document position of the start of the range. 
	@param aLength The number of characters in the range. 
	@param aMode The default, EAllAttributes means that values for all paragraph 
	format attributes are written to aFormat. EFixedAttributes means that tabs, 
	bullets and borders are not written to aFormat. */
	virtual void GetParaFormatL(CParaFormat* aFormat,TParaFormatMask& aVaries,TInt aPos,TInt aLength,
								CParaFormat::TParaFormatGetMode aMode=CParaFormat::EAllAttributes)const=0;
	
	
	/** Applies paragraph formatting to a range of paragraphs. The attributes which 
	are set in the mask are taken from aFormat and applied. The attributes which 
	are not set in the mask are not applied.
	
	@param aFormat Contains the paragraph format attribute values to apply. 
	@param aMask Specifies which paragraph format attributes should be applied. 
	@param aPos The document position of the start of the range. 
	@param aLength The number of characters in the range. */
	virtual void ApplyParaFormatL(const CParaFormat* aFormat,const TParaFormatMask& aMask,TInt aPos,TInt aLength)=0;
	//
	// Character format attributes
	
	
	/** Gets the effective character formatting which applies to a range of characters. 
	Also gets a bit mask which indicates which values change over the region, 
	and whose value is therefore indeterminate. 
	
	@param aFormat On return, contains the character format values for the range 
	of characters. For global text, this is the character formatting of the entire 
	text object. 
	@param aVaries On return, indicates which character format attributes vary 
	over the range and whose values are therefore indeterminate. For global text, 
	this mask contains a value of zero for every attribute because formatting 
	is applied globally. 
	@param aPos The document position of the start of the range. 
	@param aLength The number of characters in the range. */
	virtual void GetCharFormat(TCharFormat& aFormat,TCharFormatMask& aVaries,TInt aPos,TInt aLength)const=0;
	
	
	/** Applies character formatting to a range of characters. The attributes which 
	are set in the mask are taken from aFormat and applied. The attributes which 
	are not set in the mask are not applied.
	
	@param aFormat Contains the character format attribute values to apply. 
	@param aMask Bitmask specifying which character format attributes should be 
	applied. 
	@param aPos Document position from which to apply the new character formatting. 
	
	@param aLength The number of characters to which the new formatting should 
	be applied. */
	virtual void ApplyCharFormatL(const TCharFormat& aFormat,const TCharFormatMask& aMask,TInt aPos,TInt aLength)=0;
	};

#endif

