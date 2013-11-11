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



const TUint32 KStyleListExternallyOwned=0x01;
const TUint32 KParaTypeIsSingle=0x02;

 

TBool CRichText::StyleListPresent()const
/** Tests whether the rich text object uses a style list. The style list may be 
owned by the object itself, or may be externally owned.

@return ETrue if the object uses a style list. EFalse if not */
	{return iStyleList.IsPtr() && iStyleList.AsPtr();}


 
CStyleList* CRichText::StyleList()const
/** Gets a pointer to the style list used by the rich text object.

@return The object's style list. NULL if no style list present. */
	{return (StyleListPresent()) ? iStyleList.AsPtr() : NULL;}


 

TInt CRichText::StyleCount()const
/** Gets the number of styles contained in the rich text object's style list. Returns 
zero if there is no style list present.

@return The number of styles in the style list */
	{return (StyleListPresent()) ? iStyleList->Count():0;}


void CRichText::SetStyleListExternallyOwned(TBool aExternallyOwned)
/** Sets whether the style list used by this rich text object is owned by 
the object itself, or is externally owned.

@param aExternallyOwned ETrue if the style list should be marked as 
externally owned, EFalse if not. */
	{
	if (aExternallyOwned) iFlags|=KStyleListExternallyOwned;
	else iFlags&=~KStyleListExternallyOwned;
	}


 

TBool CRichText::StyleListExternallyOwned()const
/** Tests whether the style list used by this rich text object is owned by the 
object itself, or is externally owned. This value is set using SetStyleListExternallyOwned().

@return ETrue if the rich text object's style list is externally owned. EFalse 
if it owns its style list, or if it does not use a style list. */
	{return iFlags&KStyleListExternallyOwned;}


 
void CRichText::NotifyStyleDeletedL(const CParagraphStyle* aStyle)
/** Removes a style from all paragraphs to which it applies. The formatting is 
reset to the global character and paragraph format layers, except that any 
specific formatting which has been applied to the paragraphs is retained.

Notes:

This function should be called on the text content object after deleting a 
style in the style list.

A panic occurs if the rich text object does not use a style list (this can 
be tested for using StyleListPresent()).

@param aStyle Pointer to the style to remove from the rich text object. */
	{NotifyStyleChangedL(NULL,aStyle);}


 

MRichTextStoreResolver* CRichText::StoreResolver()const
/** Gets the store resolver. A store resolver may be set during construction, or 
by calling CRichText::SetPictureFactory().

@return The store resolver. This determines which file store the picture is 
stored in. */
	{return iStoreResolver;}


 

MPictureFactory* CRichText::PictureFactory()const
/** Gets the picture factory. A picture factory may be set during construction, 
or by calling CRichText::SetPictureFactory().

@return The picture factory */
	{return iPictureFactory;}


