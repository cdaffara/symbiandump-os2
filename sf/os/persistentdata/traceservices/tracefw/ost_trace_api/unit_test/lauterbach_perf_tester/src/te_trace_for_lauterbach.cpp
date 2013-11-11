// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file te_trace_for_lauterbach.cpp
 @internalTechnology
*/
#include "te_trace_for_lauterbach.h"
#include <e32btrace.h>
#include "utracemacros.h"

//using namespace UTrace;


	EXPORT_C TBool ExportedEmpty()
		{
		return EFalse;
		}

	EXPORT_C TBool Exported()
		{
		for(TInt i = 0; i < 1000;)
			i++;
		return EFalse;
		}

	EXPORT_C TBool TraceNormalAny(const TAny* aData, TInt aSize)
		{
		return UTRACE_SECONDARY_ANY(192, 222, 2, ETrue, ETrue, aData, aSize);
		}

	EXPORT_C TBool TraceNormal2Data(TUint32 aData1, TUint32 aData2)
		{
		return UTRACE_SECONDARY_2(192, 222, 2, ETrue, ETrue, aData1,aData2);
		}

	EXPORT_C TBool TraceFormatIdAny(const TAny* aData, TInt aSize)
		{
#ifdef __KERNEL_MODE__
		return UTRACE_SECONDARY_ANY(192, 222, 2, ETrue, ETrue, aData, aSize);
#else
		TUint32* dataAndFormatId = new TUint32[(aSize + 4)/4 + 1];
		//if KErrNoMemory
		if(dataAndFormatId == NULL)
			return EFalse;
		TUint32 pad = 33;
		memcpy(&dataAndFormatId[0], &pad, 4);
		memcpy(&dataAndFormatId[1], aData, aSize);
		TBool logged = UTRACE_SECONDARY_ANY(192, 222, 2, ETrue, ETrue, dataAndFormatId, aSize+4);
		delete [] dataAndFormatId;
		return logged;
#endif
		}

	EXPORT_C TBool TraceFormatId2Data(TUint32 aData1, TUint32 aData2)
		{
		TUint32 packet[3];
		packet[0] = 33;
		packet[1] = aData1;
		packet[2] = aData2;
		return UTRACE_SECONDARY_ANY(192, 222, 2, ETrue, ETrue, &packet, sizeof(packet));
		}

	EXPORT_C TBool print8(const TDesC8& aDes)
		{
		return UTRACE_SECONDARY_ANY(192, 222, 2, ETrue, ETrue, aDes.Ptr(), aDes.Size());
		}
	
