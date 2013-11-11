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


#ifndef INLINETEXT_H_
#define INLINETEXT_H_

#include <e32std.h>
#include <tagma.h>

/**
@publishedPartner
*/
const TUid KInlineTextApiExtensionUid = { 0x101FD03D };


/**
Class used to provide custom formatting functionality within Form
as an extended interface (via the GetExtendedInterface mechanism
supplied in MTmSource). Basically allows inline text insertion,
that is, insertion of non-backing store text into the CTmTextLayout 
formatting data used when drawing to the graphics device.

Implementors of derived classes need to ensure that, in addition
to anything else it does, their overload of GetExtendedInterface
reacts to being prompted with the UID KInlineTextApiExtensionUid
by returning their class cast as an MTmInlineTextSource. It should
also invoke GetExtendedInterface on its other parent for any
unrecognised UIDs.

@publishedPartner
@released
@see MTmSource::GetExtendedInterface
@see MFormCustomInterfaceProvider
*/
class MTmInlineTextSource
	{
public:
	/**
	Reports the next position into which inline text should be inserted
	@param aFrom
		The document position and character edge to start from.
	@param aMaxLength
		The maximum length within which to report inline text.
		It means that inline text at position X should be reported if
		aFrom <= X && X < aFrom + aMaxLength.
		Also report trailing inline text at position aFrom + aMaxLength
		because it is really attached to the preceding character.
		Always report only the first inline text position >= aFrom.
	@param aNext
		On exit the position of the next bit of inline text to be inserted.
		N.B. The position of trailing text following position N and the 
		position of leading text preceding position N+1 are both 
		considered to be N+1 - and the trailing text comes first so if
		aFrom specifies a leading edge do not report trailing edge
		inline text unless its position is greater than aFrom.
		A panic EBadReturnValue will result otherwise.
	@return
		KErrNone if a position is found within the specified range,
		KErrNotFound otherwise.
	@post
		if KErrNone returned then aFrom <= aNext
		&& GetInlineText(aNext).Length() != 0
		&& (GetInlineText(X).Length() == 0 for all
		TTmDocPos X such that aFrom < X && X < aNext)
		else if KErrNotFound returned
		GetInlineText(X).Length() == 0 for all
		TTmDocPos X such that aFrom <= X && X < aFrom + aMaxLength
	*/
	virtual TInt GetNextInlineTextPosition(const TTmDocPos& aFrom, TInt aMaxLength, TTmDocPos& aNext) = 0;

	/**
	Gets a view of the text to be inserted at aAt.
	@param aAt
		Document position, including character edge, being queried.
	@return
		Any inline text that should be attached to the specified character edge at aAt.
	*/
	virtual TPtrC GetInlineText(const TTmDocPos& aAt) = 0;
	};

#endif	// INLINETEXT_H_

