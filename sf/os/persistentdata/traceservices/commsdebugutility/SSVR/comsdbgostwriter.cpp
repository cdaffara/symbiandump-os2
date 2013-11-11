// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the Flogger server side data writing
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgwriter.h"

_LIT8(KTypeIdentifierBinary, "b");

COstv2Writer* COstv2Writer::NewL()
	{
	return new(ELeave) COstv2Writer();
	}

void COstv2Writer::LogString(const TDesC8& aString)
	{
	OstPrintf(iContext, aString);
	}

void COstv2Writer::LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
	TUint32 length = static_cast<TUint32>(aBinaryString.Length());
	TBuf8<sizeof(TUint32)> lengthString(4);
	lengthString[0] = static_cast<TUint8>((length & 0x000000ff) >> 0);
	lengthString[1] = static_cast<TUint8>((length & 0x0000ff00) >> 8);
	lengthString[2] = static_cast<TUint8>((length & 0x00ff0000) >> 16);
	lengthString[3] = static_cast<TUint8>((length & 0xff000000) >> 24);
	OstPrintf(iContext, "%S\t%S\t%S\t%S\t%S", &aSubSystem, &aComponent, &KTypeIdentifierBinary, &lengthString, &aBinaryString);
	}


// NOPs for ost
void COstv2Writer::ClearLogL()
	{
	}

void COstv2Writer::FlushLogL()
	{
	}

void COstv2Writer::SetForceFlush(const TBool /*aOn*/)
	{
	}

