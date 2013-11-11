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


CRichText* CWordModel::Text()
/** Gets the engine's rich text object.

@return Engine's rich text object */
	{return iText;}

CStyleList* CWordModel::StyleList()
/** Gets the engine's style list object.

@return Engine's style list object */
	{return iStyleList;}

CPrintSetup* CWordModel::PrintSetup()
/** Gets the engine's print setup object.

@return Engine's print setup object */
	{return iPrintSetup;}

CArrayFix<TInt>* CWordModel::PageTable()
/** Gets the engine's page table array.

A page table is an array of integers; each integer represents the number of 
characters on a page. It is required for pagination.

@return Page table array */
	{return iPageTable;}

CStyleShortCutList* CWordModel::StyleShortCutList()
/** Gets the engine's style short cuts array.

This object stores a list of keyboard shortcuts to the styles (as returned 
by StyleList()). The array is in the same order as the style list array.

@return Engine's style short cuts array */
	{return iStyleShortCutList;}

TChar& CWordModel::NormalStyleShortCut()
/** Gets the keybord short cut for the normal style.

@return Keybord short cut for the normal style */
	{return iNormalStyleShortCut;}
