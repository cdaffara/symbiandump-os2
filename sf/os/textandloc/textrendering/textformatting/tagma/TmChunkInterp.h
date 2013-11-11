/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TMCHUNKINTERP_H_
#define TMCHUNKINTERP_H_

#include "TMINTERP.H"

/**
 * Interpreter for finding text chunks adjoining a given document position.
 * @internalComponent
 */
class TTmChunkInterpreter : public TTmInterpreter
	{
public:
	enum TContainment
		{
		ENotInChunk = 0,
		ELeftEdgeOfChunk = 1,
		ERightEdgeOfChunk = 2,
		EMiddleOfChunk = 3
		};
	TTmChunkInterpreter(const CTmTextLayout& aTextLayout);
	/**
	 * Sets the iteration to the leftmost chunk of the line containing aDocPos.
	 * @return
	 *		ETrue if this succeeded, EFalse if the line is not formatted, or
	 *		contains no text.
	 */
	TBool ChunkSetToLeftOfLine(const TTmDocPosSpec& aDocPos);
	/**
	 * Returns the first character position in the current chunk.
	 */
	TInt ChunkStart() const { return iStart; }
	/**
	 * Returns one past the last character position in the current chunk.
	 */
	TInt ChunkEnd() const { return iEnd; }
	/**
	 * Returns ETrue if the current chunk is displayed right-to-left.
	 */
	TBool ChunkRightToLeft() const { return iRightToLeft; }
	/**
	 * Returns whether aDocPos is in the chunk or not, and if so where in the
	 * chunk it is.
	 */
	TContainment ChunkContainsPos(const TTmDocPosSpec& aDocPos) const;
	/**
	 * Moves to the next chunk right within this line.
	 * @return
	 *		ETrue if there is such a chunk, EFalse if the end of the line was
	 *		reached, so no chunk could be found.
	 */
	TBool ChunkNext();

private:
	TInt iStart;
	TInt iEnd;
	TBool iRightToLeft;
	};

#endif
