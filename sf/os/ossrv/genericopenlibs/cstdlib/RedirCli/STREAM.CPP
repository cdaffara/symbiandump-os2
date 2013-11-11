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
//

#include <e32std.h>
#include <redirstr.h>
#include <redircli.h>
//
// -------------- Stream implementation ------------------------
//
EXPORT_C void CStreamBase2::CompleteRead(const RMessage2 &aMessage) 
	{
	// retrieve the max length of the buffer the client has allocated
	TInt length = aMessage.Int1();
	// create a local buffer for placing characters to be written to
	// client. This is a copy of the the iBuf internal buffer of the stream
	TBuf8<256> buf(iBuf);

	if (length < buf.Length() ) 
		{
		// then we can't copy all the buffer to the client so we have reduce
		// the buffers length
		buf.SetLength(length);
		// delete appropriate part of stream's internal buffer
		iBuf.Delete(0,length);
		} 
	else 
		{
		// we can copy all of the buffer to the client so we set the streams
		// descriptor to zero length
		iBuf.Zero();
		}
	// copy the local descriptor to the client.
	TRAPD(ret,aMessage.WriteL(0, buf));
	// inform the client of success/failure by completing the message
	aMessage.Complete(ret);
	return;
	}

EXPORT_C void CStreamFactoryBase2::AppendToStatus(const TDesC& aMsg) { iStatusMsg.Append(aMsg); }

EXPORT_C const TDesC& CStreamFactoryBase2::GetStatus() const { return iStatusMsg; }

// Dummy functions to replace previous exports so def file does not need reordering.
EXPORT_C void DummyReserved1() {};
EXPORT_C void DummyReserved2() {};
EXPORT_C void DummyReserved3() {};
EXPORT_C void DummyReserved4() {};
EXPORT_C void DummyReserved5() {};
EXPORT_C void DummyReserved6() {};
EXPORT_C void DummyReserved7() {};
EXPORT_C void DummyReserved8() {};
EXPORT_C void DummyReserved9() {};
EXPORT_C void DummyReserved10() {};
EXPORT_C void DummyReserved11() {};
