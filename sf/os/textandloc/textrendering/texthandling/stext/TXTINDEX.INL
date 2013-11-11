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


void CRichTextIndex::ExternalizePhraseCountL(RWriteStream& aStream,TInt aPhraseCount)const
	{aStream.WriteInt32L(aPhraseCount);}


TInt CRichTextIndex::ParagraphCount()const
	{return (iParaIx)?iParaIx->Count():0;}


TInt CRichTextIndex::PhraseCount()const
	{return (iPhraseIx)?iPhraseIx->Count():0;}


const CStreamStore* CRichTextIndex::ResolvedStreamStore(TInt aPos)const
	{
	MRichTextStoreResolver* res=iText.StoreResolver();
	return (res)
		? &(res->StreamStoreL(aPos))
		: NULL;
	}


void CRichTextIndex::SetPhraseSplit(TBool aBool)
	{
	if (aBool) iFlags|=KPhraseSplit;
	else iFlags&=~KPhraseSplit;
	}


TBool CRichTextIndex::PhraseSplit()const
	{return iFlags&KPhraseSplit;}


void CRichTextIndex::SetSpecificMarkupInternalized(TBool aBool)
	{
	if (aBool) iFlags|=KSpecificMarkupInternalized;
	else iFlags&=~KSpecificMarkupInternalized;
	}


TBool CRichTextIndex::SpecificMarkupInternalized()const
	{return iFlags&KSpecificMarkupInternalized;}


RPhraseAttribsEntry::RPhraseAttribsEntry(const RPhraseAttribsEntry& aPhrase)
	{AssignAndRelease(aPhrase);}


RPhraseAttribsEntry& RPhraseAttribsEntry::operator=(const RPhraseAttribsEntry& aPhrase)
	{
	AssignAndRelease(aPhrase);
	return *this;
	}

inline TBool CParaAttribs::IsShared()const
	{return iRefCount>0;}


inline TBool RPhraseAttribsEntry::IsPicturePhrase()const
	{return iLength==EPictureIndicator;}

inline TInt RPhraseAttribsEntry::Length() const
	{return (iLength>=0) ? iLength : EPicturePhraseLength;}


template <class T>
TInt CRichTextStoreMap<T>::Count()const
// Return a count of the number of items in the map
//
	{return (iMap) ? iMap->Count() : 0;}


template <class T>
const TRichTextMapEntry& CRichTextStoreMap<T>::At(TInt aIndex)const
// Return the map entry specified by the the offset aIndex.
//
	{return (*iMap)[aIndex];}
