// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include <caf/streaming/keystreamsink.h>
#include "ipsec/ipseckeystreamsink.h"
#include <s32mem.h>

const TInt KExpandSize = 128;
  
using namespace StreamAccess;  
  
EXPORT_C TPtr8 CKeyStreamSink::ExternalizeLC() const
	{
	CBufFlat* buf = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(buf);
		
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);
	this->DoExternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
			
	HBufC8 *allocatedBuf = HBufC8::NewL(buf->Size());
	TPtr8 ptr(allocatedBuf->Des());
	buf->Read(0, ptr, buf->Size());
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(allocatedBuf);
	return ptr;
	}

EXPORT_C CKeyStreamSink* CKeyStreamSink::InternalizeLC(const TDesC8 &aBuf)
	{
	CKeyStreamSink* ret(NULL);
	RDesReadStream readStream(aBuf);
	CleanupClosePushL(readStream);
	TUint16 keyStreamSinkType = readStream.ReadUint16L();
	switch (keyStreamSinkType)
		{
		case EIpSecSinkId: ret = CIpSecKeyStreamSink::NewLC(readStream); break;
		default: User::Leave(KErrNotSupported);
		}
	CleanupStack::Pop(ret);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PushL(ret);
	return ret;
	}
