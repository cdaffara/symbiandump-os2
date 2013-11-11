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


#include "TmChunkInterp.h"

TTmChunkInterpreter::TTmChunkInterpreter(const CTmTextLayout& aTextLayout)
	: TTmInterpreter(TTmInterpreterParam(aTextLayout))
	{
	}

TBool TTmChunkInterpreter::ChunkSetToLeftOfLine(
	const TTmDocPosSpec& aDocPos)
	{
	if(!DocPosToLine(aDocPos))
		return EFalse;
	return ChunkNext();
	}

TTmChunkInterpreter::TContainment
	TTmChunkInterpreter::ChunkContainsPos(const TTmDocPosSpec& aDocPos) const
	{
	if (aDocPos.iPos < iStart)
		return ENotInChunk;
	if (iEnd < aDocPos.iPos)
		return ENotInChunk;
	if (iStart < aDocPos.iPos && aDocPos.iPos < iEnd)
		return EMiddleOfChunk;
	// sort out the boundary cases.
	switch (aDocPos.iType)
		{
	case TTmDocPosSpec::ERightToLeft:
		if (iRightToLeft)
			return aDocPos.iPos == iStart? ERightEdgeOfChunk : ELeftEdgeOfChunk;
		break;
	case TTmDocPosSpec::ELeftToRight:
		if (!iRightToLeft)
			return aDocPos.iPos == iStart? ELeftEdgeOfChunk : ERightEdgeOfChunk;
		break;
	case TTmDocPosSpec::ELeading:
		if (aDocPos.iPos == iStart)
			return iRightToLeft? ERightEdgeOfChunk : ELeftEdgeOfChunk;
		break;
	case TTmDocPosSpec::ETrailing:
		if (aDocPos.iPos == iEnd)
			return iRightToLeft? ELeftEdgeOfChunk : ERightEdgeOfChunk;
		break;
		}
	return ENotInChunk;
	}

TBool TTmChunkInterpreter::ChunkNext()
	{
	while (Next() && Op() != EOpLine)
		{
		if (Op() == EOpText || Op() == EOpSpecialChar)
			{
			iStart = StartChar();
			iEnd = EndChar();
			iRightToLeft = RightToLeft();
			return ETrue;
			}
		else
			Skip();
		}
	return EFalse;
	}
