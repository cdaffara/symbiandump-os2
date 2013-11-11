#ifndef __s32file_private_h__
#define __s32file_private_h__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <s32buf.h>
/**
 * @file
 * @internalComponent
 */

namespace store_private 
{


class MemStreamBuf : public MStreamBuf
	{
public:
	MemStreamBuf(const CStreamStore& aStore, TStreamId aId);
	MemStreamBuf(const CStreamStore& aStore, TStreamId aId, 
				 MStreamBuf *aStreamIn, std::streamoff aOffset, TUint32 aLength);

	void AppendFromFile(MStreamBuf *aStreamIn, TUint32 aLength);

	// Symbian definition does not have a virtual destructor and relies on DoRelease virtual...

	TStreamId StreamId() const;

	typedef std::vector<TUint8> StreamData;
	const StreamData &GetData() const;
	
private:
	virtual TInt DoReadL(TAny *aPtr,TInt aMaxLength);
	virtual void DoWriteL(const TUint8* aPtr,TInt aLength);
	virtual TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
	virtual void DoRelease();

	const CStreamStore *iStore;
	TStreamId iStreamId;
	TUint32 iCurrentReadIndex;
	TUint32 iCurrentWriteIndex;
	StreamData iData;
	};

}; // end of namespace store_private 

#endif
