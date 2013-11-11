// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ////////////////////////             HIpcStream                 ///////////////////////////////////////
// 
//

/**
 
 Initializes HIpcStream data members.
 
 @param aHost A pointer to the buffer with the stream data
 @param aReadPos The position data has to be read from
 
 @see HIpcStream::SynchL()
 
 @panic SqlDb 7 In _DEBUG mode. aHost is NULL.
*/
inline HIpcStream::HIpcStream(MStreamBuf* aHost, TInt aReadPos):
	iHost(*aHost),
	iRPos(aReadPos),
	iWPos(0)
	{
	__ASSERT_DEBUG(aHost != NULL, __SQLPANIC(ESqlPanicInternalError));
	}

/**
Relases the stream buffer.
*/
inline HIpcStream::~HIpcStream()
	{
	iHost.Release();
	}

/**
@return The stream data size.
*/
inline TInt HIpcStream::SizeL()
	{
	return iHost.SizeL();
	}

/**
Synchronizes the stream data buffer with the stream.

@see HIpcStream::HIpcStream()
*/
inline void HIpcStream::SynchL()
	{
	iHost.SynchL();
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////             HIpcReadBuf                 //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes HIpcReadBuf data members.

@param aDes Points to the data which will be accessed via HIpcReadBuf instance.
*/
inline HIpcReadBuf::HIpcReadBuf(const TDesC8& aDes)
	{
	TUint8* ptr = const_cast <TUint8*> (aDes.Ptr());
	Set(ptr, ptr + aDes.Length(), MStreamBuf::ERead);
	}

/**
Creates an instance of HIpcReadBuf class.

@param aDes Points to the data which will be accessed via HIpcReadBuf instance.

@return A pointer to the created HIpcReadBuf instance.

@leave KErrNoMemory, an out of memory condition has occured;
*/
inline HIpcReadBuf* HIpcReadBuf::NewL(const TDesC8& aDes)
	{
	return new (ELeave) HIpcReadBuf(aDes);
	}
