// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines Mixin class for a BufferManager
// 
//

#ifndef __EZBUFFERMANAGER_H__
#define __EZBUFFERMANAGER_H__

class CEZZStream;

/**
Interface class to manage input and output buffers for compression and de-compression

@publishedAll
@released
*/
class MEZBufferManager
	{
public:

	/**
	Initialise the stream with input and output buffers and starts reading

	@param aZStream the stream to initialise
	*/
	virtual void InitializeL(CEZZStream &aZStream) = 0;
	
	/**
	Set the stream's input buffer and starts reading
	
	@param aZStream the steam whose input buffer to set
	*/	
	virtual void NeedInputL(CEZZStream &aZStream) = 0;
	
	/**
	Set the stream's output buffer and start writing

	@param aZStream the steam whose output buffer to set
	*/	
	virtual void NeedOutputL(CEZZStream &aZStream) = 0;
	
	/**
	Finish writing to the stream

	@param aZStream the stream to complete writing to
	*/	
	virtual void FinalizeL(CEZZStream &aZStream) = 0;
	};

#endif

