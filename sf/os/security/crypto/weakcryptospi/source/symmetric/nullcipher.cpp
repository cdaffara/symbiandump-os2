/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nullcipher.h"

const TUint KNullKeyBytes = 0;

EXPORT_C CNullCipher* CNullCipher::NewL(void)
	{
	CNullCipher* self = new(ELeave)CNullCipher;
	return self;
	}

EXPORT_C CNullCipher* CNullCipher::NewLC(void)
	{
	CNullCipher *self = NewL();
	CleanupStack::PushL(self);
	return self;
	}

void CNullCipher::Reset(void)
	{
	}

TInt CNullCipher::KeySize(void) const
	{
	return KNullKeyBytes;
	}

void CNullCipher::DoProcess(TDes8& /*aData*/)
	{
	}

CNullCipher::CNullCipher(void)
	{
	}
