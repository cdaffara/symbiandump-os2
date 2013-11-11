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
* A write stream that throws away all its input, but keeps track of how many
* bytes have been written to it.  It is used for determining the amount of
* memory needed to store externalised objects.
*
*/


/**
 @file
*/
#include "nullbuf.h"
EXPORT_C TNullBuf::TNullBuf() : iBytesWritten(0) 
	{
	}

EXPORT_C TUint TNullBuf::BytesWritten() 
	{
	return iBytesWritten;
	}

void TNullBuf::DoWriteL(const TAny*,TInt aLength)
	{
	iBytesWritten += aLength;
	}
// End of file
