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
 @file te_parser.h
 @internalTechnology
*/
#ifndef TE_PARSER_H_
#define TE_PARSER_H_

#include <e32std.h>
#include <e32svr.h>
#include <d32btrace.h>
#include "te_tracerecord.h"
#include "te_printer.h"

const TUint8 KJunkTraceCategory = 255;

/**
 * This class sends and reads traces from the log file and then compares them against each other.
 * It's currently not testing: ComponentId, payload, TraceId or actual content of ContextId or PC.
 */
class TTraceParser
{
public:
	TTraceParser();
	~TTraceParser();
	TInt ParseRawBuffer(TUint8* aRawBuffer, TUint aBufferSize, RPointerArray<TTraceConfigs>& aLoggedTraces);

public:
	TInt		iNumSystemTraces;	//make public so that test classes can see how many system traces there are
	
private:
	TBool 	PreProcessTrace(TTraceConfigs& aTrace, const TUint8* aData);
	TUint32 ReadTraceWord(const TUint8*& header);

private:
	TBool 		iMissingTraceFound;
	TUint64		iTimestamp;
	TBool 		iTimestamp2Present;
	TBool		iLastTrace;
	TBool		iTestTrace;
};

class TTraceHeaderParser
{
public:
	TTraceHeaderParser();
	~TTraceHeaderParser();
	TInt ParseRawBuffer(TUint8* aRawBuffer, TUint aBufferSize, CArrayPtrSeg<TTraceHeaderInfo>*& aLoggedTraces);

public:
	TInt		iNumSystemTraces;	//make public so that test classes can see how many system traces there are
	
private:
	TBool 	PreProcessTrace(TTraceHeaderInfo& aTraceHeader, const TUint8* aData);
	TUint32 ReadTraceWord(const TUint8*& header);

private:
	TBool 		iMissingTraceFound;
	TBool		iLastTrace;
	TBool		iTestTrace;
};

#endif /*TE_PARSER_H_*/
