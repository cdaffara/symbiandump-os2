/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for DER encoding of NULLs
*
*/


#include <asn1enc.h>

EXPORT_C CASN1EncNull* CASN1EncNull::NewLC()
	{
	CASN1EncNull* self = new (ELeave) CASN1EncNull;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncNull* CASN1EncNull::NewL()
	{
	CASN1EncNull* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CASN1EncNull::CASN1EncNull()
: CASN1EncPrimitive(EASN1Null)
	{
	}


void CASN1EncNull::CalculateContentsLengthDER()
	{
	iContentsLengthDER = 0;
	}


void CASN1EncNull::WriteContentsDERL(TDes8& /* aBuf */) const
	{
	// Nothing to do
	}
