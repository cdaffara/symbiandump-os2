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
#include "te_apicaller.h"
#include "te_tracerecord.h"
#include "te_logger.h"



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
	//TInt	ReadTraces();
	//TInt 	WriteTraces();
	//TInt	CompareTraces();
private:
	TBool 	PreProcessTrace(TTraceConfigs& aTrace, const TUint8* aData);
	TUint32 ReadTraceWord(const TUint8*& header);
	//TTraceConfigs* NewTrace(TInt aSequence);
public:
//	RPointerArray<TTraceConfigs> iLoggedTraces;
//	RPointerArray<TTraceConfigs> iSentTraces;
private:
	TBool 		iMissingTraceFound;
	TUint64		iTimestamp;
	TBool 		iTimestamp2Present;
};

#endif /*TE_PARSER_H_*/
