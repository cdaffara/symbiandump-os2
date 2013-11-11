// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file te_utracecmds.cpp
 @internalTechnology
*/

#include "te_utracecmds.h"


CUTraces::~CUTraces()
{
	/*Destructor*/
}

CUTraces::CUTraces()
{
	/*Constructor*/
}

bool CUTraces::DoTheTrace(const int TracerID)
{
	bool ret = 0;
	TUTrace trace(UTracePrimary, UTraceSecondary, schema, context, pc);
	TUTrace truetrace(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
	TUTrace fasttrace(UTracePrimary, schema, context, pc);
	TUTrace fasttruetrace(UTracePrimary, schema, context, pcTrue);
	switch(TracerID)
	{
		case 0:
		ret = trace.Trace();
		break;
		case 1:
		ret = trace.Trace(Tracea1);
		break;
		case 2:
		ret = trace.Trace(Tracea1, Tracea2);
		break;
		case 3:
		ret = trace.Trace(aData, aSize);
		break;
		case 4:
		ret = truetrace.Trace();
		break;
		case 5:
		ret = truetrace.Trace(Tracea1);
		break;
		case 6:
		ret = truetrace.Trace(Tracea1, Tracea2);
		break;
		case 7:
		ret = truetrace.Trace(aData, aSize);
		break;
		case 8:
		ret = fasttrace.Trace();
		break;
		case 9:
		ret = fasttrace.Trace(Tracea1);
		break;
		case 10:
		ret = fasttrace.Trace(Tracea1, Tracea2);
		break;
		case 11:
		ret = fasttrace.Trace(Tracea1, Tracea2, Tracea3);
		break;
		case 12:
		ret = fasttrace.Trace(aData, aSize);
		break;
		case 13:
		ret = fasttruetrace.Trace();
		break;
		case 14:
		ret = fasttruetrace.Trace(Tracea1);
		break;
		case 15:
		ret = fasttruetrace.Trace(Tracea1, Tracea2);
		break;
		case 16:
		ret = fasttruetrace.Trace(Tracea1, Tracea2, Tracea3);
		break;
		case 17:
		ret = fasttruetrace.Trace(aData, aSize);
		break;
		default:
		break;
	}		
	return ret;
}

bool CUTraces::DoTheSetTrace(const int TracerID, TUTrace& TraceObject)
{
	bool ret=0;
	switch(TracerID)
	{
		case 0:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pc);
		ret = TraceObject.Trace();
		break;
		case 1:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pc);
		ret = TraceObject.Trace(Tracea1);
		break;
		case 2:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pc);
		ret = TraceObject.Trace(Tracea1, Tracea2);
		break;
		case 3:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pc);
		ret = TraceObject.Trace(aData, aSize);
		break;
		case 4:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
		ret = TraceObject.Trace();
		break;
		case 5:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
		ret = TraceObject.Trace(Tracea1);
		break;
		case 6:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
		ret = TraceObject.Trace(Tracea1, Tracea2);
		break;
		case 7:
		TraceObject.Set(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
		ret = TraceObject.Trace(aData, aSize);
		break;
		case 8:
		TraceObject.Set(UTracePrimary, schema, context, pc);
		ret = TraceObject.Trace();
		break;
		case 9:
		TraceObject.Set(UTracePrimary, schema, context, pc);
		ret = TraceObject.Trace(Tracea1);
		break;
		case 10:
		TraceObject.Set(UTracePrimary, schema, context, pc);
		ret = TraceObject.Trace(Tracea1, Tracea2);
		break;
		case 11:
		TraceObject.Set(UTracePrimary, schema, context, pc);
		ret = TraceObject.Trace(Tracea1, Tracea2, Tracea3);
		break;
		case 12:
		TraceObject.Set(UTracePrimary, schema, context, pc);
		ret = TraceObject.Trace(aData, aSize);
		break;
		case 13:
		TraceObject.Set(UTracePrimary, schema, context, pcTrue);
		ret = TraceObject.Trace();
		break;
		case 14:
		TraceObject.Set(UTracePrimary, schema, context, pcTrue);
		ret = TraceObject.Trace(Tracea1);
		break;
		case 15:
		TraceObject.Set(UTracePrimary, schema, context, pcTrue);
		ret = TraceObject.Trace(Tracea1, Tracea2);
		break;
		case 16:
		TraceObject.Set(UTracePrimary, schema, context, pcTrue);
		ret = TraceObject.Trace(Tracea1, Tracea2, Tracea3);
		break;
		case 17:
		TraceObject.Set(UTracePrimary, schema, context, pcTrue);
		ret = TraceObject.Trace(aData, aSize);
		break;
		default:
		break;
	}
	return ret;	
}
bool CUTraces::DoTheStaticTrace(const int TracerID)

{
	bool ret = 0;
	switch(TracerID)
	{
		case 0:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pc);
		break;
		case 1:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pc, Tracea1);
		break;
		case 2:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pc, Tracea1, Tracea2);
		break;
		case 3:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pc, aData, aSize);
		break;
		case 4:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
		break;
		case 5:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pcTrue, Tracea1);
		break;
		case 6:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pcTrue, Tracea1, Tracea2);
		break;
		case 7:
		ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pcTrue, aData, aSize);
		break;
		case 8:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pc);
		break;
		case 9:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pc, Tracea1);
		break;
		case 10:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pc, Tracea1, Tracea2);
		break;
		case 11:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pc, Tracea1, Tracea2, Tracea3);
		break;
		case 12:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pc, aData, aSize);
		break;
		case 13:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pcTrue);
		break;
		case 14:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pcTrue, Tracea1);
		break;
		case 15:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pcTrue, Tracea1, Tracea2);
		break;
		case 16:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pcTrue, Tracea1, Tracea2, Tracea3);
		break;
		case 17:
		ret = TUTrace::TracePrimary(UTracePrimary, schema, context, pcTrue, aData, aSize);
		break;
		default:
		break;
	}
	return ret;

}


bool CUTraces::DoThePrintf(const int TracerID)
{
	bool ret = 0;
#ifndef __KERNEL_MODE__
	TBuf16<0x100> iBuf16;
	iBuf16 = KName16;
#endif
	TBuf8<0x100> iBuf8;
	iBuf8 = KName8;
	TUTrace trace(UTracePrimary, UTraceSecondary, schema, context, pc);
	TUTrace truetrace(UTracePrimary, UTraceSecondary, schema, context, pcTrue);
	TUTrace fasttrace(UTracePrimary, schema, context, pc);
	TUTrace fasttruetrace(UTracePrimary, schema, context, pcTrue);
	switch(TracerID)
	{
#ifndef __KERNEL_MODE__
		case 0:
		ret = trace.Printf("c-style \"string\" with number %i!", 9999);
		break;
		case 1:
		ret = trace.Printf(KName16, 9999);
		break;
		case 2:
		ret = trace.Printf(iBuf8);
		break;
		case 3:
		ret = trace.Printf(iBuf16);
		break;
		case 4:
		ret = truetrace.Printf("c-style \"string\" with number %i!", 9999);
		break;
		case 5:
		ret = truetrace.Printf(KName16, 9999);
		break;
		case 6:
		ret = truetrace.Printf(iBuf8);
		break;
		case 7:
		ret = truetrace.Printf(iBuf16);
		break;
#endif
		case 8:
		ret = fasttrace.Printf("c-style \"string\" with number %i!", 9999);
		break;
#ifndef __KERNEL_MODE__
		case 9:
		ret = fasttrace.Printf(KName16, 9999);
		break;
		case 10:
		ret = fasttrace.Printf(iBuf8);
		break;
		case 11:
		ret = fasttrace.Printf(iBuf16);
		break;
#endif
		case 12:
		ret = fasttruetrace.Printf("c-style \"string\" with number %i!", 9999);
		break;
#ifndef __KERNEL_MODE__
		case 13:
		ret = fasttruetrace.Printf(KName16, 9999);
		break;
		case 14:
		ret = fasttruetrace.Printf(iBuf8);
		break;
		case 15:
		ret = fasttruetrace.Printf(iBuf16);
		break;
#endif
		default:
		break;
		
		}		
	return ret;	
}

bool CUTraces::DoTheStaticPrintf(const int TracerID)
{
	bool ret = 0;
#ifndef __KERNEL_MODE__
	TBuf16<0x100> iBuf16;
	iBuf16 = KName16;
#endif
	TBuf8<0x100> iBuf8;
	iBuf8 = KName8;
	switch(TracerID)
	{
#ifndef __KERNEL_MODE__
		case 0:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pc, "static string no %i", 9999);
		break;
		case 1:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pc, iBuf8);
		break;
		case 2:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pc, KName16, 9999);
		break;
		case 3:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pc, iBuf16);
		break;	
		case 4:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pcTrue, "static string no %i", 9999);
		break;
		case 5:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pcTrue, iBuf8);
		break;
		case 6:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pcTrue, KName16, 9999);
		break;
		case 7:
		ret = TUTrace::Printf(UTracePrimary, UTraceSecondary, context, pcTrue, iBuf16);
		break;
#endif
		case 8:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pc, "static string no %i", 9999);
		break;
		case 9:
#ifndef __KERNEL_MODE__
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pc, iBuf8);
		break;
		case 10:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pc, KName16, 9999);
		break;
		case 11:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pc, iBuf16);
		break;
#endif
		case 12:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pcTrue, "static string no %i", 9999);
		break;
#ifndef __KERNEL_MODE__
		case 13:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pcTrue, iBuf8);
		break;
		case 14:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pcTrue, KName16, 9999);
		break;
		case 15:
		ret = TUTrace::PrintfPrimary(UTracePrimary, context, pcTrue, iBuf16);
		break;
#endif
		default:
		break;
	}
	return ret;
}

bool CUTraces::UHEAPDoTheTrace(const int TracerID)
{
	bool ret = 0;
#ifndef __KERNEL_MODE__
	__UHEAP_MARK;	
	ret = DoTheTrace(TracerID);
	__UHEAP_MARKEND;
#endif
	return ret;	
}

bool CUTraces::UHEAPDoTheStaticTrace(const int TracerID)
{
	bool ret = 0;
#ifndef __KERNEL_MODE__
	__UHEAP_MARK;	
	ret = DoTheStaticTrace(TracerID);
	__UHEAP_MARKEND;
#endif
	return ret;	
}

int CUTraces::SizeOfTrace(const int TracerID)
{
	//This sends the size in bytes of the trace that we want to test
		
	// UTraceSecondary+ nothing else = 16 bytes
	// UTraceSecondary+ UTracePrimary + nothing else = 20 bytes
	// UTraceSecondary+ pc + nothing else = 20 bytes
	// UTraceSecondary+ UTracePrimary + pc + nothing else = 24 bytes
	// for passing data, TUint32 = 4 bytes
	int TraceSize = 0;
	int UTracePrimarytracesize = 16;
	switch(TracerID)
	{
		case 0:
		TraceSize=UTracePrimarytracesize+sizeof(UTraceSecondary);
		break;
		case 1:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+sizeof(Tracea1);
		break;
		case 2:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+sizeof(Tracea1)+sizeof(Tracea2);
		break;
		case 3:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+aSize;
		break;
		case 4:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+sizeof(TBool);
		break;
		case 5:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+sizeof(Tracea1)+sizeof(TBool);
		break;
		case 6:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+sizeof(Tracea1)+sizeof(Tracea2)+sizeof(TBool);
		break;
		case 7:
		TraceSize=sizeof(UTraceSecondary)+UTracePrimarytracesize+aSize+sizeof(TBool);
		break;
		case 8:
		TraceSize=UTracePrimarytracesize;
		break;
		case 9:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1);
		break;
		case 10:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+sizeof(Tracea2);
		break;
		case 11:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+sizeof(Tracea2)+sizeof(Tracea3);
		break;
		case 12:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+aSize;
		break;
		case 13:
		TraceSize=UTracePrimarytracesize+sizeof(TBool);
		break;
		case 14:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+sizeof(TBool);
		break;
		case 15:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+sizeof(Tracea2)+sizeof(TBool);
		break;
		case 16:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+sizeof(Tracea2)+sizeof(Tracea3)+sizeof(TBool);
		break;
		case 17:
		TraceSize=UTracePrimarytracesize+sizeof(Tracea1)+aSize+sizeof(TBool);
		break;
		default:
		break;
	}
	return TraceSize;
}


int CUTraces::SizeOfStaticTrace(const int TracerID)
{
	int TraceSize = SizeOfTrace(TracerID);
	return TraceSize;
}


