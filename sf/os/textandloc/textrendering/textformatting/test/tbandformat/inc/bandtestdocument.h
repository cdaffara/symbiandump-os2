/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __BANDTESTDOCUMENT_H
#define __BANDTESTDOCUMENT_H

#include <e32std.h>

/**
@file
@internalComponent 
*/
class MDesCArray;
class CRichText;
class CBandMaintainer;
class CTmTextLayout;
class TBandTestAction;

/**
This class represents a specification for a document and a view. 

- The document content is represented by a descriptor array in which each descriptor is a paragraph. 

- The view is represented simply by the TInt aTopOfBand argument, which tells us how
many paragraphs to lose from the top of the band. So if it is zero, then we just initialise
the doc and don't scroll at all; if it is 1 we scroll down until 1 paragraph is removed from
the top of the band, and so on. Currently it's basically only ever set to 1 or 0, and is thus
used as a boolean "top_of_doc!=top_of_band"

Having initialised one of these objects with the document content and view, you can set up 
the CRichText and associated text views by calling SetupDocumentL. This will clear any text
that exists in the richtext, re-create it according to the specification for the document 
contents and set up the view according to the view specification.
*/
class TDocumentSpec
	{
public:
	IMPORT_C TDocumentSpec(const MDesCArray& aTextContents, TInt aTopOfBand);
	IMPORT_C void SetupDocumentL(CRichText& aRichText, CBandMaintainer& aMaintainer);
private:	
	const MDesCArray& iContents;
	TInt iTopOfBand;
	};
	
/**
Given an array of lines in aLines, this function will first retrieve a subset of the lines thought to be 
of particular interest to the reformatting algorithm. 

In general, for each paragraph in the lines supplied, lines of interest are:
(1) special cases:
	- the first line
	- the second line
	- the penultimate line
	- the last line
(2) normal cases
- at least 2 lines from the middle

So:
- if the paragraph contains 6 lines or fewer, we take them all, giving us the 4 special cases,
and two lines from the middle
- otherwise, we take the 4 special cases, and a couple from the middle

It then retrieves the start, middle and end-1 positions from each line, and put them, in order, in aPositions
*/	
IMPORT_C void PositionsToTestL(const RArray<TTmLineInfo>& aLines, RArray<TInt>& aPositions);

/**
Given:
- a document specification aDocumentSpec
- an action aAction,
- an array of positions aPositions

This function will iteratively execute the action at each position supplied. 
For each position N it will iteratively execute the action as follows:
- at N, with a length of 1
- at N, with a length of ((N+1)-N) +1
- at N, with a length of ((N+2)-N) +1

So if the positions are 1, 5 and 10, it will execute actions as follows:
pos 1, length 1
pos 1, length 5
pos 1, length 10
pos 5, length 1
pos 5, length 5
pos 10, length 1

It will reset the document and view each time.
*/
IMPORT_C void ExecuteTestL(CRichText& aRichText, CBandMaintainer& aMaintainer, 
	TBandTestAction& aAction, TDocumentSpec& aDocumentSpec, RArray<TInt>& aPositions);

/**
Given
- a document specification aDocumentSpec
- a string to insert aTextToInsert
- an array of positions aPositions

This function will initialise the document and view, then iteratively insert the string
at each position.

It will reset the document and view each time.
*/
IMPORT_C void InsertTextL(CRichText& aRichText, CBandMaintainer& aMaintainer, 
	CBandValidator& aValidator, TDocumentSpec& aDocumentSpec, const TDesC& aTextToInsert, RArray<TInt>& aPositions);

/**
Given a text layout aLayout, this function will retrieve all the visible lines and
place them in aVisibleLines.
*/
IMPORT_C void GetVisibleLinesL(const CTextLayout& aLayout, RArray<TTmLineInfo>& aVisibleLines);

/**
Given a text layout aTmLayout, this function will retrieve all the lines constituting the
formatted band and place them in aLinesInBand
*/
IMPORT_C void GetLinesInBandL(const CTmTextLayout& aTmLayout, RArray<TTmLineInfo>& aLinesInBand);


#endif