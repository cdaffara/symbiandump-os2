// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Sets the read position.
// 
//

inline void HIpcBuf::SetPos(TRead, TInt aPos)
	{
	iRPos = aPos;
	}

/**
Sets the write position.
*/
inline void HIpcBuf::SetPos(TWrite, TInt aPos)
	{
	iWPos = aPos;
	}

/**
@return Read position
*/
inline TInt HIpcBuf::Pos(TRead) const
	{
	return iRPos;
	}

/**
@return Write position
*/
inline TInt HIpcBuf::Pos(TWrite) const
	{
	return iWPos;
	}

/**
Moves the read position.
*/
inline TInt HIpcBuf::MovePos(TRead, TInt aOffset)
	{
	return iRPos += aOffset;
	}

/**
@return The difference between read and write positions.
*/
inline TInt HIpcBuf::Lag(TRead) const
	{
	return Ptr(ERead) - End(ERead);
	}

/**
@return The difference between the write position and the beginning of the buffer.
*/
inline TInt HIpcBuf::Lag(TWrite) const
	{
	return Ptr(EWrite) - iBuf.iData;
	}

inline TInt HIpcBuf::Mark(TRead) const
	{
	return Pos(ERead) + Lag(ERead);
	}

inline TInt HIpcBuf::Mark(TWrite) const
	{
	return Pos(EWrite) + Lag(EWrite);
	}
