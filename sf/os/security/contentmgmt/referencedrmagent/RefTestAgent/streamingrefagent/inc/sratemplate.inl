/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* The functions defined in this file provide template functions which are used
* by both streaming reference and test agent.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRATEMPLATE_INL
#define SRATEMPLATE_INL

#include <f32file.h>

const TInt KExpandSize = 50; ///< Granularity of dynamic buffer.

template <class C> TPtr8 EncodeLC(C& object, HBufC8*& aDes)
/**
	Externalizes an object of type C to a buffer by using its encode method. Leaves the pointer on the cleanup stack. 
 */
	{
	CBufFlat* buf = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(buf);
	
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);
	object.EncodeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	
	aDes = HBufC8::NewL(buf->Size());
	TPtr8 ptr(aDes->Des());
	buf->Read(0, ptr, buf->Size());
	
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(aDes);
	return ptr;
	}

#endif /* SRATEMPLATE_INL */