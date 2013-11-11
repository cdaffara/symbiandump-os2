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
 @file te_trace_for_lauterbach.h
 @internalTechnology
*/

#ifndef TEST_UTRACE_H_
#define TEST_UTRACE_H_
#include <opensystemtrace_types.h>

//test methods
inline TBool InlinedEmpty()
	{
	return EFalse;
	};

inline TBool Inlined()
	{
	for(TInt i = 0; i < 1000;)
		i++;
	return EFalse;
	};

IMPORT_C TBool ExportedEmpty();

IMPORT_C TBool Exported();


template<typename T>
inline TBool TemplatedEmpty(const T&)
	{
	return EFalse;
	}

template<typename T>
inline TBool Templated(const T& aStruct)
	{
	return TraceNormalAny(&aStruct, sizeof(aStruct));
	}


IMPORT_C TBool TraceNormalAny(const TAny* aData, TInt aSize);

IMPORT_C TBool TraceNormal2Data(TUint32 aData1, TUint32 aData2);

IMPORT_C TBool TraceFormatIdAny(const TAny* aData, TInt aSize);

IMPORT_C TBool TraceFormatId2Data(TUint32 aData1, TUint32 aData2);

IMPORT_C TBool print8(const TDesC8& aDes);


#endif /*TEST_UTRACE_H_*/
