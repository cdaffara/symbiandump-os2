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
* Implementation of the MStreamBuf interface that throws away all
* data written to it but keeps track of how many bytes have been
* written to it.  It does not support reading.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
#ifndef __NULLBUF_H__
#define __NULLBUF_H__
#include <e32base.h>
#include <s32buf.h>

NONSHARABLE_CLASS(TNullBuf) : public MStreamBuf
	{
public:
	IMPORT_C TNullBuf();
	IMPORT_C TUint BytesWritten();
private:
	// Implement MStreamBuf vritual
	virtual void DoWriteL(const TAny* aPtr,TInt aLength);
private:
	TUint iBytesWritten;
	};
#endif
