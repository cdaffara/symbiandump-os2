/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



/**
 @file te_utracecmds.h
 @internalTechnology
*/
#if (!defined __TE_UTRACE_CMDS_H__)
#define __TE_UTRACE_CMDS_H__

#include "te_setfilterparameters.h"

class CUTraces //: public CBase
{
public:
	virtual ~CUTraces();
	CUTraces();
	virtual bool DoTheTrace(const int TracerID);
	virtual bool DoTheSetTrace(const int TracerID, TUTrace& TraceObject);
	virtual bool DoTheStaticTrace(const int TracerID);
	virtual bool DoThePrintf(const int TracerID);
	virtual bool DoTheStaticPrintf(const int TracerID);
	virtual bool UHEAPDoTheTrace(const int TracerID);
	virtual bool UHEAPDoTheStaticTrace(const int TracerID);
	virtual int SizeOfTrace(const int TracerID);
	virtual int SizeOfStaticTrace(const int TracerID);
//private:

};

#endif
