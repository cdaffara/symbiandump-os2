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


 

RParagraphStyleInfo::RParagraphStyleInfo()
	: iStyle(NULL),iStyleForNextPara(NULL)
/** This default C++ constructor is used to construct the object, initializing 
both style pointers to NULL. */
	{}

RParagraphStyleInfo::RParagraphStyleInfo(CParagraphStyle* aStyle,CParagraphStyle* aStyleForNextPara)
	: iStyle(aStyle),iStyleForNextPara(aStyleForNextPara)
/** The C++ constructor is used to construct the object with a pointer to the style 
information for the current paragraph and optionally a pointer to the style 
information for the next paragraph.

@param aStyle The style to apply to the current paragraph. 
@param aStyleForNextPara Optionally, the style to apply to the next paragraph. 
By default, NULL. */
	{}

 

void RParagraphStyleInfo::Discard()
/** Deletes the style pointer owned by the object. The pointer to the style for 
the following paragraph is not deleted. */
	{delete iStyle;}


 

CCharFormatLayer* CParagraphStyle::CharFormatLayer()const
/** Gets a pointer to the character format layer owned by the object.

Note

The style's character formatting is set and retrieved using this pointer.

@return Pointer to the style's global character format layer. */
	{return iCharFormatLayer;}

 
TInt CParagraphStyle::OutlineLevel()const
/** Gets the style's outline level.

@return The style's outline level. */
	{return iOutlineLevel;}


 

void CParagraphStyle::SetOutlineLevel(TInt aOutlineLevel)
/** Sets the style's outline level.

@param aOutlineLevel The style's new outline level. */
	{iOutlineLevel=aOutlineLevel;}


 

void CParagraphStyle::SetType(TUid aType)
/** Sets the style's type UID. On construction, the style's type UID is initialized 
to KUserDefinedParagraphStyleUid to distinguish it from ordinary paragraph 
format layers, which have a type of KNormalParagraphStyleUid. This function 
can be used to change it to another value.

@param aType The style's type UID. */
	{iType=aType;}


 
CParagraphStyle* CParagraphStyle::CloneL()const
/** Creates and returns a CParagraphStyle object which is a clone of the current 
style.

@return Pointer to a clone of the current style. */
	{return STATIC_CAST(CParagraphStyle*,DoCloneL());}


 

const RParagraphStyleInfo& CStyleList::operator[](TInt aIndex)const
/** Gets a style from the style list, from its index into the array. Two versions 
are supplied. The compiler chooses the appropriate version based on the use 
made of the returned reference. If it is used in an expression where the reference 
can be modified, then the non-const version is chosen.

@param aIndex The index of the style into the list. The first style is at 
position zero. Must be within the bounds of the array or a panic occurs. 
@return A const reference to the style at position aIndex in the array. 
@return A non-const reference to the style at position aIndex in the array. */
/** Gets a style from the style list, from its index into the array. Two versions 
are supplied. The compiler chooses the appropriate version based on the use 
made of the returned reference. If it is used in an expression where the reference 
can be modified, then the non-const version is chosen.

@param aIndex The index of the style into the list. The first style is at 
position zero. Must be within the bounds of the array or a panic occurs. 
@return A const reference to the style at position aIndex in the array. 
@return A non-const reference to the style at position aIndex in the array. */
	{return iList->At(aIndex);}


RParagraphStyleInfo& CStyleList::operator[](TInt aIndex)
	{return iList->At(aIndex);}

 

TInt CStyleList::Count()const
/** Gets the number of styles in the style list.

@return The number of styles in the style list */
	{return (iList) ? iList->Count() : 0;}
